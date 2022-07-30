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
#include <filesystem>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::high_resolution_clock;
using std::ios;
using std::ifstream;
using std::cout;
using std::endl;

typedef void Sigfunc(int);
Sigfunc *signal(int, Sigfunc *);

int countSIGINT = 0;
int port;
auto t_start = high_resolution_clock::now();
auto t_end = high_resolution_clock::now();
double diff = duration_cast<milliseconds>(t_end - t_start).count();
char directory[100];

struct file_to_send {
    char name[100];
    int clientId;
};

struct mesg_buffer {
    long mesg_type;
    int clientId;
    char mesg_text[100]; 
    char signal_caught[25];
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
        cout << "Nb de data byte dans le fichier: " << info.st_size << endl; // # de data byte dans file
    
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
     
    switch(sigNumber) {
        //SIGABRT
        case(6):
            cout << "SIGABRT received" << std::endl;
            end_queue(port);
            exit(0);
        break;
        //SIGINT
        case(2):

            if(countSIGINT == 0) {
                t_start = high_resolution_clock::now();
            }

            countSIGINT++;
            if(countSIGINT == 2) {
                countSIGINT = 0;
                t_end = high_resolution_clock::now();
                diff = duration_cast<milliseconds>(t_end - t_start).count();
                cout << "\nRecu deux SIGINT consecutifs en: " << diff << " ms" << std::endl;
                if(diff < 2000) {
                    cout << "SIGINT recu deux fois en moins de 2s, on quitte..." << std::endl;
                
                    //Envoie du msg au client
                    mesg_buffer msg;
                    
                    
                    msg.mesg_type = msg.clientId;
                    msg.clientId = msg.clientId;
                    strcpy(msg.signal_caught,"SIGINT");
                    send_msg(msg);
                    
                    //On clean!
                    end_queue(1);
                    end_queue(port);
                    exit(0);
                    break;
                }
            }

            else {
                    cout << "\nSIGINT received" << std::endl;
                }
        break;
        //SIGTERM
        case(15):
            cout << "SIGTERM received" << std::endl;
            end_queue(port);
            exit(0);
        break;
        //SIGHUP
        case(1):
            cout << "SIGHUP received" << std::endl;
            end_queue(port);
            exit(0);
        break;
        //SIGQUIT
        case(3):
            cout << "SIGQUIT received, exiting..." << std::endl;
            end_queue(port);
            exit(0);
        break;
        //SIGTSTP
        case(18):
            cout << "SIGTSTP received" << std::endl;
            end_queue(port);
            exit(0);
        break;
        //SIGILL
        case(4):
            cout << "SIGILL received" << std::endl;
            end_queue(port);
            exit(0);
        break;
        //SIGBUS
        case(10):
            cout << "SIGBUS received" << std::endl;
            end_queue(port);
            exit(0);
        break;
        //SIGSEGV
        case(11):
            cout << "SIGSEGV received" << std::endl;
                end_queue(port);
                exit(0);
        break;
        //SIGFPE
        case(8):
            cout << "SIGFPE received" << std::endl;
            end_queue(port);
            exit(0);
        break;
    }
}

int main(int argc, char* argv[]) {
	cout << "Server started" << endl;
	// Association du signal avec la procédure de gestion (callback).
    port = atoi(argv[1]);
    //port = 1337;
    auto path = std::filesystem::current_path().parent_path().parent_path()  / "transfer_folder/";
    strcpy(directory,  path.c_str()); // '/tp4/IFT630-TP4/transfer_folder/file1.txt'

    mesg_buffer leMessage;
    msgget(port, 0666|IPC_CREAT);

     // Association du signal avec la procédure de gestion (callback).
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


	while (true) {
        get_msg(port);
	}
    exit(0);
}
