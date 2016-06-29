#ifndef LIB_IO
#define LIB_IO
struct command; // Incomplete "forward" declaration to avoid including whole lib_ipc.h

int next_integer(char **buf_offset);
int next_command(char **buf_offset, struct command *cmd);
int get_num_ops(char *s);
int strlenf(char *s);

#endif
