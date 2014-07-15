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

#include "bitwise.h"

#include "libcute/util/mmiohandle.h"

/*!
 * This function converts a "32-bit synchsafe integer" found e.g. in MP3
 * headers, and converts it to a standard 32-bit integer type.
 *
 * These "synchsafe integers" are more or less 32-bit integers, except every
 * 8th bit is ignored. So:
 *
 *     1111 1111 1111 1111 (0xFFFF) =>  0011 0111 1111 0111 1111 (0x37F7F)
 *
 * It should be noted that, if you are getting the input from an MP3 header,
 * the number we return DOES NOT INCLUDE THE MP3 FOOTER (IF PRESENT). It is up
 * to you to take our output and turn it into a sane number.
 *
 * Also note that it is your responsibility to make sure the buffer and offset
 * provided are valid; we don't do any bounds-checking.
 *
 * \param f The file handle containing the raw data.
 * \param o The offset in the buffer to start at (by default, 0).
 * \return The value given as a normal 32-bit integer.
 */
uint32_t CSBitwise::fromSynchsafeInt32(
	const CSMMIOHandle &f, uint64_t o)
{
	uint32_t result = 0;

	result |= static_cast<uint32_t>( f.at(o+0) & 0x7F ) << 21;
	result |= static_cast<uint32_t>( f.at(o+1) & 0x7F ) << 14;
	result |= static_cast<uint32_t>( f.at(o+2) & 0x7F ) << 7;
	result |= static_cast<uint32_t>( f.at(o+3) & 0x7F );

	return result;
}

/*!
 * This function writes the given 32-bit integer to the given MMIO file handle
 * at the given offset as a "32-bit synchsafe integer". For more information on
 * what exactly this format means, see fromSynchsafeInt32().
 *
 * \param f The file to write the integer to.
 * \param o The offset in the file to write the integer at.
 * \param i The integer to write to the file.
 */
void CSBitwise::toSynchsafeInt32(
	CSMMIOHandle &f, uint64_t o, uint32_t i)
{
	f.set(o + 3, static_cast<uint8_t>(i & 0x7F));
	f.set(o + 2, static_cast<uint8_t>((i >> 7) & 0x7F));
	f.set(o + 1, static_cast<uint8_t>((i >> 14) & 0x7F));
	f.set(o + 0, static_cast<uint8_t>((i >> 21) & 0x7F));
}
