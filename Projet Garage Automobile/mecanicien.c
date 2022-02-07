#include <stdio.h>
#include <stdlib.h>

#include "verif.h"
#include "types.h"


#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <assert.h>
#include <string.h>

#include <sys/sem.h>
#include <sys/shm.h>

#include <time.h>

// Variables globale de mécanicien
int file_mess;
int sem_id;


// Fonction mettant fin au processus mécanicien proprement (a condition qu'il ait terminé )
void endMecanicien() {

	// Destruction de la file IPC

	printf("\nFin du mécanicien\n");

	exit(EXIT_SUCCESS);
}


int checkToolIsAvailable(int tool_num, int needed_tool, int actual_sem_value) 
{

 	if(actual_sem_value >= needed_tool) 
 	{


 		return 1;
 	}
	return 0;

}

int createMecanicienFile(int numero_ordre) 
{

	key_t cle_chef_meca;
	int file_mess;
	char buffer[256];
	char fichier_cle[20];

	// Création d'un fichier tmp
	sprintf(fichier_cle, "%s%d", FICHIER_CLE_MECA, numero_ordre);
	sprintf(buffer, "touch %s", fichier_cle);
	system(buffer); 

	// Création d'une nouvelle file de message (1 par mécanicien)
	cle_chef_meca = ftok(fichier_cle, 'a');
	file_mess = msgget(cle_chef_meca, IPC_CREAT|0600); // nouvelle file pour communiquer avec le mécanicien

	printf("MECANICIEN_%d - Nouvelle file IPC %d \n", numero_ordre, file_mess);

	return file_mess;
}


int main(int argc, char *argv[])
{
	int numero_ordre;
	int i, j;

	if(argc != 2) 
	{
		printf("Mécanicien incorrect!\n");

		return EXIT_FAILURE;
	}

	// Convertion de l'argument en entier
	numero_ordre = strtol(argv[1], NULL, 0);

	printf("MECANICIEN_%d - Nouveau mécanicien ! \n", numero_ordre);


	// Fin du mécanicien à la récéption du signal SIGINT 
	signal(SIGINT, endMecanicien); 

	key_t cle_chef_meca;
   	int nb_lus;

   	key_t sem_key = ftok(FICHIER_CLE_SEM, 1);
   	sem_id = semget(sem_key, 1, 0600|IPC_EXCL);;




   	notification notif;
   	notif.type = 1;

   	sleep(1);

	file_mess = createMecanicienFile(numero_ordre);

	// Struct de la requete du chef d'atelier
	requete_chef_vers_mecanicien requete_meca;

	struct sembuf p[4];
	struct sembuf v[4];

	struct sembuf sembufP;
	struct sembuf sembufV;

	while(1) 
	{
		
		// Reçoit requête du chef d'atelier (travail a faire) sur sa file
			// requête ayant le nb d'outils par catégorie

		nb_lus = msgrcv(file_mess, &requete_meca, sizeof(requete_meca)-sizeof(long int), 1, 0); 
		printf("MECANICIEN_%d - Reçoit une requête et les outils %d,%d,%d & %d\n", numero_ordre, requete_meca.nb_outil[0],requete_meca.nb_outil[1],requete_meca.nb_outil[2],requete_meca.nb_outil[3]);

		// Réservation des outils (sémaphores)
		printf("MECANICIEN_%d - Réservation des outils en cours...\n", numero_ordre);



		for(i = 0; i<4; i++) 
		{
			sembufP.sem_num = i;
			sembufP.sem_op = -requete_meca.nb_outil[i];
			sembufP.sem_flg = 0;

			p[i] = sembufP;
				
			sembufV.sem_num = i;
			sembufV.sem_op = +requete_meca.nb_outil[i];
			sembufV.sem_flg = 0;

			v[i] = sembufP;
			
		}
		semop(sem_id,p,0); // opération



/*
		for(i = 0; i < 4; i++) // pour les 4 sémaphores
		{



			int value = semctl(sem_id, i, GETVAL);

			//while(checkToolIsAvailable(i, requete_meca.nb_outil[i], value) == 0) {} // Attend que les outils i se libèrent
			// une fois libéré, on fait une opération sur le sémaphore.
			
			p->sem_num = i; // sémaphore numéro i
			p->sem_op = -requete_meca.nb_outil[i]; // décrémentation en fonction de ce qu'a envoyé le chef d'atelier
			p->sem_flg = 0;
			
			semop(sem_id,p,i); // opération
		}
*/


		printf("MECANICIEN_%d - Travail en cours...\n", numero_ordre);
		// Réalise le travail demandé (via usleep)

		time_t msec_avant = time(NULL);

		for(i = 0; i < 4; i++)
		{

			// Pour chaque outils alloué, le mécanicien vas travailler 1 seconde
			sleep(1);
			
		}

		time_t msec_apres = time(NULL);

		time_t chrono = msec_apres-msec_avant;
		notif.temps_seconde = chrono;


		semop(sem_id,v,0); // opération pour libérer les outils


		// Envoie sa réponse au chef d'atelier (notification)

		nb_lus = msgsnd(file_mess, &notif, sizeof(notification)-sizeof(long int), 0);
		printf("MECANICIEN_%d - Travail terminé, notification envoyé au chef d'atelier.\n", numero_ordre);



	}

	endMecanicien();
	
}
