#ifndef LIB_IO
#define LIB_IO
struct command; // Incomplete "forward" declaration to avoid including whole lib_ipc.h

/**
 * @brief grabs the next integer in a string, also while changing its pointer offset
 *
 * @param buf_offset a pointer to a string. Needed to move forward the string offset
 * after having read an integer.
 *
 * @return the next integer found in the string
 */
int next_integer(char **buf_offset);

/**
 * @brief grabs the next instrution of type (struct command) in the given string,
 * also while changing the pointer offset of the string.
 *
 * @param buf_offset a pointer to a string. Needed to move forward the string offset
 * after having read an instruction.
 * @param cmd a pointer to an instruction structure of type (struct command), stores
 * the value of the command found in the string.
 *
 * @return the id of the process to whom the instruction returned in cmd is destined.
 */
int next_command(char **buf_offset, struct command *cmd);

/**
 * @brief grabs the number of instructions of type (struct command) contained in a given string.
 * next_command() does so by checking for the number of '\n' character found in the file, being
 * careful to exclude whitespace found at the beginning or the end.
 *
 * @param s the string to be scanned.
 *
 * @return the number of instructions counted.
 */
int get_num_ops(char *s);

/**
 * @brief strlenf() is a formatted string length counter.
 * it simply returns the number of bytes found in the string until
 * one of the following strings is found: '\0',"%s","%d". The number of bytes
 * excludes the matched string.
 *
 * @param s the string to be scanned.
 *
 * @return the length of the string up until one of the following is found: '\0', "%s", "%s".
 */
int strlenf(char *s);

#endif
