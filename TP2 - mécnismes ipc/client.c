#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <assert.h>
#include "types.h"





int main(int argc, char *argv[])
{

	if(argc != 4) 
	{
		printf("usage : %s <operation> <operande> <operation>\n", argv[0]);
		return EXIT_FAILURE;
	}


    key_t cle;			/* cle de la file     */
    int file_mess;		/* ID de la file    */
    requete_t requete;
    reponse_t reponse;
	ssize_t res_rcv;
    int gauche, droite; 
    char op;



    pid_t pid = getpid(); 
	printf("CLIENT - PID client : %d\n",pid);


     /* Recuperation des arguments */ 
    gauche = (int)strtol(argv[1],NULL,0);
    droite = (int)strtol(argv[3],NULL,0);

 	op = argv[2][0];

	if 
	(
		(op != '+') 
		&& (op != '-') 
		&& (op != '*') 
		&& (op != '/') 
		&& (op != '%')
	)


	printf("bad operation");

	/* Calcul de la cle */
	cle = ftok(FICHIER_CLE, 'a');
	assert(cle != -1);
	printf("CLIENT - Clé de la file : %d\n", cle);



	/* Recuperation file de message : */

	file_mess = msgget(cle, IPC_CREAT);

	assert(file_mess != -1);


	printf("CLIENT - File message: %d\n", file_mess);


	fprintf(stdout, "CLIENT -\t\tLe client %d vas envoyer : %d %c %d\n", pid, gauche, op, droite);


	/* creation de la requete :          */
	requete.type  = 1;
	requete.op  = op;
	requete.expediteur  = pid;
	requete.g  = gauche;
	requete.d  = droite;

	printf("CLIENT - Création de la requête\n");

	/* envoi de la requete :             */
	res_rcv = msgsnd(file_mess, &requete, sizeof(requete)-sizeof(long int), 0);

	assert(res_rcv != -1); 



	printf("CLIENT - Envoie de la requête au serveur : %d\n", res_rcv);



	/* attente de la reponse :           */
		printf("CLIENT -Attend la réponse du serveur \n");

		res_rcv = msgrcv(file_mess, &reponse, sizeof(reponse)-sizeof(long int), 1, 0); // bloquant
	assert(res_rcv != -1); 



	/* affichage de la reponse           */
		printf("CLIENT - Reponse du serveur reçue : %d \n", reponse.resu);




	exit(0);

	return EXIT_SUCCESS;

}

