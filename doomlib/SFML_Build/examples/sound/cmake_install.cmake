# Install script for directory: C:/Users/Paul/Documents/GitHub/crispy-doom/doomlib/SFML/examples/sound

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/SFML")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "examples" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./examples/sound" TYPE EXECUTABLE FILES "C:/Users/Paul/Documents/GitHub/crispy-doom/doomlib/SFML_Build/examples/sound/Debug/sound-d.exe")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./examples/sound" TYPE EXECUTABLE FILES "C:/Users/Paul/Documents/GitHub/crispy-doom/doomlib/SFML_Build/examples/sound/Release/sound.exe")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./examples/sound" TYPE EXECUTABLE FILES "C:/Users/Paul/Documents/GitHub/crispy-doom/doomlib/SFML_Build/examples/sound/MinSizeRel/sound.exe")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./examples/sound" TYPE EXECUTABLE FILES "C:/Users/Paul/Documents/GitHub/crispy-doom/doomlib/SFML_Build/examples/sound/RelWithDebInfo/sound.exe")
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "examples" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./examples/sound" TYPE FILE FILES "C:/Users/Paul/Documents/GitHub/crispy-doom/doomlib/SFML/examples/sound/Sound.cpp")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "examples" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./examples/sound" TYPE DIRECTORY FILES "C:/Users/Paul/Documents/GitHub/crispy-doom/doomlib/SFML/examples/sound/resources")
endif()

