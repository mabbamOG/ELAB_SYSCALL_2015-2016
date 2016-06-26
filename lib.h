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
