struct sembuf *op;
struct command *CMDMEM;

#define BUFSIZE 2048


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
    while(1)
    {
        P(work,id);
            switch(CMDMEM[id].instruction)// Get Data
            {
                case '+':// Drop Result
                    CMDMEM[id].res = CMDMEM[id].par1 + CMDMEM[id].par2;
                    break;
                case '-':
                    CMDMEM[id].res = CMDMEM[id].par1 - CMDMEM[id].par2;
                    break;
                case '/':
                    CMDMEM[id].res = CMDMEM[id].par1 / CMDMEM[id].par2;
                    break;
                case '*':
                    CMDMEM[id].res = CMDMEM[id].par1 * CMDMEM[id].par2;
                    break;
                default:
                    break;
            }
            CMDMEM[id].instruction = '!';// ??? needed?
        V(free,id);
    }
}

void master(int id)
{
    P(free,id);
    // Get result...
    if (CMDMEM[id] == '!')
        RESULTS[
    // Drop the data
    // ...
    V(work,id);
}

void help()
{
    fprintf(stderr,"uso: %s <nome file di calcolo> <nome file per i risultati>\n");
    fprintf(stderr,"requirements:\n\
            * no line must be empty\n\
            * you may use as many spaces as you like is each line\n\
            * every line must contain expected information\n");
    exit(1);
}


int main(int argc, char **argv)
{
    // Check if correct launch:
    if (argc != 3)
        help();

    // Init resources:
    int finput = open(argv[1], O_RDONLY, S_IRUSR | S_IRGRP);
    int foutput = open(argv[2], O_WRONLY | O_CREAT | O_SYNC, S_IWUSR | S_IWGRP);

    // Get threads:
    char buf[BUFSIZE];
    char *buf_offset = buf;
    int nread = read(finput, buf, BUFSIZE)
    printf("Getting number of threads...\n");
    int NPROCS = next_number(&buf_offset);
    if (buf_offset >= buf + BUFSIZE)
        exception("please make the start of the file shorter");
    if (NPROCS<1)
        exception("please create more processes");

    //  Init shared resources:
    int key_t KEY;
    KEY = ftok(argv[2],'a');
    int semfreeid = semget(KEY, NPROCS+1, IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG); // execute permission is meaningless for semaphores, but ok..
    union semun cmdinfo;
    {
        unsigned short array[NPROCS] = {1};
        cmdinfo.array = array;
        semctl(semfree, 0, SETALL, cmdinfo);
    }
    KEY = ftok(argv[2],'b');
    int semworkid = semget(KEY, NPROCS+1, IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG); 
    {
        unsigned short array[NPROCS] = {0};
        cmdinfo.array = array;
        semctl(semfree, 0, SETALL, cmdinfo);
    }
    KEY = ftok(argv[2],'c'); // can be 'a'?
    int shmid = shmget(KEY, sizeof(struct command)*(NPROCS+1), IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG);
    struct command *CMDMEM = shmat(shmid, NULL, 0);

    // Make threads:
    for (int id = 1; id<=NPROCS; ++id)
    {
        int pid = fork();
        if (pid==0)
        {
            slave(id);
            exit(1);
        }
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
