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




#ifndef LIB_ERROR
#define LIB_ERROR

/**
 * @brief this function is used to clean up IPC resources and exit() whenever a syscall returns an error.
 *
 * @param s a string to be printed to stderr before freeing IPC resources and quitting.
 */
void exception(char *s);

/**
 * @brief signal handler for quitting the program.
 *
 * @param sigid the id of the signal received.
 */
void force_quit(int sigid);

/**
 * @brief prints formatted strings to stderr, but can only handle 
 * one argument and only of type (char *) or (int).
 *
 * @param s the formatted string telling debugf() what to print.
 * @param ... the variable arguments list (va_list) containg possible
 * futher arguments. see printf() from <stdio.h> for an example of this.
 */
void debugf(char *s, ...);

#endif
