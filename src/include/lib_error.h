#ifndef LIB_ERROR
#define LIB_ERROR

/**
 * @brief this function is used to clean up IPC resources and exit() whenever a syscall returns an error.
 *
 * @param s a string to be printed to stderr before freeing IPC resources and quitting.
 */
void exception(char *s);

/**
 * @brief signal handler for quitting the program.
 *
 * @param sigid the id of the signal received.
 */
void force_quit(int sigid);

/**
 * @brief prints formatted strings to stderr, but can only handle 
 * one argument and only of type (char *) or (int).
 *
 * @param s the formatted string telling debugf() what to print.
 * @param ... the variable arguments list (va_list) containg possible
 * futher arguments. see printf() from <stdio.h> for an example of this.
 */
void debugf(char *s, ...);

#endif
