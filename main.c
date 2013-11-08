
#include "instruction.h"

#include <stdio.h>

int main(int argc, char **argv)
{
	FILE *trace = stdin;
	size_t count = 0;

	instruction_t i = {0};

	while (!instruction_get(&i, trace)) {
		++count;
		instruction_clean(&i);
	}

	printf("Parsed instructions: %zu\n", count);

	return 0;
}
