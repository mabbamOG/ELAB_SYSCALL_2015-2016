/** @file
 *
 * @brief this is the file containing some functions useful for operating on strings, that do not belong in the main file.
 *
 * The get_num_ops() next_integer() and next_command() functions are used to parse the main input file (already read into a string) 
 * and aid the program in swiftly extracting needed information.
 * The strlen() function is used by the debugf() function from "lib_error.h" and parses strings as well.
 *
 */
#include "lib_io.h"
#include "lib_ipc.h"

int strlenf(char *s)
{
    int offset = 0;
    while(s[offset] != '\0') // break if end of string
        if(s[offset++] == '%')
            if (s[offset] == 's' || s[offset] == 'd') //  brak if formatted string
                break;
    return offset;
}

int get_num_ops(char *s)
{
    int res = 0;

    // Ignore count of rubbish before the first instruction
    while (*s == ' ' || *s == '\n')
        ++s;
    
    // Count all newlines
    ++res;
    while (*s!='\0')
        if (*s++ == '\n')
            ++res;

    // Remove count of rubbish at the end of the file
    --s;
    while (*s == ' ' || *s == '\n')
        if (*s-- == '\n')
            --res;

    return res;
}

int next_integer(char **buf_offset)
{
    char *s = *buf_offset;

    // Ignore rubbish
    while(*s== ' ' || *s=='\n')
        ++s;

    // Check for negative integers
    int sign = 1;
    if (*s=='-')
    {
        sign = -1;
        s++;
    }
    
    // Gather all ascii digits
    int res = 0;
    while(*s>='0' && *s<='9')
        res = res*10 + *s++ - '0';

    *buf_offset = s; // modify buffer offset!
    return sign*res;
}
int next_command(char **buf_offset, struct command *cmd)
{
    int id = next_integer(buf_offset); // the id of the process for this command
    cmd->par1 = next_integer(buf_offset); // the first parameter of this command

    // Ignore rubbish
    while(**buf_offset==' ')
        ++*buf_offset;
    cmd->instr = **buf_offset; // the instruction of this command
    ++*buf_offset;

    cmd->par2 = next_integer(buf_offset); // the second parameter of this command
    return id;
}
