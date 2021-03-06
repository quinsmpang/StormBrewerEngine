cmake_minimum_required(VERSION 3.0)

include_directories(. ../. .././External)
set(CMAKE_CXX_STANDARD 14)

if (WEB)
  add_definitions(-D_WEB)
  include_directories(.././External/Web)
  set(PLATFORM_SRC_GameServer 
            )
elseif (MSVC)
  add_definitions(-D_WINDOWS)
  include_directories(.././External/Windows)
  set(PLATFORM_SRC_GameServer 
            )
elseif (APPLE)
  add_definitions(-D_MACOS)
  include_directories(/usr/include/freetype2)
  set(PLATFORM_SRC_GameServer 
            )
elseif (UNIX)
  add_definitions(-D_LINUX)
  include_directories(/usr/include/freetype2)
  set(PLATFORM_SRC_GameServer 
            )
endif()

set(GENERIC_SRC_GameServer 
            ./GameClient.cpp
            ./GameEventReconciler.cpp
            ./GameInstance.cpp
            ./GameInstanceManager.cpp
            ./GameServer.cpp
            )
add_library(GameServer ${GENERIC_SRC_GameServer} ${PLATFORM_SRC_GameServer} )