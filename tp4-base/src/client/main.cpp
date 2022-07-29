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

int timer = 0;

struct mesg_buffer
{
    long mesg_type;
    int clientId;
    char mesg_text[100];
};

// INFO for msgrcv msgsnd : https://linux.die.net/man/2/msgrcv
// g�n�ral: https://man7.org/linux/man-pages/man7/sysvipc.7.html

static void send_msg(key_t key, mesg_buffer msg, int clientId)
{
    int msgid;

    // msgget creates a message queue, ici on a enlever IPC_CREAT car on veut crash quand on as pas le bon data.
    msgid = msgget(key, 0666);

    // So the server can send back to us personally
    msg.clientId = clientId;
    // So just the server will listen to us (so no client id with 1337)
    msg.mesg_type = 1;

    msgsnd(msgid, &msg, sizeof(msg), 0);
}

static mesg_buffer get_msg(key_t clientId)
{
    int msgid = -1;

    // msgget creates a message queue, ici on a enlever IPC_CREAT car on veut crash quand on as pas le bon data.
    while(msgid==-1){
        msgid = msgget(clientId, 0666);
        };
    mesg_buffer message;

    while(msgrcv(msgid, &message, sizeof(message), clientId, IPC_NOWAIT)==-1){};

    return message;
}

int main(int argc, char *argv[])
{
    cout << "Client started" << endl;
    bool isConnected = false;
    int useId = atoi(argv[1]);
    int defaultPort = atoi(argv[2]);

    mesg_buffer leMessage = {1, useId};
    strcpy(leMessage.mesg_text, argv[3]);

    send_msg(defaultPort, leMessage, useId);
    while (true)
    {
        leMessage = get_msg(useId);
        if (strcmp(leMessage.mesg_text, "Close the connection") == 0)
        {
            isConnected = false;
            cout << "Connection closed" << endl;
            return 0;
        }
        else if (strcmp(leMessage.mesg_text ,"Connection accepted")==0)
        {
            cout << "Connection opened" << endl;
            isConnected = true;
        }
        else if (isConnected)
        {
            cout << "File part received " << leMessage.mesg_text << endl;
        }
        else
        {
            cout << "unexpected message: " << leMessage.mesg_text << endl;
            cout << "aborting" << endl;
        }
        //sleep(timer); // periode de repos entre chaque segment
    }
}
