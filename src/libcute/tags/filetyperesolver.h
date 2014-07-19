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

#ifndef INCLUDE_LIBCUTE_TAGS_FILE_TYPE_RESOLVER_H
#define INCLUDE_LIBCUTE_TAGS_FILE_TYPE_RESOLVER_H

#include <cstdint>

#include <taglib/fileref.h>

class CSMMIOHandle;

/*!
 * \brief This class provides a way to resolve the file type of input files.
 *
 * We determine the filetype not by file extension (which can be sort of
 * unrealiable) but rather by reading a very small portion of the file and
 * checking it against known header signatures. We extend TagLib's
 * FileTypeResolver class so we can be used directly with TagLib, or you can
 * simply call createFile() manually and use that file pointer directly.
 */
class CSFileTypeResolver : public TagLib::FileRef::FileTypeResolver
{
	public:
		CSFileTypeResolver();
		virtual ~CSFileTypeResolver();

		virtual TagLib::File *createFile(TagLib::FileName fn,
			bool ap = true,
			TagLib::AudioProperties::ReadStyle aps =
			TagLib::AudioProperties::Average) const;

	private:
		bool isValidFtyp(const CSMMIOHandle &f,
			uint64_t o) const;
};

#endif
