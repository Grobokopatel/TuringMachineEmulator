#include <errno.h>
#include <string.h>
#include <stdio.h>

#define clean_errno() ( errno == 0 ? "None" : strerror( errno ) )
#define log_err( M, ... ) fprintf( stderr, "[ERROR] ( %s:%d: errno: %s ) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__ )