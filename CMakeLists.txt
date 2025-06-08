cmake_minimum_required(VERSION 3.10)
project(VulkanGLFWProject)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Agregamos GLFW y GLM como subdirectorios
add_subdirectory(external/glfw)
add_subdirectory(external/glm)

# Incluir headers
include_directories(
    external/glfw/include
    "$ENV{VULKAN_SDK}/Include"
    core
    complementary
    types
)


# Vincular directorios de librerías
link_directories("$ENV{VULKAN_SDK}/Lib")

# Ejecutable
add_executable(${PROJECT_NAME} 
main.cpp
)

# Linkeo
target_link_libraries(${PROJECT_NAME}
    glfw
    vulkan-1
    glm
)
