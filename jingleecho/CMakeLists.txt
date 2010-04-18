cmake_minimum_required(VERSION 2.6)

project(jingleecho)

set(QT_USE_QTXML)

find_package(Qt4 REQUIRED)
find_package(QCA2 REQUIRED)
find_package(Iris REQUIRED)

add_definitions(-DIRIS)


CMAKE_INCLUDE_PATH += ipv6 irisbot
CMAKE_INCLUDE_PATH += ../ipv6 ../logger
CMAKE_INCLUDE_PATH += ..

pkg_check_modules(FARSIGHT farsight2-0.10)

include_directories(${QT_INCLUDES} {QCA2_INCLUDES} ${IRIS_INCLUDES} ${FARSIGHT_INCLUDES}

link_directories(${IRIS_LIB_PATH})

link_libraries(${IRIS_LIBS} ${FARSIGHT_LIBS})

# Input
set(JINGLEECHO_HEADERS codecs.h \
           conference.h \
           fstjingle.h \
           jingle-candidate.h \
           jingle-content.h \
           jingle-description.h \
           jingle-transport.h \
           jinglemanager.h \
           jinglesession.h \
           parser.h \
           pjid.h \
           qpipeline.h \
           session.h \
           stream.h \
           payloadparameter.h \
           mediaconfig.h \
           testmediaconfig.h \
           framesize.h \
           avoutput.h \
           avoutputmanager.h \
           jinglesessiongroup.h \
           ipv6/v6interface.h
        )
set(JINGLEECHO_SOURCES \
           codecs.cpp \
           conference.cpp \
           fstjingle.cpp \
           jingle-candidate.cpp \
           jingle-content.cpp \
           jingle-description.cpp \
           jingle-transport.cpp \
           jinglemanager.cpp \
           jinglesession.cpp \
           parser.cpp \
           pjid.cpp \
           qpipeline.cpp \
           session.cpp \
           stream.cpp \
           payloadparameter.cpp \
           mediaconfig.cpp \
           testmediaconfig.cpp \
           framesize.cpp \
           avoutput.cpp \
           avoutputmanager.cpp \
           jinglesessiongroup.cpp \
           ../ipv6/v6interface.c \
           ../logger/logger.cpp \
    )



add_library(jingleecho ${JINGLEECHO_SOURCES} ${JINGLEECHO_HEADERS)


