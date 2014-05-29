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

#ifndef INCLUDE_LIBCUTE_DEFINES_H
#define INCLUDE_LIBCUTE_DEFINES_H

	#define SERIALIZATION_VERSION QDataStream::Qt_4_0

	#define CUTE_SYNC_VERSION_MAJ 1
	#define CUTE_SYNC_VERSION_MIN 0
	#define CUTE_SYNC_VERSION_BUG 0

	// This can be used to uniquely identify our app in a guaranteed way.
	#define CUTE_SYNC_APP_UUID ("ce9aadd4-3947-486a-b267-28fbd8a9c137")

	/*
	 * Define our LUNUSED macro, which is used on win32 to suppress the
	 * "unreferenced local variable" warning (4101) in cases where we are
	 * very intentionally NOT referencing said variable.
	 */
	#ifndef LUNUSED
		#ifdef _WIN32
			#define LUNUSED(x) x;
		#else
			#define LUNUSED(x)
		#endif
	#endif

	#ifndef UNUSED
		#if defined(__GNUC__)
			#define UNUSED(x) UNUSED_ ## x __attribute__((unused))
		#else
			#define UNUSED(x)
		#endif
	#endif

#endif
