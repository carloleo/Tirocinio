/*
 * Questo file contiene un implementazione prototipo del key master. All'intero del prototipo svolge il ruolo di server
 */

#include <stdio.h>
#include "utility.h"
#include "rsa.h"
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <signal.h>

#define PORT 6006
#define MAXCONNECTIONS 32


uint8_t *symmetric_key; //variabile che contiene la chiave di sessione
__sig_atomic_t newkey = 0; //è utilizzata per far capire al server quando deve aggiornare la chiave di sessione
uint8_t * generateKey(void);
void printKey(uint8_t* key);
static void handler(int);
__sig_atomic_t termina = 0; //variabile utilizzata per indicare la terminazione
uint8_t* resetSymmetric_key(uint8_t* key);
uint64_t   first,second;//contengono rispettivamente il primo ed il secondo intero con i quali la chiave e cifrata
static void free_memory(int);

int main() {
    int fd_server;
    int fd_client;
    struct sockaddr_in server_addr;
    can_frame *frame_out;
    can_frame *frame_in;
    uint64_t curr_n;
    uint64_t curr_e;
    struct sigaction s,ss;
    int done = 0;
    int notused;

    memset((void*)&s,0,sizeof(s));
    s.sa_handler = &handler; // registro nuovo gestore per il segnale SIGALRM, utilizzato come timer per aggiornare la chiave
    ISLESSZERO(sigaction(SIGALRM,&s,NULL),notused,"sigaction fail")

    memset((void*)&ss,0,sizeof(s));
    ss.sa_handler = &free_memory;
    ISLESSZERO(sigaction(SIGINT,&ss,NULL),notused,"sigaction fail") //registro gestore per SIGINT (ctrl + C) per liberare la memoria
    																//quando il server termina

    bzero((char *) &server_addr, sizeof(server_addr)); //inizializzo la struttura

    ISLESSZERO(socket(AF_INET,SOCK_STREAM,0),fd_server,"socket fail") //apro il socket

    setsockopt(fd_server,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int)); //riuso del socket



    server_addr.sin_family=AF_INET; //tipo di socket
    server_addr.sin_port = PORT;//porta
    server_addr.sin_addr.s_addr = INADDR_ANY; //questo macro fa si che vengano recapitati al processo pacchetti da ogni interfaccia di rete
    ISLESSZERO(bind(fd_server,(struct sockaddr*)&server_addr,sizeof(server_addr)),notused,"bind fail")//associo al socket il suo socket address


    ISLESSZERO(listen(fd_server,MAXCONNECTIONS),notused,"bind fail") //eseguo la listen per dichiare che su tale socket accetto richieste di connesione


    ISNULL(generateKey(),symmetric_key,"generate key fail"); //genero la chiave di sessione

    //stampo a video la chiave di sessione generata
    printKey(symmetric_key);


    alarm(10);// imposto il timer per aggiornare la chiave a 10s
    //loop key master
    while (1){
        printf("server wait connection\n");
        fflush(stdout);
        if(newkey) symmetric_key = resetSymmetric_key(symmetric_key);//aggiorno la chiave di sessione
        if(((fd_client = accept(fd_server,NULL,NULL)) == -1) && (errno != EINTR)){ //aspetto richieste di connesione
            perror("accept fail");
            exit(1);
        }
        if(termina) exit(0); //al processo è stato recapitato SIGINT mentre aspettava una connesione-> termina
        if(newkey && (errno == EINTR)){ //sono stato interrotto metre aspettavo la connesione, quindi aggiono la chiave. SIGALRM recapitato al processo
            symmetric_key = resetSymmetric_key(symmetric_key);
            continue;
        }
        printf("connection  accept\n");
        ISNULL(read_frame(fd_client),frame_in,"read value n") //leggo n
        curr_n = toUnit(frame_in->payload);//trasformo i byte in un intero 
        free(frame_in);
        ISNULL(read_frame(fd_client),frame_in,"read value e") //leggo n
        curr_e = toUnit(frame_in->payload); //trasformo i byte in un intero
        free(frame_in);



        ISNULL(calloc(1, sizeof(can_frame)),frame_out,"calloc frame fail") //alloco la memoria per il frame utilizzato per comunicare la chiave di sessione
        frame_out->ID.value = KEYMASTERID;//setto il campo id del frame 
        frame_out->DLC.value = 15;
        int count = 0;
        while (!done){ //invio della chiave di sessione cifrata
           uint8_t* tmp;
           uint8_t* m;
           if(count == 0){
               count += 1;
               m = toArray(first); //trasformo il primo intero in un array di byte
               tmp = encrypt(m,curr_n,curr_e); //cifro il primo intero 
               free(m);
           }
           else {
           	   m = toArray(second); //trasformo il secondo intero in un array di byte
               tmp = encrypt(m,curr_n,curr_e); //cifro il secondo intero
               free(m);
               count += 1;
           }
           for(int i = 0; i < SIZEPAYLOAD; i++){ //inserisco l'intero cifrato nel payload del frame
               frame_out->payload[i] = tmp[i];
           }
           free(tmp);
           ISZERO(send_frame(fd_client,frame_out),notused,"send frame fail")  //invio il frame
           if(count == 2) done = 1; //chiave di sessione inviata

        }
        done = 0;
        free(frame_out); //libero la memoria
        printf("client served\n");
        fflush(stdout);
        close(fd_client);//chiudo il socket del client perchè esso termina dopo aver ricevuto la chiave di sessione
    }



}

/*
 * @brief: funzione utilizzata per cifrare la chiave di sessione
 */


uint8_t *generateKey(void) {
    srand((unsigned int) time(NULL) % MODULO);
    uint8_t *key = calloc(BYTEKEY, sizeof(uint8_t));
    uint8_t *tmp = calloc(SIZEPAYLOAD,sizeof(uint8_t));
    uint8_t*  reduced_byte = NULL;
    if (!key || !tmp) return NULL;
    for (int i = 0; i < BYTEKEY; i++) { //genero byte random
        key[i] = (uint8_t) rand();
    }

    // prendo i primi 8
    for (int j = 0; j < SIZEPAYLOAD ; ++j) {
        tmp[j] = key[j];
    }

    // li converto in un intero per poterlo cifrare, minore del limite inferiore per il valore n definito
    uint64_t num = toUnit(tmp) % INT32_MAX;
    first = num;
    reduced_byte = toArray(num);
    //li sostituisco nelle lore posizioni
    for (int j = 0; j < SIZEPAYLOAD ; ++j) {
         key[j] = reduced_byte[j];
    }
    free(reduced_byte);
    int j = 8;
    //prendo i secondi 8 byte
    for (int k = 0; k < SIZEPAYLOAD ; ++k) {
        tmp[k] = key[j];
        j++;

    }
    // li converto in un intero per poterlo cifrare, minore del limite inferiore per il valore n definito
    num = toUnit(tmp) % INT32_MAX;
    second = num;
    reduced_byte = toArray(num);
    j = 8;
    //li sostituisco nelle loro posizioni
    for (int l = 0; l < SIZEPAYLOAD; ++l) {
        key[j] = reduced_byte[l];
        j++;
    }
    free(reduced_byte); //libero la memoria
    free(tmp);
    return key;
}
/*
	@brief: funzione utilizzata per gestire il segnale SIGALRM
			imposta la variabile newkey a 1 per fare aggiornare
			la chiave di sessione
*/

static void handler(int sign) {
    newkey = 1;

}
/*
	@brief: stampa la chiave di sessione key
*/

void  printKey(uint8_t* key){
    printf("SYMMETRIC KEY SERVER\n");
    for (int i = 0; i < BYTEKEY ; ++i) {
        printf("%u\t",key[i]);
    }
    printf("\n");
    fflush(stdout);
}
/*
	@brief: aggiorna la chiave di sessione
*/

uint8_t*  resetSymmetric_key(uint8_t* key){
    free(key); //libera la memoria
    key = generateKey(); //genera la nuova chiave di sessione
    printKey(key);
    newkey = 0; //ripristina lo stato
    alarm(10); //riavvia il timer
    return key;
}
/*
	@brief: ripulisce la memoria alla ricezione
			del segnale che implica la terminazione
*/

void free_memory(int sign){
	if(symmetric_key) free(symmetric_key);
	termina = 1;
}
