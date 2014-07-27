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

#ifndef INCLUDE_LIBCUTE_UTIL_SYSTEM_UTILS_H
#define INCLUDE_LIBCUTE_UTIL_SYSTEM_UTILS_H

#include <string>
#include <cstdint>

#ifdef _WIN32
	#include <windows.h>
#else
	extern "C"
	{
		#include <dirent.h>
	}
#endif

/*!
 * \brief This class provides several OS-specific utility functions.
 */
class CSSystemUtils
{
	public:
		static std::string getHumanReadableSize(uint64_t b, int p = 2);
		static uint64_t getDeviceUsed(const std::string &p);
		static uint64_t getDeviceAvailable(const std::string &p);
		static uint64_t getDeviceCapacity(const std::string &p);
		static double getDeviceUsedPercent(const std::string &p);

		static int64_t getFileCount(const std::string &p,
			bool r = true);

	private:
		#ifdef _WIN32
			static int64_t getFileCountRec(HANDLE h,
				const std::string &p, WIN32_FIND_DATA e,
				bool r);
		#else
			static int64_t getFileCountRec(DIR *d,
				const std::string &p, struct stat *s, bool r);
		#endif
};

#endif
