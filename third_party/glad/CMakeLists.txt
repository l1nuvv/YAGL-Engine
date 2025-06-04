# CMakeLists.txt for GLAD
cmake_minimum_required(VERSION 3.20)

# Create GLAD library
add_library(glad STATIC
        src/glad.c
)

# Include directories
target_include_directories(glad PUBLIC
        include
)

# Link libraries (if needed)
if (WIN32)
  target_link_libraries(glad PRIVATE opengl32)
elseif (APPLE)
  find_package(OpenGL REQUIRED)
  target_link_libraries(glad PRIVATE ${OPENGL_LIBRARIES})
else ()
  find_package(OpenGL REQUIRED)
  target_link_libraries(glad PRIVATE OpenGL::GL)
endif ()
