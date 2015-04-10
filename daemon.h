/* Author(s): Justin Liddicoat, Daniel Durazo, Tsosie Schneider
 *
 * This header file contains everything needed for the
 * code in daemon.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>
#include <signal.h>

// Function prototypes
void daemon_init(void);
void daemon_exit(int);
