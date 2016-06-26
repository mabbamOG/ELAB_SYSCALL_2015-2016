#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
// THIS PROGRAM SUCKS, NEED TO CLEAN UP THE MAGAZZINO STRUCTURE, NOTHING MORE THAN AN ARRAY IS REQUIRED. BETTER YET, DONT USE A STRUCTURE AT ALL!

const int LIBERI = 0;
const int DISPONIBILI = 1;
const int MUTEX = 2;
int shmid = -1;
int semid = -1;
struct Magazzino *data = NULL;
#define QUEUESIZE 5
struct sembuf *op;

void exception(char *s)
{
    fprintf(stderr,"==>%s: ",s);
    perror("ERR: ");
    fprintf(stderr, "freeing resources....");
    // Removing shared memory
    if (shmid!=-1)
        shmdt(data);
        shmctl(shmid, IPC_RMID, NULL);
    // Removing semaphores
    if (semid!=-1)
        semctl(semid, 0, IPC_RMID);
    fprintf(stderr, "done!\n");
    free(op);
    exit(1);
}

void force_quit(int sigid)
{
    exception("FORCED TO QUIT");
}

struct Magazzino
{
    int testa;
    int totali;
    char queue[QUEUESIZE];
};

union semun  // needed for semctl() syscall
{
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO, Linux-Specific*/
};

void P(int semchoice)
{
    op->sem_num = semchoice;
    op->sem_op = -1;
    op->sem_flg = 0;
    semop(semid, op, 1);
}


void V(int semchoice)
{
    op->sem_num = semchoice;
    op->sem_op = 1;
    op->sem_flg = 0;
    semop(semid, op, 1);
}



int main(void)
{
    op = malloc(sizeof(struct sembuf));
    signal(SIGINT, force_quit);
    printf("FATHER: creating some stuff...\n");
    // Getting some shared memory
    shmid = shmget(666, sizeof(struct Magazzino), 0666 | IPC_CREAT | IPC_EXCL); 
        if (shmid==-1) exception("shmget");
    data = shmat(shmid, NULL, 0); 
        if (data==(void *)-1) exception("shmat");
        data->testa=0;
        data->totali=0;
    // Getting some semaphores
    semid = semget(666, 3, 0666 | IPC_CREAT | IPC_EXCL); 
        if (semid==-1) exception("semget");
        union semun cmdinfo;
        unsigned short array[3];
        array[LIBERI] = 5; array[DISPONIBILI] = 0; array[MUTEX] = 1;
        cmdinfo.array = array;
        if (semctl(semid, 0, SETALL, cmdinfo)==-1) exception("semctl");
    // Ready to start Producer-Consumer program!
    printf("FATHER: forking!\n");
    int pid = fork();
        if (pid==-1) exception("fork");
    if (pid==0)
    {
        printf("\t\t\t\tCONSUMER: start!\n");
        char c = ' ';
        while(c != 'r')
        {
            printf("\t\t\t\tCONSUMER: wait if empty...\n");
            P(DISPONIBILI);
                P(MUTEX);
                    c = data->queue[ data->testa ];
                    data->testa = (data->testa + 1) % QUEUESIZE;
                    data->totali--;
                V(MUTEX);
                printf("\t\t\t\tCONSUMER: got %c\n",c);
            printf("\t\t\t\tCONSUMER: tell producer we got one!\n");
            V(LIBERI);
        }
        return 0;
    }
    else
    {
        printf("PRODUCER: start!\n");
        for(char i = 'a'; i<='r';++i)
        {
            printf("PRODUCER: wait if full...\n");
            P(LIBERI);
                P(MUTEX);
                    data->queue[ (data->testa + data->totali) % QUEUESIZE] = i;
                    data->totali++;
                V(MUTEX);
                printf("PRODUCER: made %c\n",i);
            printf("PRODUCER: tell consumer we made one!\n");
            V(DISPONIBILI);
        }
        printf("* FATHER: waiting for child to quit...\n");
        wait(NULL);
        // Removing shared memory
        shmdt(data);
        shmctl(shmid, IPC_RMID, NULL);
        // Removing semaphores
        semctl(semid, 0, IPC_RMID);
        printf("* FATHER: :-)\n");
        return 0;
    }
}
