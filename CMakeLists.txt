cmake_minimum_required(VERSION 4.0)
project(YAGL-Engine)

# Подавляем предупреждение о неиспользуемых переменных vcpkg
if (DEFINED VCPKG_TARGET_TRIPLET)
  set(_unused_var ${VCPKG_TARGET_TRIPLET})
  unset(_unused_var)
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-Wall -Wextra)
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

# ====== Shader Embedding Function ======
function(embed_shader SHADER_FILE OUTPUT_FILE VARIABLE_NAME)
  file(READ ${SHADER_FILE} SHADER_CONTENT)

  # Экранируем специальные символы для C++ строки
  string(REPLACE "\\" "\\\\" SHADER_CONTENT "${SHADER_CONTENT}")
  string(REPLACE "\"" "\\\"" SHADER_CONTENT "${SHADER_CONTENT}")
  string(REPLACE "\r\n" "\n" SHADER_CONTENT "${SHADER_CONTENT}")
  string(REPLACE "\n" "\\n\"\n\"" SHADER_CONTENT "${SHADER_CONTENT}")

  # Генерируем header файл
  file(WRITE ${OUTPUT_FILE}
          "#pragma once\n\n"
          "// Auto-generated from ${SHADER_FILE}\n"
          "// Do not modify this file manually!\n\n"
          "namespace EmbeddedShaders {\n"
          "    constexpr const char* ${VARIABLE_NAME} = \"${SHADER_CONTENT}\";\n"
          "}\n"
  )

  message(STATUS "Generated embedded shader: ${OUTPUT_FILE}")
endfunction()

# ====== Third-party ======
add_subdirectory(third_party/glfw)
add_subdirectory(third_party/glm)
add_subdirectory(third_party/spdlog)

#stb_image
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE third_party/stb)

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

# ====== Generate Embedded Shaders ======
set(GENERATED_SHADERS_DIR "${CMAKE_BINARY_DIR}/generated/shaders")
file(MAKE_DIRECTORY ${GENERATED_SHADERS_DIR})

# Определяем пути к шейдерам (проверяем разные возможные локации)
set(SHADER_SEARCH_PATHS
        "${CMAKE_SOURCE_DIR}/shaders"
        "${CMAKE_SOURCE_DIR}/assets/shaders"
        "${CMAKE_SOURCE_DIR}/game/shaders"
)

set(SHADER_SOURCE_DIR "")
foreach (SEARCH_PATH ${SHADER_SEARCH_PATHS})
  if (EXISTS "${SEARCH_PATH}")
    set(SHADER_SOURCE_DIR ${SEARCH_PATH})
    break()
  endif ()
endforeach ()

if (NOT SHADER_SOURCE_DIR)
  message(FATAL_ERROR "Shader directory not found! Searched in: ${SHADER_SEARCH_PATHS}")
endif ()

message(STATUS "Using shader directory: ${SHADER_SOURCE_DIR}")

# Генерируем embedded шейдеры для каждого найденного файла
file(GLOB VERTEX_SHADERS "${SHADER_SOURCE_DIR}/*.vert")
file(GLOB FRAGMENT_SHADERS "${SHADER_SOURCE_DIR}/*.frag")

set(GENERATED_SHADER_HEADERS "")

foreach (SHADER_FILE ${VERTEX_SHADERS})
  get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
  string(TOUPPER ${SHADER_NAME} SHADER_NAME_UPPER)
  set(OUTPUT_FILE "${GENERATED_SHADERS_DIR}/${SHADER_NAME}_vert.h")
  set(VARIABLE_NAME "${SHADER_NAME_UPPER}_VERTEX_SHADER")

  embed_shader(${SHADER_FILE} ${OUTPUT_FILE} ${VARIABLE_NAME})
  list(APPEND GENERATED_SHADER_HEADERS ${OUTPUT_FILE})
endforeach ()

foreach (SHADER_FILE ${FRAGMENT_SHADERS})
  get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
  string(TOUPPER ${SHADER_NAME} SHADER_NAME_UPPER)
  set(OUTPUT_FILE "${GENERATED_SHADERS_DIR}/${SHADER_NAME}_frag.h")
  set(VARIABLE_NAME "${SHADER_NAME_UPPER}_FRAGMENT_SHADER")

  embed_shader(${SHADER_FILE} ${OUTPUT_FILE} ${VARIABLE_NAME})
  list(APPEND GENERATED_SHADER_HEADERS ${OUTPUT_FILE})
endforeach ()

# Создаем общий header файл со всеми шейдерами
set(ALL_SHADERS_HEADER "${GENERATED_SHADERS_DIR}/AllShaders.h")
file(WRITE ${ALL_SHADERS_HEADER}
        "#pragma once\n\n"
        "// Auto-generated header with all embedded shaders\n"
        "// Do not modify this file manually!\n\n"
)

foreach (HEADER ${GENERATED_SHADER_HEADERS})
  get_filename_component(HEADER_NAME ${HEADER} NAME)
  file(APPEND ${ALL_SHADERS_HEADER} "#include \"${HEADER_NAME}\"\n")
endforeach ()

# ====== Engine (Static library) ======
file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
        engine/core/*.cpp
        engine/platform/*.cpp
        engine/render/*.cpp
        engine/utils/*.cpp
)

file(GLOB_RECURSE ENGINE_HEADERS CONFIGURE_DEPENDS
        engine/core/*.h
        engine/platform/*.h
        engine/render/*.h
        engine/utils/*.h
)

add_library(yagl_engine STATIC ${ENGINE_SRC} ${ENGINE_HEADERS})

target_include_directories(yagl_engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/engine
        ${GENERATED_SHADERS_DIR}  # Добавляем папку с генерированными шейдерами
)

target_link_libraries(yagl_engine
        glfw
        ${GLAD_LIBRARIES}
        glm::glm
        spdlog::spdlog
        stb_image
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
        ${GENERATED_SHADERS_DIR}  # Добавляем папку с генерированными шейдерами
)

target_link_libraries(${PROJECT_NAME}
        yagl_engine
)

# Убеждаемся, что шейдеры генерируются перед сборкой
add_dependencies(${PROJECT_NAME} yagl_engine)

# ====== Copy Shaders (для разработки) ======
if (EXISTS "${CMAKE_SOURCE_DIR}/shaders")
  file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})
  message(STATUS "Copying shaders from ${CMAKE_SOURCE_DIR}/shaders to ${CMAKE_BINARY_DIR}/shaders")
endif ()

# ====== MinGW static linking ======
if (WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(${PROJECT_NAME} -static-libgcc -static-libstdc++)
endif ()
