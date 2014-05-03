QMAKE_CXXFLAGS += -std=c++0x
DEPENDPATH += src/
QT += network

csrelease {
	CONFIG += release
	QMAKE_CXXFLAGS += -O2 -fomit-frame-pointer

	INCLUDEPATH += 3rdparty/prefix/release/include/gpod-1.0 3rdparty/prefix/release/include/taglib
	LIBS += -L3rdparty/prefix/release/lib
}

csdebug {
	CONFIG += debug
	QMAKE_CXXFLAGS += -O0
	DEFINES += CUTESYNC_DEBUG

	INCLUDEPATH += 3rdparty/prefix/debug/include/gpod-1.0 3rdparty/prefix/debug/include/taglib
	LIBS += -L3rdparty/prefix/debug/lib
}

cutesync {
	TEMPLATE = app
	TARGET = CuteSync

	INCLUDEPATH += /usr/include/glib-2.0 /usr/lib64/glib-2.0/include /usr/include/gdk-pixbuf-2.0 \
		/usr/include/libpng15 src
	LIBS += -lm -lglib-2.0 -lgio-2.0 -pthread -lgdk_pixbuf-2.0 -lgobject-2.0 -lgthread-2.0 -lrt -lgpod -ltag

	#INCLUDEPATH += /usr/include/glib-2.0 /usr/lib64/glib-2.0/include /usr/include/gdk-pixbuf-2.0 \
	#	/usr/include/libpng15 /usr/include/gpod-1.0 /usr/include/taglib src
	#LIBS += -lm -lglib-2.0 -lgio-2.0 -pthread -lgdk_pixbuf-2.0 -lgobject-2.0 -lgthread-2.0 -lrt -lgpod -ltag

	# We haven't actually produced Mac icons yet...
	# For Mac OSX Only!
	#ICON = icons/mac/CuteSync.icns

	HEADERS += src/CuteSyncConfig.h \
		src/CuteSyncDefines.h \
		src/CuteSyncMainWindow.h \
		src/collections/CuteSyncAbstractCollection.h \
		src/collections/CuteSyncAbstractCollectionConfigWidget.h \
		src/collections/CuteSyncDirCollection.h \
		src/collections/CuteSyncDirCollectionConfigWidget.h \
		src/collections/CuteSyncDirTrack.h \
		src/collections/CuteSyncGeneralCollectionConfigWidget.h \
		src/collections/CuteSyncIPodCollection.h \
		src/collections/CuteSyncIPodCollectionConfigWidget.h \
		src/collections/CuteSyncIPodTrack.h \
		src/collections/CuteSyncCollectionTypeResolver.h \
		src/collections/CuteSyncTrack.h \
		src/dialogs/CuteSyncAboutDialog.h \
		src/dialogs/CuteSyncNewCollectionDialog.h \
		src/dialogs/CuteSyncSyncDialog.h \
		src/dialogs/inspector/CuteSyncInspectorAboutDialog.h \
		src/dialogs/inspector/CuteSyncInspectorCollectionConfigDialog.h \
		src/dialogs/inspector/CuteSyncInspectorConfigDialog.h \
		src/settings/CuteSyncSettingsManager.h \
		src/tags/CuteSyncFileTypeResolver.h \
		src/tags/CuteSyncTaggedFile.h \
		src/thread/CuteSyncCollectionLocker.h \
		src/util/CuteSyncMMIOHandle.h \
		src/util/CuteSyncSystemUtils.h \
		src/widgets/CuteSyncCollectionDelegate.h \
		src/widgets/CuteSyncCollectionInspector.h \
		src/widgets/CuteSyncCollectionListWidget.h \
		src/widgets/CuteSyncCollectionModel.h \
		src/widgets/CuteSyncDDSelectorModel.h \
		src/widgets/CuteSyncDDSelectorWidget.h \
		src/widgets/CuteSyncIconListWidget.h

	SOURCES += src/CuteSync.cpp \
		src/CuteSyncMainWindow.cpp \
		src/collections/CuteSyncAbstractCollection.cpp \
		src/collections/CuteSyncAbstractCollectionConfigWidget.cpp \
		src/collections/CuteSyncDirCollection.cpp \
		src/collections/CuteSyncDirCollectionConfigWidget.cpp \
		src/collections/CuteSyncDirTrack.cpp \
		src/collections/CuteSyncGeneralCollectionConfigWidget.cpp \
		src/collections/CuteSyncIPodCollection.cpp \
		src/collections/CuteSyncIPodCollectionConfigWidget.cpp \
		src/collections/CuteSyncIPodTrack.cpp \
		src/collections/CuteSyncCollectionTypeResolver.cpp \
		src/collections/CuteSyncTrack.cpp \
		src/dialogs/CuteSyncAboutDialog.cpp \
		src/dialogs/CuteSyncNewCollectionDialog.cpp \
		src/dialogs/CuteSyncSyncDialog.cpp \
		src/dialogs/inspector/CuteSyncInspectorAboutDialog.cpp \
		src/dialogs/inspector/CuteSyncInspectorCollectionConfigDialog.cpp \
		src/dialogs/inspector/CuteSyncInspectorConfigDialog.cpp \
		src/settings/CuteSyncSettingsManager.cpp \
		src/tags/CuteSyncFileTypeResolver.cpp \
		src/tags/CuteSyncTaggedFile.cpp \
		src/thread/CuteSyncCollectionLocker.cpp \
		src/util/CuteSyncMMIOHandle.cpp \
		src/util/CuteSyncSystemUtils.cpp \
		src/widgets/CuteSyncCollectionDelegate.cpp \
		src/widgets/CuteSyncCollectionInspector.cpp \
		src/widgets/CuteSyncCollectionListWidget.cpp \
		src/widgets/CuteSyncCollectionModel.cpp \
		src/widgets/CuteSyncDDSelectorModel.cpp \
		src/widgets/CuteSyncDDSelectorWidget.cpp \
		src/widgets/CuteSyncIconListWidget.cpp

	RESOURCES += icons.qrc \
		data.qrc
}

mkipod {
	TEMPLATE = app
	CONFIG += console
	QT -= gui

	INCLUDEPATH += /usr/include/glib-2.0 /usr/lib64/glib-2.0/include src
	LIBS += -lglib-2.0 -lgio-2.0 -lgpod -ltag

	TARGET = mkipod

	SOURCES += src/mkipod/mkipod.cpp
}

ifsck {
	TEMPLATE = app
	CONFIG += console
	QT -= gui

	INCLUDEPATH += /usr/include/glib-2.0 /usr/lib64/glib-2.0/include src
	LIBS += -lglib-2.0 -lgio-2.0 -lgpod -ltag

	TARGET = ifsck

	SOURCES += src/ifsck/ifsck.cpp
}

id3ck {
	TEMPLATE = app
	CONFIG += console
	QT -= gui

	INCLUDEPATH += src

	TARGET = id3ck

	HEADERS += src/util/CuteSyncMMIOHandle.h

	SOURCES += src/id3ck/id3ck.cpp \
		src/util/CuteSyncMMIOHandle.cpp
}
