// TODO: standard naming of "get" and "next" functions, and all other functions!
// TODO: check if i really need +1 size on both buf and semaphores and shmem
// FIXME: is buf_offset really needed now that i keep whole file in memory and never go back?
// TODO: split lib.h in multiple libraries containing different function types
// TODO: ??? put global vars in lib.h?
// TODO: finish father routine
// TODO: find_free_process()
// TODO: exception()
#include "lib.h"

// GLOBAL VARIABLES:
#define MINFILESIZE 7
#define MAXFILESIZE 10485760 // 10MB
int SEMID_FREE = -1;
int SEMID_WORK = -1;
int SHMID = -1;
struct command *SHM = (void *) -1;

void exception(char *s)
{
    printf("%s\n",s);
    printf("Freeing resources...\n");
    printf("* Semaphore %d\n",SEMID_FREE);
    if (SEMID_FREE != -1)
        semctl(...);
    printf("* Semaphore %d\n",SEMID_WORK);
    if (SEMID_WORK != -1)
        semctl(...);
    if (SHM != (void *) -1)
        shmdt(...);
    printf("* Shared Memory %d\n",SHMID);
    if (SHMID != -1)
        shmctl(...);
    printf("DONE!\n");
}

void init_shared_resources(char *keystr,int res_size)
{
    // Create First Semaphore Array
    int key_t KEY = ftok(keystr,'a');
        if (KEY == -1)
            exception("ERROR while getting unique key for shared resource SEMID_FREE!");
    SEMID_FREE = semget(KEY, res_size, IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG); // execute permission is meaningless for semaphores, but ok..
        if (SEMID_FREE == -1)
            exception("ERROR while getting id for shared resource SEMID_FREE!");
        union semun cmdinfo;
        unsigned short array[res_size] = {1};
        cmdinfo.array = array;
        if (semctl(SEMID_FREE, 0, SETALL, cmdinfo) == -1)
            exception("ERROR while setting values for shared resource SEMID_FREE!");

    // Create Second Semaphore Array
    KEY = ftok(keystr,'b');
        if (KEY == -1)
            exception("ERROR while getting unique key for shared resource SEMID_WORK!");
    int SEMID_WORK = semget(KEY, res_size, IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG); 
        if (SEMID_WORK == -1)
            exception("ERROR while getting id for shared resource SEMID_WORK!");
        for (int i=0; i<res_size;++i)
            cmdinfo.array[i] = 0;
        if (semctl(SEMID_WORK, 0, SETALL, cmdinfo) == -1)
            exception("ERROR while setting values for shared resource SEMID_WORK!");
    
    // Create and attach Shared Memory
    KEY = ftok(keystr,'c'); // can be 'a'?
        if (KEY == -1)
            exception("ERROR while getting unique key for shared resource SHMID!");
    int SHMID = shmget(KEY, sizeof(struct command)*res_size, IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG);
        if (SHMID == -1)
            exception("ERROR while getting id for shared resource SHMID!");
    struct command *SHMEM = shmat(SHMID, NULL, 0);
        if (SHMEM == (void *) -1)
            exception("ERROR while attaching shared memory SHMID to process!");

    // Initialize shared memory:
    for(int i=0; i<res_size; ++i)
        SHMEM[i].instr = '?'; // no op yet
}


void slave(int id)// ??? Should make get_data | drop result, more explicit?
{
    while(true)
    {
        P(SEMID_WORK,id);
            switch(SHM[id].instr)// Get Data
            {
                case '+':// Drop Result
                    SHM[id].res = SHM[id].par1 + SHM[id].par2;
                    break;
                case '-':
                    SHM[id].res = SHM[id].par1 - SHM[id].par2;
                    break;
                case '/':
                    SHM[id].res = SHM[id].par1 / SHM[id].par2;
                    break;
                case '*':
                    SHM[id].res = SHM[id].par1 * SHM[id].par2;
                    break;
                case 'K':
                    printf("PROC #%d: QUITTING!\n",id);
                    exit(0);
                    break;
                default:
                    exception("invalid instruction found!");
                    break;
            }
            SHM[id].instr = '!'; // telling master i have ever run any op
        V(SEMID_FREE,id);
    }
}

void master(int id, struct command *cmd, int *RESULTS)
{
    P(SEMID_FREE,id);
        // Get result...
        if (SHM[id].instr == '!') // making sure this isn't the first instruction
            RESULTS[SHM[id].res_pos] = SHM[id].res;
        // Drop the data
        SHM[id].res_pos = cmd->res_pos;
        SHM[id].instr = cmd->instr;
        SHM[id].par1 = cmd->par1;
        SHM[id].par2 = cmd->par2;
    V(SEMID_WORK,id);
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

    // Get access to input and output files:
    int fdinput = open(argv[1], O_RDONLY, S_IRUSR | S_IRGRP);
    if (fdinput == -1)
        exception("ERROR while opening input file!");
    int fdoutput = open(argv[2], O_WRONLY | O_CREAT | O_SYNC, S_IWUSR | S_IWGRP);
    if (fdoutput == -1)
        exception("ERROR while opening output file!");

    // Get Input filesize:
    struct stat stats;
    if (fstat(finput, &stats) == -1)
        exception("ERROR while getting input file filesize!");
    const int FILESIZE = stats->st_size + 1; // !!! +1 because i need space for end marker!
    if (FILESIZE < MINFILESIZE + 1)
        exception("ERROR input file too small");
    if (FILESIZE > MAXFILESIZE + 1)
        exception("ERROR input file way too big");

    // Read file into buffer:
    char buf[FILESIZE];
    char *buf_offset = buf;
    if(read(finput, buf, FILESIZE) == -1)
        exception("ERROR while loading input file into buffer!");
    buf[FILESIZE-1]='@';
    if (close(fdinput) == -1)
        exception("ERROR while closing input file!");

    // Get threads:
    printf("Getting number of threads...\n");
    const int NPROCS = next_number(&buf_offset);
    if (NPROCS<1)
        exception("please create more processes");

    //  Init shared resources:
    init_shared_resources(NPROCS + 1); // !!! +1 because i want to use "id" freely

    // Make threads:
    for (int id = 1; id<=NPROCS; ++id)
    {
        int pid = fork();
        if (pid == -1)
            exception("ERROR while forking!");
        else if (pid==0)
            slave(id);
    }

    // Father:
    struct command cmd;
    const int NOPS = get_num_ops(buf_offset);
    int RESULTS[NOPS];
    for (int j = 0; j<NOPS; ++j)
    {
        // Setup info to send to process:
        cmd.res_pos = j;
        int id = next_command(buf_offset,&cmd);
        if (id == 0)
            id = find_free_process();

        // Send Off Command (possibly grabbing result for previously sent command)
        master(id, &cmd, RESULTS);
    }
}
