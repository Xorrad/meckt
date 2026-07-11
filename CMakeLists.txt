cmake_minimum_required(VERSION 3.28)

project(meckt LANGUAGES C CXX)

###############################################
#  Options                                    #
###############################################
option(RUN_TESTS_POST_BUILD "Automatically run tests after building" ON)

##################################################################
#  Global configuration                                          #
##################################################################
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Put executables in <build>/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<IF:$<BOOL:${CMAKE_CONFIGURATION_TYPES}>,$<CONFIG>,>")

# Enable folder grouping in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

##################################################################
#  Dependencies                                                  #
##################################################################
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
#  doctest                                    #
###############################################
add_library(doctest INTERFACE)

target_include_directories(doctest INTERFACE "${CMAKE_SOURCE_DIR}/vendor/doctest")

set_target_properties(doctest PROPERTIES FOLDER "vendor")

###############################################
#  cmrc                                       #
###############################################
FetchContent_Declare(
  cmrc
  GIT_REPOSITORY https://github.com/vector-of-bool/cmrc.git
  GIT_TAG master
)
FetchContent_MakeAvailable(cmrc)

# set_target_properties(cmrc PROPERTIES FOLDER "vendor")

###############################################
#  backward-cpp                               #
###############################################
FetchContent_Declare(backward
    GIT_REPOSITORY https://github.com/bombela/backward-cpp
    GIT_TAG master
)
FetchContent_MakeAvailable(backward)

set_target_properties(backward PROPERTIES FOLDER "vendor")

###############################################
#  Jomini Parser                              #
###############################################
file(GLOB JOMINI_LIB_FILES
    "${CMAKE_SOURCE_DIR}/vendor/jomini-parser/src/Jomini.cpp"
    "${CMAKE_SOURCE_DIR}/vendor/jomini-parser/src/Jomini.hpp"
)

add_library(jomini-parser STATIC ${JOMINI_LIB_FILES})

source_group(
    TREE "${CMAKE_SOURCE_DIR}/vendor/jomini-parser"
    PREFIX "jomini-parser"
    FILES ${JOMINI_LIB_FILES}
)

target_include_directories(jomini-parser PUBLIC vendor/jomini-parser/src)
target_compile_features(jomini-parser PUBLIC cxx_std_20)

target_link_libraries(jomini-parser PUBLIC
    SFML::Graphics
    SFML::Window
    SFML::System
)

##################################################################
#  meckt                                                         #
##################################################################

###############################################
#  meckt-resources                            #
###############################################

# Embeed resources such as shaders and fonts.
cmrc_add_resource_library(
    meckt-resources
    NAMESPACE Resources

    resources/app/fonts/figtree_medium.ttf
    resources/app/fonts/notoemoji.ttf
    resources/app/fonts/notosans.ttf
    resources/app/shaders/heightmap_landmass.frag
    resources/app/shaders/provinces_landmass.frag
    resources/app/shaders/provinces.frag
    resources/app/shaders/provinces.vert
    
    resources/tests/resource_manager/test.png
)

###############################################
#  meckt-core                                 #
###############################################
file(GLOB_RECURSE MECKT_CORE_SOURCES
    CONFIGURE_DEPENDS
    src/core/*.cpp
    src/core/*.hpp
)

source_group(
    TREE ${CMAKE_SOURCE_DIR}
    FILES ${MECKT_CORE_SOURCES}
)
source_group("cmake" REGULAR_EXPRESSION "cmake_pch.*")

add_library(meckt-core STATIC ${MECKT_CORE_SOURCES})

target_compile_definitions(meckt-core PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

target_precompile_headers(meckt-core PUBLIC src/core/pch.hpp)

target_include_directories(meckt-core PUBLIC src/core)
target_include_directories(meckt-core SYSTEM PUBLIC vendor) # SYSTEM suppresses vendor warnings
target_include_directories(meckt-core SYSTEM PUBLIC vendor/jomini-parser/src) # SYSTEM suppresses vendor warnings

target_link_libraries(meckt-core PUBLIC
    imgui-sfml
    jomini-parser
    nfd
    lodepng
    fmt
    nlohmann_json::nlohmann_json
    backward
    meckt-resources
)

if (WIN32)
    target_link_libraries(meckt-core PRIVATE
        opengl32
        gdi32
        winhttp
    )
elseif (UNIX)
    find_package(CURL REQUIRED)
    target_link_libraries(meckt-core PRIVATE CURL::libcurl)
endif()

###############################################
#  meckt-app                                  #
###############################################

file(GLOB_RECURSE MECKT_APP_SOURCES
    CONFIGURE_DEPENDS
    src/app/*.cpp
    src/app/*.hpp
)

source_group(
    TREE ${CMAKE_SOURCE_DIR}
    FILES ${MECKT_APP_SOURCES}
)

add_executable(meckt-app ${MECKT_APP_SOURCES})

set_target_properties(meckt-app PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY
        "$<TARGET_FILE_DIR:meckt-app>"
)

target_compile_definitions(meckt-app PUBLIC
    SFML_STATIC
    $<$<CONFIG:Debug>:DEBUG>
)

target_precompile_headers(meckt-app PRIVATE src/app/pch.hpp)

target_include_directories(meckt-app PUBLIC src)
target_include_directories(meckt-app SYSTEM PUBLIC vendor) # SYSTEM suppresses vendor warnings
target_include_directories(meckt-app SYSTEM PUBLIC vendor/jomini-parser/src) # SYSTEM suppresses vendor warnings

target_link_libraries(meckt-app PRIVATE
    meckt-core
)

if(WIN32)
    target_sources(meckt-app PRIVATE src/app/build/version.rc)
	source_group(TREE ${CMAKE_SOURCE_DIR} FILES src/app/build/version.rc)
endif()

# Copy resources directory next to the executable
add_custom_command(
    TARGET meckt-app
    POST_BUILD
    COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}>
            $<$<CONFIG:Debug>:-E>
            $<$<CONFIG:Debug>:copy_directory>
            $<$<CONFIG:Debug>:"${CMAKE_SOURCE_DIR}/resources/app">
            $<$<CONFIG:Debug>:"$<TARGET_FILE_DIR:meckt-app>/resources/app">
    COMMENT "Post-build: Copying resources directory..."
)

###############################################
#  meckt-tests                                #
###############################################

file(GLOB_RECURSE MECKT_TESTS_SOURCES
    CONFIGURE_DEPENDS
    src/tests/*.cpp
    src/tests/*.hpp
)

source_group(
    TREE ${CMAKE_SOURCE_DIR}
    FILES ${MECKT_TESTS_SOURCES}
)

add_executable(meckt-tests ${MECKT_TESTS_SOURCES})

target_include_directories(meckt-tests PUBLIC src)
target_include_directories(meckt-tests SYSTEM PUBLIC vendor) # SYSTEM suppresses vendor warnings
target_include_directories(meckt-tests SYSTEM PUBLIC vendor/jomini-parser/src) # SYSTEM suppresses vendor warnings

set_target_properties(meckt-tests PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY
        "$<TARGET_FILE_DIR:meckt-tests>"
)

target_compile_definitions(meckt-tests PUBLIC
    $<$<CONFIG:Debug>:DEBUG>
)

target_link_libraries(meckt-tests PUBLIC
    meckt-core
    doctest
)

# Copy resources next to the executable in post-build steps
add_custom_command(
    TARGET meckt-tests
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/resources/"
            "$<TARGET_FILE_DIR:meckt-tests>/resources/"
    COMMENT "Post-build: Copying resources directory..."
)

# Automatically run the tests
if(RUN_TESTS_POST_BUILD)
add_custom_command(
    TARGET meckt-tests
    POST_BUILD
    COMMAND "$<TARGET_FILE:meckt-tests>"
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:meckt-tests>"
    COMMENT "Post-build: Executing meckt-tests..."
)
endif()

enable_testing()
add_test(NAME meckt-tests COMMAND meckt-tests)
set_tests_properties(meckt-tests PROPERTIES
    WORKING_DIRECTORY $<TARGET_FILE_DIR:meckt-tests>
)