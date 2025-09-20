cmake_minimum_required(VERSION 3.10)
project(VulkanGLFWProject)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Subdirectorios
add_subdirectory(external/glfw)
add_subdirectory(external/glm)
add_compile_definitions(DEBUG=true)

# Headers propios
include_directories(
    external/glfw/include
    external/imgui
    core
    complementary
    types
)

# Vulkan según SO
if(WIN32)
    message(STATUS "Configurando Vulkan para Windows (LunarG SDK)")
    include_directories("$ENV{VULKAN_SDK}/Include")
    link_directories("$ENV{VULKAN_SDK}/Lib")
    set(VULKAN_LIB vulkan-1)
else()
    message(STATUS "Configurando Vulkan para Linux (pacman)")
    find_package(Vulkan REQUIRED)
    set(VULKAN_LIB Vulkan::Vulkan)
endif()

file(GLOB CORE_SOURCES
    core/*.cpp
)

file(GLOB COMPLEMENTARY_SOURCES
    complementary/*.cpp
)

# Ejecutable
add_executable(${PROJECT_NAME} 
    main.cpp
    ${CORE_SOURCES}
    ${COMPLEMENTARY_SOURCES}
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_vulkan.cpp
)

# Linkeo
target_link_libraries(${PROJECT_NAME}
    glfw
    glm
    ${VULKAN_LIB}
)
