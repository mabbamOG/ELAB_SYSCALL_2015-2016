// ??? do i really need this?
void exception(char *s);

struct command
{
    char instr;
    int par1, par2, res, res_pos;
};

void P(int semid, int semchoice)
{
    struct sembuf op = { semchoice, -1, 0 };
    if (semop(semid, &op, 1) == -1)
        exception("ERROR while waiting on semaphore!");
}


void V(int semid, int semchoice)
{
    struct sembuf op = { semchoice, 1, 0 };
    if (semop(semid, &op, 1) == -1)
        exception("ERROR while signaling semaphore!");
}

int get_num_ops(char *s)
{
    int res = 0;
    // Ignore count of rubbish before the first instruction
    while (*s == ' ' || *s == '\n')
        ++s;
    ++res;
    // Count all newlines
    while (*s!='\0')
        if (*s++ == '\n')
            ++res;
    // Remove count of rubbish at the end of the file
    while (*s == ' ' || *s == '\n')
        if (*s-- == '\n')
            --res;
    return res;
}

int next_integer(char **buf_offset)
{
    char *s = *buf_offset;
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
