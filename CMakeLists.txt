cmake_minimum_required(VERSION 3.5)
project(Zoltraak)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Fetch Libraries
find_package(Vulkan REQUIRED)

include(FetchContent)

# 1. fmt
FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt.git GIT_TAG 10.2.1)
FetchContent_MakeAvailable(fmt)

# 2. GLM
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.3)
FetchContent_MakeAvailable(glm)

# 3. vkbootstrap
FetchContent_Declare(vkbootstrap GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap.git GIT_TAG v0.3.1)
FetchContent_MakeAvailable(vkbootstrap)

# 4. fastgltf (Requires simdjson, which it downloads automatically)
FetchContent_Declare(fastgltf GIT_REPOSITORY https://github.com/spnda/fastgltf.git GIT_TAG v0.8.0)
FetchContent_MakeAvailable(fastgltf)

FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        release-3.2.4 # Always check for the latest release tag
)
FetchContent_MakeAvailable(SDL3)

#Executables
add_executable(${PROJECT_NAME} src/main.cpp)

#Includes and Libraries
target_include_directories(${PROJECT_NAME} PRIVATE include)

target_include_directories(${PROJECT_NAME} PUBLIC assets)

target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan SDL3::SDL3 fmt::fmt glm::glm vk-bootstrap fastgltf)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/com.xenitype.zoltraak.desktop.in
        ${CMAKE_CURRENT_BINARY_DIR}/com.xenitype.zoltraak.desktop
        @ONLY
)

if(UNIX AND NOT APPLE)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            # Copy the file
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_BINARY_DIR}/com.xenitype.zoltraak.desktop
            $ENV{HOME}/.local/share/applications/com.xenitype.zoltraak.desktop
            # Update the Wayland/X11 application database
            COMMAND update-desktop-database $ENV{HOME}/.local/share/applications/
            COMMENT "Updating Linux .desktop file and application database..."
    )
endif()