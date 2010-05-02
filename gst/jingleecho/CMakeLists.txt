cmake_minimum_required(VERSION 2.6)

project(jingleecho)

set(QT_USE_QTXML)

list(APPEND CMAKE_MODULE_PATH ../../../CMakeMod)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeMod)

find_package(Qt4 REQUIRED)
# to locate QCA2, locate KDE4 cmake modules using package
find_package(KDE4)
find_package(QCA2 REQUIRED)
find_package(Iris REQUIRED)
find_package(PkgConfig)

add_definitions(-DIRIS -D_POSIX_SOURCE)


list(APPEND LOCAL_INCLUDES ipv6 irisbot ../ipv6 ../logger ..)

pkg_check_modules(FARSIGHT farsight2-0.10)

include_directories(${QT_INCLUDES} ${QCA2_INCLUDES} ${IRIS_INCLUDES} ${FARSIGHT_INCLUDE_DIRS} ${LOCAL_INCLUDES})

link_directories(${IRIS_LIB_PATH} ${FARSIGHT_LIBRARY_DIRS})

link_libraries(${IRIS_LIBS} ${FARSIGHT_LIBRARIES})

# Input
set(JINGLEECHO_HEADERS codecs.h 
           conference.h 
           fstjingle.h 
           jingle-candidate.h 
           jingle-content.h 
           jingle-description.h 
           jingle-transport.h 
           jinglemanager.h 
           jinglesession.h 
           parser.h 
           pjid.h
           qpipeline.h 
           session.h 
           stream.h 
           payloadparameter.h 
           mediaconfig.h 
           testmediaconfig.h 
           framesize.h
           avoutput.h 
           avoutputmanager.h 
           jinglesessiongroup.h 
           ipv6/v6interface.h
           gstvideowatcher.h
           audiowatcher.h
        )
set(JINGLEECHO_SOURCES 
           codecs.cpp 
           conference.cpp 
           fstjingle.cpp 
           jingle-candidate.cpp 
           jingle-content.cpp 
           jingle-description.cpp 
           jingle-transport.cpp 
           jinglemanager.cpp 
           jinglesession.cpp 
           parser.cpp 
           pjid.cpp 
           qpipeline.cpp 
           session.cpp 
           stream.cpp 
           payloadparameter.cpp 
           mediaconfig.cpp 
           testmediaconfig.cpp 
           framesize.cpp 
           avoutput.cpp 
           avoutputmanager.cpp 
           jinglesessiongroup.cpp 
           ../ipv6/v6interface.c 
           ../logger/logger.cpp 
           gstvideowatcher.h
           gstvideowatcher.cpp
           audiowatcher.h
           audiowatcher.cpp
    )

#QT4_WRAP_CPP(MOC_FILES audiowatcher.h gstvideowatcher.h)

add_subdirectory(irisbot)

add_library(jingleecho ${JINGLEECHO_SOURCES} ${MOC_FILES})


