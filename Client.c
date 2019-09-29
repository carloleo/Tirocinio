/*
 * Questo file contiene l'implementazione del client, ovvero un modulo del key-manager 
 * per avviare il programma ./Client -i [IP del server]
 */

#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "rsa.h"
#include "utility.h"

#define PORT 6006
void help(void);
void fill_payload(uint8_t *,uint64_t);

int main(int argc, char* argv[]) {

    int sockfd, portno, n;
    struct sockaddr_in server_addr;
    RSA_param * param;
    can_frame* frame_in;
    can_frame* frame_out;
    uint8_t symmetric_key[BYTEKEY];
    int notused;
    int done = 0;
    if(argc != 3){
        help();
    }

    memset((void*)symmetric_key,0, sizeof(symmetric_key));
    param = init(); //genero parametri dell'RSA, quindi chiave pubblica e privata

    memset((void*)&server_addr, 0,(size_t) sizeof(server_addr));

    server_addr.sin_family= AF_INET; //tipo di socket
    server_addr.sin_port = PORT; //porta del server
    server_addr.sin_addr.s_addr = inet_addr(argv[2]); //ip del server


    sockfd = socket(AF_INET,SOCK_STREAM,0); //apro il socket

    ISLESSZERO(connect(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr)),notused,"connect fail")//mi connetto al server
    printf("connected\n");

    ISNULL(calloc(1,sizeof(can_frame)),frame_out,"calloc fail")//alloco il frame can per inviare la chiave pubblica al server
    frame_out->ID.value = ((unsigned int) rand()) % MAXID; //genero id che identifica una ECU
    frame_out->DLC.value = 15; //questo codice associato al campo DLC indica che i byte inviati sono 8, quindi tutto il payload viene utilizzato

    fill_payload(frame_out->payload,param->n); //inerisco n nel payload

    ISZERO(send_frame(sockfd,frame_out),notused,"send_frame fail") //invio n

    fill_payload(frame_out->payload,param->e); //inserisco e nel payload

    ISZERO(send_frame(sockfd,frame_out),notused,"send_frame fail") //invio e

    int j = 0; //variabile utilizzata per costruire la chiave e capire quando si è ricevuta completamente
    while (!done) {
        ISNULL(read_frame(sockfd), frame_in, "read frame fail")
        if (frame_in->ID.value == KEYMASTERID) { //controllo dell' ID
            uint8_t *tmp = decipher(frame_in->payload, param->n, param->d);//decifro il payload

            
            for (int i = 0; i < SIZEPAYLOAD; i++) { //costruisco la chiave di sessione
                symmetric_key[j] = tmp[i];
                j += 1;
            }
            free(frame_in);
            free(tmp);
            if (j == BYTEKEY) done = 1; //chiave di sessione ricevuta completamente
        } else break;
    }

    if(done){ //chiave ricevuta dal key master
        printf("SYMMETRIC KEY CLIENT\n");
        for (int k = 0; k < BYTEKEY; ++k) {
            printf("%u\t",symmetric_key[k]); //stampo a video la chiave di sessione, byte a byte codificati come interi
        }
        printf("\n");
    }
    else printf("INVALID ID RECIVED\n"); //sono uscito dal ciclo perchè l'id non combaciava con quello del key master


    //pulisco la memoria e chiudo il socket
    free(frame_out);
    free(param);
    close(sockfd);


    return 0;
}
/*
    @brief: funzione di aiuto
*/
void help(void){
    printf("Passare argomenti al programma\n");
    printf("./client -i [ip server]");
    exit(0);
}
/*
    @brief: funzione utilizzata per riempire il payload di un frame CAN
*/

void fill_payload(uint8_t* payload,uint64_t t) {
    uint8_t *tmp = toArray(t);

    for (int i = 0; i < SIZEPAYLOAD; i++) {
        payload[i] = tmp[i];
    }
    free(tmp);
}
