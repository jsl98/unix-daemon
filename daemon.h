/* Homework 7
 * Author: Justin Liddicoat, Daniel Durazo, Tsosie Schneider
 *	Last Modified:  9 April, 2015
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <syslog.h>
 
//function prototypes
//not sure if they should be void but using it as a placeholder
void init_daemon(void)
void daemon_exit(int sig_number)