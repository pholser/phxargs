#ifndef PHXARGS_EXIT_CODES_H
#define PHXARGS_EXIT_CODES_H

/* Exit codes defined by the xargs specification. */

/* Command ran but exited with a non-zero status between 1 and 125. */
#define PHXARGS_STATUS_CHILD_FAILED 123

/* Command exited with status 255, signalling phxargs to halt. */
#define PHXARGS_STATUS_HALT 124

/* Command was terminated by a signal. */
#define PHXARGS_STATUS_SIGNALLED 125

/* Command was found but could not be executed (not executable). */
#define PHXARGS_STATUS_NOT_EXECUTABLE 126

/* Command was not found. */
#define PHXARGS_STATUS_NOT_FOUND 127

#endif  // PHXARGS_EXIT_CODES_H
