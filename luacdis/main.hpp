#pragma once
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/format.h>
#include <iostream>
//#include "utils.hpp"
#include "luacparse.hpp"
#include "stack.hpp"
#include "writer.hpp"


constexpr size_t LLASM_START_BUFFER_SIZE = 65536;