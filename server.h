/* Author(s): Justin Liddicoat, Daniel Durazo, Tsosie Schneider
 *
 * This header file contains everything needed for
 * the server program running as a daemon.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "daemon.h"

#define FALSE 0
#define TRUE !FALSE

typedef struct container{
    pthread_mutex_t lock;
    int *memory;
    int sock_descriptor;
} Container;

//Protype the thread function
void *conn_handler(void *sock_descriptor);
int threeAplusOne(int start, int total, int* memory);
