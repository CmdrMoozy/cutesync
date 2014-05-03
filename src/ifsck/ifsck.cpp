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
#include <iomanip>
#include <cstdlib>
#include <cstdint>

#include <QDirIterator>
#include <QString>
#include <QSet>
#include <QDir>

#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>

extern "C"
{
	#include <glib.h>
	#include <gpod-1.0/gpod/itdb.h>
}

int main(int argc, char *argv[])
{
	// Check that we were given some command-line arguments.

	if(argc != 2)
	{
		std::cout << "You must provide the path of an existing iPod to check.\n";
		return 1;
	}

	// Get an absolute mount point, because libgpod's get_mountpoint function is made of failure.

	QDir mp(QString::fromUtf8(argv[1]));
	QString mountPoint = mp.absolutePath();

	// Try loading the iTunes DB from the path given.

	GError *error = NULL;
	Itdb_iTunesDB *itdb = NULL;

	itdb = itdb_parse(argv[1], &error);

	if( (error != NULL) || (itdb == NULL) )
	{
		if(error != NULL)
		{
			std::cout << "Error while loading iPod from path: " << error->message << "\n";
			g_error_free(error);
		}
		else
		{
			std::cout << "Unknown error while loading iPod from path.\n";
		}

		if(itdb != NULL) itdb_free(itdb);
		return 1;
	}

	// Print out some device information.

	const Itdb_IpodInfo *itdbInfo = itdb_device_get_ipod_info(itdb->device);

	std::cout << "Successfully loaded iPod at " << mountPoint.toLatin1().data() << "\n";
	std::cout << "\tModel Number:  " << itdbInfo->model_number                                                   << "\n";
	std::cout << "\tGeneration:    " << itdb_info_get_ipod_generation_string(itdbInfo->ipod_generation)          << "\n";
	std::cout << "\tCapacity:      " << itdbInfo->capacity << " GiB"                                             << "\n";
	std::cout << "\tArtwork?       " << std::boolalpha << (bool)itdb_device_supports_artwork(itdb->device)       << "\n";
	std::cout << "\tChapter Image? " << std::boolalpha << (bool)itdb_device_supports_chapter_image(itdb->device) << "\n";
	std::cout << "\tPhotos?        " << std::boolalpha << (bool)itdb_device_supports_photo(itdb->device)         << "\n";
	std::cout << "\tPodcasts?      " << std::boolalpha << (bool)itdb_device_supports_podcast(itdb->device)       << "\n";
	std::cout << "\tVideos?        " << std::boolalpha << (bool)itdb_device_supports_video(itdb->device)         << "\n";

	// First thing's first: build a list of files on the iPod.

	QSet<QString> ipodFiles;
	mp.cd("iPod_Control");
	mp.cd("Music");
	QDirIterator walker(mp.absolutePath(), QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

	while(walker.hasNext())
	{
		walker.next();
		ipodFiles.insert(
			QDir::cleanPath(walker.fileInfo().absoluteFilePath()).replace(mountPoint, "")
				.replace(QDir::separator(), ":")
		);
	}

	// Next! Build a list of files from the iTunes DB.

	QSet<QString> ipodDBFiles;
	GList *trackList = g_list_first(itdb->tracks);
	while(trackList != NULL)
	{
		Itdb_Track *t = static_cast<Itdb_Track *>(trackList->data);
		trackList = trackList->next;
		ipodDBFiles.insert(QString::fromUtf8(t->ipod_path));
	}

	// Now, to start checking things: find any files that are on the device, but not in the database.

	std::cout << "\n\nChecking for orphaned files...\n";

	QSet<QString> difference = ipodFiles;
	difference -= ipodDBFiles;

	if(difference.count() > 0)
	{
		QSetIterator<QString> ofIter(difference);
		while(ofIter.hasNext())
			std::cout << "\t" << ofIter.next().toUtf8().data() << "\n";
	}
	else
	{
		std::cout << "\tNONE! :-)\n";
	}

	#ifdef __GNUC__
		#warning TODO - Delete these files
	#endif

	// Next, check the reverse: things that are in the database, but are not present on the device.

	std::cout << "\n\nChecking for orphaned database entries...\n";

	difference = ipodDBFiles;
	difference -= ipodFiles;

	if(difference.count() > 0)
	{
		QSetIterator<QString> odIter(difference);
		while(odIter.hasNext())
			std::cout << "\t" << odIter.next().toUtf8().data() << "\n";
	}
	else
	{
		std::cout << "\tNONE! :-)\n";
	}

	#ifdef __GNUC__
		#warning TODO - Prompt the user for repairs instead of proceeding automatically
	#endif

	// Remove these orphaned database entries from the database.

	trackList = g_list_first(itdb->tracks);
	while(trackList != NULL)
	{
		Itdb_Track *t = static_cast<Itdb_Track *>(trackList->data);
		trackList = trackList->next;

		// Search for tracks that need to be removed from the database.

		if(difference.contains(QString::fromUtf8(t->ipod_path)))
		{
			// Remove the track's thumbnails, if any.

			itdb_track_remove_thumbnails(t);

			// Remove the track from any playlists it is a part of.

			GList *playlistList = g_list_first(itdb->playlists);
			while(playlistList != NULL)
			{
				itdb_playlist_remove_track(static_cast<Itdb_Playlist *>(playlistList->data), t);
				playlistList = playlistList->next;
			}

			// Remove the track from the database.

			itdb_track_remove(t);
		}
	}

	// Next, we want to see if there are any tracks in the iTunes DB that are not in the MPL.

	#ifdef __GNUC__
		#warning TODO - This should only check for tracks that SHOULD be in the MPL ie not podcasts
	#endif

	std::cout << "\n\nChecking for MPL inconsistencies...\n";
	uint64_t mplc;

	trackList = g_list_first(itdb->tracks);
	mplc = 0;
	while(trackList != NULL)
	{
		if(!itdb_playlist_contains_track( itdb_playlist_mpl(itdb), static_cast<Itdb_Track *>(trackList->data) ))
		{
			std::cout << "\t" << static_cast<Itdb_Track *>(trackList->data)->ipod_path << "\n";
			++mplc;
		}

		trackList = trackList->next;
	}

	if(mplc == 0)
		std::cout << "\tNONE! :-)\n";

	#ifdef __GNUC__
		#warning TODO - Add these track to the MPL
	#endif

	// Save the iTunes DB to the device.

	if(!itdb_write(itdb, &error))
	{
		if(error != NULL)
		{
			std::cout << "Error writing iTunes DB: " << error->message << "\n";

			g_error_free(error);
			error = NULL;
			return 1;
		}
		else
		{
			std::cout << "Unknown error writing iTunes DB.\n";

			return 1;
		}
	}

	// Clean up & exit.

	itdb_free(itdb);
	return 0;
}
