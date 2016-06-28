#include "lib_error.h"

#include <stdarg.h>

void exception(char *s)
{
    printf("%s\n",s);
    destroy_shared_resources();
    exit(1);
}

void force_quit(int sigid)
{
    exception("ERROR received Ctrl-C signal!");
}

void debug(char *formatted, ...)
{
    va_list args;
    va_start(args, s); // s is the last argument before the va_list

    char s[] = formatted;
    static char printbuf[500];
    int printsize = sprintf(printbuf, "%s", formatted);

    while (*s != '\0')
        if(*s++ == '%')
            if (*s == 's')
            {
                s[-1] = '\0';
                printsize = sprintf(printbuf, "%s%s%s", formatted, va_arg(args, char *), s+1);
                break;
            }
            else if (*s == 'd')
            {
                s[-1] = '\0';
                size = sprintf(printbuf, "%s%d%s", formatted, va_arg(args, int), s+1);
                break;
            }
    int nwrite = write(2, printbuf,printsize);
    if (nwrite<printsize || nwrite == -1) // Se la stampa a video fallisce, non e' possibile nemmeno avvertire l'utente
        exit(0);
    va_end(args);
}
