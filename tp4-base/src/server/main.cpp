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
#include <fstream>
#include <chrono>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;
using std::ios;
using std::ifstream;
using std::cout;
using std::endl;
typedef void Sigfunc(int);
Sigfunc *signal(int, Sigfunc *);

time_t t;
char* dt = ctime(&t);
int countSIGINT = 0;
int port;
auto t_start = system_clock::now();
auto t_end = system_clock::now();
double diff = duration_cast<milliseconds>(t_end - t_start).count();
static volatile sig_atomic_t keep_running = 2;
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
    file_to_send* file_info;
    file_info = (file_to_send*) arg;
    cout<<"Sending: '"<< file_info->name<<"' to client: "<<file_info->clientId<<endl;
    ifstream file(file_info->name, ios::binary);
    while(!file.eof()){
        file.read(txtBuffer, 100);
        strcpy(message.mesg_text,txtBuffer);
        message.mesg_type = 1;
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
    msgid = msgget(key, 0666| IPC_CREAT);
    mesg_buffer msg;
    mesg_buffer toSend;
    if (msgrcv(msgid, &msg, sizeof(msg), 1, IPC_NOWAIT) != -1) {
        
        cout<<"Connection demand from : " << msg.clientId<<endl;
        
        toSend = mesg_buffer{msg.clientId, msg.clientId, "Connection accepted"};
        send_msg(toSend);
        cout<<"Response sent"<<endl;

        pthread_t thread;
        file_to_send info;
        strcat(directory,msg.mesg_text);
        strcpy(info.name,directory);
        info.clientId =  msg.clientId;
        pthread_create(&thread,NULL,send_file,(void*)&info);
		
        
    }
    // intéssant: https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-msgrcv-message-receive-operation
    // en bref on pourra faire en sorte que le serveur reçoit tout mais send spécifiquemment a certaine client
    // avec un client ID ? (voir le param msgtyp )
   //

    
}

static void end_queue(key_t key) {
    cout << "Deleting queue :" + key << std::endl;
    // Supprimer a la fin !
    int msgid = msgget(key, 0666 | IPC_CREAT);
    msgctl(msgid, IPC_RMID, NULL);
}

static void close_connection(key_t key) {
    cout << "Closing connection :" + key << std::endl;
    // Supprimer a la fin !
    int msgid = msgget(key, 0666 | IPC_CREAT);
    msgctl(msgid, IPC_RMID, NULL);
}

// Procédure qui gère le signal.
void handle_signint(int sigNumber) {
     
    switch(sigNumber) {
        //SIGABRT
        case(6):
            cout << "SIGABRT received" << std::endl;
        break;
        //SIGINT
        case(2):
            cout << "SIGINT received" << std::endl;
            countSIGINT++;
            if(countSIGINT == 2) {
                cout << "SIGINT received twice, exiting..." << std::endl;
                
                keep_running = 0;
                end_queue(port);
                exit(0);
            }

        break;
        //SIGTERM
        case(15):
            cout << "SIGTERM received" << std::endl;
        break;
        //SIGHUP
        case(1):
            cout << "SIGHUP received" << std::endl;
        break;
        //SIGQUIT
        case(3):
            cout << "SIGQUIT received, exiting..." << std::endl;
            // close_connection(port);
            end_queue(port);
            exit(0);
        break;
        //SIGTSTP
        case(18):
            cout << "SIGTSTP received" << std::endl;
        break;
        //SIGILL
        case(4):
            cout << "SIGILL received" << std::endl;
        break;
        //SIGBUS
        case(10):
            cout << "SIGBUS received" << std::endl;
        break;
        //SIGSEGV
        case(11):
            cout << "SIGSEGV received" << std::endl;
        break;
        //SIGFPE
        case(8):
            cout << "SIGFPE received" << std::endl;
        break;
    }
}

int main(int argc, char* argv[]) {
	cout << "Server started" << endl;
	// Association du signal avec la procédure de gestion (callback).
    //port = atoi(argv[1]);
    port = 1337;
    //char directory[100] = argv[2];
    strcpy(directory,  "/src/ift630/tp4/IFT630-TP4/transfer_folder/");

	signal(SIGABRT, handle_signint);
    signal(SIGINT, handle_signint);
    signal(SIGINT, handle_signint);
    signal(SIGTERM, handle_signint);
    signal(SIGHUP, handle_signint);
    signal(SIGQUIT, handle_signint);
    signal(SIGTSTP, handle_signint);
    signal(SIGILL, handle_signint);
    signal(SIGBUS, handle_signint);
    signal(SIGSEGV, handle_signint);
    signal(SIGFPE, handle_signint);
    mesg_buffer leMessage;

	while (true) {
        get_msg(port);
	}
    exit(0);
}
