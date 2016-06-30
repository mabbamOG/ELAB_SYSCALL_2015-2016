/** @file
 *
 * @brief this file provides functionality to handle all errors, including messages sent to stderr (through debugf()).
 *
 * The exception() function is very common throughout the whole project, and is extremely helpful in ensuring compact
 * sycall error managent. Since its functionality resembles that of common exceptions (such as in C++, Java or Python languages),
 * its name reflects this.
 * The force_quit() function is required as a signal handler to register the program for receiving a SIGINT (Ctrl-C) signal
 * from the user. It is mainly a precaution.
 * Finally, debugf() is a marvellous little function allowing me to easily "printf" data to stderr without loss of similarity,
 * while instead using direct syscalls, as per project requirements.
 *
 */
#include "lib_error.h"

#include "lib_io.h"
#include "lib_ipc.h"
#include <sys/uio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

void exception(char *s)
{
    debugf("%s\n",s); // print formatted error message
    destroy_shared_resources();
    exit(1);
}

void force_quit(int sigid)
{
    exception("ERROR received Ctrl-C signal!");
}

void debugf(char *s, ...) //  MAX 1 argument!
{
    // Check whether this is a normal string or a formatted one
    int offset = strlenf(s);

    // Normal string:
    if (s[offset] == '\0')
    {
        if (write(2, s, offset) < offset)
            exit(0); // can't alert user
    }

    // Formatted string:
    else
    {
        // Initialize variable arguments list (s is the last argument before the list)
        va_list args;
        va_start(args, s);
        
        // Split string into 3
        char *a = s; // before the formatted argument
        int alen = offset-1;
        static char b[100]; // buffer to store the formatted argument
        int blen = 0;
        char *c = s+offset+1; // after the formatted argument
        int clen = strlenf(c);

        // Store formatted argument into b buffer
        if (c[-1] == 'd')
            blen = sprintf(b, "%d", va_arg(args, int));
        else if (c[-1] == 's')
            blen = sprintf(b, "%s", va_arg(args, char *));

        // Write into single buffer for atomicity, as writev seems to be faulty... :-(
        static char buf[500];
        snprintf(buf, alen+1, "%s", a);
        snprintf(buf+alen, blen+1, "%s", b);
        snprintf(buf+alen+blen, clen+1, "%s", c);
        if (write(2, buf, alen+blen+clen) < alen+blen+clen)
            exit(0); // can't alert user

        // Destroy variable arguments list
        va_end(args);
    }
}
