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

#ifndef INCLUDE_CUTE_SYNC_MMIO_HANDLE_H
#define INCLUDE_CUTE_SYNC_MMIO_HANDLE_H

/*
 * <string> needs to be included FIRST on Apple hosts to not interfere with
 * our other includes.
 */
#ifdef __APPLE__
	#include <string>
#endif

#ifdef _WIN32
	#include <Windows.h>
#else
	extern "C"
	{
		#include <sys/types.h>
		#include <stdint.h>
	}

	namespace MMAP
	{
		extern "C"
		{
			#include <sys/stat.h>
			#include <unistd.h>
		}

		typedef struct stat FileStats;
	}
#endif

/*
 * However, if <string> is included first on, e.g., Linux, then it will interfere with
 * our other includes, so it needs to be included AFTER.
 */
#ifndef __APPLE__
	#include <string>
#endif

#include <cstdint>

/*!
 * \brief This class provides an interface to do memory-mapped IO in a platform-independent way.
 *
 * Our interface is currently implemented on:
 *     Windows
 *     Linux/UNIX
 *     Mac
 *
 * It should be noted that Windows, unlike other platforms, DOES use buffered I/O when doing MMIO, so this
 * class may not be as performant on that platform.
 */
class CuteSyncMMIOHandle
{
	public:
		enum OpenMode
		{
			ReadOnly,
			ReadWrite
		};

		CuteSyncMMIOHandle(const std::string &p = "");
		virtual ~CuteSyncMMIOHandle();

		std::string getPath() const;
		void setPath(const std::string &p);
		OpenMode getMode() const;

		bool open(OpenMode m, bool c = false, uint64_t s = 0);
		bool isOpen() const;
		void flush();
		void close();

		uint64_t getLength() const;

		char at(uint64_t o) const;
		bool at(uint64_t o, char *b, uint64_t l) const;
		bool set(uint64_t o, char c);
		bool set(uint64_t o, const char *c, uint64_t l);

	private:
		std::string path;
		OpenMode mode;

		#ifdef _WIN32
			HANDLE fileHandle;
			HANDLE mmioHandle;
			char *view;
		#else
			MMAP::FileStats *fstats;
			char *view;
		#endif
};

#endif
