cmake_minimum_required(VERSION 3.5)

project(f1overlay LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32 OR MINGW)
    # cross compiling settings
    set(SYSROOT "/home/tommy/apps/mxe/usr")
    set(ROOT_PATH "${SYSROOT}/x86_64-w64-mingw32.static")

    #set(SYSROOT "${CMAKE_SOURCE_DIRECTORY}")
    #set(ROOT_PATH "${SYSROOT}/usr/x86_64-w64-mingw32.static") # todo create with symlink?

    set(CMAKE_FIND_ROOT_PATH "${ROOT_PATH}")
    SET(CMAKE_SYSROOT "${SYSROOT}")
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    #set(CMAKE_PREFIX_PATH "${ROOT_PATH}/lib")
    #list(APPEND CMAKE_PREFIX_PATH "${ROOT_PATH}/lib")
    #list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/dep/python2.7-mingw/")
    #link_directories(BEFORE SYSTEM "${ROOT_PATH}/lib")
    #set(CMAKE_LIBRARY_PATH "${ROOT_PATH}/lib")
    #list(APPEND CMAKE_LIBRARY_PATH "${ROOT_PATH}/lib")
    include_directories(BEFORE SYSTEM "${CMAKE_SOURCE_DIR}/dep/python2.7-mingw/include")
    link_directories(BEFORE SYSTEM "${CMAKE_SOURCE_DIR}/dep/python2.7-mingw/lib64")
    SET(CMAKE_SHARED_LIBRARY_SUFFIX .dll)
    SET(CMAKE_SHARED_LIBRARY_PREFIX "")
    SET(CMAKE_EXECUTABLE_SUFFIX .exe)
    set(CPACK_SYSTEM_NAME "Windows")
    set(CONDITIONAL_WIN32_FLAG WIN32)
    set(CONDITIONAL_WIN32_SOURCES asettosharedmemoryreader.cpp)
elseif (UNIX)
    # dont strip rpath on installed libs
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    set(LINUX_LINK_QT Qt5::Gui)
endif ()

set(QML_IMPORT_PATH ${CMAKE_SOURCE_DIR}/plugins CACHE STRING "" FORCE)
SET(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})

#find_package(Python2 COMPONENTS Development REQUIRED)
find_package(Qt5 COMPONENTS Widgets Quick Gui Core Qml REQUIRED)
find_package(Qt5QuickCompiler)

#qtquick_compiler_add_resources(RESOURCES example.qrc)
#qt5_use_modules(myapp Quick Core Network)
SET(sources
    f1_data_receiver.cpp
    #${CONDITIONAL_WIN32_SOURCES}
)


#add_executable(f1overlay
#    #${CONDITIONAL_WIN32_FLAG}
#    ${sources}
#    main.cpp
#)

add_executable(qmloverlay
    #${CONDITIONAL_WIN32_FLAG}
    mainqml.cpp
    python_runner.cpp
    plugin_selector_window.cpp

)

add_executable(tests
    #${CONDITIONAL_WIN32_FLAG}
    tests/python_runner_tests.cpp
    python_runner.cpp
)


#add_library(overlay SHARED
#    wheelslipwindow.cpp
#    plugin_selector_window.cpp
#    overlay_interface.cpp
#)

# fake target
#set(QML_SOURCES glob)
#FILE(GLOB QML_SOURCES plugins/*.qml)
# FIXME why did i add this?
#ADD_CUSTOM_TARGET(plugins SOURCES ${QML_SOURCES})

include_directories(${Qt5Gui_INCLUDE_DIRS})

#target_link_libraries(f1overlay       PRIVATE ${LINUX_LINK_QT} python2.7 overlay)
#target_link_libraries(overlay     PRIVATE Qt5::Gui python2.7 )
target_link_libraries(qmloverlay     PRIVATE Qt5::Core Qt5::Qml Qt5::Quick Qt5::Gui Qt5::Widgets python2.7 )
target_link_libraries(tests     PRIVATE Qt5::Core Qt5::Qml Qt5::Quick Qt5::Gui  python2.7 )


configure_file(overlay.py overlay.py COPYONLY)
configure_file(f1_2019_telemetry_reader.py  f1_2019_telemetry_reader.py COPYONLY)

install(FILES overlay.py f1_2019_telemetry_reader.py DESTINATION bin)
#install(TARGETS f1overlay DESTINATION bin)
#install(TARGETS overlay DESTINATION bin)

set(CPACK_SOURCE_GENERATOR "ZIP")
#set(CPACK_OUTPUT_FILE_PREFIX "")
set(CPACK_GENERATOR "ZIP" CACHE STRING "Generators to support. semi-colon delimited list")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_SYSTEM_NAME}") # no version

include(CPack)
