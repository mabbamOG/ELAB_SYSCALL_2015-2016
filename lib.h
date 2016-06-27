void P(int semid, int semchoice)
{
    op->sem_num = semchoice;
    op->sem_op = -1;
    op->sem_flg = 0;
    semop(semid, op, 1);
}


void V(int semid, int semchoice)
{
    op->sem_num = semchoice;
    op->sem_op = 1;
    op->sem_flg = 0;
    semop(semid, op, 1);
}

union semun  // needed for semctl() syscall
{
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO, Linux-Specific*/
};

struct command
{
    char instruction;
    int par1, par2, res;
};
