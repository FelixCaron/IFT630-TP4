/***************************************************************************************************


***************************************************************************************************/

#include "../../includes/pcslib.h"
#include "../../includes/signal.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

using std::cout;
using std::endl;
typedef void Sigfunc(int);
Sigfunc *signal(int, Sigfunc *);

int port;

struct file_to_send {
    char name[100];
    int clientId;
};
struct mesg_buffer {
    long mesg_type;
    int clientId;
    char mesg_text[100];
    
} message;

// INFO for msgrcv msgsnd : https://linux.die.net/man/2/msgrcv
// général: https://man7.org/linux/man-pages/man7/sysvipc.7.html
static void *send_file(void* arg){
    file_to_send* file_info;
    file_info = (file_to_send*) arg;
    cout<<"Sending: '"<< file_info->name<<"' to client: "<<file_info->clientId<<endl;
    //HERE WE SEND THE FILE with messages
    cout<<"file sent"<<endl;
    
}
static void send_msg(key_t key, mesg_buffer msg) {
    int msgid;

    // msgget creates a message queue
    // and returns identifier
    msgid = msgget(key, 0666 | IPC_CREAT);

    msgsnd(msgid, &msg, sizeof(msg), 0);

   
}

static mesg_buffer get_msg(key_t key) {
    int msgid;

    // msgget creates a message queue
    // and returns identifier
    // voir les param
    msgid = msgget(key, 0666| IPC_CREAT);
    mesg_buffer msg;
    mesg_buffer toSend;
    if (msgrcv(msgid, &msg, sizeof(msg), 1, IPC_NOWAIT) != -1) {
        
        cout<<"Connection demand from : " << msg.clientId<<endl;
        toSend = {msg.clientId, msg.clientId, "Connection accepted"};
        send_msg(msg.clientId, toSend);
        cout<<"Response sent"<<endl;

        pthread_t thread;
        file_to_send info = {"Nom du fichier", msg.clientId};
        pthread_create(&thread,NULL,send_file,(void*)&info);
		
        
    }
    // intéssant: https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-msgrcv-message-receive-operation
    // en bref on pourra faire en sorte que le serveur reçoit tout mais send spécifiquemment a certaine client
    // avec un client ID ? (voir le param msgtyp )
   //

    return message;
}

static void end_queue(key_t key) {
    cout << "Deleting queue :" + key << std::endl;
    // Supprimer a la fin !
    int msgid = msgget(key, 0666 | IPC_CREAT);
    msgctl(msgid, IPC_RMID, NULL);
}

// Procédure qui gère le signal.
void handle_signint(int sigNumber) {
    end_queue(port);
	exit(0);
	// ...
}

int main(int argc, char* argv[]) {
	cout << "Server started" << endl;
	// Association du signal avec la procédure de gestion (callback).
    //port = atoi(argv[1]);
    port = 1337;
	signal(SIGINT, handle_signint);
    mesg_buffer leMessage;

	while (true) {
        leMessage = get_msg(port);
	}
    exit(0);
}
