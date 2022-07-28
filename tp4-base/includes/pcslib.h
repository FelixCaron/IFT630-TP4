/*******************************************************************/
/*******************************************************************/
/*                                                                 */
/*  Cette classe permet de creer des objets de type processus avec */
/*  des operations pour creer un processu, detruire un processus   */
/*  et attendre la fin d'execution d'un processus.                 */
/*                                                                 */
/*******************************************************************/
/*******************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>


class Pcs {
private:
	int pcsid;
	int status;
public:
	Pcs();
	Pcs(char *fichier);
	Pcs(char *fichier, char *const args[]);
	void Fork(char *fichier);
	int Join();
	void Detruit();
};

