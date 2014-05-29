/*
 * CuteSync - A media library management and synchronization application.
 * Copyright (C) 2011 Axel Rasmussen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "FileTypeResolver.h"

#include <QFile>

#include <taglib/mp4file.h>
#include <taglib/mpegfile.h>

#include "libcute/util/MMIOHandle.h"

/*!
 * This is our default constructor, which creates our new object.
 */
CuteSyncFileTypeResolver::CuteSyncFileTypeResolver()
{
}

/*!
 * This is our default destructor, which cleans up & destroys our object.
 */
CuteSyncFileTypeResolver::~CuteSyncFileTypeResolver()
{
}

/*!
 * This function takes an input file and decides what type of tile it is. This is done by reading the
 * file, not by something as naieve as file extension checking.
 *
 * Note that the "TagLib::FileName" typedef is really a "const char *".
 *
 * \param fn The path to the file we are to resolve.
 * \param ap Whether or not to read audio properties.
 * \param aps How accurate audio property reading should be.
 */
TagLib::File *CuteSyncFileTypeResolver::createFile(TagLib::FileName fn, bool ap, TagLib::AudioProperties::ReadStyle aps) const
{
	CuteSyncMMIOHandle file(fn);

	if(!file.open(CuteSyncMMIOHandle::ReadOnly))
		return NULL;

	// Check if this file has an "ftyp" header consistent with MP4 containers.
	if(uceq(file.at(0), 0x00) &&
		uceq(file.at(1), 0x00) &&
		uceq(file.at(2), 0x00) &&
		uceq(file.at(3), 0x18) &&
		uceq(file.at(4), 0x66) &&
		uceq(file.at(5), 0x74) &&
		uceq(file.at(6), 0x79) &&
		uceq(file.at(7), 0x70))
	{
		// If the "ftyp" listed after this header is one we recognize, return a valie MP4 file.
		if(isValidFtyp(file, 8))
			return new TagLib::MP4::File(fn, ap, aps);
	}

	// If it starts with an MP3 frame header, it is an MP3 file.
	if( uceq(file.at(0), 0xFF) && (uceq(file.at(1), 0xFB) || uceq(file.at(1), 0xFA)) )
		return new TagLib::MPEG::File(fn, ap, aps);

	/*
	 * If the file starts with an ID3(v2) header, then we are going to read past the tag to make sure it is
	 * indeed MP3. From http://www.id3.org/id3v2.4.0-structure:
	 *
	 *     An ID3v2 tag can be detected with the following pattern:
	 *         $49 44 33 yy yy xx zz zz zz zz
	 *     Where yy is less than $FF, xx is the 'flags' byte and zz is less than $80.
	 */
	if( uceq(file.at(0), 0x49) && uceq(file.at(1), 0x44) && uceq(file.at(2), 0x33) && uclt(file.at(3), 0xFF) &&
		uclt(file.at(4), 0xFF) && uclt(file.at(6), 0x80) && uclt(file.at(7), 0x80) && uclt(file.at(8), 0x80) &&
		uclt(file.at(9), 0x80) )
	{
		// Try to read the ID3 header size, and skip past it.
		uint64_t tagsize = static_cast<uint64_t>(fromSynchsafeInt32(file, 6));
		tagsize += 10; // Account for the first portion of the header.
		if(file.at(5) & 0x10) tagsize += 10; // Account for footer, if present.

		// Make sure there is an MP3 frame header where there should be.
		if( uceq(file.at(tagsize), 0xFF) && (uceq(file.at(tagsize+1), 0xFB) || uceq(file.at(tagsize+1), 0xFA)) )
		{
			return new TagLib::MPEG::File(fn, ap, aps);
		}
		else
		{
			/*
			 * WE ARE GOING TO ASSUME THE ID3V2 TAG IS SIMPLY CORRUPT - I.E., THE TAGSIZE STORED IS NOT THE ACTUAL
			 * SIZE OF THE TAG. This means we need to start at the top of the file and search it for a valid MP3
			 * frame header. If we find one, we should let the user know that they should fix their files. If we
			 * don't, then this is (probably) a non-MP3 file with an ID3v2 tag (which is actually okay).
			 */

			for(uint64_t i = 0; i < (file.getLength()-1); ++i)
			{
				if(uceq(file.at(i), 0xFF))
				{
					if( uceq(file.at(i+1), 0xFB) || uceq(file.at(i+1), 0xFA) )
						return new TagLib::MPEG::File(fn, ap, aps);

					// If the next byte in the file isn't 0xFF either, skip ahead.
					if( !uceq(file.at(i+1), 0xFF) ) ++i;
				}
			}
		}
	}

	return NULL;
}

/*!
 * This function takes a byte buffer and tests if it contains a valid "ftyp" identifier. The list of ftyp
 * identifiers we consider valid comes from http://www.ftyps.com/. As a general rule, the identifiers we
 * ignore are ones that a) pertain to things other than audio (typically video), or b) are not part of
 * the official standard (e.g. Flash audio/video files). As such, we should support anything iTunes produces
 * as well as things encoded by free MP4/AAC libraries like faad/faac.
 *
 * Note that it is up to the caller to make sure the offset provided is valid. We expect at least FOUR
 * BYTES to occupy the indices o, o+1, o+2 and o+3. If any of these indices go off the end of the handle
 * given, then our behavior is undefined.
 *
 * \param f The file handle containing the raw data.
 * \param o The offset of the first identifier byte in the buffer.
 * \return True if the identifier was valid, or false otherwise.
 */
bool CuteSyncFileTypeResolver::isValidFtyp(const CuteSyncMMIOHandle &f, uint64_t o) const
{
	static const char ftyps[8][4] = {
		{'M', '4', 'A', ' '},
		{'M', '4', 'B', ' '},
		{'M', '4', 'P', ' '},
		{'m', 'p', '2', '1'},
		{'m', 'p', '4', '1'},
		{'m', 'p', '4', '2'},
		{'i', 's', 'o', '2'},
		{'i', 's', 'o', 'm'}
	};

	if(!f.isOpen()) return false;

	for(int i = 0; i < 8; ++i)
	{
		if( uceq(f.at(o), ftyps[i][0]) &&
			uceq(f.at(o+1), ftyps[i][1]) &&
			uceq(f.at(o+2), ftyps[i][2]) &&
			uceq(f.at(o+3), ftyps[i][3]) )
		{
			return true;
		}
	}

	return false;
}

/*!
 * This function converts a "32-bit synchsafe integer" found e.g. in MP3 headers, and converts it to a standard
 * 32-bit integer type.
 *
 * These "synchsafe integers" are more or less 32-bit integers, except every 8th bit is ignored. So:
 *     1111 1111 1111 1111 (0xFFFF) =>  0011 0111 1111 0111 1111 (0x37F7F)
 *
 * It should be noted that, if you are getting the input from an MP3 header, the number we return DOES NOT INCLUDE
 * THE MP3 FOOTER (IF PRESENT). It is up to you to take our output and turn it into a sane number.
 *
 * Also note that it is your responsibility to make sure the buffer and offset provided are valid; we don't do
 * any bounds-checking.
 *
 * \param f The file handle containing the raw data.
 * \param o The offset in the buffer to start at (by default, 0).
 * \return The value given as a normal 32-bit integer.
 */
uint32_t CuteSyncFileTypeResolver::fromSynchsafeInt32(const CuteSyncMMIOHandle &f, uint64_t o) const
{
	uint32_t result = 0;

	result |= static_cast<uint32_t>( f.at(o+0) & 0x7F ) << 21;
	result |= static_cast<uint32_t>( f.at(o+1) & 0x7F ) << 14;
	result |= static_cast<uint32_t>( f.at(o+2) & 0x7F ) << 7;
	result |= static_cast<uint32_t>( f.at(o+3) & 0x7F );

	return result;
}

/*!
 * This is one of our unsigned comparison operators. It is here for convenience, so we can compare bytes we
 * read in an unsigned way without manually casting each time.
 *
 * This particular function is an Unsigned Compare Less-Than.
 *
 * \param a The first byte to compare.
 * \param b The second byte to compare.
 * \return True if a is less than b, or false otherwise.
 */
bool CuteSyncFileTypeResolver::uclt(char a, char b) const
{
	return static_cast<unsigned char>(a) < static_cast<unsigned char>(b);
}

/*!
 * This is one of our unsigned comparison operators. It is here for convenience, so we can compare bytes we
 * read in an unsigned way without manually casting each time.
 *
 * This particular function is an Unsigned Compare Less-Than-or-Equal.
 *
 * \param a The first byte to compare.
 * \param b The second byte to compare.
 * \return True if a is less than or equal to b, or false otherwise.
 */
bool CuteSyncFileTypeResolver::uclte(char a, char b) const
{
	return static_cast<unsigned char>(a) <= static_cast<unsigned char>(b);
}

/*!
 * This is one of our unsigned comparison operators. It is here for convenience, so we can compare bytes we
 * read in an unsigned way without manually casting each time.
 *
 * This particular function is an Unsigned Compare EQual.
 *
 * \param a The first byte to compare.
 * \param b The second byte to compare.
 * \return True if a is equal to b, or false otherwise.
 */
bool CuteSyncFileTypeResolver::uceq(char a, char b) const
{
	return static_cast<unsigned char>(a) == static_cast<unsigned char>(b);
}

/*!
 * This is one of our unsigned comparison operators. It is here for convenience, so we can compare bytes we
 * read in an unsigned way without manually casting each time.
 *
 * This particular function is an Unsigned Compare Greater-Than-or-Equal.
 *
 * \param a The first byte to compare.
 * \param b The second byte to compare.
 * \return True if a is greater than or equal to b, or false otherwise.
 */
bool CuteSyncFileTypeResolver::ucgte(char a, char b) const
{
	return static_cast<unsigned char>(a) >= static_cast<unsigned char>(b);
}

/*!
 * This is one of our unsigned comparison operators. It is here for convenience, so we can compare bytes we
 * read in an unsigned way without manually casting each time.
 *
 * This particular function is an Unsigned Compare Greater-Than.
 *
 * \param a The first byte to compare.
 * \param b The second byte to compare.
 * \return True if a is greater than b, or false otherwise.
 */
bool CuteSyncFileTypeResolver::ucgt(char a, char b) const
{
	return static_cast<unsigned char>(a) > static_cast<unsigned char>(b);
}