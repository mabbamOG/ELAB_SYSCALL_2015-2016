#include "lib_ipc.h"

int SEMID_FREE = -1;
int SEMID_WORK = -1;
int SHMID = -1;
struct command *SHM = (void *) -1;

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

void init_shared_resources(char *keystr,int res_size)
{
    // Create First Semaphore Array
    key_t KEY = ftok(keystr,'x');
        if (KEY == -1)
            exception("ERROR while getting unique key for shared resource SEMID_FREE!");
    SEMID_FREE = semget(KEY, res_size, IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG);
        if (SEMID_FREE == -1)
            exception("ERROR while getting id for shared resource SEMID_FREE!");
        unsigned short array[res_size];
        for (int i=0; i<res_size; ++i)
            array[i] = 1;
        if (semctl(SEMID_FREE, 0, SETALL, &array) == -1)
            exception("ERROR while setting values for shared resource SEMID_FREE!");
    printf("FATHER: Sem1 is %d\n",SEMID_FREE);

    // Create Second Semaphore Array
    KEY = ftok(keystr,'y');
        if (KEY == -1)
            exception("ERROR while getting unique key for shared resource SEMID_WORK!");
    SEMID_WORK = semget(KEY, res_size, IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG); 
        if (SEMID_WORK == -1)
            exception("ERROR while getting id for shared resource SEMID_WORK!");
        for (int i=0; i<res_size;++i)
            array[i] = 0;
        if (semctl(SEMID_WORK, 0, SETALL, &array) == -1)
            exception("ERROR while setting values for shared resource SEMID_WORK!");
    printf("FATHER: Sem2 is %d\n",SEMID_WORK);
    
    // Create and attach Shared Memory
    KEY = ftok(keystr,'z'); // can be 'a'?
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

