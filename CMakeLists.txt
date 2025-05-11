cmake_minimum_required(VERSION 3.16)
project(GL_Portal)

set(CMAKE_CXX_STANDARD 17)

find_package(GLEW   REQUIRED)
find_package(glfw3  REQUIRED)
find_package(OpenGL REQUIRED)

set(SRC_DIR      ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR  ${CMAKE_SOURCE_DIR}/include)
set(IMGUI_DIR    ${CMAKE_SOURCE_DIR}/external/imgui)
set(ASSIMP_DIR ${CMAKE_SOURCE_DIR}/external/assimp)
add_subdirectory(${ASSIMP_DIR})

file(GLOB_RECURSE CPP_SOURCES CONFIGURE_DEPENDS
     ${SRC_DIR}/*.cpp)

set(GLAD_SOURCE  ${SRC_DIR}/glad/glad.c)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

set(ALL_SOURCE_FILES
    ${CPP_SOURCES}
    ${GLAD_SOURCE}
    ${IMGUI_SOURCES}
)

set(ALL_INCLUDE_DIRS
    ${INCLUDE_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${ASSIMP_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/stb  
    ${GLEW_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${OPENGL_EGL_INCLUDE_DIRS}
)

set(ALL_LIBRARIES
    dl
    pthread
    GLEW::GLEW
    glfw
    OpenGL::GL
    OpenGL::OpenGL
    OpenGL::GLU
    OpenGL::GLX
    OpenGL::EGL
    assimp
)

set(ALL_COMPILE_DEFS
    -DWINDOW_NAME="${PROJECT_NAME}"
    $<$<CONFIG:Debug>:-DDEBUG_MOUSE_POS>
)

set(ALL_COMPILE_OPTS
    -Wpessimizing-move
    -Wredundant-move
)

add_executable(${PROJECT_NAME} ${ALL_SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${ALL_INCLUDE_DIRS})
target_link_libraries      (${PROJECT_NAME} PUBLIC ${ALL_LIBRARIES})
target_compile_definitions (${PROJECT_NAME} PUBLIC ${ALL_COMPILE_DEFS})
target_compile_options     (${PROJECT_NAME} PUBLIC ${ALL_COMPILE_OPTS})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
