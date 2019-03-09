##########################################################
cmake_minimum_required(VERSION 3.8)
if ($ENV{TRAVIS_BUILD_NUMBER})
    project(elegram_client VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
else ()
    project(elegram_client VERSION 0.0.1)
endif ()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb -Wall -Wextra -pedantic \
-pedantic-errors -pedantic ")

set(CMAKE_CXX_STANDARD 14)

find_package(Qt5 COMPONENTS Qml Quick Network Gui REQUIRED)
qt5_add_resources(QT_RESOURCES qml.qrc)

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
        protobuf/messages.pb.cc
        src/backend.cpp
        src/jobs/jobsutils.cpp
        )

add_executable(${PROJECT_NAME} ${SOURCES} ${QT_RESOURCES})

target_link_libraries(${PROJECT_NAME} Qt5::Quick)
target_link_libraries(${PROJECT_NAME} Qt5::Qml)
target_link_libraries(${PROJECT_NAME} Qt5::Gui)
target_link_libraries(${PROJECT_NAME} Qt5::Network)
target_link_libraries(${PROJECT_NAME} protobuf)

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
