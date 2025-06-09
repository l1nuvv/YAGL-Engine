cmake_minimum_required(VERSION 3.16)
project(YAGL-Engine)

# Подавляем предупреждение о неиспользуемых переменных vcpkg
if (DEFINED VCPKG_TARGET_TRIPLET)
  # Переменная определена, но мы ее не используем в этом проекте
  # так как используем git submodules для зависимостей
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-Wall -Wextra -pedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(/W4)
endif ()

# GLFW build settings
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# spdlog build settings - исправленные настройки
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "" FORCE)
set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_ALL OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(SPDLOG_ENABLE_PCH ON CACHE BOOL "" FORCE)

# GLM build settings
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_INSTALL OFF CACHE BOOL "" FORCE)

# ====== Third-party ======
add_subdirectory(third_party/glfw)
add_subdirectory(third_party/glm)
add_subdirectory(third_party/spdlog)

# GLAD
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/CMakeLists.txt")
  add_subdirectory(third_party/glad)
  set(GLAD_LIBRARIES glad)
else ()
  file(GLOB GLAD_SOURCES "third_party/glad/src/*.c")
  add_library(glad STATIC ${GLAD_SOURCES})
  target_include_directories(glad PUBLIC third_party/glad/include)
  set(GLAD_LIBRARIES glad)
endif ()

# ====== Engine (Static library) ======
file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
        engine/core/*.cpp
        engine/platform/*.cpp
        engine/render/*.cpp
        engine/util/*.cpp
)

file(GLOB_RECURSE ENGINE_HEADERS CONFIGURE_DEPENDS
        engine/core/*.h
        engine/platform/*.h
        engine/render/*.h
        engine/util/*.h
)

add_library(yagl_engine STATIC ${ENGINE_SRC} ${ENGINE_HEADERS})

target_include_directories(yagl_engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/engine
)

target_link_libraries(yagl_engine
        glfw
        ${GLAD_LIBRARIES}
        glm::glm
        spdlog::spdlog
)

# ====== Game (Executable) ======
file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS
        game/*.cpp
        game/*.h
        main.cpp
)

add_executable(${PROJECT_NAME} ${GAME_SRC})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/game
        ${CMAKE_CURRENT_SOURCE_DIR}/engine
)

target_link_libraries(${PROJECT_NAME}
        yagl_engine
)

# ====== Copy Shaders ======
if (EXISTS "${CMAKE_SOURCE_DIR}/shaders")
  file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})
  message(STATUS "Copying shaders from ${CMAKE_SOURCE_DIR}/shaders to ${CMAKE_BINARY_DIR}/shaders")
endif ()

# ====== MinGW static linking ======
if (WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(${PROJECT_NAME} -static-libgcc -static-libstdc++)
endif ()
