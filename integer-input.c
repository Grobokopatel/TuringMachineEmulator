#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_DIGITS 12

static inline void clear_input_buffer()
{
	char c = 0;
	while ((c = getchar()) != '\n' && c != EOF);
}

void read_int(int* inputInteger)
{
	char* inputBuffer = malloc(sizeof(char) * MAX_DIGITS);
	memset(inputBuffer, 0, MAX_DIGITS);
	char* input = NULL;
	while (input == NULL) 
	{
		input = fgets(inputBuffer, MAX_DIGITS, stdin);

		if (inputBuffer[strlen(inputBuffer) - 1] != '\n') 
		{
			fprintf(stderr, "[ERROR]: Too many characters: max input is %d chars.\n", MAX_DIGITS);
			clear_input_buffer();
			input = NULL;
			abort();
		}

		errno = 0;
		char* endptr = NULL;
		*inputInteger = strtol(input, &endptr, 10);

		if (input == endptr) 
		{
			input[strcspn(input, "\n")] = 0;
			printf("Invalid input: no integer found in %s.\n", input);
			input = NULL;
		}
		if (errno != 0) 
		{
			fprintf(stderr, "[ERROR]: That doesn't look like an integer.\n");
			input = NULL;
		}
	}
	free(inputBuffer);
}