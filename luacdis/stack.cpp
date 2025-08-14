#include "stack.hpp"


void stackpush(int*& stack, size_t& top, size_t& size, int num) {
	if ((top + 2) * 4 >= size) {
	    size *= 2;
		int* stk = (int*)realloc(stack, size);
		if (stk == NULL) {
			printf("Error: Failed to reallocate stack");
			free(stack);
			exit(1);
		}
		stack = stk;
	}
	stack[top] = num;
	top++;
}


int stackpop(int* stack, size_t& top) {
	top--;
	int ret = stack[top];
	return ret;
}


int removeDuplicates(int* arr, int n) {
	if (n == 0) return 0;

	int j = 0;
	for (int i = 1; i < n; ++i) {
		if (arr[i] != arr[j]) {
			++j;
			arr[j] = arr[i];
		}
	}
	return j + 1;
}