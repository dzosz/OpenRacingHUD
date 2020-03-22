cmake_minimum_required(VERSION 3.5)

project(f1-overlay LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# dont strip rpath on installed libs
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

find_package(Qt5 COMPONENTS Widgets Gui REQUIRED)

SET(sources
    main.cpp
    wheelslipwindow.cpp
    )

  add_executable(f1-overlay
    ${sources}
  )
  add_library(f1-overlay-lib SHARED
    c_interface.cpp
    ${sources}
)

target_link_libraries(f1-overlay PRIVATE  Qt5::Gui)
target_link_libraries(f1-overlay-lib PRIVATE  Qt5::Gui)


install(FILES f1_overlay.py DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(TARGETS f1-overlay-lib DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(TARGETS f1-overlay DESTINATION ${CMAKE_SOURCE_DIR}/bin)
