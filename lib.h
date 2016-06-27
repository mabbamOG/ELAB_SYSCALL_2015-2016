void P(int semid, int semchoice)
{
    struct sembuf op = { semchoice, -1, 0 };
    semop(semid, &op, 1);
}


void V(int semid, int semchoice)
{
    struct sembuf op = { semchoice, 1, 0 };
    semop(semid, &op, 1);
}

// ???
union semun  // needed for semctl() syscall
{
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO, Linux-Specific*/
};

struct command
{
    char instr;
    int par1, par2, res, res_pos;
};

int get_num_ops(char **buf_offset)
{
    char *s = *buf_offset;
    int res = 0;
    while (*s!='@')
        if (*s++ == '\n')
            ++res;
    if (*(s-1) == '\n') // if last character of file is '\n', than i need to remove it
        --res;
    return res;
}

int next_integer(char **buf_offset)
{
    char *s = *buf_offset
    while(*s== ' ' || *s=='\n')
        ++s;
    int sign = 1;
    if (*s=='-')
    {
        sign = -1;
        s++;
    }
    int res = 0;
    while(*s>='0' && *s<='9')
        res = res*10 + *s++ - '0';
    *buf_offset = s;
    return sign*res;
}
int next_command(char **buf_offset, struct command *cmd)
{
    int id = next_integer(buf_offset);
    cmd->par1 = next_integer(buf_offset);
    while(**buf_offset==' ')
        ++*buf_offset;
    cmd->instr = **buf_offset;
    ++*buf_offset;
    cmd->par2 = next_integer(buf_offset);
    return id;
}
