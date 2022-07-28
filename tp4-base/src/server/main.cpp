/***************************************************************************************************


***************************************************************************************************/

#include "../../includes/pcslib.h"
#include "../../includes/signal.h"

typedef void Sigfunc(int);
Sigfunc *signal(int, Sigfunc *);

// Procédure qui gère le signal.
void handle_signint(int sigNumber) {
	
	// ...
}

int main(int argc, char* argv[]) {

	// Association du signal avec la procédure de gestion (callback).
	signal(SIGINT, handle_signint);

	// ...
	
	exit(0);
}
