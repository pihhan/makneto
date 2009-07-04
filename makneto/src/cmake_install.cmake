# Install script for directory: /home/pihhan/Projekty/makneto/branches/pihhan/makneto/src

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Debug")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    IF(EXISTS "$ENV{DESTDIR}/usr/local/bin/makneto")
      FILE(RPATH_CHECK
           FILE "$ENV{DESTDIR}/usr/local/bin/makneto"
           RPATH "/usr/local/lib")
    ENDIF(EXISTS "$ENV{DESTDIR}/usr/local/bin/makneto")
    FILE(INSTALL DESTINATION "/usr/local/bin" TYPE EXECUTABLE FILES "/home/pihhan/Projekty/makneto/branches/pihhan/makneto/src/makneto")
    IF(EXISTS "$ENV{DESTDIR}/usr/local/bin/makneto")
      FILE(RPATH_CHANGE
           FILE "$ENV{DESTDIR}/usr/local/bin/makneto"
           OLD_RPATH "/home/pihhan/Projekty/makneto/branches/pihhan/makneto/src/CMakeFiles/makneto.dir/cutestuff/network:/home/pihhan/Projekty/makneto/branches/pihhan/makneto/src/CMakeFiles/makneto.dir/cutestuff/util:/home/pihhan/Projekty/makneto/branches/pihhan/makneto/src/CMakeFiles/makneto.dir/cutestuff/legacy"
           NEW_RPATH "/usr/local/lib")
      IF(CMAKE_INSTALL_DO_STRIP)
        EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/usr/local/bin/makneto")
      ENDIF(CMAKE_INSTALL_DO_STRIP)
    ENDIF(EXISTS "$ENV{DESTDIR}/usr/local/bin/makneto")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")
  FILE(INSTALL DESTINATION "/usr/local/share/applications/kde4" TYPE FILE FILES "/home/pihhan/Projekty/makneto/branches/pihhan/makneto/src/makneto.desktop")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")
  FILE(INSTALL DESTINATION "/usr/local/share/config.kcfg" TYPE FILE FILES "/home/pihhan/Projekty/makneto/branches/pihhan/makneto/src/makneto.kcfg")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")
  FILE(INSTALL DESTINATION "/usr/local/share/apps/makneto" TYPE FILE FILES "/home/pihhan/Projekty/makneto/branches/pihhan/makneto/src/maknetoui.rc")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/pihhan/Projekty/makneto/branches/pihhan/makneto/src/icons/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

