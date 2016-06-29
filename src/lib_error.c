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
    debugf("%s\n",s);
    destroy_shared_resources();
    exit(1);
}

void force_quit(int sigid)
{
    exception("ERROR received Ctrl-C signal!");
}

void debugf(char *s, ...) //  MAX 1 argument!
{

    int offset = strlenf(s);
    if (s[offset] == '\0')
    {
        if (write(2, s, offset) < offset)
            exit(0); // can't alert user
    }
    else
    {
        // Initialize variable arguments list (s is the last argument before the list)
        va_list args;
        va_start(args, s);
        
        // Split string into 3
        char *a = s;
        int alen = offset-1;
        char b[100];
        int blen = 0;
        char *c = s+offset+1;
        int clen = strlenf(c);
        if (c[-1] == 'd')
            blen = sprintf(b, "%d", va_arg(args, int));
        else if (c[-1] == 's')
            blen = sprintf(b, "%s", va_arg(args, char *));

        // Write into single buffer for atomicity (writev is faulty...)
        char buf[500];
        snprintf(buf, alen+1, "%s", a);
        snprintf(buf+alen, blen+1, "%s", b);
        snprintf(buf+alen+blen, clen+1, "%s", c);
        if (write(2, buf, alen+blen+clen) < alen+blen+clen)
            exit(0); // can't alert user

        // Destroy variable arguments list
        va_end(args);
    }
}
