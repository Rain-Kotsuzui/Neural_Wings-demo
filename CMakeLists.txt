cmake_minimum_required(VERSION 3.11)

project(Neural_Wings-demo)

if (EMSCRIPTEN)
    set(PLATFORM "Web")
    add_definitions(-DPLATFORM_WEB)
else()
    set(PLATFORM "Desktop")
    add_definitions(-DPLATFORM_DESKTOP)
endif()

message(STATUS "当前平台: ${PLATFORM}")

include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
)
FetchContent_MakeAvailable(raylib)


FetchContent_Declare(
  enet
  GIT_REPOSITORY https://github.com/lsalzman/enet.git
  GIT_TAG master
)
FetchContent_MakeAvailable(enet)


set(SOURCES src/main.cpp)


if (PLATFORM STREQUAL "Web")
    add_executable(${PROJECT_NAME} ${SOURCES})

    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

    target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
    target_link_options(${PROJECT_NAME} PRIVATE 
        "-sUSE_GLFW=3" 
        "-sASYNCIFY"
        "--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/src/shell.html"
    )

    #  resources/ 打包
    # target_link_options(${PROJECT_NAME} PRIVATE "--preload-file" "${CMAKE_SOURCE_DIR}/resources@resources")

else()
    
    add_executable(${PROJECT_NAME} ${SOURCES})
    
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib enet)
endif()