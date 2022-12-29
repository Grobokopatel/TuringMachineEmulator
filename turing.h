#include <stdbool.h>

#ifndef _turing_h_
#define _turing_h_

#define MAX_TRANSITIONS 100
#define MAX_STATE_NAME_LENGTH 15

struct State;
struct Transition;

typedef struct Transition Transition;
typedef struct State State;

typedef enum {
	Left, Right, Stay
} Direction;

struct Transition {
	struct State *from;
	struct State *to;
	Direction direction;
	char input;
	char write;
};

struct State {
	char name[ MAX_STATE_NAME_LENGTH ];
	struct Transition* transitions[ MAX_TRANSITIONS ];
	int transitionsCount;
	bool isHalt;
};

struct TuringMachine {
	State** states;
	State* current;
	int state_count;
	int head;
};

typedef struct TuringMachine TuringMachine;

Transition* create_transition( State*, State*, Direction, char, char );
void destroy_transition( Transition* );
void print_transition ( Transition* );

State* create_state( const char*, bool );
bool add_state_to_transition( State*, Transition* );
void print_state( State* );
void destroy_state( State* );

TuringMachine* create_turing_machine(size_t);
void destroy_turing_machine( TuringMachine* );
bool add_state_to_turing_machine( TuringMachine*, State* );
void print_tape_step_debug( TuringMachine*, char* );
State* take_step( TuringMachine*, char** );
void run_turing_machine( TuringMachine*, char** );
void* xmalloc(size_t);
char** read_and_split_line(size_t, size_t);
void read_int(int*);
void clear_input_buffer();
char* read_line();

#endif
