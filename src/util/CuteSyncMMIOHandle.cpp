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

#include "CuteSyncMMIOHandle.h"

#ifndef _WIN32
	#include <cstdlib>
	#include <cstdio>

	namespace MMAP
	{
		extern "C"
		{
			#include <sys/mman.h>
			#include <fcntl.h>
		}
	}
#endif

/*!
 * This is our default constructor, which creates a new MMIO handle with the given (optional) file path.
 *
 * \param p The path to the file you want to work with, or "".
 */
CuteSyncMMIOHandle::CuteSyncMMIOHandle(const std::string &p)
	: path(p)
{
	#ifdef _WIN32
		fileHandle = NULL;
		mmioHandle = NULL;
		view = NULL;
	#else
		fstats = NULL;
		view = NULL;
	#endif
}

/*!
 * This is our default destructor, which flushes any outstanding I/O operations (if any), closes all of our
 * file descriptors, and then destroys our object.
 */
CuteSyncMMIOHandle::~CuteSyncMMIOHandle()
{
	close();
}

/*!
 * This function returns the path that our handle is currently working with.
 *
 * \return Our current path.
 */
std::string CuteSyncMMIOHandle::getPath() const
{
	return path;
}

/*!
 * This function sets the path we are currently working with. Note that, if we are already open, this function
 * takes no action - it doesn't make sense for our path to be different than the file we actually have open.
 *
 * \param p The new path to work with.
 */
void CuteSyncMMIOHandle::setPath(const std::string &p)
{
	if(!isOpen())
		path = p;
}

/*!
 * This function returns our current open mode. Note that if our handle is not currently open, this value is
 * meaningless and arbitrary. This value is, likewise, set via the open() function.
 *
 * \return Our current open mode.
 */
CuteSyncMMIOHandle::OpenMode CuteSyncMMIOHandle::getMode() const
{
	return mode;
}

/*!
 * This function opens our handle's current path with the given open mode, optionally creating a new file to work with of a
 * specified size.
 *
 * Note that the file creation parameters are only used if the open mode is ReadWrite. It doesn't make sense why one would
 * want to create a new, empty file of some given size and then only be able to /read/ its contents. Likewise, these  options
 * are very useful when writing a file, because by definition MMIO can only write to existing offsets in a file; it will
 * not "append" to the file, increasing its size. If you open in ReadWrite mode and don't provide these parameters, then the
 * file will be opened as-is, and its size and contents will not be messed with.
 *
 * Our behavior when told to create a file of a given size, if it already exists, is to simply truncate the existing file and
 * then updating its size to the size specified.
 *
 * It should also be noted that opening empty files will fail; a file must contain at least one byte for us to map to.
 *
 * It should ALSO be noted that, on Windows, some Win32 API functions are really finnicky about filenames and/or paths. They
 * are sometimes CASE SENSITIVE even, so you should make sure to canonicalize the path you give us before calling this, or
 * it may fail unexpectedly.
 *
 * \param m The mode to open our path with.
 * \param c Whether or not to create a new file (only for ReadWrite). This is false by default.
 * \param s The size of the new file to be created, in bytes. This MUST be specified if c is true, and it must be > 0.
 * \return True on success, or false on failure.
 */
bool CuteSyncMMIOHandle::open(CuteSyncMMIOHandle::OpenMode m, bool c, uint64_t s)
{
	mode = m;
	if(getPath().empty()) return false;
	if(isOpen()) return false;

	#ifdef _WIN32

		// Open the file handle.

		DWORD fAccess;
		switch(getMode())
		{
			case ReadOnly:
				fAccess = GENERIC_READ;
				break;

			case ReadWrite:
				fAccess = GENERIC_READ | GENERIC_WRITE;
				break;
		};

		fileHandle = CreateFile(getPath().c_str(), fAccess, 0, NULL, (c ? CREATE_NEW | TRUNCATE_EXISTING : OPEN_EXISTING),
			FILE_ATTRIBUTE_NORMAL, NULL);
		if(fileHandle == INVALID_HANDLE_VALUE)
		{
			fileHandle = NULL;
			return false;
		}

		if(c)
		{
			if(s < 1)
			{
				CloseHandle(fileHandle);
				fileHandle = NULL;

				return false;
			}

			char *buf = new char[1048576];
			for(int i = 0; i < 1048576; ++i) buf[i] = 0x00;

			uint64_t w = s;
			DWORD written;

			while(w > 0)
			{
				if(!WriteFile(fileHandle, buf, static_cast<DWORD>(w % 1048576), &written, NULL))
				{
					CloseHandle(fileHandle);
					fileHandle = NULL;

					delete[] buf;
					return false;
				}

				w = (w >= 1048576 ? w - 1048576 : 0);
			}

			delete[] buf;
		}

		LARGE_INTEGER i;
		GetFileSizeEx(fileHandle, &i);
		if(i.QuadPart <= 0)
		{
			CloseHandle(fileHandle);
			fileHandle = NULL;

			return false;
		}

		// Create our file mapping.

		DWORD mAccess;
		switch(getMode())
		{
			case ReadOnly:
				mAccess = PAGE_READONLY;
				break;

			case ReadWrite:
				mAccess = PAGE_READWRITE;
				break;
		};

		mmioHandle = CreateFileMapping(fileHandle, NULL, mAccess, 0, 0, NULL);
		if(CuteSyncMMIOHandle == NULL)
		{
			CloseHandle(fileHandle);
			fileHandle = NULL;

			return false;
		}

		// Open our file view.

		DWORD vAccess;
		switch(getMode())
		{
			case ReadOnly:
				vAccess = FILE_MAP_READ;
				break;

			case ReadWrite:
				vAccess = FILE_MAP_WRITE;
				break;
		};

		view = (char *) MapViewOfFile(mmioHandle, vAccess, 0, 0, 0);
		if(view == NULL)
		{
			CloseHandle(mmioHandle);
			mmioHandle = NULL;

			CloseHandle(fileHandle);
			fileHandle = NULL;

			return false;
		}

		return true;

	#else

		// Open the file handle.

		int fd;

		int fFlags;
		switch(getMode())
		{
			case ReadOnly:
				fFlags = O_RDONLY;
				break;

			case ReadWrite:
				fFlags = O_RDWR | (c ? O_CREAT | O_TRUNC : 0);
				break;
		};

		fd = MMAP::open(getPath().c_str(), fFlags, (mode_t) S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if(fd == -1)
			return false;

		if(c)
		{
			if(s <= 1)
			{
				MMAP::close(fd);
				return false;
			}

			if(!MMAP::lseek(fd, s-1, SEEK_SET))
			{
				MMAP::close(fd);
				return false;
			}

			if(!MMAP::write(fd, "", 1))
			{
				MMAP::close(fd);
				return false;
			}
		}

		// Get our file stats.

		fstats = new MMAP::FileStats;
		if(MMAP::fstat(fd, fstats) != 0)
		{
			delete fstats;
			fstats = NULL;

			MMAP::close(fd);

			return false;
		}

		if(fstats->st_size <= 0)
		{
			delete fstats;
			fstats = NULL;

			MMAP::close(fd);

			return false;
		}

		// Create our file map.

		int mFlags = 0;
		switch(getMode())
		{
			case ReadOnly:
				mFlags = PROT_READ;
				break;

			case ReadWrite:
				mFlags = PROT_READ | PROT_WRITE;
				break;
		}

		view = (char *) MMAP::mmap(NULL, fstats->st_size, mFlags, MAP_SHARED, fd, 0);
		if(view == MAP_FAILED)
		{
			delete fstats;
			fstats = NULL;

			MMAP::close(fd);

			view = NULL;
			return false;
		}

		MMAP::close(fd);
		return true;

	#endif
}

/*!
 * This function tests if our handle is currently open, and as such is ready to be used.
 *
 * \return True if we are open, or false otherwise.
 */
bool CuteSyncMMIOHandle::isOpen() const
{
	#ifdef _WIN32
		return (view != NULL);
	#else
		return ( (fstats != NULL) && (view != NULL) );
	#endif
}

/*!
 * This function flushes any outstanding I/O operations to the disk. Note that, on platforms that DO NOT use buffered
 * I/O for MMIO (i.e., Linux/UNIX/Mac), this function does nothing. Also note that this function similarly does nothing
 * if our handle is not open.
 */
void CuteSyncMMIOHandle::flush()
{
	if(isOpen())
	{
		#ifdef _WIN32
			FlushViewOfFile(view, 0);
		#else
			// Linux/UNIX don't buffer memory-mapped IO; nothing to flush.
		#endif
	}
}

/*!
 * This function flushes any outstanding I/O operations to the disk and then closes our MMIO handle. Note that if we are
 * not already open, then no action is taken.
 */
void CuteSyncMMIOHandle::close()
{
	flush();
	if(isOpen())
	{
		#ifdef _WIN32
			UnmapViewOfFile(view);
			view = NULL;

			CloseHandle(CuteSyncMMIOHandle);
			CuteSyncMMIOHandle = NULL;

			CloseHandle(fileHandle);
			fileHandle = NULL;
		#else
			MMAP::munmap(view, fstats->st_size);
			view = NULL;

			delete fstats;
			fstats = NULL;
		#endif
	}
}

/*!
 * This function returns the length of the file we are currently working with, in bytes. Note that if we don't
 * currently have a file opened, or we are unable to retrieve the file's size for some reason, 0 is returned
 * instead.
 *
 * \return The size of our current file.
 */
uint64_t CuteSyncMMIOHandle::getLength() const
{
	if(isOpen())
	{
		#ifdef _WIN32
			LARGE_INTEGER i;
			if(!GetFileSizeEx(fileHandle, &i))
				return 0;

			return static_cast<uint64_t>(i.QuadPart);
		#else
			return static_cast<uint64_t>(fstats->st_size);
		#endif
	}

	return 0;
}

/*!
 * This is one of our read functions, which retrieves a single byte from our file at the given offset. If we
 * don't have a file opened, a null-terminator ('\0') is returned instead. Also note that we don't do any
 * bounds checking; it is up to the caller to make sure that 0 <= o < filesize.
 *
 * \param o The offset of the desired byte.
 * \return The desired byte, or a null-terminator if an error occurs.
 */
char CuteSyncMMIOHandle::at(uint64_t o) const
{
	if(isOpen())
	{
		#ifdef _WIN32
			return view[o];
		#else
			return view[o];
		#endif
	}

	return '\0';
}

/*!
 * This is another one of our read functions. It is similar to our single-byte at() function, except it grabs
 * a whole series of bytes from the file, and places them in a given buffer. If we don't have a file opened,
 * the buffer is filled with null-terminators ('\0'), and false is returned. Also note that we don't do any
 * bounds checking; it is up to the caller to make sure that the offsets in our file are in-bounds, and that
 * the buffer provided is large enough to store the amount of data requested.
 *
 * \param o The offset in the file to start at.
 * \param b The buffer to store the data in.
 * \param l The length of the desired data, in bytes.
 * \return True on success, or false on failure.
 */
bool CuteSyncMMIOHandle::at(uint64_t o, char *b, uint64_t l) const
{
	if(isOpen())
	{
		#ifdef _WIN32
			for(uint64_t i = o; (i-o) < l; ++i)
				b[i-o] = view[i];

			return true;
		#else
			for(uint64_t i = o; (i-o) < l; ++i)
				b[i-o] = view[i];

			return true;
		#endif
	}

	for(uint64_t i = 0; i < l; ++i)
		b[i] = '\0';

	return false;
}

/*!
 * This is one of our write functions. It sets the byte at a given offset in our current file to the given value.
 * Note that if we don't have a file opened in ReadWrite mode, we don't write anything and instead just return
 * failure. Also note that we don't do bounds checking; it is up to the caller to make sure that 0 <= o < filesize.
 *
 * \param o The offset of the desired byte.
 * \param c The new value for the specified byte.
 * \return True on success, or false on failure.
 */
bool CuteSyncMMIOHandle::set(uint64_t o, char c)
{
	if( isOpen() && (getMode() == CuteSyncMMIOHandle::ReadWrite) )
	{
		#ifdef _WIN32
			view[o] = c;
			return true;
		#else
			view[o] = c;
			return true;
		#endif
	}

	return false;
}

/*!
 * This is another one of our write functions. It is similar to our single-byte set() function, except we set a whole
 * series of bytes using data from the given buffer. Note that if we don't have a file opened in ReadWrite mode, we
 * don't write anything and instead just return failure. Also note that we don't do bounds checking; it is up to the
 * caller to make sure that the given offsets in our file are in-bounds, and that the buffer given actually contains
 * the amount of data specified.
 *
 * \param o The offset to start writing at.
 * \param b The buffer storing the new data.
 * \param l The length of the data to write, in bytes.
 * \return True on success, or false on failure.
 */
bool CuteSyncMMIOHandle::set(uint64_t o, const char *c, uint64_t l)
{
	if( isOpen() && (getMode() == CuteSyncMMIOHandle::ReadWrite) )
	{
		#ifdef _WIN32
			for(uint64_t i = o; (i-o) < l; ++i)
				view[i] = c[i-o];

			return true;
		#else
			for(uint64_t i = o; (i-o) < l; ++i)
				view[i] = c[i-o];

			return true;
		#endif
	}

	return false;
}

