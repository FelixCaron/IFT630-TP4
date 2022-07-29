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
#include <sys/stat.h>
#include <fstream>

using std::ios;
using std::ifstream;
using std::cout;
using std::endl;
typedef void Sigfunc(int);
Sigfunc *signal(int, Sigfunc *);

int port;
char directory[100];
struct file_to_send {
    char name[100];
    int clientId;
};
struct mesg_buffer {
    long mesg_type;
    int clientId;
    char mesg_text[100]; 
};

static void send_msg(mesg_buffer msg) {
    int msgid;

    // msgget creates a message queue
    // and returns identifier
    msgid = msgget(msg.clientId, 0666 | IPC_CREAT);

    msgsnd(msgid, &msg, sizeof(msg), 0);

   
}

// INFO for msgrcv msgsnd : https://linux.die.net/man/2/msgrcv
// général: https://man7.org/linux/man-pages/man7/sysvipc.7.html
static void *send_file(void* arg){
    mesg_buffer message;
    char txtBuffer[100];
    struct stat info;
    file_to_send* file_info;
    file_info = (file_to_send*) arg;
    cout<<"Sending: '"<< file_info->name<<"' to client: "<<file_info->clientId<<endl;
    
    ifstream file(file_info->name, ios::binary);

    if(stat(file_info->name, &info) != 0)
        perror("stat() error\n");
    cout << info.st_size << endl; // # de data byte dans file
    
    while(file.peek() != EOF)
    {
        file.read(txtBuffer, 100);
        strcpy(message.mesg_text,txtBuffer);
        message.mesg_type = file_info->clientId;
        message.clientId = file_info->clientId;
        send_msg(message);
    }
    cout<<"file sent"<<endl;
    mesg_buffer msg = {file_info->clientId, file_info->clientId, "Close the connection"};
    send_msg(msg);
}


static void get_msg(key_t key) {
    int msgid;

    // msgget creates a message queue
    // and returns identifier
    // voir les param
    msgid = msgget(key, 0666);
    mesg_buffer msg;
    mesg_buffer toSend;
    if (msgrcv(msgid, &msg, sizeof(msg), 1, IPC_NOWAIT) != -1) {
        
        cout<<"Connection demand from : " << msg.clientId<<endl;
        
        toSend = mesg_buffer{msg.clientId, msg.clientId, "Connection accepted"};
        send_msg(toSend);
        cout<<"Response sent"<<endl;

        pthread_t thread;
        file_to_send info;
        
        strcpy(info.name,directory);
        strcat(info.name,msg.mesg_text);
        info.clientId =  msg.clientId;

        pthread_create(&thread,NULL,send_file,(void*)&info); 
    }
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
    //port = 1337;

    strcpy(directory,  "/tp4/IFT630-TP4/transfer_folder/");
	signal(SIGINT, handle_signint);
    mesg_buffer leMessage;
    msgget(port, 0666|IPC_CREAT);
	while (true) {
        get_msg(port);
	}
    exit(0);
}
