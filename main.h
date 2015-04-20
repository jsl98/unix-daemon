/* Author(s): Justin Liddicoat, Daniel Durazo, Tsosie Schneider
 *
 * This header file contains everything needed for
 * the main/driver program.
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
