#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
// TODO: standard naming of "get" and "next" functions, and all other functions!
// TODO: check if i really need +1 size on both buf and semaphores and shmem
// FIXME: is buf_offset really needed now that i keep whole file in memory and never go back?
// TODO: split lib.h in multiple libraries containing different function types
// TODO: ??? put global vars in lib.h?
// TODO: finish father routine
// FIXME: remove union, leave only array
// TODO: do i really need to detach shared memory?
// TODO: ??? fflush needed?
#include "lib.h"

// GLOBAL VARIABLES:
#define MINFILESIZE 7
#define MAXFILESIZE 10485760 // 10MB
int SEMID_FREE = -1;
int SEMID_WORK = -1;
int SHMID = -1;
struct command *SHM = (void *) -1;


void destroy_shared_resources()
{
    printf("Freeing resources...\n");
    printf("* Semaphore %d\n",SEMID_FREE);
    if (SEMID_FREE != -1)
        if (semctl(SEMID_FREE, 0, IPC_RMID) == -1)
            printf("!!!\t(please run: ipcrm -s %d)\n",SEMID_FREE);
    printf("* Semaphore %d\n",SEMID_WORK);
    if (SEMID_WORK != -1)
        if (semctl(SEMID_WORK, 0, IPC_RMID) == -1)
            printf("!!!\t(please run: ipcrm -s %d)\n",SEMID_WORK);
    if (SHM != (void *) -1)
        if (shmdt(SHM) == -1)
            printf("!!!\t(error detaching shared memory from process...should be no problem)\n");
    printf("* Shared Memory %d\n",SHMID);
    if (SHMID != -1)
        if (shmctl(SHMID, IPC_RMID, NULL) == -1)
            printf("!!!\t(plrease run: ipcrm -m %d)\n",SHMID);
    printf("DONE!\n");
}

void exception(char *s)
{
    printf("%s\n",s);
    perror("==>PERROR: ");
    destroy_shared_resources();
    exit(1);
}

void force_quit(int sigid)
{
    exception("ERROR received Ctrl-C signal!");
}

void init_shared_resources(char *keystr,int res_size)
{
    // Create First Semaphore Array
    key_t KEY = ftok(keystr,'a');
        if (KEY == -1)
            exception("ERROR while getting unique key for shared resource SEMID_FREE!");
    SEMID_FREE = semget(KEY, res_size, IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG); // execute permission is meaningless for semaphores, but ok..
        if (SEMID_FREE == -1)
            exception("ERROR while getting id for shared resource SEMID_FREE!");
        union semun cmdinfo;
        unsigned short array[res_size];
        for (int i=0; i<res_size; ++i)
            array[i] = 1;
        cmdinfo.array = array;
        if (semctl(SEMID_FREE, 0, SETALL, cmdinfo) == -1)
            exception("ERROR while setting values for shared resource SEMID_FREE!");
    printf("FATHER: Sem1 is %d\n",SEMID_FREE);

    // Create Second Semaphore Array
    KEY = ftok(keystr,'b');
        if (KEY == -1)
            exception("ERROR while getting unique key for shared resource SEMID_WORK!");
    SEMID_WORK = semget(KEY, res_size, IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG); 
        if (SEMID_WORK == -1)
            exception("ERROR while getting id for shared resource SEMID_WORK!");
        for (int i=0; i<res_size;++i)
            cmdinfo.array[i] = 0;
        if (semctl(SEMID_WORK, 0, SETALL, cmdinfo) == -1)
            exception("ERROR while setting values for shared resource SEMID_WORK!");
    printf("FATHER: Sem2 is %d\n",SEMID_WORK);
    
    // Create and attach Shared Memory
    KEY = ftok(keystr,'c'); // can be 'a'?
        if (KEY == -1)
            exception("ERROR while getting unique key for shared resource SHMID!");
    SHMID = shmget(KEY, sizeof(struct command)*res_size, IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG);
        if (SHMID == -1)
            exception("ERROR while getting id for shared resource SHMID!");
    SHM = shmat(SHMID, NULL, 0);
        if (SHM == (void *) -1)
            exception("ERROR while attaching shared memory SHMID to process!");
    printf("FATHER: Shm is %d\n",SHMID);

    // Initialize shared memory:
    for(int i=0; i<res_size; ++i)
        SHM[i].instr = '?'; // no op yet
}

int find_free_procs(int nprocs)
{
    struct sembuf op = { 0, -1, IPC_NOWAIT };
    int id;
    for (id=1; id<=nprocs; ++id)
    {
        op.sem_num = id;
        if (semop(SEMID_FREE, &op, 1) == 0)
        {
            V(SEMID_FREE,id); // free up resource i just used
            break;
        }
    }
    // if not found, use first proc
    if (id == nprocs + 1)
        id = 1;
    return id;
}


void slave(int id)// ??? Should make get_data | drop result, more explicit?
{
    while(1)
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
                    exception("ERROR invalid instruction found!");
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

void help(char *prog_name)
{
    fprintf(stderr,"uso: %s <nome file di calcolo> <nome file per i risultati>\n",prog_name);
    fprintf(stderr,"requirements:\n\
            * there must be no empty lines between command instructions\n\
            * every line must contain expected information\n");
    exit(0);
}


int main(int argc, char **argv)
{
    if (signal(SIGINT, force_quit) == SIG_ERR)
        exception("ERROR while registering SIGINT signal!");
    // Check if correct launch:
    if (argc != 3)
        help(argv[0]);

    // Get access to input and output files:
    printf("FATHER: opening files...\n");
    int fdinput = open(argv[1], O_RDONLY, S_IRUSR | S_IRGRP);
    if (fdinput == -1)
        exception("ERROR while opening input file!");
    int fdoutput = open(argv[2], O_WRONLY | O_CREAT | O_SYNC, S_IWUSR | S_IWGRP);
    if (fdoutput == -1)
        exception("ERROR while opening output file!");

    // Get Input filesize:
    struct stat stats;
    if (fstat(fdinput, &stats) == -1)
        exception("ERROR while getting input file filesize!");
    const int FILESIZE = stats.st_size + 1; // !!! +1 because i need space for end marker!
    if (FILESIZE < MINFILESIZE + 1)
        exception("ERROR input file too small");
    if (FILESIZE > MAXFILESIZE + 1)
        exception("ERROR input file way too big");

    // Read file into buffer:
    printf("FATHER: reading input file...\n");
    char buf[FILESIZE];
    char *buf_offset = buf;
    if(read(fdinput, buf, FILESIZE-1) == -1)
        exception("ERROR while loading input file into buffer!");
    buf[FILESIZE-1]='\0';
    /// ??? fflush?
    if (close(fdinput) == -1)
        exception("ERROR while closing input file!");

    // Get threads:
    const int NPROCS = next_integer(&buf_offset);
    if (NPROCS<1)
        exception("please create more processes");

    //  Init shared resources:
    printf("FATHER: getting shared resources...\n");
    init_shared_resources(argv[2],NPROCS + 1); // !!! +1 because i want to use "id" freely

    // Make threads:
    printf("FATHER: forking off to %d children...\n",NPROCS);
    for (int id = 1; id<=NPROCS; ++id)
    {
        int pid = fork();
        if (pid == -1)
            exception("ERROR while forking!");
        else if (pid==0) // Child:
            slave(id);
    }

    // Father:
    struct command cmd;
    const int NOPS = get_num_ops(buf_offset);
    int RESULTS[NOPS];

    // Send all commands
    printf("FATHER: sending %d commands...\n",NOPS);
    for (int j = 0; j<NOPS; ++j)
    {
        // Setup info to send to process:
        int id = next_command(&buf_offset,&cmd);
        if (id == 0)
            id = find_free_procs(NPROCS);

        // Send Off Command (possibly grabbing result for previously sent command)
        cmd.res_pos = j;
        master(id, &cmd, RESULTS);
    }

    // Wait for all processing to be over, save eventual data, kill child
    printf("FATHER: waiting for procs to finish...\n");
    for (int id=1; id<=NPROCS; ++id)
    {
        P(SEMID_FREE, id);
        if (SHM[id].instr == '!')
            RESULTS[SHM[id].res_pos] = SHM[id].res;
        SHM[id].instr = 'K';
        V(SEMID_WORK,id);
    }

    // Wait for all children to exit
    printf("FATHER: waiting for children to die...\n");
    while( wait(NULL) > 0); // ??? should i wait for nprocs processes?

    // Write RESULTS to buf buffer
    printf("FATHER: writing results to output file...\n");
    int output_size = 0;
    for (int j=0; j<NOPS; ++j)
        output_size += sprintf(buf + output_size, "%d\n", RESULTS[j] );

    // Write buffer to output file
    int nwrite = write(fdoutput, buf, output_size);
    if (nwrite < output_size || nwrite == -1)
        exception("ERROR while writing to output file!");
    if (close(fdoutput) == -1)
        exception("ERROR while closing output file!");
    
    // Free up resources
    destroy_shared_resources();
    printf("OKOKOKOKOKOKOKOKOKOKOKOKOKOK :-)\n");
    return 0;
}
