cmake_minimum_required(VERSION 2.6)

set(QT_USE_QTXML true)
set(QT_USE_QTNETWORK true)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeMod)

find_package(Qt4 COMPONENTS QtCore QtGui QtXml QtNetwork REQUIRED)
# to locate QCA2, locate KDE4 cmake modules using package
find_package(KDE4)
find_package(QCA2 REQUIRED)
find_package(Iris REQUIRED)
find_package(PkgConfig)

add_definitions(-DIRIS -D_POSIX_SOURCE)

include(${QT_USE_FILE})

list(APPEND LOCAL_INCLUDES ipv6 irisbot ../ipv6 ../logger ..)

pkg_check_modules(FARSIGHT farsight2-0.10)
pkg_check_modules(FARSIGHT_IFACES gstreamer-interfaces-0.10 )

include_directories(${QT_INCLUDES} ${QCA2_INCLUDES} ${IRIS_INCLUDES} ${FARSIGHT_INCLUDE_DIRS} ${LOCAL_INCLUDES} ${FARSIGHT_IFACES_INCLUDE_DIRS})

link_directories(${IRIS_LIB_PATH} ${FARSIGHT_LIBRARY_DIRS})


# Input
set(IRISBOT_HEADERS codecs.h 
    irisbot.h
    irisjinglemanager.h 
    jinglevideowindow.h
    qtjinglesession.h
    gstvideowidget.h
    aspectedlayout.h
    avparticipantwidget.h
    volumebar.h
)

set(IRISBOT_SOURCES 
    irisbot.cpp
    irisjinglemanager.cpp 
    jinglevideowindow.cpp 
    qtjinglesession.cpp
    gstvideowidget.cpp
    aspectedlayout.cpp
    avparticipantwidget.cpp
    volumebar.cpp
)


QT4_WRAP_CPP(MOC_FILES qtjinglesession.h jinglevideowindow.h avparticipantwidget.h gstvideowidget.h volumebar.h irisbot.h irisjinglemanager.h)


#link_libraries(${IRIS_LIBS} ${FARSIGHT_LIBRARIES})

add_executable(irisbot ${IRISBOT_SOURCES} ${MOC_FILES})

target_link_libraries(irisbot ${QT_LIBRARIES} ${QCA2_LIBRARIES} 
    ${IRIS_LIBRARIES} ${FARSIGHT_LIBRARIES} ${FARSIGHT_IFACES_LIBRARIES} 
    jingleecho)


