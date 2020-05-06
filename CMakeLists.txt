cmake_minimum_required(VERSION 3.5)

project(OpenRacingHUD LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
enable_testing()

if (WIN32 OR MINGW)
    # cross compiling settings
    # manually create symlink to your MXE/usr dir
    set(SYSROOT "${CMAKE_SOURCE_DIR}/usr")
    set(ROOT_PATH "${SYSROOT}/x86_64-w64-mingw32.static")

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

    # windows static library issue workaround for 'Module "QtQuick" plugin "qtquick2plugin" not found'
    find_library(QtQuick qtquick2plugin PATHS ${ROOT_PATH}/qt5/qml/QtQuick.2/)
    find_library(QtQuickQmlWorker Qt5QmlWorkerScript PATHS ${ROOT_PATH}/qt5/lib/)
    find_library(QtQuickWindow windowplugin PATHS ${ROOT_PATH}/qt5/qml/QtQuick/Window.2/)    
    SET(PLATFORM_SPECIFIC_LIBS ${QtQuick} ${QtQuickWindow} ${QtQuickQmlWorker})

    set(Python2_LIBRARIES "${CMAKE_SOURCE_DIR}/dep/python2.7-mingw/lib64/libpython27.a")

    # todo figure out why mingw mxe builds depend on python27.dll
    set(Python2_WIN_SHARED_LIB "${CMAKE_SOURCE_DIR}/dep/python2.7-mingw/lib64/python27.dll")
    install(FILES ${Python2_WIN_SHARED_LIB} DESTINATION bin)

    #find_library(QUICK_PLUGIN qtquick2plugin qtquickcontrols2plugin qquicklayoutsplugin qmlsettingsplugin widgetsplugin PATHS ${ROOT_PATH}/qt5/qml/QtQuick.2/ ${ROOT_PATH}/qt5/qml/QtQuick/ ${ROOT_PATH}/qt5/qml/Qt/labs/settings/ ${ROOT_PATH}/qt5/qml/QtQuick/PrivateWidgets/)
    # -lqxcb -lqtquick2plugin -lqtquickcontrolsplugin -ldialogplugin -lqquicklayoutsplugin -lwindowplugin -lqmlsettingsplugin -lwidgetsplugin -lqxcb-glx-integration -lqevdevkeyboardplugin -lqevdevmouseplugin"
elseif (UNIX)
    # dont strip rpath on installed libs
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    set(PLATFORM_SPECIFIC_LIBS "")
    find_package(Python2 COMPONENTS Development REQUIRED)
endif ()

# IDE completion
set(QML_IMPORT_PATH ${CMAKE_SOURCE_DIR}/qml CACHE STRING "" FORCE)
SET(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})

find_package(Qt5 COMPONENTS Widgets Quick Core Qml REQUIRED)

add_executable(qmloverlay
    mainqml.cpp
    python_runner.cpp
    plugin_selector_window.cpp
)

#add_custom_target(copy-mingw-shared-libs ALL
#    COMMAND bash -c "export LDD=\"${ROOT_PATH}/bin/peldd.exe\"; export TARGET_BIN=\"$<TARGET_FILE:qmloverlay>\"; export SEARCH_DIR=\"${CMAKE_SOURCE_DIR}\" ; export TARGET_DIR=\"${CMAKE_BINARY_DIR}\"; bash ${CMAKE_SOURCE_DIR}/scripts/copy_mingw_deps.sh; export TARGET_DIR=\"${CMAKE_SOURCE_DIR}/bin\"; bash ${CMAKE_SOURCE_DIR}/scripts/copy_mingw_deps.sh"
#    DEPENDS qmloverlay
#    VERBATIM)

add_executable(tests
    tests/python_runner_tests.cpp
    python_runner.cpp
)

# fake target - show plugins in IDE
FILE(GLOB_RECURSE QML_SOURCES qml/*.qml)
ADD_CUSTOM_TARGET(plugins SOURCES ${QML_SOURCES})
FILE(GLOB_RECURSE PYTHON_SCRIPTS scripts/*.py)
ADD_CUSTOM_TARGET(scripts SOURCES ${PYTHON_SCRIPTS})

# symlink dependencies
add_custom_target(copy-qml-files ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/qml ${CMAKE_BINARY_DIR}/qml
    DEPENDS ${MY_TARGET})

add_custom_target(copy-scripts ALL
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/scripts ${CMAKE_BINARY_DIR}/scripts
    DEPENDS ${MY_TARGET})

include_directories(${Qt5Gui_INCLUDE_DIRS})

target_link_libraries(qmloverlay     PRIVATE Qt5::Core Qt5::Qml Qt5::Quick Qt5::Widgets ${PLATFORM_SPECIFIC_LIBS} ${Python2_LIBRARIES})
target_link_libraries(tests     PRIVATE Qt5::Core ${Python2_LIBRARIES})
add_test(alltests tests)

install(TARGETS qmloverlay DESTINATION bin)
install(DIRECTORY scripts DESTINATION bin FILES_MATCHING PATTERN "*.py")
install(DIRECTORY qml DESTINATION bin FILES_MATCHING PATTERN "*.qml")

set(CPACK_SOURCE_GENERATOR "ZIP")
#set(CPACK_OUTPUT_FILE_PREFIX "")
set(CPACK_GENERATOR "ZIP" CACHE STRING "Generators to support. semi-colon delimited list")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_SYSTEM_NAME}") # no version

include(CPack)
