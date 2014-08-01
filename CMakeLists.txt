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
SET(DBG_FLAGS "${DBG_FLAGS} -DQT_NO_KEYWORDS")

SET(REL_FLAGS "-Wall -Wextra -ansi -pedantic -Wshadow -Wpointer-arith")
SET(REL_FLAGS "${REL_FLAGS} -Wcast-qual -std=c++0x -fomit-frame-pointer")
SET(REL_FLAGS "${REL_FLAGS} -DQT_NO_KEYWORDS")

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DBG_FLAGS}")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${REL_FLAGS}")

STRING(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
IF(CMAKE_BUILD_TYPE_LOWER MATCHES debug)
	ADD_DEFINITIONS(-DCUTESYNC_DEBUG)
ENDIF()

INCLUDE_DIRECTORIES("src" ${GLIB_INCLUDE_DIRS} ${GDK-PIXBUF_INCLUDE_DIRS})

# Define libcute's source files.

SET(libcute_HEADERS

	src/libcute/defines.h

	src/libcute/collections/abstractcollection.h
	src/libcute/collections/abstractcollectionconfigwidget.h
	src/libcute/collections/collectiontyperesolver.h
	src/libcute/collections/dircollection.h
	src/libcute/collections/dircollectionconfigwidget.h
	src/libcute/collections/dirtrack.h
	src/libcute/collections/generalcollectionconfigwidget.h
	src/libcute/collections/ipodcollection.h
	src/libcute/collections/ipodcollectionconfigwidget.h
	src/libcute/collections/ipodtrack.h
	src/libcute/collections/track.h

	src/libcute/tags/filetyperesolver.h
	src/libcute/tags/taggedfile.h

	src/libcute/thread/collectionthreadpool.h
	src/libcute/thread/pausablethread.h

	src/libcute/util/bitwise.h
	src/libcute/util/guiutils.h
	src/libcute/util/mmiohandle.h
	src/libcute/util/systemutils.h

	src/libcute/widgets/collectionlistitem.h
	src/libcute/widgets/collectionmodel.h

)

SET(libcute_SOURCES

	src/libcute/collections/abstractcollection.cpp
	src/libcute/collections/abstractcollectionconfigwidget.cpp
	src/libcute/collections/collectiontyperesolver.cpp
	src/libcute/collections/dircollection.cpp
	src/libcute/collections/dircollectionconfigwidget.cpp
	src/libcute/collections/dirtrack.cpp
	src/libcute/collections/generalcollectionconfigwidget.cpp
	src/libcute/collections/ipodcollection.cpp
	src/libcute/collections/ipodcollectionconfigwidget.cpp
	src/libcute/collections/ipodtrack.cpp
	src/libcute/collections/track.cpp

	src/libcute/tags/filetyperesolver.cpp
	src/libcute/tags/taggedfile.cpp

	src/libcute/thread/collectionthreadpool.cpp
	src/libcute/thread/pausablethread.cpp

	src/libcute/util/bitwise.cpp
	src/libcute/util/guiutils.cpp
	src/libcute/util/mmiohandle.cpp
	src/libcute/util/systemutils.cpp

	src/libcute/widgets/collectionlistitem.cpp
	src/libcute/widgets/collectionmodel.cpp

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

	src/cutesync/dialogs/aboutdialog.h
	src/cutesync/dialogs/newcollectiondialog.h
	src/cutesync/dialogs/syncdialog.h

	src/cutesync/dialogs/inspector/inspectoraboutdialog.h
	src/cutesync/dialogs/inspector/inspectorcollectionconfigdialog.h
	src/cutesync/dialogs/inspector/inspectorconfigdialog.h

	src/cutesync/settings/settingsmanager.h

	src/cutesync/widgets/collectiondelegate.h
	src/cutesync/widgets/collectioninspector.h
	src/cutesync/widgets/collectionlistwidget.h
	src/cutesync/widgets/ddselectormodel.h
	src/cutesync/widgets/ddselectorwidget.h
	src/cutesync/widgets/iconlistwidget.h

)

SET(cutesync_SOURCES

	src/cutesync/application.cpp
	src/cutesync/cutesync.cpp
	src/cutesync/mainmenubar.cpp
	src/cutesync/mainwindow.cpp

	src/cutesync/dialogs/aboutdialog.cpp
	src/cutesync/dialogs/newcollectiondialog.cpp
	src/cutesync/dialogs/syncdialog.cpp

	src/cutesync/dialogs/inspector/inspectoraboutdialog.cpp
	src/cutesync/dialogs/inspector/inspectorcollectionconfigdialog.cpp
	src/cutesync/dialogs/inspector/inspectorconfigdialog.cpp

	src/cutesync/settings/settingsmanager.cpp

	src/cutesync/widgets/collectiondelegate.cpp
	src/cutesync/widgets/collectioninspector.cpp
	src/cutesync/widgets/collectionlistwidget.cpp
	src/cutesync/widgets/ddselectormodel.cpp
	src/cutesync/widgets/ddselectorwidget.cpp
	src/cutesync/widgets/iconlistwidget.cpp

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
