cmake_minimum_required(VERSION 3.10)
project(Tube)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_library(glad STATIC src/gl.c)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

# Tentar encontrar GLM
find_package(glm QUIET)

add_executable(Tube src/main.cpp)  

target_include_directories(Tube PRIVATE
    ${GLFW_INCLUDE_DIRS}
    include
)

target_link_libraries(Tube
    glad
    ${GLFW_LIBRARIES}
    m
    dl
)

# Linkar GLM se encontrado
if(glm_FOUND)
    target_link_libraries(Tube glm::glm)
endif()

if (WIN32)
    target_link_libraries(Tube opengl32)
endif()

add_custom_command(TARGET Tube POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/wall.jpg
    $<TARGET_FILE_DIR:Tube>
)