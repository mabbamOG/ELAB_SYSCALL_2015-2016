#ifndef LIB_IPC
#define LIB_IPC

extern int SEMID_FREE;
extern int SEMID_WORK1;
extern int SHMID;
extern struct command *SHM;

struct command
{
    char instr;
    int par1, par2, res, res_pos;
};

void P(int semid, int semchoice);
void V(int semid, int semchoice);
void destroy_shared_resources();
void init_shared_resources(char *keystr,int res_size);

#endif
