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

#include "SystemUtils.h"

#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <cstring>

#ifdef _WIN32
	#include <lmcons.h>
#else
	#include <cstdio>

	extern "C"
	{
		#include <sys/statvfs.h>
		#include <sys/types.h>
		#include <sys/stat.h>
		#include <unistd.h>
	}
#endif

/*!
 * This function converts the given size in bytes to a human-readable format (similar to that returned by
 * `df -h` on UNIX systems). We divide by 1,024 until the value is less than 1,024, and we attach an
 * appropriate unit (i.e., 'MiB' or 'GiB') at the end.
 *
 * This function is platform-independent.
 *
 * \param b The size to convert, in bytes.
 * \param p The decimal precision to use - the number of decimal places.
 * \return A human-readable string.
 */
std::string CuteSyncSystemUtils::getHumanReadableSize(uint64_t b, int p)
{
	std::string units;
	double s;

	p = (p < 0) ? 2 : p;

	// Do some division, and figure out our units.

	if(b >= 1099511627776ULL)
	{
		units = "TiB";
		s = ( static_cast<double>(b) /
			static_cast<double>(1099511627776ULL) );
	}
	else if(b >= 1073741824ULL)
	{
		units = "GiB";
		s = ( static_cast<double>(b) /
			static_cast<double>(1073741824ULL) );
	}
	else if(b >= 1048576)
	{
		units = "MiB";
		s = ( static_cast<double>(b) / static_cast<double>(1048576) );
	}
	else if(b >= 1024)
	{
		units = "KiB";
		s = ( static_cast<double>(b) / static_cast<double>(1024) );
	}
	else
	{
		units = "Bytes";
		s = static_cast<double>(b);
	}

	// Print our result.

	std::ostringstream ss;

	ss << std::fixed << std::setprecision(p) << s;
	ss << " " << units;

	return ss.str();
}

/*!
 * This is one of our disk space functions. It returns the used space on a device, indicated by the given
 * path. The path provided can be /any file or directory inside the device/, or the mount point
 * itself.
 *
 * On Windows, this function will return the same value as the "Used space:" line in the drive properties
 * dialog. This is equivalent to the capacity of the device, less the TOTAL free space - including any
 * "extra privileged space" not included by "available" space.
 *
 * On Linux/UNIX/Mac, this function will return the same value as 'df'. This value is fairly straightforward:
 * it is the amount of space in use on the device in bytes.
 *
 * This function is currently implemented on:
 *     Windows
 *     Linux/UNIX
 *     Mac
 *
 * \param p The path of a file/directory inside the desired device, or its mount point.
 * \return The used space on the device in bytes, or 0 on error.
 */
uint64_t CuteSyncSystemUtils::getDeviceUsed(const std::string &p)
{
	#ifdef _WIN32
		std::string path = p;

		// If we were given a file, get its parent directory.
		DWORD attribs = GetFileAttributes(path.c_str());
		if(!(attribs & FILE_ATTRIBUTE_DIRECTORY))
		{
			int i = static_cast<int>(p.find_last_of('\\'));
			path = path.substr(0, i);
		}

		ULARGE_INTEGER free, capacity;
		if(GetDiskFreeSpaceEx(path.c_str(), NULL, &capacity, &free))
			return static_cast<uint64_t>( capacity.QuadPart - free.QuadPart );
		else
			return 0;
	#else
		struct statvfs info;
		int r = statvfs(p.c_str(), &info);

		if(r != 0) return 0;

		return (
			( static_cast<uint64_t>(info.f_frsize) * static_cast<uint64_t>(info.f_blocks) ) -
			( static_cast<uint64_t>(info.f_frsize) * static_cast<uint64_t>(info.f_bfree) )
		);
	#endif
}

/*!
 * This is one of our disk space functions. It returns the available space on a device, indicated by the
 * given path. The path provided can be /any file or directory inside the device/, or the mount point
 * itself. Our return value may vary between operating systems even for the same device.
 *
 * On Windows, this function will return the same value as the "Free space:" line in the drive properties
 * dialog. This value may exclude some "extra privileged space" that IS included by getDeviceFree(),
 * although this is generally not the case. On windows, these two functions are typically the same.
 *
 * On Linux/UNIX/Mac, this function will return the same value as 'df'. According to the related man page,
 * this value is "Number of free blocks available to non-privileged process." That is, the capacity of a
 * disk is equal to used + available + (some extra privileged space).
 *
 * This function is currently implemented on:
 *     Windows
 *     Linux/UNIX
 *     Mac
 *
 * \param p The path of a file/directory inside the desired device, or its mount point.
 * \return The available space on the device in bytes, or 0 on error.
 */
uint64_t CuteSyncSystemUtils::getDeviceAvailable(const std::string &p)
{
	#ifdef _WIN32
		std::string path = p;

		// If we were given a file, get its parent directory.
		DWORD attribs = GetFileAttributes(path.c_str());
		if(!(attribs & FILE_ATTRIBUTE_DIRECTORY))
		{
			int i = static_cast<int>(p.find_last_of('\\'));
			path = path.substr(0, i);
		}

		ULARGE_INTEGER available;
		if(GetDiskFreeSpaceEx(path.c_str(), &available, NULL, NULL))
			return static_cast<uint64_t>(available.QuadPart);
		else
			return 0;
	#else
		struct statvfs info;
		int r = statvfs(p.c_str(), &info);

		if(r != 0) return 0;

		return (static_cast<uint64_t>(info.f_frsize) * static_cast<uint64_t>(info.f_bavail));
	#endif
}

/*!
 * This is one of our disk space functions. It returns the capacity of a device, indicated by the given path.
 * The path provided can be /any file or directory inside the device/, or the mount point itself. This
 * is the TOTAL capacity of the device, in bytes.
 *
 * This function is currently implemented on:
 *     Windows
 *     Linux/UNIX
 *     Mac
 *
 * \param p The path of a file/directory inside the desired device, or its mount point.
 * \return The capacity of the device in bytes, or 0 on error.
 */
uint64_t CuteSyncSystemUtils::getDeviceCapacity(const std::string &p)
{
	#ifdef _WIN32
		std::string path = p;

		// If we were given a file, get its parent directory.
		DWORD attribs = GetFileAttributes(path.c_str());
		if(!(attribs & FILE_ATTRIBUTE_DIRECTORY))
		{
			int i = static_cast<int>(p.find_last_of('\\'));
			path = path.substr(0, i);
		}

		ULARGE_INTEGER capacity;
		if(GetDiskFreeSpaceEx(path.c_str(), NULL, &capacity, NULL))
			return static_cast<uint64_t>(capacity.QuadPart);
		else
			return 0;
	#else
		struct statvfs info;
		int r = statvfs(p.c_str(), &info);

		if(r != 0) return 0;

		return (static_cast<uint64_t>(info.f_frsize ) * static_cast<uint64_t>(info.f_blocks));
	#endif
}

/*!
 * This is one of our disk space functions. It returns the percentage of space used on a device, indicated by
 * the given path. The path provided can be /any file or directory inside the device/, or the mount point
 * itself.
 *
 * On Windows, this function returns a percentage similar to what can be calculated from the drive properties
 * dialog. That is, the formula we use is: used space / (used space + available space).
 *
 * On Linux/UNIX/Mac, this function returns the same value as 'df'. This value isn't as straightforward as
 * just doing used space divided by capacity. It ignores the "extra privileged space" not included in
 * available, so the formula is (used space / (used space + available space)). The value returned by 'df',
 * more specifically, is ceil(getDeviceUsedPercent()).
 *
 * This function is currently implemented on:
 *     Windows
 *     Linux/UNIX
 *     Mac
 *
 * \param p The path of a file/directory inside the desired device, or its mount point.
 * \return The percentage of space used on the device, or 0.0 on error.
 */
double CuteSyncSystemUtils::getDeviceUsedPercent(const std::string &p)
{
	#ifdef _WIN32
		std::string path = p;

		// If we were given a file, get its parent directory.
		DWORD attribs = GetFileAttributes(path.c_str());
		if(!(attribs & FILE_ATTRIBUTE_DIRECTORY))
		{
			int i = static_cast<int>(p.find_last_of('\\'));
			path = path.substr(0, i);
		}

		ULARGE_INTEGER capacity, free, available;
		if(GetDiskFreeSpaceEx(path.c_str(), &available, &capacity, &free))
		{
			return (
				static_cast<double>( capacity.QuadPart - free.QuadPart ) /
				static_cast<double>( (capacity.QuadPart - free.QuadPart) + available.QuadPart )
			) * 100.0;
		}
		else
		{
			return 0.0;
		}
	#else
		struct statvfs info;
		int r = statvfs(p.c_str(), &info);

		if(r != 0) return 0.0;

		uint64_t used = ( static_cast<uint64_t>(info.f_frsize) * static_cast<uint64_t>(info.f_blocks) ) -
			( static_cast<uint64_t>(info.f_frsize) * static_cast<uint64_t>(info.f_bfree) );
		uint64_t available = static_cast<uint64_t>(info.f_frsize) * static_cast<uint64_t>(info.f_bavail);

		return (
			static_cast<double>( used ) /
			static_cast<double>( used + available )
		) * 100.0;
	#endif
}

/*!
 * This function returns the number of files present in a given directory. Symlinks (for UNIX-like platforms) are IGNORED - they
 * do not count as files, and they are not followed), while hidden files ARE counted.
 *
 * This function is currently implemented on:
 *     Windows
 *     Linux/UNIX
 *     Mac
 *
 * \param p The path of the directory to process.
 * \param r Whether or not we should search recursively.
 * \return The number of files inside the given path, or -1 if an error occurs.
 */
int64_t CuteSyncSystemUtils::getFileCount(const std::string &p, bool r)
{
	#ifdef _WIN32
		// Make sure the given path isn't too long.

		if( (p.length() + 3) > MAX_PATH )
			return -1;

		// Make sure we end with a terminator.

		std::string path = p;
		if(path.at(path.length() - 1) != '\\')
			path += '\\';

		std::string spath = path;
		spath += '*';

		// Calculate the result!

		PVOID wowfsr = NULL;
		Wow64DisableWow64FsRedirection(&wowfsr);

		WIN32_FIND_DATA entry;
		HANDLE fhandle = FindFirstFile(spath.c_str(), &entry);

		if(fhandle == INVALID_HANDLE_VALUE)
		{
			Wow64RevertWow64FsRedirection(wowfsr);
			return -1;
		}

		int64_t ret = CuteSyncSystemUtils::getFileCountRec(fhandle, path, entry, r);

		Wow64RevertWow64FsRedirection(wowfsr);
		FindClose(fhandle);

		return ret;
	#else
		// Setup our temporary variables.

		DIR *d = opendir(p.c_str());
		if(d == NULL)
			return -1;

		struct stat *s = new struct stat;

		// Process the path to ensure it is valid.

		std::string path = p;

		if(path.at(path.length() - 1) != '/')
			path += '/';

		// Calculate the result and clean up.

		int64_t ret = CuteSyncSystemUtils::getFileCountRec(d, path, s, r);

		closedir(d);
		delete s;

		return ret;
	#endif
}

#ifdef _WIN32
/*!
 * This function does the real work for the Windows version of getFileCount() by searching through a directory recursively.
 * This function only really makes sense on the Windows platform.
 *
 * \param h The current "find files" handle.
 * \param p The current full path.
 * \param e The current find data buffer.
 * \param r Whether or not we should recurse.
 * \return The current file total.
 */
int64_t CuteSyncSystemUtils::getFileCountRec(HANDLE h, const std::string &p, WIN32_FIND_DATA e, bool r)
{
	DWORD attr;
	int64_t ret = 0;

	// Keep looping through the entries in this directory.

	do
	{
		if( (strcmp(".", e.cFileName) == 0) || (strcmp("..", e.cFileName) == 0) )
			continue;

		std::string apath = p;
		apath += e.cFileName;

		attr = GetFileAttributes(apath.c_str());

		if(attr & FILE_ATTRIBUTE_DIRECTORY)
		{
			// We found a directory - recurse!

			if(r)
			{
				std::string dpath = apath;
				dpath += '\\';

				std::string spath = dpath;
				spath += '*';

				WIN32_FIND_DATA entry;
				HANDLE subdir = FindFirstFile(spath.c_str(), &entry);

				if(subdir == INVALID_HANDLE_VALUE)
					continue;

				ret += CuteSyncSystemUtils::getFileCountRec(subdir, dpath, entry, r);

				FindClose(subdir);
			}
		}
		else
		{
			// Just a file; increase the count.

			++ret;
		}
	} while(FindNextFile(h, &e));

	return ret;
}
#else
/*!
 * This function does the real work for the Linux/UNIX version of getFileCount() by searching through a directory recursively.
 * This function only makes sense on the Linux/UNIX platforms.
 *
 * \param d The current directory pointer.
 * \param p The current full path.
 * \param s Our temporary stat structure.
 * \param r Whether or not we should recurse.
 * \return The current file total.
 */
int64_t CuteSyncSystemUtils::getFileCountRec(DIR *d, const std::string &p, struct stat *s, bool r)
{
	struct dirent *entry = readdir(d);
	int64_t fcnt = 0;

	while(entry != NULL)
	{
		if( (strcmp(".", entry->d_name) == 0) || (strcmp("..", entry->d_name) == 0) )
		{
			entry = readdir(d);
			continue;
		}

		lstat( (p + entry->d_name).c_str(), s );

		if(S_ISREG(s->st_mode))
		{
			++fcnt;
		}
		else if(S_ISDIR(s->st_mode) && r)
		{
			DIR *subdir = opendir( (p + entry->d_name).c_str() );

			if(subdir != NULL)
				fcnt += CuteSyncSystemUtils::getFileCountRec(subdir, (p + entry->d_name + "/"), s, r);

			closedir(subdir);
		}

		entry = readdir(d);
	}

	return fcnt;
}
#endif
