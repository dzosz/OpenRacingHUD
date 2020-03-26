cmake_minimum_required(VERSION 3.5)

project(f1overlay LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32 OR MINGW)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    list(APPEND CMAKE_PREFIX_PATH "/home/tommy/apps/mxe/usr/x86_64-w64-mingw32.static/lib")
    include_directories("/home/tommy/apps/mxe/usr/x86_64-w64-mingw32.static/include/")
    SET(CMAKE_SHARED_LIBRARY_SUFFIX .dll)
    SET(CMAKE_SHARED_LIBRARY_PREFIX "")
    SET(CMAKE_EXECUTABLE_SUFFIX .exe)
    set(CPACK_SYSTEM_NAME "Windows")
elseif (UNIX)
    # dont strip rpath on installed libs
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif ()


#find_package(Python2 COMPONENTS Development REQUIRED)
SET(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})

message("include ${Python2_INCLUDE_DIRS}")


find_package(Qt5 COMPONENTS Gui REQUIRED)

SET(sources
    main.cpp
    wheelslipwindow.cpp
    f1_data_receiver.cpp
    pluginselectorwindow.cpp
)

add_executable(f1overlay
    ${sources}
)

#add_library(f1-overlay-lib SHARED
#    #c_interface.cpp
#    ${sources}
#)

#target_link_libraries(f1overlay PRIVATE Qt5::Gui Python2::Module ${Python2_LIBRARIES} )
target_link_libraries(f1overlay PRIVATE Qt5::Gui  python2.7)
#target_link_libraries(f1-overlay-lib PRIVATE  Qt5::Gui python2.7)

configure_file(f1_overlay.py  f1_overlay.py COPYONLY)
configure_file(f1_2019_telemetry_reader.py  f1_2019_telemetry_reader.py COPYONLY)

install(FILES f1_overlay.py f1_2019_telemetry_reader.py DESTINATION bin)
install(TARGETS f1overlay DESTINATION bin)
#install(TARGETS f1-overlay-lib DESTINATION bin)

set(CPACK_SOURCE_GENERATOR "ZIP")
#set(CPACK_OUTPUT_FILE_PREFIX "")
set(CPACK_GENERATOR "ZIP" CACHE STRING "Generators to support. semi-colon delimited list")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_SYSTEM_NAME}") # no version

include(CPack)
