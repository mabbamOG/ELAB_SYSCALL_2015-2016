#ifndef LIB_IO
#define LIB_IO

int next_integer(char **buf_offset);
int next_command(char **buf_offset, struct command *cmd);
int get_num_ops(char *s);

#endif
