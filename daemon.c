/* Homework 
 * Author: Justin Liddicoat, Daniel Durazo, Tsosie Schneider
 *	Last Modified:  9 April, 2015
 */
 
#include "daemon.h"
 
void init_daemon(void){
 
}
 
void daemon_exit(int sig_number){
	syslog(LOG_NOTICE, "The Daemon with PID %d exists ...", getpid());
	closelog();
	exit(EXIT_SUCCESS);
}