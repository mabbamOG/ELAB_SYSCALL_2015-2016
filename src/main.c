/** @file 
 *
 * @brief this is the main file for the program.
 *
 * main.c contains only the main() function along with the basic control functions
 * that describe the main logical steps required by the program. All other functions
 * have been placed into their own modules.
 *
 * @author Mario A. Barbara
 * @copyright 2016 Mario A. Barbara. All rights reserved.
 * @license This project is released under the GNU Public License version 3.
 * The complete LICENSE file should be provided along with the project's source code.
 *
 */
// TODO: standard naming of "get" and "next" functions, and all other functions!
// TODO: should i use buf_offset?
// TODO: ??? do i need more static variables in functions?
// TODO: ??? fix naming of some functions?
// TODO: should i change buf_offset to offset?
// TODO: check im not using any man 3 calls except printf or ftok ecc...
#include "lib_ipc.h"
#include "lib_io.h"
#include "lib_error.h"
#include <signal.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// GLOBAL CONSTANTS
/// 8B, the input file should be at least this big!
#define MINFILESIZE 8 
/// 10MB, the  input file should not exceed this amount!
#define MAXFILESIZE 10485760 

/**
 * @brief the main processing unit for the father, used to send off commands
 * to child-processes one at a time.
 *
 * @param id the id of the process to send the command off to.
 * @param cmd the pointer to a struct command containing the information
 * fot the command to be sent.
 * @param RESULTS the pointer to the father's array of command results. It
 * is passed on to this function in order to be able to gather results to
 * possible past commands sent to child-process id.
 */
void master(int id, struct command *cmd, int *RESULTS);

/**
 * @brief the main processing unit for a child process.
 *
 * @param id the id for this child-process, required to manage shared memory
 * accesses.
 */
void slave(int id);

/**
 * @brief gets the id for a process that is currently free.
 *
 * @param nprocs the number of processes that need to be tested for availability.
 *
 * @return the id of a child waiting on a semaphore to execute commands. If no
 * free children are found then 1 is returned.
 */
int find_free_proc(int nprocs);

/** 
 * @brief prints information about usage of the program.
 *
 * @param prog_name the name of the current program, argv[0] is expected to be used when calling this function.
 * */
void help(char *prog_name);

void help(char *prog_name)
{
    debugf("uso: %s <nome file di calcolo> <nome file per i risultati>\n",prog_name);
    debugf("requirements:\n\
            * there must be no empty lines between command instructions\n\
            * every line must contain all expected information\n");
    exit(0);
}

int main(int argc, char **argv)
{
    // Registering Ctr-C signal
    if (signal(SIGINT, force_quit) == SIG_ERR)
        exception("ERROR while registering SIGINT signal!");

    // Check if correct launch
    if (argc != 3)
        help(argv[0]);

    // Get access to input and output files:
    debugf("FATHER: opening files...\n");
    int fdinput = open(argv[1], O_RDONLY, 0);
    if (fdinput == -1)
        exception("ERROR while opening input file!");
    int fdoutput = open(argv[2], O_WRONLY | O_CREAT | O_SYNC | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fdoutput == -1)
        exception("ERROR while opening output file!");

    // Get Input filesize:
    struct stat stats;
    if (fstat(fdinput, &stats) == -1)
        exception("ERROR while getting input file filesize!");
    const int FILESIZE = stats.st_size;
    if (FILESIZE < MINFILESIZE)
        exception("ERROR input file too small");
    if (FILESIZE > MAXFILESIZE)
        exception("ERROR input file way too big");

    // Read file into buffer:
    debugf("FATHER: reading input file...\n");
    char buf[FILESIZE];
    char *buf_offset = buf;
    if(read(fdinput, buf, FILESIZE) == -1)
        exception("ERROR while loading input file into buffer!");
    buf[FILESIZE-1]='\0'; // set end marker
    if (close(fdinput) == -1)
        exception("ERROR while closing input file!");

    // Get threads:
    const int NPROCS = next_integer(&buf_offset);
    if (NPROCS<1)
       exception("please create more processes");

    //  Init shared resources:
    debugf("FATHER: getting shared resources...\n");
    init_shared_resources(argv[2],NPROCS + 1); // !!! +1 because i want to use "id" freely

    // Make threads:
    debugf("FATHER: forking off to %d children...\n",NPROCS);
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
    debugf("FATHER: sending %d commands...\n",NOPS);
    for (int j = 0; j<NOPS; ++j)
    {
        // Setup info to send to process:
        int id = next_command(&buf_offset,&cmd);
        if (id == 0)
            id = find_free_proc(NPROCS);

        // Send Off Command (possibly grabbing result for previously sent command)
        cmd.res_pos = j;
        master(id, &cmd, RESULTS);
    }

    // Wait for all processing to be over, save eventual data, kill child
    debugf("FATHER: waiting for procs to finish...\n");
    for (int id=1; id<=NPROCS; ++id)
    {
        P(SEMID_FREE, id);
        if (SHM[id].instr == '!')
            RESULTS[SHM[id].res_pos] = SHM[id].res;
        SHM[id].instr = 'K';
        V(SEMID_WORK,id);
    }

    // Wait for all children to exit
    debugf("FATHER: waiting for children to die...\n");
    while( wait(NULL) > 0); // ??? should i wait for nprocs processes?

    // Write RESULTS to buf buffer
    debugf("FATHER: writing results to output file...\n");
    int output_size = 0;
    for (int j=0; j<NOPS; ++j)
        output_size += sprintf(buf + output_size, "%d\n", RESULTS[j] );

    // Write buffer to output file
    if(write(fdoutput, buf, output_size) < output_size)
        exception("ERROR while writing to output file!");
    if (close(fdoutput) == -1)
        exception("ERROR while closing output file!");
    
    // Free up resources
    destroy_shared_resources();
    debugf("Program Completed Successfully! :-)\n");
    return 0;
}


int find_free_proc(int nprocs)
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
                    debugf("PROC #%d: QUITTING!\n",id);
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

