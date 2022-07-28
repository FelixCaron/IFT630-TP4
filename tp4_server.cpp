// tp4_server.cpp : Defines the entry point for the application.
//

#include "tp4_server.h"
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

using namespace std;

struct mesg_buffer {
    long mesg_type;
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
    //printf("Data send is : %s \n", msg.mesg_text);
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
    msgrcv(msgid, &message, sizeof(message), 0, IPC_NOWAIT); //
    //printf("Data receive is : %s \n", message.mesg_text);

    return message;
}

static void end_queue(key_t key) {
    cout << "Deleting queue :" + key << std::endl;
    // Supprimer a la fin !
    int msgid = msgget(key, 0666 | IPC_CREAT);
    msgctl(msgid, IPC_RMID, NULL);
}

int main(int argc, char* argv[])
{
    cout << "You have entered " << argc << " arguments:" << "\n";


    for (int i = 0; i < argc; ++i)
        cout << argv[i] << "\n";

    //string path = argv[1]; //Le chemin du répertoire contenant les fichiers à transférer
    // and 'more' : Les paramètres nécessaires pour identifier le ou les ports à utiliser pour communiquer avec le serveur.
    mesg_buffer leMessage = { 1 , "nice text"};

    send_msg(1337, leMessage);
    strcpy(leMessage.mesg_text, "this is a public test");

    send_msg(1337, leMessage);
    sleep(1);
    cout << "receive time" << "\n";
    leMessage = get_msg(1337);
    cout << leMessage.mesg_text << "\n";

    cout << "bla" << "\n";

    return 0;
}
