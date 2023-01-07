#include "turing.h"
#include "errors.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define MAX_LINE_LEN 80

char** read_and_split_line(size_t count, size_t maxStringSize)
{
	char inputString[100];
	char** words = xmalloc(count * sizeof(char*));
	words[0] = xmalloc(count * (maxStringSize+1));
	for (int i = 1; i < count; ++i)
		words[i] = words[i - 1] + (maxStringSize+1);
	int indexCtr = 0;
	int wordIndex = 0;
	int totalWords = 0;

	fgets(inputString, sizeof inputString, stdin);

	for (indexCtr = 0; indexCtr <= strlen(inputString)-1; indexCtr++)
	{
		if (inputString[indexCtr] == ' ' || inputString[indexCtr] == '\0' || inputString[indexCtr] == '\n')
		{
			words[totalWords][wordIndex] = '\0';

			totalWords++;

			wordIndex = 0;
		}
		else
		{
			words[totalWords][wordIndex] = inputString[indexCtr];

			wordIndex++;
		}
	}

	return words;
}

Transition* parse_transition( char* transition, State** states, int statesLength)
{
	State *st1 = NULL;
	State *st2 = NULL;
	int i = 0, parsed = 0;
	char s1[ MAX_STATE_NAME_LENGTH ], s2[ MAX_STATE_NAME_LENGTH ];
	char format[ MAX_LINE_LEN ];
	char input, move, write;
	Direction dir;


	sprintf( format, "%%%ds %%c -> %%%ds %%c %%c",
		( MAX_STATE_NAME_LENGTH - 1 ),
		( MAX_STATE_NAME_LENGTH - 1 )
	);

	parsed = sscanf( transition, format, s1, &input, s2, &write, &move );

	if( parsed < 5 ) 
	{
		log_err( "Wrong transition line in file." );
		return NULL;
	}

	for( i = 0; i < statesLength; i++ ) 
	{
		State *state = states[i];

		if( strcmp( state->name, s1 ) == 0 ) 
		{
			st1 = state;
		}

		if( strcmp( state->name, s2 ) == 0 ) 
		{
			st2 = state;
		}
	}

	if( !st1 || !st2 ) 
	{
		log_err( "Parse error: could not find the states of one of the transitions specified" );
		return NULL;
	}

	switch( move ) 
	{
		case 'L':
			dir = Left;
			break;

		case 'R':
			dir = Right;
			break;

		case 'S':
			dir = Stay;
			break;
		default:
			log_err( "Parse error: could not parse direction of one of the transitions." );
			return NULL;
	}

	Transition *trans = create_transition( st1, st2, dir, input, write );

	if( !add_state_to_transition( st1, trans )) 
	{
		log_err( "Failed to add state transition." );
		return NULL;
	}

	return trans;
}

TuringMachine* parse_turing_machine()
{
	int statesCount;
	read_int(&statesCount);
	TuringMachine *machine = create_turing_machine(statesCount);
	if(statesCount < 3)
	{
		log_err( "Number of states is less than 3" );
		goto out;
	}

	char** states = read_and_split_line(statesCount, 10);
	char** startAndHalt = read_and_split_line(2, 10);
	char* startName = startAndHalt[0];
	char* haltName = startAndHalt[1];
	State* start_state = NULL;
	for( int i = 0; i < statesCount; i++ ) 
	{
		State* state = create_state( states[i], strcmp(states[i], haltName) == 0);
		if (strcmp(states[i], startName) == 0)
			start_state = state;

		if( !state )
			goto out;

		if( !add_state_to_turing_machine( machine, state )) 
		{
			log_err( "Failed to add state." );
			goto out;
		}
	}

	assert(start_state);

	int transitionsCount;
	read_int(&transitionsCount);
	for(int i=0; i< transitionsCount; ++i)
	{
		char* transition = read_line();
		Transition *trans = parse_transition(transition, machine->states, machine->state_count);

		if( !trans ) 
			break;
	}

	machine->current = start_state;
	if( !machine->current )
	{
		log_err( "Turing machine should have atleast one state" );
		goto out;
	}

	return machine;

out:
	destroy_turing_machine( machine );

	return NULL;
}

char* read_line() 
{
	char* line = malloc(100), * linep = line;
	size_t lenmax = 100, len = lenmax;
	int c;

	if (line == NULL)
		return NULL;

	for (;;) {
		c = fgetc(stdin);
		if (c == EOF)
			break;

		if (--len == 0) {
			len = lenmax;
			char* linen = realloc(linep, lenmax *= 2);

			if (linen == NULL) {
				free(linep);
				return NULL;
			}
			line = linen + (line - linep);
			linep = linen;
		}

		if ((*line++ = c) == '\n')
		{
			--line;
			break;
		}
	}
	*line = '\0';
	return linep;
}
