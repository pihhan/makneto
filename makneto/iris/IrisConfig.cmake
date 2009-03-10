

# Pihhanovy pokusy s IRIS
FIND_LIBRARY(LIB_IRIS NAMES iris irisnet
    PATHS /usr/lib /usr/local/lib ../../iris 
    ENV IRIS_TREE 
    PATH_SUFFIXES lib)
MESSAGE(STATUS "Iris is at: ${LIB_IRIS}")

FIND_PATH(IRIS_F_INCLUDE xmpp.h
    PATHS /usr /usr/local ../../iris ENV IRIS_TREE
    PATH_SUFFIXES include include/iris)
MESSAGE(STATUS "Iris include at: ${IRIS_F_INCLUDE}")

set(IRIS_TREE "../../iris")
set(IRIS_LIB_PATH ${IRIS_TREE}/lib)
set(IRIS_INCLUDE_PATH ${IRIS_TREE}/include/iris)


set(MANUAL_IRIS_PATH $ENV{IRIS_TREE})
message(status "Environment is: ${MANUAL_IRIS_PATH}")
IF   (MANUAL_IRIS_PATH)
    list(APPEND CMAKE_REQUIRED_INCLUDES "${MANUAL_IRIS_PATH}/include")
    list(APPEND CMAKE_REQUIRED_INCLUDES "${MANUAL_IRIS_PATH}/include/iris")
ENDIF   (MANUAL_IRIS_PATH)

include(CheckIncludeFileCXX)
#include(CheckIncludeFiles)
message(STATUS "required_includes: ${CMAKE_REQUIRED_INCLUDES}")
set(HEADER xmpp)
IF("${HEADER}" MATCHES "^${HEADER}$")
 message(STATUS "Matches ${HEADER}")
ENDIF("${HEADER}" MATCHES "^${HEADER}$")

CHECK_INCLUDE_FILE_CXX(HEADER IRIS_INCLUDES)
MESSAGE(STATUS "Iris include: ${IRIS_INCLUDES}")


include(CheckLibraryExists)
Check_library_exists(iris XMPP::Connector::havePeerAddress ${IRIS_LIB_PATH} IRIS_EXIST_CONNECTOR)
MESSAGE(STATUS "Iris library: ${IRIS_EXIST_CONNECTOR}")

Check_library_exists(irisnet XMPP::NetInterface::name ${IRIS_LIB_PATH} IRISNET_EXIST_NETINTERFACE)
MESSAGE(STATUS "Irisnet library: ${IRISNET_EXIST_NETINTERFACE}")

Check_library_exists(${LIB_IRIS} XMPP::Connector::havePeerAddress "" IRIS_EXIST_CONNECTOR2)
MESSAGE(STATUS "Iris/find library: ${IRIS_EXIST_CONNECTOR2}")
Check_library_exists(${LIB_IRIS} XMPP::NetInterface::name "" IRISNET_EXIST_NETINTERFACE2)
MESSAGE(STATUS "Iris/find library: ${IRISNET_EXIST_NETINTERFACE2}")



set(IRIS_LIBS 
    iris
    irisnet
    )
set(IRIS_INCLUDE ${IRIS_TREE}/include/iris ${IRIS_TREE}/src)

set(Iris_FOUND true)

mark_as_advanced(IRIS_INCLUDE IRIS_LIBS)

