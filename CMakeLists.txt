cmake_minimum_required(VERSION "3.20")

cmake_policy("SET" "CMP0091" "NEW")

project("luacdis" LANGUAGES "CXX")

message("CMAKE_CXX_COMPILER_ID = {${CMAKE_CXX_COMPILER_ID}}")
message("MSVC = {${MSVC}}")
message("MSVC_VERSION = {${MSVC_VERSION}}")

option("ENABLE_ASAN" "Enable AddressSanitizer" ON)
set(HAS_ASAN false)



if(ENABLE_ASAN)
    add_library("asan_options" INTERFACE)

    if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")

        include("CheckCXXCompilerFlag")
        check_cxx_compiler_flag("-fsanitize=address" "HAS_ASAN")

        if(HAS_ASAN) 

            target_compile_options("asan_options" INTERFACE "-fsanitize=address")
            target_link_options("asan_options" INTERFACE "-fsanitize=address")

        endif()

    elseif(MSVC)

        include("CheckCXXCompilerFlag")
        check_cxx_compiler_flag("/fsanitize=address" "HAS_ASAN")

        if(HAS_ASAN)

            target_compile_options("asan_options" INTERFACE "/fsanitize=address")
            target_link_options("asan_options" INTERFACE "/fsanitize=address")

        endif()

    else()

        message(WARNING "ASan is not supported")

    endif()

    if(MSVC_VERSION AND HAS_ASAN)

        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

    endif()
endif()



file("GLOB_RECURSE" "SOURCES"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file("GLOB_RECURSE" "HEADERS" "CONFIGURE_DEPENDS"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

add_executable("luacdis" ${SOURCES} ${HEADERS})

target_include_directories("luacdis" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")




if((CMAKE_CXX_COMPILER_ID MATCHES "^Clang$") AND MSVC_VERSION)

    target_compile_options("luacdis" PRIVATE "-Wall" "-Wextra" "-Werror")
    target_compile_definitions("luacdis" PRIVATE "_CRT_SECURE_NO_WARNINGS")

elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$" )

    target_compile_options("luacdis" PRIVATE "-Wall" "-Wextra" "-Werror")

elseif(MSVC)

    target_compile_options("luacdis" PRIVATE "/W4" "/WX")
	target_compile_definitions("luacdis" PRIVATE "_CRT_SECURE_NO_WARNINGS")

endif()



if(ENABLE_ASAN AND HAS_ASAN)
    target_link_libraries("luacdis" PRIVATE "asan_options")
endif()

add_subdirectory("third-party/dragonbox")
target_link_libraries("luacdis" PRIVATE "dragonbox::dragonbox_to_chars")


target_compile_features("luacdis" PRIVATE "c_std_11" "cxx_std_17")
