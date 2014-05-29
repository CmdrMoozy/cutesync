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

#ifdef CUTESYNC_DEBUG
	#include <iostream>
#endif

#include <QApplication>
#include <QLocalSocket>

#include "libcute/Defines.h"
#include "cutesync/CuteSyncMainWindow.h"
#include "cutesync/settings/CuteSyncSettingsManager.h"

int main(int argc, char *argv[])
{
	/**************************************************************************************************
	 * We MUST declare our application BEFORE creating any QLocalSockets on Win32, or the application *
	 * segfaults. This is not the case, however, on *nix.                                             *
	 *                                                                                                *
	 * http://www.qtcentre.org/threads/43522-QLocalSocket-issue-difference-between-*nix-amp-win32     *
	 **************************************************************************************************/
	QApplication app(argc, argv);

	/*
	 * Test if another instance of CuteSync is already running. If so, we are just going to exit. The other
	 * process should notice our connection, and bring its window to the front appropriately.
	 */
	QLocalSocket s;
	s.connectToServer(CUTE_SYNC_APP_UUID);
	if(s.waitForConnected(2000))
	{
#ifdef CUTESYNC_DEBUG
		std::cout << "We were able to detect an already running instance of CuteSync. Terminating!\n";
#endif

		return 0;
	}

	CuteSyncSettingsManager settingsManager;

	CuteSyncMainWindow w(&settingsManager);
	w.show();

	return app.exec();
}
