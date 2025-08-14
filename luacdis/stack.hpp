#pragma once
#include "stdlib.h"
#include <stdio.h>


void stackpush(int*& stack, size_t& top, size_t& size, int num);
int stackpop(int* stack, size_t& top);
int removeDuplicates(int* arr, int n);
