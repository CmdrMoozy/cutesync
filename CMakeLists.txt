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
SET(DBG_FLAGS "${DBG_FLAGS} -DCUTESYNC_DEBUG")

SET(REL_FLAGS "-Wall -Wextra -ansi -pedantic -Wshadow -Wpointer-arith")
SET(REL_FLAGS "${REL_FLAGS} -Wcast-qual -std=c++0x -fomit-frame-pointer")

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DBG_FLAGS}")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${REL_FLAGS}")

INCLUDE_DIRECTORIES("src" ${GLIB_INCLUDE_DIRS} ${GDK-PIXBUF_INCLUDE_DIRS})

# Define libcute's source files.

SET(libcute_HEADERS

	src/libcute/Defines.h

	src/libcute/collections/CuteSyncAbstractCollection.h
	src/libcute/collections/CuteSyncAbstractCollectionConfigWidget.h
	src/libcute/collections/CuteSyncCollectionTypeResolver.h
	src/libcute/collections/CuteSyncDirCollection.h
	src/libcute/collections/CuteSyncDirCollectionConfigWidget.h
	src/libcute/collections/CuteSyncDirTrack.h
	src/libcute/collections/CuteSyncGeneralCollectionConfigWidget.h
	src/libcute/collections/CuteSyncIPodCollection.h
	src/libcute/collections/CuteSyncIPodCollectionConfigWidget.h
	src/libcute/collections/CuteSyncIPodTrack.h
	src/libcute/collections/CuteSyncTrack.h

	src/libcute/tags/CuteSyncFileTypeResolver.h
	src/libcute/tags/CuteSyncTaggedFile.h

	src/libcute/thread/CuteSyncCollectionLocker.h

	src/libcute/util/CuteSyncMMIOHandle.h
	src/libcute/util/CuteSyncSystemUtils.h

	src/libcute/widgets/CuteSyncCollectionModel.h

)

SET(libcute_SOURCES

	src/libcute/collections/CuteSyncAbstractCollection.cpp
	src/libcute/collections/CuteSyncAbstractCollectionConfigWidget.cpp
	src/libcute/collections/CuteSyncCollectionTypeResolver.cpp
	src/libcute/collections/CuteSyncDirCollection.cpp
	src/libcute/collections/CuteSyncDirCollectionConfigWidget.cpp
	src/libcute/collections/CuteSyncDirTrack.cpp
	src/libcute/collections/CuteSyncGeneralCollectionConfigWidget.cpp
	src/libcute/collections/CuteSyncIPodCollection.cpp
	src/libcute/collections/CuteSyncIPodCollectionConfigWidget.cpp
	src/libcute/collections/CuteSyncIPodTrack.cpp
	src/libcute/collections/CuteSyncTrack.cpp

	src/libcute/tags/CuteSyncFileTypeResolver.cpp
	src/libcute/tags/CuteSyncTaggedFile.cpp

	src/libcute/thread/CuteSyncCollectionLocker.cpp

	src/libcute/util/CuteSyncMMIOHandle.cpp
	src/libcute/util/CuteSyncSystemUtils.cpp

	src/libcute/widgets/CuteSyncCollectionModel.cpp

)

# Define CuteSync's source files.

SET(cutesync_RESOURCES

	data.qrc
	icons.qrc

)

SET(cutesync_HEADERS

	src/cutesync/CuteSyncMainWindow.h

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

	src/cutesync/CuteSync.cpp
	src/cutesync/CuteSyncMainWindow.cpp

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

# Define id3ck's source files.

SET(id3ck_HEADERS



)

SET(id3ck_SOURCES

	src/id3ck/id3ck.cpp

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

ADD_LIBRARY(cute STATIC ${libcute_SOURCES})
TARGET_LINK_LIBRARIES(cute m ${GLIB_GIO_LIBRARIES} ${GLIB_GOBJECT_LIBRARIES} ${GLIB_GTHREAD_LIBRARIES} ${GLIB_LIBRARIES} ${GDK-PIXBUF_LIBRARIES} rt gpod tag ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES})

QT5_ADD_RESOURCES(cutesync_RESOURCES_RCC ${cutesync_RESOURCES})
ADD_EXECUTABLE(CuteSync ${cutesync_RESOURCES_RCC} ${cutesync_SOURCES})
TARGET_LINK_LIBRARIES(CuteSync cute m ${GLIB_GIO_LIBRARIES} ${GLIB_GOBJECT_LIBRARIES} ${GLIB_GTHREAD_LIBRARIES} ${GLIB_LIBRARIES} ${GDK-PIXBUF_LIBRARIES} rt gpod tag ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES})

ADD_EXECUTABLE(id3ck ${id3ck_SOURCES})
TARGET_LINK_LIBRARIES(id3ck cute m ${GLIB_GIO_LIBRARIES} ${GLIB_GOBJECT_LIBRARIES} ${GLIB_GTHREAD_LIBRARIES} ${GLIB_LIBRARIES} ${GDK-PIXBUF_LIBRARIES} rt gpod tag ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES})

ADD_EXECUTABLE(ifsck ${ifsck_SOURCES})
TARGET_LINK_LIBRARIES(ifsck m ${GLIB_GIO_LIBRARIES} ${GLIB_GOBJECT_LIBRARIES} ${GLIB_GTHREAD_LIBRARIES} ${GLIB_LIBRARIES} ${GDK-PIXBUF_LIBRARIES} rt gpod tag ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES})

ADD_EXECUTABLE(mkipod ${mkipod_SOURCES})
TARGET_LINK_LIBRARIES(mkipod m ${GLIB_GIO_LIBRARIES} ${GLIB_GOBJECT_LIBRARIES} ${GLIB_GTHREAD_LIBRARIES} ${GLIB_LIBRARIES} ${GDK-PIXBUF_LIBRARIES} rt gpod tag ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES})

QT5_USE_MODULES(cute Widgets Network)
QT5_USE_MODULES(CuteSync Widgets Network)
QT5_USE_MODULES(id3ck Widgets Network)
QT5_USE_MODULES(ifsck Widgets Network)
QT5_USE_MODULES(mkipod Widgets Network)
