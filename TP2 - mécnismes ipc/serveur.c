
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <assert.h>
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int file_mess;


int operation(char op, int g, int d) {

	switch (op){
		case '+':
			return g+d;

		case '-':
			return g-d;
		case '*':
			return g*d;

		case '/':
			return g/d;

		default :
			return g%d;
	}

	return 1;

}

void arret(int s){
	/* Arret du serveur
	 * en detruisant la file 
	 * de message */	



	// SUPPRIMER LA FILE DE MESSAGE
	// Convertion du msqid en string
	char buffer[20];
	sprintf(buffer, "%d", file_mess);

	// concaténation de la commande avec le msqid
	char *args[]={"ipcrm -q", buffer};
	char buffer2[30] ;
	strcat(strcpy(buffer2, args[0]), args[1]);

	// exécution de la commande
	system(buffer2);

	exit(0);

}

int set_signal_handler(int signo, void (*handler)(int)) {
	struct sigaction sa;
	sa.sa_handler = handler;    // call `handler` on signal
	sigemptyset(&sa.sa_mask);   // don't block other signals in handler
	sa.sa_flags = 0 ;            //  don't restart system calls
	return sigaction(signo, &sa, NULL);
}

int main(int argc, char *argv[])
{

	key_t cle; /* cle de la file     */
	requete_t requete;
	reponse_t reponse;
	ssize_t nb_lus;
	ssize_t nb_snd;

	int reponse_calcul;

	/* cacul de la cle de la file    */
	cle = ftok(FICHIER_CLE,'a');
	assert(cle != -1);

	printf("SERVEUR - Clé de la file : %d\n", cle);



	/* Creation file de message :    */

	file_mess = msgget(cle, IPC_CREAT|0600);
	assert(file_mess != -1);


	srand(getpid());

	printf("SERVEUR - File message: %d\n", file_mess);


	assert(set_signal_handler(SIGINT, arret) == 0);


	while(1){ 

		/* serveur attend des requetes, de type 1 :        */
		printf("SERVEUR - Attend requête\n");
		nb_lus = msgrcv(file_mess, (void*) &requete, sizeof(requete)-sizeof(long int), 1, 0); // bloquant

		printf("SERVEUR - Requête reçue  : %ld\n", nb_lus);


		assert( nb_lus != -1);

		/* traitement de la requete :                      */

		reponse_calcul = operation(requete.op, requete.g, requete.d); 
		printf("SERVEUR - Réponse calcul %d\n", reponse_calcul);



		/* Attente aleatoire */

		sleep(rand()%3);


		printf("SERVEUR - Ecriture réponse\n");
			reponse.type  = 1;
			reponse.resu = reponse_calcul;



		/* envoi de la reponse :                           */
			printf("SERVEUR - Envoie réponse au client\n");
			nb_snd = msgsnd(file_mess, &reponse, sizeof(reponse)-sizeof(long int), 0);
			assert( nb_snd != -1);


	}
	exit(0);

}

