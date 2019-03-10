cmake_minimum_required(VERSION 3.8)

if ($ENV{TRAVIS_BUILD_NUMBER})
    project(elegram_server VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
else ()
    project(elegram_server VERSION 0.0.1)
endif ()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
find_package(Boost COMPONENTS system program_options log unit_test_framework REQUIRED)
find_package(Threads)
find_package(OpenSSL)
include_directories(${Boost_INCLUDE_DIRS})


################################################################################
## include libpqxx directories
#execute_process(COMMAND git submodule update --init -- libs/libpqxx
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
#set(LIBPQXX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/libpqxx/include
#        CACHE PATH "path to libpqxx includes")
#add_subdirectory(libs/libpqxx)
#include(libs/libpqxx/CMakeLists.txt)
#include_directories(libs/libpqxx/include)
################################################################################

add_library(messages_pb2_lib STATIC
        src/proto/messages.pb.cc)


# remove -Werror flags cause of many
set_target_properties(messages_pb2_lib PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        )

add_library(postgres_conn_lib STATIC
        src/storage/postgresql_storage.cpp
        src/hashing/hashing.cpp
        src/storage/pimpl/lazy_connection_wrapper.cpp
        )

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/exceptions/exceptions.cpp
        src/server/server.cpp
        src/server/session.cpp
        src/jobs/jobs.cpp
        )


message("${CMAKE_CXX_COMPILER_ID}")
macro(set_warning_flags target std_ver)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message("!!! GCC DETECTED !!!!")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fuse-ld=gold -fsanitize=address -Wall -Wextra -pedantic")
        set_target_properties(${target} PROPERTIES
                CXX_STANDARD ${std_ver}
                CXX_STANDARD_REQUIRED ON
                LINK_LIBRARIES pthread
                COMPILE_OPTIONS "-O0;-Wall;-Wextra;-Werror;-Wpedantic;-fsanitize=address;-ggdb"
                )
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message("!!! CLANG DETECTED !!!!")
        set_target_properties(${target} PROPERTIES
                CXX_STANDARD ${std_ver}
                CXX_STANDARD_REQUIRED ON
                COMPILE_OPTIONS "-O0;-Wall;-Wextra;-Werror;-pedantic"
                )
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message("!!! MSVC DETECTED !!!!")
        set_target_properties(${target} PROPERTIES
                CXX_STANDARD ${std_ver}
                CXX_STANDARD_REQUIRED ON
                COMPILE_OPTIONS "/W4"
                )
    else ()
        message(FATAL_ERROR "!!! NO COMPILER DETECTED, WARNING FLAGS NOT SETTED !!!!")
    endif ()
endmacro()

target_link_libraries(messages_pb2_lib
        src/protobuf
        )

set_warning_flags(postgres_conn_lib 14)
target_link_libraries(postgres_conn_lib
        crypto
        pq
        pqxx
        )

set_warning_flags(${PROJECT_NAME} 17)
target_link_libraries(${PROJECT_NAME}
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        messages_pb2_lib ## todo maybe better static ?
        postgres_conn_lib
        )

# install to bin folder our binaries
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# generate .deb package
set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT noname@nomail.no)

# include will generate proper code for .deb generation
include(CPack)
