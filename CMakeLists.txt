cmake_minimum_required(VERSION 3.23.0 FATAL_ERROR)

set(PROJECT_NAME stdfunc)

project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)

option(STDFUNC_USE_XXHASH "Enable XXHash for fast non-cryptographic hashing." OFF)
option(STDFUNC_USE_SNAPPY "Enable Snappy compression support." OFF)
option(STDFUNC_USE_ZSTD "Enable Zstandard compression support." OFF)
option(STDFUNC_USE_GLAZE "Enable Glaze for compile-time struct iteration and member detection." OFF)
option(STDFUNC_USE_CTRE "Enable CTRE for compile-time regular expressions." OFF)

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

if(STDFUNC_USE_XXHASH)
    find_package(xxHash REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC xxHash::xxhash)
endif()

if(STDFUNC_USE_SNAPPY)
    find_package(Snappy REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC snappy)
endif()

if(STDFUNC_USE_ZSTD)
    find_package(zstd REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC libzstd)
endif()

if(STDFUNC_USE_GLAZE)
    find_package(glaze REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC glaze::glaze)
endif()

if(STDFUNC_USE_CTRE)
    find_package(ctre REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC ctre::ctre)
endif()
