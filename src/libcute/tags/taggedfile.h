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

#ifndef INCLUDE_LIBCUTE_TAGS_TAGGED_FILE_H
#define INCLUDE_LIBCUTE_TAGS_TAGGED_FILE_H

#include <cstdint>

#include <QString>
#include <QFileInfo>

#include <taglib/audioproperties.h>
#include <taglib/fileref.h>

extern "C" {
	#include <glib.h>
}

/*!
 * \brief This class is a wrapper that extends the functionality of TagLib.
 *
 * We provide extra functionality that vanilla TagLib doesn't offer (or only
 * offers with a large amount of confusing code to retrieve it) such as: file
 * information (size, extensions, paths, etc.) and extra tagging information
 * (track count, disc number/count, composer, etc.).
 */
class CSTaggedFile
{
	public:
		enum FileType
		{
			APE,       // TagLib::APE::File
			ASF,       // TagLib::ASF::File
			FLAC,      // TagLib::FLAC::File
			MP4,       // TagLib::MP4::File
			MPC,       // TagLib::MPC::File
			MPEG,      // TagLib::MPEG::File
			OggFLAC,   // TagLib::Ogg::FLAC::File
			OggSpeex,  // TagLib::Ogg::Speex::File
			OggVorbis, // TagLib::Ogg::Vorbis::File
			RIFFAIFF,  // TagLib::RIFF::AIFF::File
			RIFFWAV,   // TagLib::RIFF::WAV::File
			TrueAudio, // TagLib::TrueAudio::File
			WavPack,   // TagLib::WavPack::File
			Other,
			Invalid
		};

		CSTaggedFile(const QString &p,
			const TagLib::FileRef::FileTypeResolver &r,
			bool ap = true,
			TagLib::AudioProperties::ReadStyle aps =
			TagLib::AudioProperties::Average);
		virtual ~CSTaggedFile();

		bool isNull() const;
		bool hasAudioProperties() const;
		FileType getFileType() const;
		QString getFileExtension() const;

		QString getTitle() const;
		QString getArtist() const;
		QString getAlbum() const;
		QString getComment() const;
		QString getGenre() const;
		int getYear() const;
		int getTrackNumber() const;

		int getTrackLength() const;
		int getBitrate() const;
		int getSampleRate() const;
		int getChannels() const;

		QString getAbsoluteDir() const;
		QString getAbsolutePath() const;
		QString getFileName() const;
		QString getBaseName() const;
		QString getSuffix() const;
		QString getCompleteSuffix() const;
		uint64_t getSize() const;

		int getDiscNumber() const;
		int getDiscCount() const;
		int getTrackCount() const;
		QString getComposer() const;
		QString getCopyright() const;
		QString getURL() const;
		QString getEncodedBy() const;
		QString getOriginalArtist() const;
		QString getKeywords() const;
		QString getAlbumArtist() const;

		gpointer getCoverArtwork() const;

	private:
		TagLib::File *file;
		QFileInfo *info;
};

#endif
