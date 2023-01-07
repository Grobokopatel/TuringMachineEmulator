#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "turing.h"
#include "turingparser.h"
#include "errors.h"

static bool debug = false;

void* xmalloc(size_t size)
{
	void* result = malloc(size);
	if (result == NULL)
	{
		log_err("Error trying to allocate memory\n");
		abort();
	}

	return result;
}

Transition* create_transition(State* from, State* to, Direction direction, char input, char write)
{
	assert(from);
	assert(to);

	Transition* transition = xmalloc(sizeof(Transition));

	transition->from = from;
	transition->to = to;
	transition->direction = direction;
	transition->input = input;
	transition->write = write;

	assert(transition);

	return transition;
}

void destroy_transition(Transition* transition)
{
	free(transition);
}

void print_transition(Transition* transition)
{
	if (!transition)
	{
		printf("Transition{ NULL }");
	}
	else 
	{
		printf("Transition{\n\tstate: %s\n\tinput: %c\n\tnext: %s\n\twrite: %c\n\tmove: %d\n}\n",
			   transition->from->name,
			   transition->input,
			   transition->to->name,
			   transition->write,
			   transition->direction
		);
	}
}

State* create_state(const char* name, bool isHalt)
{
	assert(name);

	State* state = xmalloc(sizeof(State));

	strncpy(state->name, name, MAX_STATE_NAME_LENGTH - 1);
	state->name[MAX_STATE_NAME_LENGTH - 1] = '\0';

	state->isHalt = isHalt;
	state->transitionsCount = 0;

	return state;
}

bool add_state_to_transition(State* state, Transition* transition)
{
	assert(state);
	assert(transition);

	if (state->transitionsCount == MAX_TRANSITIONS) 
	{
		log_err("State %s already has the maximum amount of transitions.", state->name);
		return false;
	}

	state->transitions[state->transitionsCount] = transition;
	state->transitionsCount = state->transitionsCount + 1;

	return true;
}

void print_state(State* state)
{
	if (!state)
	{
		printf("State{ NULL }");
	}
	else 
	{
		printf("State{\n\tname: %s\n\taccept: %d\n\t}\n",
			   state->name,
			   state->isHalt
		);
	}
}

void destroy_state(State* state)
{
	assert(state);

	int i = 0;

	for (i = 0; i < state->transitionsCount; i++) 
	{
		Transition* trans = state->transitions[i];
		if (!trans) 
		{
			log_err("Problem destroying state transition.");
			continue;
		}

		destroy_transition(trans);
	}

	free(state);
}

TuringMachine* create_turing_machine(size_t states_count)
{
	TuringMachine* machine = xmalloc(sizeof(TuringMachine));

	machine->state_count = 0;
	machine->current = NULL;
	machine->head = 0;
	machine->states = xmalloc(states_count * sizeof(State*));

	return machine;
}


void destroy_turing_machine(TuringMachine* machine)
{
	assert(machine);

	int i = 0;

	for (i = 0; i < machine->state_count; i++) 
	{
		State* state = machine->states[i];
		if (!state)
		{
			log_err("Error while destroying machine state.");
			continue;
		}

		destroy_state(state);
	}

	free(machine);
}

bool add_state_to_turing_machine(TuringMachine* machine, State* state)
{
	assert(machine);
	assert(state);

	machine->states[machine->state_count++] = state;

	return true;
}

void print_tape_step_debug(TuringMachine* machine, char* tape)
{
	int tape_len = strlen(tape);
	assert(machine);
	assert(tape);

	int i = 0;

	for (i = 0; i < machine->head; i++) 
	{
		printf("%c", tape[i]);
	}

	State* current = machine->current;
	if (!current) 
	{
		log_err("Failed to retrieve machines current state.");
		return;
	}

	printf(" %s ", current->name);

	for (i = machine->head; i < tape_len; i++) {
		printf("%c", tape[i]);
	}

	printf("\n");
}

State* take_step(TuringMachine* machine, char** tape_ref)
{
	char* tape = *tape_ref;
	int tape_len = strlen(tape);
	assert(machine);
	assert(tape);

	int i = 0;
	char input = tape[machine->head];
	State* state = machine->current;

	for (i = 0; i < state->transitionsCount; i++) 
	{
		Transition* trans = state->transitions[i];
		if (!trans)
		{
			log_err("error while trying to get state transition.");
			continue;
		}

		if (trans->input == input)
		{
			State* next = trans->to;

			tape[machine->head] = trans->write;

			if (trans->direction == Left)
			{
				int* head = &(machine->head);
				--(*head);
				if (*head == -1)
				{
					char* new_tape = xmalloc(tape_len * 2 + 1);
					for (int i = 0; i < tape_len; ++i)
					{
						new_tape[i] = '_';
					}
					for (int i = tape_len; i < tape_len * 2; ++i)
					{
						new_tape[i] = tape[i - tape_len];
					}
					new_tape[tape_len * 2] = '\0';
					free(tape);
					*tape_ref = new_tape;
					*head = tape_len - 1;
				}
			}
			else if (trans->direction == Right)
			{
				int* head = &(machine->head);
				++(*head);
				if (*head >= tape_len - 1)
				{
					*tape_ref = realloc(tape, tape_len * 2 + 1);
					tape = *tape_ref;
					for (int i = tape_len; i < tape_len * 2; ++i)
					{
						tape[i] = '_';
					}
					tape[tape_len * 2] = '\0';
				}
			}

			machine->current = next;

			assert(next);

			return next;
		}
	}
	return NULL;
}

void print_tape_state(char* tape, TuringMachine* machine)
{
	int tape_length = strlen(tape);
	int most_left_symbol = -1;
	int most_right_symbol = -1;
	bool shouldPrint = false;
	for (int i = 0; i < tape_length; ++i)
	{
		if (tape[i] != '_')
		{
			most_left_symbol = i;
			break;
		}
	}
	for (int i = tape_length - 1; i >= 0; --i)
	{
		if (tape[i] != '_')
		{
			most_right_symbol = i;
			break;
		}
	}

	if (most_left_symbol != -1 && most_right_symbol != -1)
	{
		for (int i = most_left_symbol; i < most_right_symbol + 1; ++i)
		{
			printf("%c", tape[i]);
		}
	}

	printf("\n");

	printf("%s %d\n", machine->current->name,  machine->head - most_left_symbol);
}

void run_turing_machine(TuringMachine* machine, char** tape_ref)
{
	char* tape = *tape_ref;
	assert(machine);
	assert(tape);

	if (!machine->current)
	{
		log_err("Turing machine has no start state");
		abort();
	}
	int stepsCount = 0;
	while (true)
	{
		State* state = take_step(machine, tape_ref);

		if (!state)
		{
			printf("FAIL after %d transitions\n", stepsCount);
			print_tape_state(*tape_ref, machine);
			return;
		}
		++stepsCount;
		if (debug)
		{
			print_tape_step_debug(machine, *tape_ref);
		}

		if (stepsCount == 100000)
		{
			printf("MADE %d transitions\n", stepsCount);
			print_tape_state(*tape_ref, machine);
			return;
		}

		if (state->isHalt)
		{
			printf("STOP after %d transitions\n", stepsCount);
			print_tape_state(*tape_ref, machine);
			return;
		}
	}
}

int main(int argc, char* argv[])
{
	TuringMachine* machine = parse_turing_machine();
	char* tape = read_line();
	int tapelength = strlen(tape);
	if (!machine)
	{
		log_err("Failed parsing the machine, exiting\n");
		abort();
	}

	if (debug) 
	{
		print_tape_step_debug(machine, tape);
	}

	run_turing_machine(machine, &tape);
	destroy_turing_machine(machine);
	free(tape);

	return 0;
}

