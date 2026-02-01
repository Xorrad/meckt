cmake_minimum_required(VERSION 3.28)

project(meckt LANGUAGES C CXX)

###############################################
#  Global C++ configuration                   #
###############################################
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Put executables in <build>/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Release)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER}
        ${CMAKE_SOURCE_DIR}/bin/${OUTPUTCONFIG})
endforeach()

# Enable folder grouping in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

###############################################
#  Dependencies                               #
###############################################
include(FetchContent)

###############################################
#  SFML                                       #
###############################################
set(BUILD_SHARED_LIBS OFF)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

set_target_properties(
    sfml-graphics
    sfml-window
    sfml-system
    PROPERTIES
        FOLDER "vendor/sfml"
)

if (WIN32)
    set_target_properties(freetype PROPERTIES FOLDER "vendor")
endif()

###############################################
#  ImGui (core library)                       #
###############################################
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.9-docking
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})

target_compile_definitions(imgui PUBLIC IMGUI_USE_WCHAR32)
target_compile_features(imgui PUBLIC cxx_std_20)

set_target_properties(imgui PROPERTIES FOLDER "vendor")

###############################################
#  ImGui-SFML                                 #
###############################################
FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG v3.0
    GIT_SHALLOW ON
    # Overwrite the CMakeLists.txt with an empty one.
    PATCH_COMMAND "${CMAKE_COMMAND}" -E echo "" > CMakeLists.txt
)
FetchContent_MakeAvailable(imgui-sfml)

add_library(imgui-sfml STATIC
    ${imgui-sfml_SOURCE_DIR}/imgui-SFML.cpp
)

target_include_directories(imgui-sfml PUBLIC 
    ${imgui_SOURCE_DIR}
    ${imgui-sfml_SOURCE_DIR}
)

target_link_libraries(imgui-sfml PUBLIC
    imgui
    SFML::Graphics
    SFML::Window
    SFML::System
)

target_compile_features(imgui-sfml PUBLIC cxx_std_20)

set_target_properties(imgui-sfml PROPERTIES FOLDER "vendor")


###############################################
#  Native File Dialog                         #
###############################################
FetchContent_Declare(
    nfd
    GIT_REPOSITORY https://github.com/mlabbe/nativefiledialog.git
    GIT_TAG master
)
FetchContent_MakeAvailable(nfd) 

add_library(nfd STATIC)
target_sources(nfd PRIVATE ${nfd_SOURCE_DIR}/src/nfd_common.c)
target_include_directories(nfd PUBLIC ${nfd_SOURCE_DIR}/src/include)

if (WIN32)
    target_sources(nfd PRIVATE ${nfd_SOURCE_DIR}/src/nfd_win.cpp)
    target_link_libraries(nfd PRIVATE ole32 uuid)
elseif (UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    target_sources(nfd PRIVATE ${nfd_SOURCE_DIR}/src/nfd_gtk.c)
    target_include_directories(nfd PRIVATE ${GTK3_INCLUDE_DIRS})
    target_link_libraries(nfd PRIVATE ${GTK3_LIBRARIES})
endif()

set_target_properties(nfd PROPERTIES FOLDER "vendor")

###############################################
#  lodepng                                    #
###############################################
FetchContent_Declare(
    lodepng
    GIT_REPOSITORY https://github.com/lvandeve/lodepng.git
    GIT_TAG master
)
FetchContent_MakeAvailable(lodepng) 

add_library(lodepng STATIC)
target_sources(lodepng PRIVATE ${lodepng_SOURCE_DIR}/lodepng.cpp)
target_include_directories(lodepng PUBLIC ${lodepng_SOURCE_DIR})

set_target_properties(lodepng PROPERTIES FOLDER "vendor")

###############################################
#  fmt                                        #
###############################################
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 12.1.0
)
FetchContent_MakeAvailable(fmt)

set_target_properties(fmt PROPERTIES FOLDER "vendor")

###############################################
#  nlohmann json                              #
###############################################
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

set_target_properties(nlohmann_json PROPERTIES FOLDER "vendor")

###############################################
#  Jomini Parser                              #
###############################################
file(GLOB JOMINI_LIB_FILES
    "${CMAKE_SOURCE_DIR}/vendor/includes/jomini-parser/src/Jomini.cpp"
    "${CMAKE_SOURCE_DIR}/vendor/includes/jomini-parser/src/Jomini.hpp"
)

# file(GLOB JOMINI_FILES
#    "${CMAKE_SOURCE_DIR}/vendor/includes/jomini-parser/src/*.cpp"
#    "${CMAKE_SOURCE_DIR}/vendor/includes/jomini-parser/src/*.hpp"
#    "${CMAKE_SOURCE_DIR}/vendor/includes/jomini-parser/tests/*.txt"
#    "${CMAKE_SOURCE_DIR}/vendor/includes/jomini-parser/.github/workflows/**"
# )

add_library(jomini-parser STATIC ${JOMINI_LIB_FILES})

source_group(
    TREE "${CMAKE_SOURCE_DIR}/vendor/includes/jomini-parser"
    PREFIX "jomini-parser"
    FILES ${JOMINI_LIB_FILES}
)

target_include_directories(jomini-parser PUBLIC vendor/includes/jomini-parser/src)
target_compile_features(jomini-parser PUBLIC cxx_std_20)

target_link_libraries(jomini-parser PUBLIC
    SFML::Graphics
    SFML::Window
    SFML::System
)

###############################################
#  meckt                                      #
###############################################
file(GLOB_RECURSE PROJECT_SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
    src/*.hpp
)

add_executable(meckt
    ${PROJECT_SOURCES}
)

if(WIN32)
    target_sources(meckt PRIVATE src/build/version.rc)
	source_group(TREE ${CMAKE_SOURCE_DIR} FILES src/build/version.rc)
endif()

source_group(
    TREE ${CMAKE_SOURCE_DIR}
    FILES ${PROJECT_SOURCES}
)

source_group("cmake" REGULAR_EXPRESSION "cmake_pch.*")

target_precompile_headers(meckt PRIVATE
    src/pch.hpp
)

target_include_directories(meckt PRIVATE
    src
    vendor/includes
    vendor/includes/jomini-parser/src
)

set_target_properties(meckt PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY
        "$<TARGET_FILE_DIR:meckt>"
)

target_compile_definitions(meckt PUBLIC
    SFML_STATIC
    $<$<CONFIG:Debug>:DEBUG>
)

###############################################
#  Link dependencies                          #
###############################################
target_link_libraries(meckt PRIVATE
    imgui-sfml
    jomini-parser
    nfd
    lodepng
    fmt
    nlohmann_json::nlohmann_json
)

if (WIN32)
    target_link_libraries(meckt PRIVATE
        opengl32
        gdi32
        winhttp
    )
elseif (UNIX)
    find_package(CURL REQUIRED)
    target_link_libraries(meckt PRIVATE CURL::libcurl)
endif()

###############################################
#  Post-build steps                           #
###############################################

# Copy assets next to the executable
add_custom_command(
    TARGET meckt
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:meckt>/assets"
)