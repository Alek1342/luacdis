cmake_minimum_required (VERSION "3.10")

project ("luacdis")


include("CheckCSourceCompiles")

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(ASAN_FLAG "/fsanitize=address")
    set(CHECK_ASAN true)
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    set(ASAN_FLAG "-fsanitize=address")
    set(CHECK_ASAN true)
endif()

if(CHECK_ASAN)
    set(_saved_required_flags "${CMAKE_REQUIRED_FLAGS}")
    set(_saved_required_link_options "${CMAKE_REQUIRED_LINK_OPTIONS}")

    string(APPEND CMAKE_REQUIRED_FLAGS " ${ASAN_FLAG}")
    list(APPEND CMAKE_REQUIRED_LINK_OPTIONS "${ASAN_FLAG}")

    check_c_source_compiles([[
        int main() {
            int value = 0;
            return value;
        }
    ]] HAS_ASAN)

    # Restore previous settings.
    set(CMAKE_REQUIRED_FLAGS "${_saved_required_flags}")
    set(CMAKE_REQUIRED_LINK_OPTIONS "${_saved_required_link_options}")

    unset(_saved_required_flags)
    unset(_saved_required_link_options)

    if(HAS_ASAN)
        message(STATUS "AddressSanitizer is supported")
        add_library("asan_options" INTERFACE)

        target_compile_options("asan_options" INTERFACE
            "$<$<CONFIG:Debug>:${ASAN_FLAG}>"
            "$<$<AND:$<CONFIG:Debug>,$<NOT:$<C_COMPILER_ID:MSVC>>>:-fno-omit-frame-pointer>"
        )

        target_link_options("asan_options" INTERFACE
            "$<$<CONFIG:Debug>:${ASAN_FLAG}>"
        )
    endif()
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

include("FetchContent")
FetchContent_Declare(
        "dragonbox"
        GIT_REPOSITORY "https://github.com/jk-jeon/dragonbox"
)
FetchContent_MakeAvailable("dragonbox")
target_link_libraries("luacdis" PRIVATE "dragonbox::dragonbox_to_chars")


 if(HAS_ASAN)
        target_link_libraries("luacdis" PRIVATE "asan_options")
    endif()

target_compile_features("luacdis" PRIVATE "c_std_11" "cxx_std_17")
target_compile_definitions("luacdis" PRIVATE "_CRT_SECURE_NO_WARNINGS")