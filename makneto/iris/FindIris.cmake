# Try to find Iris XMPP library
# Imports:
# IRIS_TREE - path to iris tree in filesystem
# Exports:
# IRIS_INCLUDES
# IRIS_LIBRARIES

# Pihhanovy pokusy s IRIS
FIND_PACKAGE(Qt4 REQUIRED)

FIND_LIBRARY(IRIS_F_LIBRARIES NAMES iris
    PATHS /usr /usr/local ${IRIS_TREE}
    ENV IRIS_TREE 
    PATH_SUFFIXES lib)

FIND_LIBRARY(IRISNET_F_LIBRARIES NAMES irisnet
    PATHS /usr /usr/local ${IRIS_TREE}
    ENV IRIS_TREE 
    PATH_SUFFIXES lib)

FIND_PATH(IRIS_F_INCLUDE xmpp.h
    PATHS /usr /usr/local ${IRIS_TREE}
    ENV IRIS_TREE
    PATH_SUFFIXES include include/iris)
MESSAGE(STATUS "Iris include at: ${IRIS_F_INCLUDE}")

FIND_PATH(IRIS_F_SRC src.pro
    PATHS /usr/src/iris /usr/local/src/iris ${IRIS_TREE}
    ENV IRIS_TREE
    PATH_SUFFIXES src)
MESSAGE(STATUS "Iris source at: ${IRIS_F_SRC}")

IF ( IRIS_F_INCLUDE AND IRIS_F_LIBRARIES AND IRISNET_F_LIBRARIES )
    set(Iris_FOUND TRUE)
    message(STATUS "Found Iris: ${IRIS_F_LIBRARIES}")
    set(IRIS_LIBRARIES "${IRIS_F_LIBRARIES};${IRISNET_F_LIBRARIES}")

    GET_FILENAME_COMPONENT(IRIS_ABSOLUTE_INCLUDE ${IRIS_F_INCLUDE} ABSOLUTE)
    GET_FILENAME_COMPONENT(IRIS_INCLUDES ${IRIS_ABSOLUTE_INCLUDE} PATH)
#    message(STATUS "Found Iris includes: ${IRIS_INCLUDE_FULL}")
#    string(REGEX REPLACE "/[^/]*$" "" IRIS_INCLUDES ${IRIS_INCLUDE_FULL})

    # Iris could not work without src as include...
    # Maybe someday fixed, until that make it working
#    GET_FILENAME_COMPONENT(IRIS_ABSOLUTE_SRC ${IRIS_F_SRC} ABSOLUTE)
#    GET_FILENAME_COMPONENT(IRIS_SOURCE ${IRIS_ABSOLUTE_SRC} PATH)
    GET_FILENAME_COMPONENT(IRIS_ABSOLUTE_SRC ${IRIS_F_SRC} ABSOLUTE)
    LIST(APPEND IRIS_INCLUDES ${IRIS_ABSOLUTE_SRC})
    message(STATUS "Found Iris includes: ${IRIS_INCLUDES}")
ELSE  ( IRIS_F_INCLUDE AND IRIS_F_LIBRARIES AND IRISNET_F_LIBRARIES )
    IF (Iris_FIND_REQUIRED)
        message( SEND_ERROR "Could NOT find Iris")
    ELSE (Iris_FIND_REQUIRED)
        message(STATUS "Could NOT find Iris")
    ENDIF(Iris_FIND_REQUIRED)
ENDIF ( IRIS_F_INCLUDE AND IRIS_F_LIBRARIES AND IRISNET_F_LIBRARIES )

set(VERBOSE 1)
# overovani, zda IRIS funguje
# FIXME: neznamo jak overit, nasledujici proste a jednoduse nefunguje
# INCLUDE(CheckIncludeFileCXX)
# LIST(APPEND CMAKE_REQUIRED_INCLUDES ${QT_INCLUDES})
# LIST(APPEND CMAKE_REQUIRED_INCLUDES ${IRIS_INCLUDES})
# message(STATUS "Checking for iris include in paths ${CMAKE_REQUIRED_INCLUDES}")
# CHECK_INCLUDE_FILE_CXX(xmpp.h IRIS_XMPP_H)
# MARK_AS_ADVANCED(IRIS_XMPP_H)
# 
# IF (IRIS_XMPP_H)
# message(STATUS "Found xmpp.h")
# ELSE (IRIS_XMPP_H)
# message (SEND_ERROR "xmpp.h not found")
# ENDIF (IRIS_XMPP_H)

MARK_AS_ADVANCED( IRIS_INCLUDES IRIS_LIBRARIES )

# 
# set(IRIS_TREE "../../iris")
# set(IRIS_LIB_PATH ${IRIS_TREE}/lib)
# set(IRIS_INCLUDE_PATH ${IRIS_TREE}/include/iris)
# 
# 
# set(MANUAL_IRIS_PATH $ENV{IRIS_TREE})
# message(status "Environment is: ${MANUAL_IRIS_PATH}")
# IF   (MANUAL_IRIS_PATH)
#     list(APPEND CMAKE_REQUIRED_INCLUDES "${MANUAL_IRIS_PATH}/include")
#     list(APPEND CMAKE_REQUIRED_INCLUDES "${MANUAL_IRIS_PATH}/include/iris")
# ENDIF   (MANUAL_IRIS_PATH)
# 
# include(CheckIncludeFileCXX)
# #include(CheckIncludeFiles)
# message(STATUS "required_includes: ${CMAKE_REQUIRED_INCLUDES}")
# set(HEADER xmpp)
# IF("${HEADER}" MATCHES "^${HEADER}$")
#  message(STATUS "Matches ${HEADER}")
# ENDIF("${HEADER}" MATCHES "^${HEADER}$")
# 
# CHECK_INCLUDE_FILE_CXX(HEADER IRIS_INCLUDES)
# MESSAGE(STATUS "Iris include: ${IRIS_INCLUDES}")
# 
# 
# include(CheckLibraryExists)
# Check_library_exists(iris XMPP::Connector::havePeerAddress ${IRIS_LIB_PATH} IRIS_EXIST_CONNECTOR)
# MESSAGE(STATUS "Iris library: ${IRIS_EXIST_CONNECTOR}")
# 
# Check_library_exists(irisnet XMPP::NetInterface::name ${IRIS_LIB_PATH} IRISNET_EXIST_NETINTERFACE)
# MESSAGE(STATUS "Irisnet library: ${IRISNET_EXIST_NETINTERFACE}")
# 
# Check_library_exists(${LIB_IRIS} XMPP::Connector::havePeerAddress "" IRIS_EXIST_CONNECTOR2)
# MESSAGE(STATUS "Iris/find library: ${IRIS_EXIST_CONNECTOR2}")
# Check_library_exists(${LIB_IRIS} XMPP::NetInterface::name "" IRISNET_EXIST_NETINTERFACE2)
# MESSAGE(STATUS "Iris/find library: ${IRISNET_EXIST_NETINTERFACE2}")
# 
# 
# 
# set(IRIS_LIBS 
#     iris
#     irisnet
#     )
# set(IRIS_INCLUDE ${IRIS_TREE}/include/iris ${IRIS_TREE}/src)
# 
# set(Iris_FOUND true)
# 
# mark_as_advanced(IRIS_INCLUDE IRIS_LIBS)
# 
