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




/** @file
 *
 * @brief this is the file containing all main IPC shared resources operating functionality.
 *
 * lib_ipc.c comes along with its own header file (lib_ipc.h), to provide a public interface to IPC functionality of this program.
 * In this file the P() and V() functions allow for simple Semaphore management, while init_shared_resources() and 
 * destroy_shared_resources() allow for simple global resource management.
 *
 */
#include "lib_ipc.h"
#include "lib_error.h"
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <stdlib.h>

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
    debugf("Freeing resources...\n");
    debugf("* Semaphore %d\n",SEMID_FREE);
    // Destroy Sem1 array, if it has already been initialized
    if (SEMID_FREE != -1)
    {
        if (semctl(SEMID_FREE, 0, IPC_RMID) == -1)
            debugf("!!!\t(please run: ipcrm -s %d)\n",SEMID_FREE);
        else SEMID_FREE = -1;
    }
    debugf("* Semaphore %d\n",SEMID_WORK);
    // Destroy Sem2 array, if it has already been initialized
    if (SEMID_WORK != -1)
    {
        if (semctl(SEMID_WORK, 0, IPC_RMID) == -1)
            debugf("!!!\t(please run: ipcrm -s %d)\n",SEMID_WORK);
        else SEMID_WORK = -1;
    }
    // Detach Shm from this process, if it has already been attached
    if (SHM != (void *) -1)
    {
        if (shmdt(SHM) == -1)
            debugf("!!!\t(error detaching shared memory from process...should be no problem)\n");
        else SHM = (void *) -1;
    }
    // Destroy Shm array, if it has already been initialized
    debugf("* Shared Memory %d\n",SHMID);
    if (SHMID != -1)
    {
        if (shmctl(SHMID, IPC_RMID, NULL) == -1)
            debugf("!!!\t(plrease run: ipcrm -m %d)\n",SHMID);
        else SHMID = -1;
    }
    debugf("DONE!\n");
}

void init_shared_resources(char *keystr,int res_size)
{
    // Create First Semaphore Array
    key_t KEY = ftok(keystr,'a');
        if (KEY == -1)
            exception("ERROR while getting unique key for shared resource SEMID_FREE!");
    debugf("FATHER: Sem1 key: %d\n", KEY);
    SEMID_FREE = semget(KEY, res_size, IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG);
        if (SEMID_FREE == -1)
            exception("ERROR while getting id for shared resource SEMID_FREE!");
        unsigned short array[res_size];
        for (int i=0; i<res_size; ++i)
            array[i] = 1;
        if (semctl(SEMID_FREE, 0, SETALL, &array) == -1)
            exception("ERROR while setting values for shared resource SEMID_FREE!");
    debugf("FATHER: Sem1 id: %d\n",SEMID_FREE);

    // Create Second Semaphore Array
    KEY = ftok(keystr,'b');
        if (KEY == -1)
            exception("ERROR while getting unique key for shared resource SEMID_WORK!");
    debugf("FATHER: Sem2 key: %d\n", KEY);
    SEMID_WORK = semget(KEY, res_size, IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG); 
        if (SEMID_WORK == -1)
            exception("ERROR while getting id for shared resource SEMID_WORK!");
        for (int i=0; i<res_size;++i)
            array[i] = 0;
        if (semctl(SEMID_WORK, 0, SETALL, &array) == -1)
            exception("ERROR while setting values for shared resource SEMID_WORK!");
    debugf("FATHER: Sem2 id: %d\n",SEMID_WORK);
    
    // Create and attach Shared Memory
    KEY = ftok(keystr,'a');
        if (KEY == -1)
            exception("ERROR while getting unique key for shared resource SHMID!");
    debugf("FATHER: Shm key: %d\n", KEY);
    SHMID = shmget(KEY, sizeof(struct command)*res_size, IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG);
        if (SHMID == -1)
            exception("ERROR while getting id for shared resource SHMID!");
    SHM = shmat(SHMID, NULL, 0);
        if (SHM == (void *) -1)
            exception("ERROR while attaching shared memory SHMID to process!");
    debugf("FATHER: Shm id: %d\n",SHMID);

    // Initialize shared memory
    for(int i=0; i<res_size; ++i)
        SHM[i].instr = '?'; // no op yet
}

