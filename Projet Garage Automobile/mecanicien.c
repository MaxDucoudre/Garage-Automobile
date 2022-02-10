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
int isWorking;


struct sembuf* getVsembuf(requete_chef_vers_mecanicien requete_meca)
{
		struct sembuf *v = malloc(sizeof(struct sembuf)*4);
		int i;
		for(i = 0; i < 4; i++)
		{
			v[i].sem_num = i;
			v[i].sem_op = -requete_meca.nb_outil[i];
			v[i].sem_flg = 0;
		}	


		return v;
}

struct sembuf* getPsembuf(requete_chef_vers_mecanicien requete_meca)
{
		struct sembuf *p = malloc(sizeof(struct sembuf)*4);
		int i;
		for(i = 0; i < 4; i++)
		{
			p[i].sem_num = i;
			p[i].sem_op = +requete_meca.nb_outil[i];
			p[i].sem_flg = 0;
		}	


		return p;

}


// Fonction mettant fin au processus mécanicien proprement (a condition qu'il ait terminé )
void endMecanicien() {

	couleur(CYAN);
	printf("Le mécanicien %d finit son travail avant de se terminer... \n", getppid());

	
	// Destruction de la file IPC (mécanicien-chef)
	msgctl(file_mess, IPC_RMID, 0);


	// On attend que le mécanicien finit son travail
	couleur(CYAN);
	printf("Fin du mécanicien! %d\n", getppid());
	exit(EXIT_SUCCESS);
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

	couleur(CYAN);
	printf("MECANICIEN_%d - Nouvelle file IPC %d \n", numero_ordre, file_mess);

	return file_mess;
}


int main(int argc, char *argv[])
{
	int numero_ordre;
	int i, j;
	int value;
	isWorking = 0;
	if(argc != 2) 
	{

		couleur(CYAN);
		printf("Mécanicien incorrect!\n");

		return EXIT_FAILURE;
	}

	// Convertion de l'argument en entier
	numero_ordre = strtol(argv[1], NULL, 0);

	couleur(CYAN);
	printf("MECANICIEN_%d - Nouveau mécanicien ! \n", numero_ordre);


	// Fin du mécanicien à la récéption du signal SIGINT 
	signal(SIGINT, endMecanicien); 

	key_t cle_chef_meca;
   	int nb_lus;

   	key_t sem_key = ftok(FICHIER_CLE_SEM, 1);
   	sem_id = semget(sem_key, 0, 0600|IPC_EXCL);;


   	notification notif;
   	notif.type = 2;


	file_mess = createMecanicienFile(numero_ordre);

	// Struct de la requete du chef d'atelier
	requete_chef_vers_mecanicien requete_meca;

	struct sembuf *p;
	struct sembuf *v;


	while(1) 
	{
		
		// Reçoit requête du chef d'atelier (travail a faire) sur sa file
		// requête ayant le nb d'outils par catégorie
		couleur(CYAN);
		printf("MECANICIEN_%d - Attend une requête\n", numero_ordre, requete_meca.nb_outil[0],requete_meca.nb_outil[1],requete_meca.nb_outil[2],requete_meca.nb_outil[3]);
		nb_lus = msgrcv(file_mess, &requete_meca, sizeof(requete_meca)-sizeof(long int), 1, 0); 
		couleur(CYAN);
		printf("MECANICIEN_%d - Reçoit une requête et les outils %d,%d,%d & %d\n", numero_ordre, requete_meca.nb_outil[0],requete_meca.nb_outil[1],requete_meca.nb_outil[2],requete_meca.nb_outil[3]);

		isWorking = 1;

		// Réservation des outils (sémaphores)
		couleur(CYAN);
		printf("MECANICIEN_%d - Réservation des outils en cours...\n", numero_ordre);

		// prépare les opérations nécéssaire sur les sémaphores pour réserver les outils
		p = getVsembuf(requete_meca);
		v = getPsembuf(requete_meca);

		semop(sem_id, p, 1); // opération de retrait des outils



		// Réalise le travail demandé (via sleep)
		couleur(CYAN);
		printf("MECANICIEN_%d - Travail en cours...\n", numero_ordre);

		time_t msec_avant = time(NULL);

		for(i = 0; i < 4; i++)
		{

			// Pour chaque outils alloué, le mécanicien vas travailler 1 seconde
			sleep(1* requete_meca.nb_outil[i]);
			
		}

		time_t msec_apres = time(NULL);
		time_t chrono = msec_apres-msec_avant;
		notif.temps_seconde = chrono; // Récupération du chronomètre et du temps de travail du mécanicien



		semop(sem_id,v,0); // opération pour libérer les outils

		// Envoie sa réponse au chef d'atelier (notification)

		nb_lus = msgsnd(file_mess, &notif, sizeof(notification)-sizeof(long int), 0);
		couleur(CYAN);
		printf("MECANICIEN_%d - Travail terminé, notification envoyé au chef d'atelier.\n", numero_ordre);
		isWorking = 0;


	}

	endMecanicien();
	
}
