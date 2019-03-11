cmake_minimum_required(VERSION 3.8)
set(QT_MINIMUM_VERSION 5.9.0)

if (DEFINED ENV{TRAVIS_BUILD_NUMBER} AND DEFINED ENV{VERSION_MAJOR} AND DEFINED ENV{VERSION_MINOR})
    project(elegram_client VERSION $ENV{VERSION_MAJOR}.$ENV{VERSION_MINOR}.$ENV{TRAVIS_BUILD_NUMBER})
else ()
    project(elegram_client VERSION 0.0.1)
endif ()

set(APPLICATION_NAME ${PROJECT_NAME})
set(APPLICATION_VERSION ${VERSION})
set(APPLICATION_ORGANIZATION "mkvdv")
set(APPLICATION_ORGANIZATION_DOMAIN "https://github.com/mkvdv/")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(QT_USE_QTMAIN TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Qt5 COMPONENTS Core Qml Quick Network Gui CONFIG REQUIRED)

if (NOT DEFINED ENV{QTDIR})
    MESSAGE(STATUS "QTDIR environment variable not set. Be sure to set the QTDIR
	cmake cache variable to the Qt root install location")
endif ()
set(QTDIR $ENV{QTDIR} CACHE STRING "Qt install path")
list(APPEND CMAKE_PREFIX_PATH ${QTDIR})

# Test for supported Qt version
find_program(QMAKE_EXECUTABLE NAMES qmake HINTS ${QTDIR} ENV QTDIR PATH_SUFFIXES bin)
execute_process(COMMAND ${QMAKE_EXECUTABLE} -query QT_VERSION OUTPUT_VARIABLE QT_VERSION)
if (QT_VERSION VERSION_LESS QT_MINIMUM_VERSION)
    MESSAGE(FATAL_ERROR "Minimum supported Qt version: ${QT_MINIMUM_VERSION}. Installed version: ${QT_VERSION}")
endif ()

add_definitions(${QT_DEFINITIONS})

qt5_add_resources(QT_RESOURCES qml.qrc)

include_directories(${INCLUDE_DIRS})

message("${CMAKE_CXX_COMPILER_ID}")
macro(set_warning_flags target std_ver)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message("!!!PUBLIC  GCC DETECTED !!!!")
        set_target_properties(${target} PROPERTIES
                CXX_STANDARD ${std_ver}
                CXX_STANDARD_REQUIRED ON
                LINK_LIBRARIES pthread
                INSTALL_RPATH_USE_LINK_PATH TRUE
                COMPILE_OPTIONS "-O2;-Wall;-Wextra;-Wpedantic;-ggdb"
                )
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message("!!! CLANG DETECTED !!!!")
        set_target_properties(${target} PROPERTIES
                CXX_STANDARD ${std_ver}
                CXX_STANDARD_REQUIRED ON
                INSTALL_RPATH_USE_LINK_PATH TRUE
                COMPILE_OPTIONS "-O2;-Wall;-Wextra;-pedantic"
                )
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message("!!! MSVC DETECTED !!!!")
        set_target_properties(${target} PROPERTIES
                CXX_STANDARD ${std_ver}
                CXX_STANDARD_REQUIRED ON
                INSTALL_RPATH_USE_LINK_PATH TRUE
                COMPILE_OPTIONS "/W4"
                )
    else ()
        message(FATAL_ERROR "!!! NO COMPILER DETECTED, WARNING FLAGS NOT SETTED !!!!")
    endif ()
endmacro()

set(SOURCES src/main.cpp
        src/backend_facade.cpp
        src/network/network_session.cpp
        src/model/chatslist.cpp
        src/model/chatsmodel.cpp
        src/model/contactslist.cpp
        src/model/contactsmodel.cpp
        src/model/messageslist.cpp
        src/model/messagesmodel.cpp
        src/message_processing/messagefactory.cpp
        src/message_processing/messageparser.cpp
        src/jobs/jobs.cpp
        src/protobuf/messages.pb.cc
        src/backend.cpp
        src/jobs/jobsutils.cpp
        )

add_executable(${PROJECT_NAME} ${SOURCES} ${QT_RESOURCES})
set_warning_flags(${PROJECT_NAME} 14)
target_link_libraries(${PROJECT_NAME}
        PUBLIC Qt5::Core
        PUBLIC Qt5::Qml
        PUBLIC Qt5::Quick
        PUBLIC Qt5::Gui
        PUBLIC Qt5::Network
        PUBLIC protobuf)

# install to bin folder our binaries
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
INCLUDE(InstallRequiredSystemLibraries)


# generate .deb package
set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT makeevvolodja@gmail.com)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

# include will generate proper code for .deb generation
include(CPack)
