cmake_minimum_required(VERSION 3.0)

include_directories(. ../. .././External)
set(CMAKE_CXX_STANDARD 14)

if (WEB)
  add_definitions(-D_WEB)
  include_directories(.././External/Web)
  set(PLATFORM_SRC_QTUtil 
            )
elseif (MSVC)
  add_definitions(-D_WINDOWS)
  include_directories(.././External/Windows)
  set(PLATFORM_SRC_QTUtil 
            )
elseif (APPLE)
  add_definitions(-D_MACOS)
  include_directories(/usr/include/freetype2)
  set(PLATFORM_SRC_QTUtil 
            )
elseif (UNIX)
  add_definitions(-D_LINUX)
  include_directories(/usr/include/freetype2)
  set(PLATFORM_SRC_QTUtil 
            )
endif()

set(GENERIC_SRC_QTUtil 
            ./DraggableList.cpp
            ./ExpandableList.cpp
            ./FocusWatcher.cpp
            ./LerpVar.cpp
            ./QTUtil.cpp
            )
add_library(QTUtil ${GENERIC_SRC_QTUtil} ${PLATFORM_SRC_QTUtil} )