cmake_minimum_required(VERSION 3.8)

if (DEFINED ENV{TRAVIS_BUILD_NUMBER} AND DEFINED ENV{VERSION_MAJOR} AND DEFINED ENV{VERSION_MINOR})
    project(elegram_server VERSION $ENV{VERSION_MAJOR}.$ENV{VERSION_MINOR}.$ENV{TRAVIS_BUILD_NUMBER})
else ()
    project(elegram_server VERSION 0.0.1)
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
find_package(Boost COMPONENTS system program_options log REQUIRED)
find_package(Threads)
find_package(OpenSSL)
include_directories(${Boost_INCLUDE_DIRS})

add_library(messages_pb2_lib STATIC
        src/proto/messages.pb.cc)

# remove -Werror flags cause of many warnings
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
        set_target_properties(${target} PROPERTIES
                CXX_STANDARD ${std_ver}
                CXX_STANDARD_REQUIRED ON
                LINK_LIBRARIES pthread
                COMPILE_OPTIONS "-O2;-Wall;-Wextra;-Werror;-Wpedantic;-ggdb"
                )
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message("!!! CLANG DETECTED !!!!")
        set_target_properties(${target} PROPERTIES
                CXX_STANDARD ${std_ver}
                CXX_STANDARD_REQUIRED ON
                COMPILE_OPTIONS "-O2;-Wall;-Wextra;-Werror;-pedantic"
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
        protobuf
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
        messages_pb2_lib
        postgres_conn_lib
        )

# install to bin folder our binaries
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# generate .deb package
set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT makeevvolodja@gmail.com)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

# include will generate proper code for .deb generation
include(CPack)
