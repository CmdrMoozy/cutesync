###############################################################################
#
# To enable "release" or "debug" builds, call CMake as follows:
#
#           cmake -DCMAKE_BUILD_TYPE=Release ..		(The default.)
#           cmake -DCMAKE_BUILD_TYPE=Debug ..
#
###############################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)
PROJECT(cutesync)

SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE OFF)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

# Define what Qt dependencies we have.

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Network REQUIRED)
FIND_PACKAGE(GLIB REQUIRED gio gobject gthread)
FIND_PACKAGE(GDK-PixBuf REQUIRED)

# Set INCLUDE and C(XX)FLAGS

IF(NOT CMAKE_BUILD_TYPE)
	MESSAGE(STATUS "No CMAKE_BUILD_TYPE specified; defaulting to release.")
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

SET(DBG_FLAGS "-rdynamic -Wall -Wextra -ansi -pedantic -Wshadow")
SET(DBG_FLAGS "${DBG_FLAGS} -Wpointer-arith -Wcast-qual -std=c++0x -O0")
SET(DBG_FLAGS "${DBG_FLAGS} -DCUTESYNC_DEBUG -DQT_NO_KEYWORDS")

SET(REL_FLAGS "-Wall -Wextra -ansi -pedantic -Wshadow -Wpointer-arith")
SET(REL_FLAGS "${REL_FLAGS} -Wcast-qual -std=c++0x -fomit-frame-pointer")
SET(REL_FLAGS "${REL_FLAGS} -DQT_NO_KEYWORDS")

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DBG_FLAGS}")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${REL_FLAGS}")

INCLUDE_DIRECTORIES("src" ${GLIB_INCLUDE_DIRS} ${GDK-PIXBUF_INCLUDE_DIRS})

# Define libcute's source files.

SET(libcute_HEADERS

	src/libcute/Defines.h

	src/libcute/collections/AbstractCollection.h
	src/libcute/collections/AbstractCollectionConfigWidget.h
	src/libcute/collections/CollectionTypeResolver.h
	src/libcute/collections/DirCollection.h
	src/libcute/collections/DirCollectionConfigWidget.h
	src/libcute/collections/DirTrack.h
	src/libcute/collections/GeneralCollectionConfigWidget.h
	src/libcute/collections/IPodCollection.h
	src/libcute/collections/IPodCollectionConfigWidget.h
	src/libcute/collections/IPodTrack.h
	src/libcute/collections/Track.h

	src/libcute/tags/FileTypeResolver.h
	src/libcute/tags/TaggedFile.h

	src/libcute/thread/CollectionLocker.h

	src/libcute/util/Bitwise.h
	src/libcute/util/MMIOHandle.h
	src/libcute/util/SystemUtils.h

	src/libcute/widgets/CollectionModel.h

)

SET(libcute_SOURCES

	src/libcute/collections/AbstractCollection.cpp
	src/libcute/collections/AbstractCollectionConfigWidget.cpp
	src/libcute/collections/CollectionTypeResolver.cpp
	src/libcute/collections/DirCollection.cpp
	src/libcute/collections/DirCollectionConfigWidget.cpp
	src/libcute/collections/DirTrack.cpp
	src/libcute/collections/GeneralCollectionConfigWidget.cpp
	src/libcute/collections/IPodCollection.cpp
	src/libcute/collections/IPodCollectionConfigWidget.cpp
	src/libcute/collections/IPodTrack.cpp
	src/libcute/collections/Track.cpp

	src/libcute/tags/FileTypeResolver.cpp
	src/libcute/tags/TaggedFile.cpp

	src/libcute/thread/CollectionLocker.cpp

	src/libcute/util/Bitwise.cpp
	src/libcute/util/MMIOHandle.cpp
	src/libcute/util/SystemUtils.cpp

	src/libcute/widgets/CollectionModel.cpp

)

# Define CuteSync's source files.

SET(cutesync_RESOURCES

	data.qrc
	icons.qrc

)

SET(cutesync_HEADERS

	src/cutesync/application.h
	src/cutesync/mainmenubar.h
	src/cutesync/mainwindow.h

	src/cutesync/dialogs/CuteSyncAboutDialog.h
	src/cutesync/dialogs/CuteSyncNewCollectionDialog.h
	src/cutesync/dialogs/CuteSyncSyncDIalog.h

	src/cutesync/dialogs/inspector/CuteSyncInspectorAboutDialog.h
	src/cutesync/dialogs/inspector/CuteSyncInspectorCollectionConfigDialog.h
	src/cutesync/dialogs/inspector/CuteSyncInspectorConfigDialog.h

	src/cutesync/settings/CuteSyncSettingsManager.h

	src/cutesync/widgets/CuteSyncCollectionDelegate.h
	src/cutesync/widgets/CuteSyncCollectionListWidget.h
	src/cutesync/widgets/CuteSyncDDSelectorModel.h
	src/cutesync/widgets/CuteSyncDDSelectorWidget.h
	src/cutesync/widgets/CuteSyncIconListWidget.h

)

SET(cutesync_SOURCES

	src/cutesync/application.cpp
	src/cutesync/cutesync.cpp
	src/cutesync/mainmenubar.cpp
	src/cutesync/mainwindow.cpp

	src/cutesync/dialogs/CuteSyncAboutDialog.cpp
	src/cutesync/dialogs/CuteSyncNewCollectionDialog.cpp
	src/cutesync/dialogs/CuteSyncSyncDialog.cpp

	src/cutesync/dialogs/inspector/CuteSyncInspectorAboutDialog.cpp
	src/cutesync/dialogs/inspector/CuteSyncInspectorCollectionConfigDialog.cpp
	src/cutesync/dialogs/inspector/CuteSyncInspectorConfigDialog.cpp

	src/cutesync/settings/CuteSyncSettingsManager.cpp

	src/cutesync/widgets/CuteSyncCollectionDelegate.cpp
	src/cutesync/widgets/CuteSyncCollectionInspector.cpp
	src/cutesync/widgets/CuteSyncCollectionListWidget.cpp
	src/cutesync/widgets/CuteSyncDDSelectorModel.cpp
	src/cutesync/widgets/CuteSyncDDSelectorWidget.cpp
	src/cutesync/widgets/CuteSyncIconListWidget.cpp

)

# Define ifsck's source files.

SET(ifsck_HEADERS



)

SET(ifsck_SOURCES

	src/ifsck/ifsck.cpp

)

# Define mkipod's source files.

SET(mkipod_HEADERS



)

SET(mkipod_SOURCES

	src/mkipod/mkipod.cpp

)

# Build our project!

SET(CUTESYNC_LIBS m ${GLIB_GIO_LIBRARIES} ${GLIB_GOBJECT_LIBRARIES})
SET(CUTESYNC_LIBS ${CUTESYNC_LIBS} ${GLIB_GTHREAD_LIBRARIES} ${GLIB_LIBRARIES})
SET(CUTESYNC_LIBS ${CUTESYNC_LIBS} ${GDK-PIXBUF_LIBRARIES} rt gpod tag)
SET(CUTESYNC_LIBS ${CUTESYNC_LIBS} ${Qt5Widgets_LIBRARIES})
SET(CUTESYNC_LIBS ${CUTESYNC_LIBS} ${Qt5Network_LIBRARIES})

ADD_LIBRARY(cute STATIC ${libcute_SOURCES})
TARGET_LINK_LIBRARIES(cute ${CUTESYNC_LIBS})

QT5_ADD_RESOURCES(cutesync_RESOURCES_RCC ${cutesync_RESOURCES})
ADD_EXECUTABLE(CuteSync ${cutesync_RESOURCES_RCC} ${cutesync_SOURCES})
TARGET_LINK_LIBRARIES(CuteSync cute ${CUTESYNC_LIBS})

ADD_EXECUTABLE(ifsck ${ifsck_SOURCES})
TARGET_LINK_LIBRARIES(ifsck cute ${CUTESYNC_LIBS})

ADD_EXECUTABLE(mkipod ${mkipod_SOURCES})
TARGET_LINK_LIBRARIES(mkipod cute ${CUTESYNC_LIBS})

QT5_USE_MODULES(cute Widgets Network)
QT5_USE_MODULES(CuteSync Widgets Network)
QT5_USE_MODULES(ifsck Widgets Network)
QT5_USE_MODULES(mkipod Widgets Network)
