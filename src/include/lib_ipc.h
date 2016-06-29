#ifndef LIB_IPC
#define LIB_IPC

/** @name Global IPC shared resources variables.
 * @{ */
extern int SEMID_FREE; /// Semaphores the father needs to wait on, and child-processes signal
extern int SEMID_WORK; /// Semaphores the child-processes neet to wait on, and father signals
extern int SHMID; /// Shared Memory id used between father and child-processes
extern struct command *SHM; /// Shared Memory pointer used between father and child-processes
/* @} */

struct command /// The structure used in Shared Memory SHM used between father and child-processes
{
    char instr;
    int par1, par2, res, res_pos;
};

/**
 * @brief the equivalent of a simple wait (P) semaphore instruction.
 *
 * @param semid the id of the semaphore array to use.
 * @param semchoice the id of the semaphore of semid to wait on.
 */
void P(int semid, int semchoice);

/**
 * @brief the equivalent of a simple signal (V) semaphore instruction.
 *
 * @param semid the id of the semaphore array to use.
 * @param semchoice the id of the semaphore of semid to wait on.
 */
void V(int semid, int semchoice);

/**
 * @brief destroys IPC shared resources declared by SEMID_FREE, SEMID_WORK, SHMID, SHM; this
 * is to prevent "memory leaks" even after the process has terminated.
 */
void destroy_shared_resources();

/**
 * @brief initializes IPC shared resources declared by SEMID_FREE, SEMID_WORK, SHMID, SHM.
 *
 * @param keystr is simply a constant string used to get a unique key for the IPC resources through ftok().
 * The program's output file is expected to be used, as there may be multiple instances of this program running
 * on multiple different input files.
 *
 * @param res_size is how long the Semaphore arrays and Shared Memory array should be. The shared memory is of
 * type (struct command) and is long res_size.
 */
void init_shared_resources(char *keystr,int res_size);

#endif
