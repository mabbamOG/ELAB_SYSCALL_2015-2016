struct sembuf *op;

#define BUFSIZE 2048

struct command
{
    char instruction;
    int par1, par2, res;
};

int next_integer(char **buf_offset)
{
    char *s = *buf_offset
    while(*s== ' ')
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
    while(**buf_offset==' ' || **buf_offset=='\n') // ????? should put this in next_integer?
        ++*buf_offset;
    int id = next_integer(buf_offset);
    cmd->par1 = next_integer(buf_offset);
    while(**buf_offset==' ')
        ++*buf_offset;
    cmd->instr = **buf_offset;
    ++*buf_offset;
    cmd->par2 = next_integer(buf_offset);
}

void slave(int id)
{
    P(work,id);
    // Get data
    // ....
    // Drop result
    // ....
    V(free,id);
}

void master(int id)
{
    P(free,id);
    // Get result...
    // ...
    // Drop the data
    // ...
    V(work,id);
}


int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr,"uso: %s <nome file di calcolo> <nome file per i risultati>\n");
        fprintf(stderr,"requirements:\n\
                * no line must be empty\n\
                * you may use as many spaces as you like is each line\n\
                * every line must contain expected information\n");
        exit(1);
    }
    int finput = open(argv[1], O_RDONLY, S_IRUSR | S_IRGRP);
    int foutput = open(argv[2], O_WRONLY | O_CREAT | O_SYNC, S_IWUSR | S_IWGRP);
    char buf[BUFSIZE];
    char *buf_offset = buf;
    int nread = 0;
    nread = read(finput, buf, BUFSIZE)
    // Get threads
    printf("Getting number of threads...\n");
    int NPROCS = next_number(&buf_offset);
    if (buf_offset >= buf + BUFSIZE)
        exception("please make the start of the file shorter");
    if (NPROCS<1)
        exception("please create more processes");
    // Make threads
    for (int id = 1; id<=NPROCS; ++id)
    {
        int pid = fork();
        if (pid==0)
            slave(id);
    }









    // Father:
    struct command *cmd = malloc(sizeof(struct command));
    do
    {
        int NOPS = get_num_ops(buf_offset);
        int RESULTS[NOPS]
        for (int j = 0; j<NOPS; ++j)
        {
            int id = next_command(buf_offset,cmd);
            if (id == 0)
            {
                // DO OPS TO FIND FREE SEMAPHORE
                //....
            }
            master(id,j,RESULTS);
        }
            
    } while( (nread=read(finput, buf, BUFSIZE)) >0);
    if (nread==-1)
        exception("error reading file");

}
