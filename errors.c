#include <stdio.h>
#include <stdlib.h>
#include "errors.h"

void log_error( char *message )
{
	if( message )
	{
		log_err( "Error: %s.\n", message );
	}
}
