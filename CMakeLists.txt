cmake_minimum_required(VERSION 3.23.0 FATAL_ERROR)

set(PROJECT_NAME stdfunc)

project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)

################################################################################
# Optional third-party dependencies used to extend stdfunc functionality
################################################################################
option(STDFUNC_USE_XXHASH "Enable XXHash for fast non-cryptographic hashing." OFF)
option(STDFUNC_USE_SNAPPY "Enable Snappy compression support." OFF)
option(STDFUNC_USE_ZSTD "Enable Zstandard compression support." OFF)
option(STDFUNC_USE_GLAZE "Enable Glaze for compile-time struct iteration and member detection." OFF)
option(STDFUNC_USE_CTRE "Enable CTRE for compile-time regular expressions." OFF)

################################################################################
# Core
################################################################################
add_library(stdfunc_core INTERFACE)
add_library(stdfunc::core ALIAS stdfunc_core)

# TODO: Change to 23
target_compile_features(stdfunc_core INTERFACE cxx_std_20)

target_include_directories(stdfunc_core
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

################################################################################
# Components
################################################################################
function(stdfunc_add_component NAME)
    add_library(${NAME}
        src/${NAME}.cpp
    )

    add_library(${PROJECT_NAME}::${NAME} ALIAS ${NAME})

    target_link_libraries(${NAME}
        PUBLIC
            stdfunc::core
    )

    target_include_directories(${NAME}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${NAME}>
            $<INSTALL_INTERFACE:include/${NAME}>
    )
endfunction()

stdfunc_add_component(stdcompress)
stdfunc_add_component(stddecompress)
stdfunc_add_component(stdfilesystem)
stdfunc_add_component(stdrandom)

################################################################################
# Optional dependencies
################################################################################
if(STDFUNC_USE_XXHASH)
    find_package(xxHash REQUIRED)
    target_link_libraries(${PROJECT_NAME}::core PUBLIC xxHash::xxhash)
endif()

if(STDFUNC_USE_SNAPPY)
    find_package(Snappy REQUIRED)
    target_link_libraries(stdcompress PUBLIC snappy)
    target_link_libraries(stddecompress PUBLIC snappy)
endif()

if(STDFUNC_USE_ZSTD)
    find_package(zstd REQUIRED)
    target_link_libraries(stdcompress PUBLIC libzstd)
    target_link_libraries(stddecompress PUBLIC libzstd)
endif()

if(STDFUNC_USE_GLAZE)
    find_package(glaze REQUIRED)
    target_link_libraries(${PROJECT_NAME}::core PUBLIC glaze::glaze)
endif()

if(STDFUNC_USE_CTRE)
    find_package(ctre REQUIRED)
    target_link_libraries(stdfilesystem PUBLIC ctre::ctre)
endif()

################################################################################
# Everything
################################################################################
add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    INTERFACE
        stdcompress
        stddecompress
        stdfilesystem
        stdrandom
)
