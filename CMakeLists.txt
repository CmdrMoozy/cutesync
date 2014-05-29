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

# Define CuteSync's source files.

SET(cutesync_RESOURCES

	data.qrc
	icons.qrc

)

SET(cutesync_HEADERS

	src/CuteSyncConfig.h
	src/CuteSyncDefines.h
	src/CuteSyncMainWindow.h

	src/collections/CuteSyncAbstractCollection.h
	src/collections/CuteSyncAbstractCollectionConfigWidget.h
	src/collections/CuteSyncCollectionTypeResolver.h
	src/collections/CuteSyncDirCollection.h
	src/collections/CuteSyncDirCollectionConfigWidget.h
	src/collections/CuteSyncDirTrack.h
	src/collections/CuteSyncGeneralCollectionConfigWidget.h
	src/collections/CuteSyncIPodCollection.h
	src/collections/CuteSyncIPodCollectionConfigWidget.h
	src/collections/CuteSyncIPodTrack.h
	src/collections/CuteSyncTrack.h

	src/dialogs/CuteSyncAboutDialog.h
	src/dialogs/CuteSyncNewCollectionDialog.h
	src/dialogs/CuteSyncSyncDIalog.h

	src/dialogs/inspector/CuteSyncInspectorAboutDialog.h
	src/dialogs/inspector/CuteSyncInspectorCollectionConfigDialog.h
	src/dialogs/inspector/CuteSyncInspectorConfigDialog.h

	src/settings/CuteSyncSettingsManager.h

	src/tags/CuteSyncTaggedFile.h
	src/tags/CuteSyncFileTypeResolver.h

	src/thread/CuteSyncCollectionLocker.h

	src/util/CuteSyncMMIOHandle.h
	src/util/CuteSyncSystemUtils.h

	src/widgets/CuteSyncCollectionDelegate.h
	src/widgets/CuteSyncCollectionListWidget.h
	src/widgets/CuteSyncCollectionModel.h
	src/widgets/CuteSyncDDSelectorModel.h
	src/widgets/CuteSyncDDSelectorWidget.h
	src/widgets/CuteSyncIconListWidget.h

)

SET(cutesync_SOURCES

	src/CuteSync.cpp
	src/CuteSyncMainWindow.cpp

	src/collections/CuteSyncAbstractCollection.cpp
	src/collections/CuteSyncAbstractCollectionConfigWidget.cpp
	src/collections/CuteSyncCollectionTypeResolver.cpp
	src/collections/CuteSyncDirCollection.cpp
	src/collections/CuteSyncDirCollectionConfigWidget.cpp
	src/collections/CuteSyncDirTrack.cpp
	src/collections/CuteSyncGeneralCollectionConfigWidget.cpp
	src/collections/CuteSyncIPodCollection.cpp
	src/collections/CuteSyncIPodCollectionConfigWidget.cpp
	src/collections/CuteSyncIPodTrack.cpp
	src/collections/CuteSyncTrack.cpp

	src/dialogs/CuteSyncAboutDialog.cpp
	src/dialogs/CuteSyncNewCollectionDialog.cpp
	src/dialogs/CuteSyncSyncDialog.cpp

	src/dialogs/inspector/CuteSyncInspectorAboutDialog.cpp
	src/dialogs/inspector/CuteSyncInspectorCollectionConfigDialog.cpp
	src/dialogs/inspector/CuteSyncInspectorConfigDialog.cpp

	src/settings/CuteSyncSettingsManager.cpp

	src/tags/CuteSyncFileTypeResolver.cpp
	src/tags/CuteSyncTaggedFile.cpp

	src/thread/CuteSyncCollectionLocker.cpp

	src/util/CuteSyncMMIOHandle.cpp
	src/util/CuteSyncSystemUtils.cpp

	src/widgets/CuteSyncCollectionDelegate.cpp
	src/widgets/CuteSyncCollectionInspector.cpp
	src/widgets/CuteSyncCollectionListWidget.cpp
	src/widgets/CuteSyncCollectionModel.cpp
	src/widgets/CuteSyncDDSelectorModel.cpp
	src/widgets/CuteSyncDDSelectorWidget.cpp
	src/widgets/CuteSyncIconListWidget.cpp

)

# Build our project!

QT5_ADD_RESOURCES(cutesync_RESOURCES_RCC ${cutesync_RESOURCES})

ADD_EXECUTABLE(CuteSync ${cutesync_RESOURCES_RCC} ${cutesync_SOURCES})
TARGET_LINK_LIBRARIES(CuteSync m ${GLIB_GIO_LIBRARIES} ${GLIB_GOBJECT_LIBRARIES} ${GLIB_GTHREAD_LIBRARIES} ${GLIB_LIBRARIES} ${GDK-PIXBUF_LIBRARIES} rt gpod tag ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES})

QT5_USE_MODULES(CuteSync Widgets Network)
