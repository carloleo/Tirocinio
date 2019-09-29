#ifndef UTILITY_LIBRARY_H
#define UTILITY_LIBRARY_H

#include <stdint-gcc.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

#define ISNULL(fun,var,msg)\
    if((var = fun) == NULL){\
        perror(msg);\
        exit(1);\
    }

#define ISZERO(fun,var,msg)\
    if((var = fun) == 0){\
        perror(msg);\
        exit(1);\
    }
#define ISLESSZERO(fun,var,msg)\
    if((var = fun ) < 0){\
        perror(msg);\
        exit(1);\
    }





#define SIZEPAYLOAD 8
#define BYTEKEY 16
#define MAXID 1048575 //2^29
#define KEYMASTERID 9897

/*
 * rappresenta il campo ID di un frame can mediante l'uso di una bitfield di 29 bit
 */

typedef struct id{
    unsigned int value : 29;
}id;

/*
 *  rappresenta il campo DLC di un frame can mediante l'uso di una bitfield di 4 bit
 */


typedef struct dlc{
    unsigned int value : 4;
}dlc;

/*
 * struttura che rappresenta un frame CAN nei sui campi piu' importanti
 */
typedef struct can_frame{
    id ID;
    dlc DLC;
    uint8_t payload[8];
}can_frame;

/*
 * @brief: legge n byte dal file descriptor fd e li memorizza in buf
 * @return: 0 in caso di errore 1 altrimenti
 */

int readn(int fd,int n, void* buf);

/*
 * @brief: scrive buf di n byte sul file descriptor fd
 * @return: 0 in caso di errore 1 altrimenti
 */
int writen(int fd, int n, void* buf);

/*
 * @brief: invia un frame CAN sul file descriptor fd
 * @return: 0 in caso di errore 1 altrimenti
 */

int send_frame(int fd,can_frame *frame);
/*
* @brief: legge un frame CAN dal file descriptor fd
* @return: NULL in caso di errore puntatore al frame letto altrimenti
*/

can_frame* read_frame(int fd);


#endif //UTILITY_LIBRARY_H
