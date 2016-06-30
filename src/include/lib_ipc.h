/***************** COPYRIGHT AND WARRANTY **********************************
    IPC and Syscall project for my O.S. course at Univr.
    Copyright (C) 2016  Mario A. Barbara (mariobarbara@tutanota.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/




#ifndef LIB_IPC
#define LIB_IPC

/** @name Global IPC shared resources variables.
 * @{ */
/// id of synchronization Semaphores the father needs to wait on, and that the child-processes signal
extern int SEMID_FREE; 
/// id of synchronization Semaphores the child-processes neet to wait on, and that the father signals
extern int SEMID_WORK; 
/// Shared Memory id used between father and child-processes
extern int SHMID; 
/// Shared Memory pointer used between father and child-processes
extern struct command *SHM; 
/* @} */

/// @brief The structure used in Shared Memory SHM used between father and child-processes.
struct command 
{
    /** @brief The character representing the instructions passed from father to child. Is set to '?'
    * when SHM is first initialized, and is set to '!' when a child-process completes a computation. This
    * is to distinguish shared memory of processes which have never completed a computation. In fact, 
    * not all the child-processes are required to execute a task.
    */
    char instr;
    /** @brief The first parameter of the command passed on to a child-process. */
    int par1;
    /** @brief The second parameter of the command passed on to a child-process. */
    int par2;
    /** @brief The result of the computation. */
    int res;
    /** @brief Is a necessary requirement to allow the father to keep sending off instructions
    * without stopping to wait for results. This value is copied over to memory when the command is given,
    * and used as an index for a local array of results when the result is grabbed.
    */
    int res_pos;
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
 * @param semchoice the id of the semaphore of semid to signal.
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
 * type (struct command) and its array is long res_size.
 */
void init_shared_resources(char *keystr,int res_size);

#endif
