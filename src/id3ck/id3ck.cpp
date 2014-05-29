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

#include <iostream>
#include <cstdint>

#include <QDirIterator>

#include "libcute/util/MMIOHandle.h"

inline bool uclt(char a, char b)
{
	return (static_cast<unsigned char>(a) < static_cast<unsigned char>(b));
}

inline bool uclte(char a, char b)
{
	return (static_cast<unsigned char>(a) <= static_cast<unsigned char>(b));
}

inline bool uceq(char a, char b)
{
	return (static_cast<unsigned char>(a) == static_cast<unsigned char>(b));
}

inline bool ucgte(char a, char b)
{
	return (static_cast<unsigned char>(a) >= static_cast<unsigned char>(b));
}

inline bool ucgt(char a, char b)
{
	return (static_cast<unsigned char>(a) > static_cast<unsigned char>(b));
}

uint32_t fromSynchsafeInt32(const CuteSyncMMIOHandle &f, uint64_t o)
{
	uint32_t result = 0;

	result |= static_cast<uint32_t>( f.at(o+0) & 0x7F ) << 21;
	result |= static_cast<uint32_t>( f.at(o+1) & 0x7F ) << 14;
	result |= static_cast<uint32_t>( f.at(o+2) & 0x7F ) << 7;
	result |= static_cast<uint32_t>( f.at(o+3) & 0x7F );

	return result;
}

void toSynchsafeInt32(CuteSyncMMIOHandle &f, uint64_t o, uint32_t i)
{
	f.set( o+3, static_cast<char>(         i & 0x7F) );
	f.set( o+2, static_cast<char>(  (i >> 7) & 0x7F) );
	f.set( o+1, static_cast<char>( (i >> 14) & 0x7F) );
	f.set( o+0, static_cast<char>( (i >> 21) & 0x7F) );
}

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		std::cout << "You must provide a directory to scan for broken MP3 files.\n";
		return 1;
	}

	QDirIterator walker(argv[1], QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
	while(walker.hasNext())
	{
		walker.next();

		CuteSyncMMIOHandle mmio(walker.fileInfo().absoluteFilePath().toLatin1().data());
		if(!mmio.open(CuteSyncMMIOHandle::ReadWrite))
		{
			std::cerr << "Failed to open: " << walker.fileInfo().absoluteFilePath().toLatin1().data() << "\n";
			continue;
		}

		// If this file starts with an MP3 frame header, it's good to go - no ID3v2 tag.
		if( uceq(mmio.at(0), 0xFF) && (uceq(mmio.at(1), 0xFB) || uceq(mmio.at(1), 0xFA)) )
			continue;

		/*
		 * If the file starts with an ID3(v2) header, then we are going to read past the tag to make sure it is
		 * indeed MP3. From http://www.id3.org/id3v2.4.0-structure:
		 *
		 *     An ID3v2 tag can be detected with the following pattern:
		 *         $49 44 33 yy yy xx zz zz zz zz
		 *     Where yy is less than $FF, xx is the 'flags' byte and zz is less than $80.
		 */
		if( uceq(mmio.at(0), 0x49) && uceq(mmio.at(1), 0x44) && uceq(mmio.at(2), 0x33) && uclt(mmio.at(3), 0xFF) &&
			uclt(mmio.at(4), 0xFF) && uclt(mmio.at(6), 0x80) && uclt(mmio.at(7), 0x80) && uclt(mmio.at(8), 0x80) &&
			uclt(mmio.at(9), 0x80) )
		{
			// Try to read the ID3 header size, and skip past it.
			uint64_t tagsize = static_cast<uint64_t>(fromSynchsafeInt32(mmio, 6));
			tagsize += 10; // Account for the first portion of the header.
			if(mmio.at(5) & 0x10) tagsize += 10; // Account for footer, if present.

			// Make sure there is an MP3 frame header where there should be.
			if( ((tagsize+3) < mmio.getLength()) && uceq(mmio.at(tagsize), 0xFF) && (uceq(mmio.at(tagsize+1), 0xFB) || uceq(mmio.at(tagsize+1), 0xFA)) )
			{
				continue;
			}
			else
			{
				std::cout << "Found broken file '" << walker.fileInfo().absoluteFilePath().toLatin1().data() << "'... ";

				/*
				 * The size in the ID3 tag seems to be incorrect; find the first MP3 frame and then update the size to be
				 * correct.
				 */

				for(uint64_t i = 0; i < (mmio.getLength()-1); ++i)
				{
					if(uceq(mmio.at(i), 0xFF))
					{
						if( uceq(mmio.at(i+1), 0xFB) || uceq(mmio.at(i+1), 0xFA) )
						{
							tagsize = (i - 10 - (mmio.at(5) & 0x10 ? 10 : 0));
							toSynchsafeInt32(mmio, 6, static_cast<uint32_t>(tagsize));

							std::cout << "FIXED!";
							break;
						}

						// If the next byte in the file isn't 0xFF either, skip ahead.
						if( !uceq(mmio.at(i+1), 0xFF) ) ++i;
					}
				}

				std::cout << "\n";
			}
		}
	}

	return 0;
}
