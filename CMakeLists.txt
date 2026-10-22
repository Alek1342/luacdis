cmake_minimum_required(VERSION "3.10")
cmake_policy(SET CMP0091 NEW)

project("luacdis")

option(ENABLE_ASAN "Enable AddressSanitizer" ON)

if(ENABLE_ASAN)
    # Use the release DLL CRT in every configuration.
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
else()
    # Default behavior: /MDd in Debug, /MD otherwise.
    set(CMAKE_MSVC_RUNTIME_LIBRARY
        "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()



file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

add_executable("luacdis" ${SOURCES} ${HEADERS})

target_include_directories("luacdis"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_options(luacdis PRIVATE
    -Wall
    -Wextra
    -Werror
)

if(ENABLE_ASAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(luacdis PRIVATE
            -fsanitize=address
        )
        target_link_options(luacdis PRIVATE
            -fsanitize=address
        )
    elseif(MSVC)
        target_compile_options(luacdis PRIVATE
            /fsanitize=address
        )
        target_link_options(luacdis PRIVATE
            /INFERASANLIBS
        )
    else()
        message(FATAL_ERROR
            "ASan configuration not supported for this compiler")
    endif()
endif()

add_subdirectory("third-party/dragonbox")
target_link_libraries("luacdis" PRIVATE "dragonbox::dragonbox_to_chars")


target_compile_features("luacdis" PRIVATE "c_std_11" "cxx_std_17")
target_compile_definitions("luacdis" PRIVATE "_CRT_SECURE_NO_WARNINGS")