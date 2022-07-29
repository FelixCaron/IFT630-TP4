/***************************************************************************************************


***************************************************************************************************/

#include "../../includes/signal.h"
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

using std::cout;
using std::endl;


struct mesg_buffer {
    long mesg_type;
    int clientId;
    char mesg_text[100];
};

// INFO for msgrcv msgsnd : https://linux.die.net/man/2/msgrcv
// g�n�ral: https://man7.org/linux/man-pages/man7/sysvipc.7.html

static void send_msg(key_t key, mesg_buffer msg, int clientId) {
    int msgid;

    // msgget creates a message queue, ici on a enlever IPC_CREAT car on veut crash quand on as pas le bon data.
    msgid = msgget(key, 0666);

    // So the server can send back to us personally
    msg.clientId = clientId;
    // So just the server will listen to us (so no client id with 1337)
    msg.mesg_type = 1;

    msgsnd(msgid, &msg, sizeof(msg), 0);
}

static mesg_buffer get_msg(key_t clientId) {
    int msgid;

    // msgget creates a message queue, ici on a enlever IPC_CREAT car on veut crash quand on as pas le bon data.
    msgid = msgget(clientId, 0666);
    mesg_buffer message;
    
    msgrcv(msgid, &message, sizeof(message), clientId, 0);

    return message;
}


int main(int argc, char* argv[]) {
	cout << "Client started" << endl;

    int useId = 1;//atoi(argv[1]);
    int defaultPort = 1337;//atoi(argv[2]);
    
    mesg_buffer leMessage = { 1 , useId, "This is a connection demand!" };

    send_msg(defaultPort, leMessage, useId);

    leMessage = get_msg(useId);
    cout << leMessage.mesg_text << "\n";

	return 0;
}