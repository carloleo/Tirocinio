/*
    Questo file contiene l'implementazione del file utility.h
*/


#include "utility.h"
#include <stdio.h>



int readn(int fd, int n, void* buf){
    int tot = 0;
    int tmp = 0;
    while (tot != n){
        if(((tmp = read(fd,buf,n)) == -1) && (errno !=  EINTR)) return 0;
        tot += tmp;
    }



    return 1;
}



int writen(int fd, int n, void *buf) {
    int tot = 0;
    int tmp = 0;
    while (tot != n){
        if((tmp = write(fd,buf,n)) == -1 && (errno !=  EINTR) && (errno != _SC_PIPE)) return 0;
        tot += tmp;
    }


    return 1;
}

int send_frame(int fd,can_frame *frame){
    int notused;
    uint32_t tmp = frame->ID.value;
    notused = writen(fd, sizeof(tmp), (void *)&tmp);
    if(!notused) return notused;
    tmp = frame->DLC.value;
    notused = writen(fd,sizeof(tmp),(void *)&tmp);
    if(!notused) return notused;
    for(int i = 0 ; i < SIZEPAYLOAD ; i++){
        notused = writen(fd, sizeof(uint8_t),(void*)&frame->payload[i]);
        if(!notused) return notused;
    }
    return 1;

}

can_frame *read_frame(int fd){
    can_frame * frame = calloc(1, sizeof(can_frame));
    int notused;
    uint32_t tmp = 0;
    notused = readn(fd, sizeof(tmp), (void *) &tmp);
    if(!notused) return NULL;
    frame->ID.value = tmp;
    tmp = 0;
    notused = readn(fd, sizeof(tmp), (void *) &tmp);
    if(!notused) return NULL;
    frame->DLC.value = tmp;
    for(int i = 0 ; i < SIZEPAYLOAD ; i++){
        notused = readn(fd, sizeof(uint8_t),(void *) &frame->payload[i]);
        if(!notused) return NULL;
    }


    return frame;
}
