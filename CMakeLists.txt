cmake_minimum_required(VERSION 4.0)
project(YAGL-Engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure compiler-specific settings
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif ()

# GLFW build settings
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# Add third-party libraries
add_subdirectory(third_party/glfw)

# spdlog logging library
add_subdirectory(third_party/spdlog)

# GLM mathematics library (header-only)
add_subdirectory(third_party/glm)

# GLAD OpenGL loader (manual setup)
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/CMakeLists.txt")
  add_subdirectory(third_party/glad)
  set(GLAD_LIBRARIES glad)
else ()
  # Manual GLAD setup if no CMakeLists.txt exists
  file(GLOB GLAD_SOURCES "third_party/glad/src/*.c")
  add_library(glad STATIC ${GLAD_SOURCES})
  target_include_directories(glad PUBLIC third_party/glad/include)
  set(GLAD_LIBRARIES glad)
endif ()

# Include additional header directories if needed
include_directories(third_party/include)

# Collect source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADERS "src/*.h" "src/*.hpp")

# Create executable target
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link required libraries
target_link_libraries(${PROJECT_NAME}
        glfw
        ${GLAD_LIBRARIES}
        spdlog::spdlog
        glm::glm
)

# For future shaders/textures when needed)
# file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

# CLion/MinGW specific settings
if (WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # MinGW specific flags
  target_link_libraries(${PROJECT_NAME} -static-libgcc -static-libstdc++)
endif ()
