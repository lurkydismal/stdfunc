cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(PROJECT_NAME stdfunc)

project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)

# TODO: Make optional
find_package(xxHash REQUIRED)
find_package(Snappy REQUIRED)
find_package(zstd REQUIRED)
find_package(glaze REQUIRED)

# TODO: Separate to sub targets
add_library(${PROJECT_NAME}
    src/stdcompress.cpp
    src/stddecompress.cpp
    src/stdfilesystem.cpp
    src/stdrandom.cpp
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# TODO: Change to 23
target_compile_features(${PROJECT_NAME}
    PUBLIC
        cxx_std_20
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# TODO: Make optional
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        xxHash::xxhash
        snappy
        libzstd
        glaze::glaze
)

