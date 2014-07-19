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

#include "taggedfile.h"

#include <QFileInfo>

#include <taglib/apefile.h>
#include <taglib/asffile.h>
#include <taglib/flacfile.h>
#include <taglib/mp4file.h>
#include <taglib/mpcfile.h>
#include <taglib/mpegfile.h>
#include <taglib/oggflacfile.h>
#include <taglib/speexfile.h>
#include <taglib/vorbisfile.h>
#include <taglib/aifffile.h>
#include <taglib/wavfile.h>
#include <taglib/trueaudiofile.h>
#include <taglib/wavpackfile.h>

extern "C" {
	#include <gio/gio.h>
	#include <gdk-pixbuf/gdk-pixbuf.h>
}

/*!
 * This is our default constructor, which creates a new object for us.
 *
 * \param p The path to the file we are supposed to load.
 * \param r The file type resolver that will create the file for us.
 * \param ap Whether or not we should read audio properties.
 * \param aps How accurate (but slow) audio property reading should be.
 */
CSTaggedFile::CSTaggedFile(const QString &p, const TagLib::FileRef::FileTypeResolver &r, bool ap, TagLib::AudioProperties::ReadStyle aps)
	: file(NULL), info(NULL)
{
	file = r.createFile(p.toUtf8().data(), ap, aps);
	if(file != NULL)
		info = new QFileInfo(p);
}

/*!
 * This is our default destructor, which cleans up & destroys our object.
 */
CSTaggedFile::~CSTaggedFile()
{
	if(info != NULL) delete info;
	if(file != NULL) delete file;
}

/*!
 * This function tests whether our file is NULL. If it is indeed NULL, then either the path you
 * provided was invalid, or it was a file type your resolver didn't recognize.
 *
 * \return True if we are NULL, or false otherwise.
 */
bool CSTaggedFile::isNull() const
{
	return (file == NULL);
}

/*!
 * This function tests whether or not our loaded file has any audio properties associated with it.
 *
 * \return True if we have audio properties, or false otherwise.
 */
bool CSTaggedFile::hasAudioProperties() const
{
	if(isNull()) return false;
	return (file->audioProperties() != NULL);
}

/*!
 * This function returns what type of file we have loaded. By default this class supports any file type
 * that TagLib supports, but you can subclass us to extend that support. Note that if we are NULL, then a
 * type of Invalid is returned. If we loaded the file successfully but it isn't a subclass we recognize,
 * then a type of Other is returned.
 *
 * \return The type of file we are referencing.
 */
CSTaggedFile::FileType CSTaggedFile::getFileType() const
{
	if(isNull()) return CSTaggedFile::Invalid;

	if(dynamic_cast<TagLib::APE::File *>(file) != NULL)
		return CSTaggedFile::APE;
	else if(dynamic_cast<TagLib::ASF::File *>(file) != NULL)
		return CSTaggedFile::ASF;
	else if(dynamic_cast<TagLib::FLAC::File *>(file) != NULL)
		return CSTaggedFile::FLAC;
	else if(dynamic_cast<TagLib::MP4::File *>(file) != NULL)
		return CSTaggedFile::MP4;
	else if(dynamic_cast<TagLib::MPC::File *>(file) != NULL)
		return CSTaggedFile::MPC;
	else if(dynamic_cast<TagLib::MPEG::File *>(file) != NULL)
		return CSTaggedFile::MPEG;
	else if(dynamic_cast<TagLib::Ogg::FLAC::File *>(file) != NULL)
		return CSTaggedFile::OggFLAC;
	else if(dynamic_cast<TagLib::Ogg::Speex::File *>(file) != NULL)
		return CSTaggedFile::OggSpeex;
	else if(dynamic_cast<TagLib::Ogg::Vorbis::File *>(file) != NULL)
		return CSTaggedFile::OggVorbis;
	else if(dynamic_cast<TagLib::RIFF::AIFF::File *>(file) != NULL)
		return CSTaggedFile::RIFFAIFF;
	else if(dynamic_cast<TagLib::RIFF::WAV::File *>(file) != NULL)
		return CSTaggedFile::RIFFWAV;
	else if(dynamic_cast<TagLib::TrueAudio::File *>(file) != NULL)
		return CSTaggedFile::TrueAudio;
	else if(dynamic_cast<TagLib::WavPack::File *>(file) != NULL)
		return CSTaggedFile::WavPack;

	return CSTaggedFile::Other;
}

/*!
 * This is a convenience function which returns the appropriate file extension for our current file type.
 *
 * \return Our file extension.
 */
QString CSTaggedFile::getFileExtension() const
{
	switch(getFileType())
	{
		case CSTaggedFile::APE: return "ape";
		case CSTaggedFile::ASF: return "asf";
		case CSTaggedFile::FLAC: return "flac";
		case CSTaggedFile::MP4: return "m4a";
		case CSTaggedFile::MPC: return "mpc";
		case CSTaggedFile::MPEG: return "mp3";
		case CSTaggedFile::OggFLAC: return "oga";
		case CSTaggedFile::OggSpeex: return "spx";
		case CSTaggedFile::OggVorbis: return "ogg";
		case CSTaggedFile::RIFFAIFF: return "aiff";
		case CSTaggedFile::RIFFWAV: return "riff";
		case CSTaggedFile::TrueAudio: return "tta";
		case CSTaggedFile::WavPack: return "wv";

		default: return "";
	};
}

/*!
 * This is one of our retrieval functions that provide the same interface as TagLib::Tag.
 * If we are NULL, then QString() is returned instead. Note that the string returned will
 * be UTF-8.
 *
 * \return Our file's title.
 */
QString CSTaggedFile::getTitle() const
{
	if(isNull()) return QString();
	return QString::fromUtf8(file->tag()->title().toCString(true));
}

/*!
 * This is one of our retrieval functions that provide the same interface as TagLib::Tag.
 * If we are NULL, then QString() is returned instead. Note that the string returned will
 * be UTF-8.
 *
 * \return Our file's artist.
 */
QString CSTaggedFile::getArtist() const
{
	if(isNull()) return QString();
	return QString::fromUtf8(file->tag()->artist().toCString(true));
}

/*!
 * This is one of our retrieval functions that provide the same interface as TagLib::Tag.
 * If we are NULL, then QString() is returned instead. Note that the string returned will
 * be UTF-8.
 *
 * \return Our file's album.
 */
QString CSTaggedFile::getAlbum() const
{
	if(isNull()) return QString();
	return QString::fromUtf8(file->tag()->album().toCString(true));
}

/*!
 * This is one of our retrieval functions that provide the same interface as TagLib::Tag.
 * If we are NULL, then QString() is returned instead. Note that the string returned will
 * be UTF-8.
 *
 * \return Our file's comment.
 */
QString CSTaggedFile::getComment() const
{
	if(isNull()) return QString();
	return QString::fromUtf8(file->tag()->comment().toCString(true));
}

/*!
 * This is one of our retrieval functions that provide the same interface as TagLib::Tag.
 * If we are NULL, then QString() is returned instead. Note that the string returned will
 * be UTF-8.
 *
 * \return Our file's genre.
 */
QString CSTaggedFile::getGenre() const
{
	if(isNull()) return QString();
	return QString::fromUtf8(file->tag()->genre().toCString(true));
}

/*!
 * This is one of our retrieval functions that provide the same interface as TagLib::Tag.
 * If we are NULL, then 0 is returned instead.
 *
 * \return Our track's year.
 */
int CSTaggedFile::getYear() const
{
	if(isNull()) return 0;
	return static_cast<int>(file->tag()->year());
}

/*!
 * This is one of our retrieval functions that provide the same interface as TagLib::Tag.
 * If we are NULL, then 0 is returned instead.
 *
 * \return Our track's track number.
 */
int CSTaggedFile::getTrackNumber() const
{
	if(isNull()) return 0;
	return static_cast<int>(file->tag()->track());
}

/*!
 * This is one of our retrieval functions that provide the same interface as TagLib::AudioProperties.
 * If we are NULL, then 0 is returned instead.
 *
 * \return Our track's length, in SECONDS.
 */
int CSTaggedFile::getTrackLength() const
{
	if(isNull() || (!hasAudioProperties())) return 0;
	return static_cast<int>(file->audioProperties()->length());
}

/*!
 * This is one of our retrieval functions that provide the same interface as TagLib::AudioProperties.
 * If we are NULL, then 0 is returned instead.
 *
 * \return Our track's bitrate.
 */
int CSTaggedFile::getBitrate() const
{
	if(isNull() || (!hasAudioProperties())) return 0;
	return static_cast<int>(file->audioProperties()->bitrate());
}

/*!
 * This is one of our retrieval functions that provide the same interface as TagLib::AudioProperties.
 * If we are NULL, then 0 is returned instead.
 *
 * \return Our track's sample rate.
 */
int CSTaggedFile::getSampleRate() const
{
	if(isNull() || (!hasAudioProperties())) return 0;
	return static_cast<int>(file->audioProperties()->sampleRate());
}

/*!
 * This is one of our retrieval functions that provide the same interface as TagLib::AudioProperties.
 * If we are NULL, then 0 is returned instead.
 *
 * \return Our track's number of channels.
 */
int CSTaggedFile::getChannels() const
{
	if(isNull() || (!hasAudioProperties())) return 0;
	return static_cast<int>(file->audioProperties()->channels());
}

/*!
 * This is one of our retrieval functions that provide information about the file itself that isn't
 * normally accessible via vanilla TagLib. If we are NULL, then QString() is returned instead.
 *
 * \return The absolute path to the directory our file is in.
 */
QString CSTaggedFile::getAbsoluteDir() const
{
	if(isNull()) return QString();
	return info->absolutePath();
}

/*!
 * This is one of our retrieval functions that provide information about the file itself that isn't
 * normally accessible via vanilla TagLib. If we are NULL, then QString() is returned instead.
 *
 * \return The absolute path to the file itself.
 */
QString CSTaggedFile::getAbsolutePath() const
{
	if(isNull()) return QString();
	return info->absoluteFilePath();
}

/*!
 * This is one of our retrieval functions that provide information about the file itself that isn't
 * normally accessible via vanilla TagLib. If we are NULL, then QString() is returned instead.
 *
 * \return The filename portion of our file's path, including extensions.
 */
QString CSTaggedFile::getFileName() const
{
	if(isNull()) return QString();
	return info->fileName();
}

/*!
 * This is one of our retrieval functions that provide information about the file itself that isn't
 * normally accessible via vanilla TagLib. If we are NULL, then QString() is returned instead.
 *
 * \return The filename portion of our file's path, EXCLUDING extensions.
 */
QString CSTaggedFile::getBaseName() const
{
	if(isNull()) return QString();
	return info->baseName();
}

/*!
 * This is one of our retrieval functions that provide information about the file itself that isn't
 * normally accessible via vanilla TagLib. If we are NULL, then QString() is returned instead.
 *
 * \return The right-most file extension of our file.
 */
QString CSTaggedFile::getSuffix() const
{
	if(isNull()) return QString();
	return info->suffix();
}

/*!
 * This is one of our retrieval functions that provide information about the file itself that isn't
 * normally accessible via vanilla TagLib. If we are NULL, then QString() is returned instead.
 *
 * \return Our file's complete file extension (everything after the first '.').
 */
QString CSTaggedFile::getCompleteSuffix() const
{
	if(isNull()) return QString();
	return info->completeSuffix();
}

/*!
 * This is one of our retrieval functions that provide information about the file itself that isn't
 * normally accessible via vanilla TagLib. If we are NULL, then 0 is returned instead.
 *
 * \return The size of our file, in BYTES.
 */
uint64_t CSTaggedFile::getSize() const
{
	if(isNull()) return 0;
	return static_cast<uint64_t>(info->size());
}

/*!
 * This is one of our retrieval functions that gather information that is technically available via TagLib,
 * but that is hard to get to because it isn't available via the common TagLib::Tag API for all formats.
 * Because of this, not all formats are supported -- if you need to support a weird format, then you should
 * subclass us and override this function. If we are NULL, then 0 is returned instead.
 *
 * Formats currently supported:
 *     - MP4 (i.e., AAC/ALAC)
 *     - MPEG (i.e., MP3/etc.)
 *
 * \return The disc number this track is on in its set.
 */
int CSTaggedFile::getDiscNumber() const
{
	if(isNull()) return 0;

	switch(getFileType())
	{
		case CSTaggedFile::MP4:
			{
				// Try casting the file pointer.
				TagLib::MP4::File *sf = dynamic_cast<TagLib::MP4::File *>(file);
				if(sf == NULL) return 0;

				// Try retrieving its format-specific tags.
				TagLib::MP4::Tag *tag = dynamic_cast<TagLib::MP4::Tag *>(sf->tag());
				if(tag == NULL) return 0;

				// Grab the disc number.
				if(tag->itemListMap().contains("disk"))
					return tag->itemListMap()["disk"].toIntPair().first;
				else
					return 0;
			}

		case CSTaggedFile::MPEG:
			{
				// Setup our variables and make sure everything is kosher.

				int cdn;
				bool ok;
				TagLib::MPEG::File *sf = dynamic_cast<TagLib::MPEG::File *>(file);

				if(sf == NULL) return 0;
				if(sf->ID3v2Tag() == NULL) return 0;

				// Retrieve the tag frame we want - TPOS.

				TagLib::ID3v2::FrameList framelist = sf->ID3v2Tag()->frameListMap()["TPOS"];
				if(framelist.isEmpty()) return 0;

				QString s = QString::fromUtf8(framelist.front()->toString().toCString(true));

				// Look for a '/' character in the tag frame.

				int pos = -1;
				for(int i = 0; i < s.length(); ++i)
				{
					if(s[i] == '/')
					{
						pos = i;
						break;
					}
				}

				// Process the tag.

				if(pos != -1)
				{
					// We found a '/', so the tag is in the format "CD Number/CD Count".
					cdn = s.left(pos).toInt(&ok);
					if(!ok) return 0;
				}
				else
				{
					// No '/', so the tag is assumed to represent solely the CD number.
					cdn = s.toInt(&ok);
					if(!ok) return 0;
				}

				// Return our result.

				return cdn;
			}

		default:
			return 0;
	}
}

/*!
 * This is one of our retrieval functions that gather information that is technically available via TagLib,
 * but that is hard to get to because it isn't available via the common TagLib::Tag API for all formats.
 * Because of this, not all formats are supported -- if you need to support a weird format, then you should
 * subclass us and override this function. If we are NULL, then 0 is returned instead.
 *
 * Formats currently supported:
 *     - MP4 (i.e., AAC/ALAC)
 *     - MPEG (i.e., MP3/etc.)
 *
 * \return The number of discs in our set.
 */
int CSTaggedFile::getDiscCount() const
{
	if(isNull()) return 0;

	switch(getFileType())
	{
		case CSTaggedFile::MP4:
			{
				// Try casting the file pointer.
				TagLib::MP4::File *sf = dynamic_cast<TagLib::MP4::File *>(file);
				if(sf == NULL) return 0;

				// Try retrieving its format-specific tags.
				TagLib::MP4::Tag *tag = dynamic_cast<TagLib::MP4::Tag *>(sf->tag());
				if(tag == NULL) return 0;

				// Grab the disc count.
				if(tag->itemListMap().contains("disk"))
					return tag->itemListMap()["disk"].toIntPair().second;
				else
					return 0;
			}
			break;

		case CSTaggedFile::MPEG:
			{
				// Setup our variables and make sure everything is kosher.

				int cdc;
				bool ok;
				TagLib::MPEG::File *sf = dynamic_cast<TagLib::MPEG::File *>(file);

				if(sf == NULL) return 0;
				if(sf->ID3v2Tag() == NULL) return 0;

				// Retrieve the tag frame we want - TPOS.

				TagLib::ID3v2::FrameList framelist = sf->ID3v2Tag()->frameListMap()["TPOS"];
				if(framelist.isEmpty()) return 0;

				QString s = QString::fromUtf8(framelist.front()->toString().toCString(true));

				// Look for a '/' character in the tag frame.

				int pos = -1;
				for(int i = 0; i < s.length(); ++i)
				{
					if(s[i] == '/')
					{
						pos = i;
						break;
					}
				}

				// Process the tag.

				if(pos != -1)
				{
					// We found a '/', so the tag is in the format "CD Number/CD Count".
					cdc = s.right(s.length() - (pos+1)).toInt(&ok);
					if(!ok) return 0;
				}
				else
				{
					// No '/', so the tag is assumed to represent solely the CD number - nothing to return.
					return 0;
				}

				// Return our result.

				return cdc;
			}
			break;

		case CSTaggedFile::RIFFAIFF:
			{
				return 0;
			}
			break;

		default:
			return 0;
	}
}

/*!
 * This is one of our retrieval functions that gather information that is technically available via TagLib,
 * but that is hard to get to because it isn't available via the common TagLib::Tag API for all formats.
 * Because of this, not all formats are supported -- if you need to support a weird format, then you should
 * subclass us and override this function. If we are NULL, then 0 is returned instead.
 *
 * Formats currently supported:
 *     - MP4 (i.e., AAC/ALAC)
 *     - MPEG (i.e., MP3/etc.)
 *
 * \return The number of tracks on this track's disc.
 */
int CSTaggedFile::getTrackCount() const
{
	if(isNull()) return 0;

	switch(getFileType())
	{
		case CSTaggedFile::MP4:
			{
				// Try casting the file pointer.
				TagLib::MP4::File *sf = dynamic_cast<TagLib::MP4::File *>(file);
				if(sf == NULL) return 0;

				// Try retrieving its format-specific tags.
				TagLib::MP4::Tag *tag = dynamic_cast<TagLib::MP4::Tag *>(sf->tag());
				if(tag == NULL) return 0;

				// Grab the track count.
				if(tag->itemListMap().contains("trkn"))
					return tag->itemListMap()["trkn"].toIntPair().second;
				else
					return 0;
			}
			break;

		case CSTaggedFile::MPEG:
			{
				// Setup our variables and make sure everything is kosher.

				int tc;
				bool ok;
				TagLib::MPEG::File *sf = dynamic_cast<TagLib::MPEG::File *>(file);

				if(sf == NULL) return 0;
				if(sf->ID3v2Tag() == NULL) return 0;

				// Retrieve the tag frame we want - TRCK.

				TagLib::ID3v2::FrameList framelist = sf->ID3v2Tag()->frameListMap()["TRCK"];
				if(framelist.isEmpty()) return 0;

				QString s = QString::fromUtf8(framelist.front()->toString().toCString(true));

				// Look for a '/' character in the tag frame.

				int pos = -1;
				for(int i = 0; i < s.length(); ++i)
				{
					if(s[i] == '/')
					{
						pos = i;
						break;
					}
				}

				// Process the tag.

				if(pos != -1)
				{
					// We found a '/', so the tag is in the format "CD Number/CD Count".
					tc = s.right(s.length() - (pos+1)).toInt(&ok);
					if(!ok) return 0;
				}
				else
				{
					// No '/', so the tag is assumed to represent solely track number - nothing to return.
					return 0;
				}

				// Return our result.

				return tc;
			}
			break;

		case CSTaggedFile::RIFFAIFF:
			{
				return 0;
			}
			break;

		default:
			return 0;
	}
}

/*!
 * This is one of our retrieval functions that gather information that is technically available via TagLib,
 * but that is hard to get to because it isn't available via the common TagLib::Tag API for all formats.
 * Because of this, not all formats are supported -- if you need to support a weird format, then you should
 * subclass us and override this function. If we are NULL, then an empty string is returned instead.
 *
 * Formats currently supported:
 *     - MP4 (i.e., AAC/ALAC)
 *     - MPEG (i.e., MP3/etc.)
 *
 * \return The track's composer.
 */
QString CSTaggedFile::getComposer() const
{
	if(isNull()) return "";

	switch(getFileType())
	{
		case CSTaggedFile::MP4:
			{
				// Try casting the file pointer.
				TagLib::MP4::File *sf = dynamic_cast<TagLib::MP4::File *>(file);
				if(sf == NULL) return 0;

				// Try retrieving its format-specific tags.
				TagLib::MP4::Tag *tag = dynamic_cast<TagLib::MP4::Tag *>(sf->tag());
				if(tag == NULL) return 0;

				// Grab the composer.
				if(tag->itemListMap().contains("@wrt"))
					return QString::fromUtf8(tag->itemListMap()["@wrt"].toStringList().toString().toCString(true));
				else
					return "";
			}
			break;

		case CSTaggedFile::MPEG:
			{
				// Setup our variables and make sure everything is kosher.

				TagLib::MPEG::File *sf = dynamic_cast<TagLib::MPEG::File *>(file);

				if(sf == NULL) return "";
				if(sf->ID3v2Tag() == NULL) return "";

				// Retrieve the tag frame we want - TCOM.

				TagLib::ID3v2::FrameList framelist = sf->ID3v2Tag()->frameListMap()["TCOM"];
				if(framelist.isEmpty()) return 0;

				QString s = QString::fromUtf8(framelist.front()->toString().toCString(true));

				// Return our result.

				return s;
			}
			break;

		case CSTaggedFile::RIFFAIFF:
			{
				return "";
			}
			break;

		default:
			return 0;
	}
}

/*!
 * This is one of our retrieval functions that gather information that is technically available via TagLib,
 * but that is hard to get to because it isn't available via the common TagLib::Tag API for all formats.
 * Because of this, not all formats are supported -- if you need to support a weird format, then you should
 * subclass us and override this function. If we are NULL, then an empty string is returned instead.
 *
 * Formats currently supported:
 *     - MP4 (i.e., AAC/ALAC)
 *     - MPEG (i.e., MP3/etc.)
 *
 * \return The track's copyright information.
 */
QString CSTaggedFile::getCopyright() const
{
	if(isNull()) return "";

	switch(getFileType())
	{
		case CSTaggedFile::MP4:
			{
				// Try casting the file pointer.
				TagLib::MP4::File *sf = dynamic_cast<TagLib::MP4::File *>(file);
				if(sf == NULL) return 0;

				// Try retrieving its format-specific tags.
				TagLib::MP4::Tag *tag = dynamic_cast<TagLib::MP4::Tag *>(sf->tag());
				if(tag == NULL) return 0;

				// Grab the copyright information.
				if(tag->itemListMap().contains("cprt"))
					return QString::fromUtf8(tag->itemListMap()["cprt"].toStringList().toString().toCString(true));
				else
					return "";
			}
			break;

		case CSTaggedFile::MPEG:
			{
				// Setup our variables and make sure everything is kosher.

				TagLib::MPEG::File *sf = dynamic_cast<TagLib::MPEG::File *>(file);

				if(sf == NULL) return "";
				if(sf->ID3v2Tag() == NULL) return "";

				// Retrieve the tag frame we want - TCOP.

				TagLib::ID3v2::FrameList framelist = sf->ID3v2Tag()->frameListMap()["TCOP"];
				if(framelist.isEmpty()) return 0;

				QString s = QString::fromUtf8(framelist.front()->toString().toCString(true));

				// Return our result.

				return s;
			}
			break;

		case CSTaggedFile::RIFFAIFF:
			{
				return "";
			}
			break;

		default:
			return 0;
	}
}

/*!
 * This is one of our retrieval functions that gather information that is technically available via TagLib,
 * but that is hard to get to because it isn't available via the common TagLib::Tag API for all formats.
 * Because of this, not all formats are supported -- if you need to support a weird format, then you should
 * subclass us and override this function. If we are NULL, then an empty string is returned instead.
 *
 * Formats currently supported:
 *     - MP4 (i.e., AAC/ALAC) - NOTE: This format doesn't store a URL field, so an empty string is always returned.
 *     - MPEG (i.e., MP3/etc.)
 *
 * \return The track's user-specified URL.
 */
QString CSTaggedFile::getURL() const
{
	if(isNull()) return "";

	switch(getFileType())
	{
		case CSTaggedFile::MP4:
			{
				return "";
			}
			break;

		case CSTaggedFile::MPEG:
			{
				// Setup our variables and make sure everything is kosher.

				TagLib::MPEG::File *sf = dynamic_cast<TagLib::MPEG::File *>(file);

				if(sf == NULL) return "";
				if(sf->ID3v2Tag() == NULL) return "";

				// Retrieve the tag frame we want - WXXX.

				TagLib::ID3v2::FrameList framelist = sf->ID3v2Tag()->frameListMap()["WXXX"];
				if(framelist.isEmpty()) return 0;

				QString s = QString::fromUtf8(framelist.front()->toString().toCString(true));

				// Return our result.

				return s;
			}
			break;

		case CSTaggedFile::RIFFAIFF:
			{
				return "";
			}
			break;

		default:
			return 0;
	}
}

/*!
 * This is one of our retrieval functions that gather information that is technically available via TagLib,
 * but that is hard to get to because it isn't available via the common TagLib::Tag API for all formats.
 * Because of this, not all formats are supported -- if you need to support a weird format, then you should
 * subclass us and override this function. If we are NULL, then an empty string is returned instead.
 *
 * Formats currently supported:
 *     - MP4 (i.e., AAC/ALAC)
 *     - MPEG (i.e., MP3/etc.)
 *
 * \return The track's user-specified encoded by field.
 */
QString CSTaggedFile::getEncodedBy() const
{
	if(isNull()) return "";

	switch(getFileType())
	{
		case CSTaggedFile::MP4:
			{
				// Try casting the file pointer.
				TagLib::MP4::File *sf = dynamic_cast<TagLib::MP4::File *>(file);
				if(sf == NULL) return 0;

				// Try retrieving its format-specific tags.
				TagLib::MP4::Tag *tag = dynamic_cast<TagLib::MP4::Tag *>(sf->tag());
				if(tag == NULL) return 0;

				// Grab the encoded by information.
				if(tag->itemListMap().contains("@enc"))
					return QString::fromUtf8(tag->itemListMap()["©enc"].toStringList().toString().toCString(true));
				else
					return "";
			}
			break;

		case CSTaggedFile::MPEG:
			{
				// Setup our variables and make sure everything is kosher.

				TagLib::MPEG::File *sf = dynamic_cast<TagLib::MPEG::File *>(file);

				if(sf == NULL) return "";
				if(sf->ID3v2Tag() == NULL) return "";

				// Retrieve the tag frame we want - TENC.

				TagLib::ID3v2::FrameList framelist = sf->ID3v2Tag()->frameListMap()["TENC"];
				if(framelist.isEmpty()) return 0;

				QString s = QString::fromUtf8(framelist.front()->toString().toCString(true));

				// Return our result.

				return s;
			}
			break;

		case CSTaggedFile::RIFFAIFF:
			{
				return "";
			}
			break;

		default:
			return 0;
	}
}

/*!
 * This is one of our retrieval functions that gather information that is technically available via TagLib,
 * but that is hard to get to because it isn't available via the common TagLib::Tag API for all formats.
 * Because of this, not all formats are supported -- if you need to support a weird format, then you should
 * subclass us and override this function. If we are NULL, then an empty string is returned instead.
 *
 * Formats currently supported:
 *     - MP4 (i.e., AAC/ALAC) - NOTE: This format doesn't store an original artist field, so an empty string is always returned.
 *     - MPEG (i.e., MP3/etc.)
 *
 * \return The track's "original artist."
 */
QString CSTaggedFile::getOriginalArtist() const
{
	if(isNull()) return "";

	switch(getFileType())
	{
		case CSTaggedFile::MP4:
			{
				return "";
			}
			break;

		case CSTaggedFile::MPEG:
			{
				// Setup our variables and make sure everything is kosher.

				TagLib::MPEG::File *sf = dynamic_cast<TagLib::MPEG::File *>(file);

				if(sf == NULL) return "";
				if(sf->ID3v2Tag() == NULL) return "";

				// Retrieve the tag frame we want - TOPE.

				TagLib::ID3v2::FrameList framelist = sf->ID3v2Tag()->frameListMap()["TOPE"];
				if(framelist.isEmpty()) return 0;

				QString s = QString::fromUtf8(framelist.front()->toString().toCString(true));

				// Return our result.

				return s;
			}
			break;

		case CSTaggedFile::RIFFAIFF:
			{
				return "";
			}
			break;

		default:
			return 0;
	}
}

/*!
 * This is one of our retrieval functions that gather information that is technically available via TagLib,
 * but that is hard to get to because it isn't available via the common TagLib::Tag API for all formats.
 * Because of this, not all formats are supported -- if you need to support a weird format, then you should
 * subclass us and override this function. If we are NULL, then an empty string is returned instead.
 *
 * Formats currently supported:
 *     - MP4 (i.e., AAC/ALAC)
 *     - MPEG (i.e., MP3/etc.) - NOTE: This format doesn't store a keywords field, so an empty string is always returned.
 *
 * \return The track's keywords.
 */
QString CSTaggedFile::getKeywords() const
{
	if(isNull()) return "";

	switch(getFileType())
	{
		case CSTaggedFile::MP4:
			{
				// Try casting the file pointer.
				TagLib::MP4::File *sf = dynamic_cast<TagLib::MP4::File *>(file);
				if(sf == NULL) return 0;

				// Try retrieving its format-specific tags.
				TagLib::MP4::Tag *tag = dynamic_cast<TagLib::MP4::Tag *>(sf->tag());
				if(tag == NULL) return 0;

				// Grab the keywords.
				if(tag->itemListMap().contains("keyw"))
					return QString::fromUtf8(tag->itemListMap()["keyw"].toStringList().toString().toCString(true));
				else
					return "";
			}
			break;

		case CSTaggedFile::MPEG:
			{
				return "";
			}
			break;

		case CSTaggedFile::RIFFAIFF:
			{
				return "";
			}
			break;

		default:
			return 0;
	}
}

/*!
 * This is one of our retrieval functions that gather information that is technically available via TagLib,
 * but that is hard to get to because it isn't available via the common TagLib::Tag API for all formats.
 * Because of this, not all formats are supported -- if you need to support a weird format, then you should
 * subclass us and override this function. If we are NULL, then an empty string is returned instead.
 *
 * Formats currently supported:
 *     - MP4 (i.e., AAC/ALAC)
 *     - MPEG (i.e., MP3/etc.) - NOTE: This format doesn't store an album artist field, so an empty string is always returned.
 *
 * \return The artist for the album this track is on.
 */
QString CSTaggedFile::getAlbumArtist() const
{
	if(isNull()) return "";

	switch(getFileType())
	{
		case CSTaggedFile::MP4:
			{
				// Try casting the file pointer.
				TagLib::MP4::File *sf = dynamic_cast<TagLib::MP4::File *>(file);
				if(sf == NULL) return 0;

				// Try retrieving its format-specific tags.
				TagLib::MP4::Tag *tag = dynamic_cast<TagLib::MP4::Tag *>(sf->tag());
				if(tag == NULL) return 0;

				// Grab the album artist.
				if(tag->itemListMap().contains("aART"))
					return QString::fromUtf8(tag->itemListMap()["aART"].toStringList().toString().toCString(true));
				else
					return "";
			}
			break;

		case CSTaggedFile::MPEG:
			{
				return "";
			}
			break;

		case CSTaggedFile::RIFFAIFF:
			{
				return "";
			}
			break;

		default:
			return 0;
	}
}

/*!
 * This function returns, as a GdkPixbuf object, the embedded cover artwork in this file. If the file doesn't contain any
 * embedded artwork, NULL is returned instead. Note that different formats do embedded artwork differently, so this function
 * is format-specific.
 *
 * Note that it is up to the caller to free the memory occupied by the returned pixbuf object.
 *
 * Formats currently supported:
 *      - MP4 (i.e., AAC/ALAC)
 *      - MPEG (i.e., MP3/etc.)
 *
 * \return The embedded cover artwork for this song.
 */
gpointer CSTaggedFile::getCoverArtwork() const
{
	if(isNull()) return NULL;

	TagLib::ByteVector data;

	switch(getFileType())
	{
		case CSTaggedFile::MP4:
			{
				// Try casting the file pointer.
				TagLib::MP4::File *sf = dynamic_cast<TagLib::MP4::File *>(file);
				if(sf == NULL) return NULL;

				// Try retrieving its format-specific tags.
				TagLib::MP4::Tag *tag = dynamic_cast<TagLib::MP4::Tag *>(sf->tag());
				if(tag == NULL) return NULL;

				// Try to grab the cover art from the file.

				if(tag->itemListMap().contains("covr"))
				{
					TagLib::List<TagLib::MP4::CoverArt> covr = tag->itemListMap()["covr"].toCoverArtList();

					// We expect only a single cover art item in a file.

					if(covr.size() != 1)
						return NULL;

					// Retrieve the image data.

					data = covr.front().data();
				}
				else
				{
					return NULL;
				}
			}
			break;

		case CSTaggedFile::MPEG:
			{
				// Setup our variables and make sure everything is kosher.

				TagLib::MPEG::File *sf = dynamic_cast<TagLib::MPEG::File *>(file);

				if(sf == NULL) return NULL;
				if(sf->ID3v2Tag() == NULL) return NULL;

				// Retrieve the tag frame we want - APIC.

				TagLib::ID3v2::FrameList framelist = sf->ID3v2Tag()->frameListMap()["APIC"];
				TagLib::List<TagLib::ID3v2::AttachedPictureFrame *> covr;
				TagLib::ID3v2::AttachedPictureFrame *apic;

				// Look for pictures with the type FrontCover first.

				for(uint32_t i = 0; i < framelist.size(); ++i)
				{
					apic = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame *>(framelist[i]);

					if(apic != NULL)
						if(apic->type() == TagLib::ID3v2::AttachedPictureFrame::FrontCover)
							covr.append(apic);
				}

				if(covr.size() == 0)
				{
					// Try looking for "Other" pictures as a fallback.

					for(uint32_t i = 0; i < framelist.size(); ++i)
					{
						apic = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame *>(framelist[i]);

						if(apic != NULL)
							if(apic->type() == TagLib::ID3v2::AttachedPictureFrame::Other)
								covr.append(apic);
					}
				}

				// Retrieve the image data.

				if(covr.size() == 1)
					data = covr.front()->picture();
				else
					return NULL;
			}
			break;

		case CSTaggedFile::RIFFAIFF:
			{
				return NULL;
			}
			break;

		default:
			return NULL;
	};

	// Load whatever data we have retrieved at this point, if any.

	if(!data.isEmpty())
	{
		GError *error = NULL;
		GInputStream *reader = g_memory_input_stream_new_from_data(data.data(), data.size(), NULL);

		GdkPixbuf *pixbuf = gdk_pixbuf_new_from_stream(reader, NULL, &error);
		if(error != NULL)
		{
			g_error_free(error);
			pixbuf = NULL;
		}

		g_input_stream_close(reader, NULL, NULL);
		g_object_unref(reader);

		return pixbuf;
	}
	else
	{
		return NULL;
	}
}
