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

struct mesg_buffer {
    long mesg_type;
    int clientId;
    char mesg_text[100];
} message;

// INFO for msgrcv msgsnd : https://linux.die.net/man/2/msgrcv
// général: https://man7.org/linux/man-pages/man7/sysvipc.7.html

static void send_msg(key_t key, mesg_buffer msg) {
    int msgid;

    // msgget creates a message queue
    // and returns identifier
    msgid = msgget(key, 0666 | IPC_CREAT);

    msgsnd(msgid, &msg, sizeof(msg), 0);

    // // display the message
    printf("Data send is : %s \n", msg.mesg_text);
}

static mesg_buffer get_msg(key_t key) {
    int msgid;

    // msgget creates a message queue
    // and returns identifier
    // voir les param
    msgid = msgget(key, 0666 | IPC_CREAT);

    // intéssant: https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-msgrcv-message-receive-operation
    // en bref on pourra faire en sorte que le serveur reçoit tout mais send spécifiquemment a certaine client
    // avec un client ID ? (voir le param msgtyp )
    if (msgrcv(msgid, &message, sizeof(message), 0, IPC_NOWAIT) != -1) {
        printf("Data receive is : %s \n", message.mesg_text);
        cout << "clientId:" + message.clientId << std::endl;

        strcpy(message.mesg_text, "this is a public test");
        message.mesg_type = message.clientId;
        send_msg(1337, message);
    }; //

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
    port = atoi(argv[1]);
    port = 1337;
	signal(SIGINT, handle_signint);
    mesg_buffer leMessage = { 1 ,0, "nice text" };

	while (true) {
        leMessage = get_msg(port);
	}
    exit(0);
}
