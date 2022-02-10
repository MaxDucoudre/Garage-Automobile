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


#include <time.h>



key_t cle_chef_meca;
int isWorking;
// Fonction mettant fin au processus ched d'atelier proprement (a condition qu'il ait terminé )
void endChefAtelier() {
	// enlever processus & IPC
	couleur(JAUNE);
	printf("Le chef d'atelier finit sa tache avant de se terminer...\n");


	//while(isWorking == 1){}
	couleur(JAUNE);
	printf("Fin du chef d'atelier !\n");

	exit(EXIT_SUCCESS);
}


int getMecaFile(int numero_orde_mecanicien) 
{
	int file_mess;
	char fichier_cle_meca[50];
	sprintf(fichier_cle_meca, "%s%d", FICHIER_CLE_MECA, numero_orde_mecanicien);

	cle_chef_meca = ftok(fichier_cle_meca, 'a');
	file_mess = msgget(cle_chef_meca, IPC_CREAT|0600); // nouvelle file pour communiquer avec le mécanicien

	return file_mess;
}


int main(int argc, char *argv[])
{

	// VERIFICATION DES ARGUMENTS
	if(argc != 7) 
	{	
		couleur(JAUNE);
		printf("Chef d'atelier incorrect (%d)!\n", argc);

		return EXIT_FAILURE;
	}

	int i;

	for(i = 1; i < argc; i++)
	{
		if(isNumber(argv[i]) == 0) 
		{
			couleur(JAUNE);
			printf("Il faut mettre des entiers en arguments du chef d'atelier!\n");
			return EXIT_FAILURE;
		}
	}

	// FIN VERIFICATION DES ARGUMENTS
	isWorking = 0;
	// Convertion des arguments en entiersé	
	int numero_ordre = strtol(argv[1], NULL, 0);
	int nb_outil_1_max = strtol(argv[2], NULL, 0); 
	int nb_outil_2_max = strtol(argv[3], NULL, 0);
	int nb_outil_3_max = strtol(argv[4], NULL, 0);
	int nb_outil_4_max = strtol(argv[5], NULL, 0);
	int nb_mecanicien = strtol(argv[6], NULL, 0);

	couleur(JAUNE);
	printf("CHEF_%d - Nouveau chef d'atelier !\n", numero_ordre);

	signal(SIGINT, endChefAtelier); 

	// Permet de récupérer la clé de la file relié au chef d'atelier
	key_t cle;
	char fichier_cle[50];

	sprintf(fichier_cle, "%s%d", FICHIER_CLE, numero_ordre);


	cle = ftok(fichier_cle, 'a');
	int file_mess = msgget(cle, IPC_CREAT|0600);


	int file_mess_meca;
	int choosen_mecanicien;

	couleur(JAUNE);
	printf("CHEF_%d - Récupération de la clé %d et de la file %d\n",numero_ordre, cle, file_mess);

	// variables
	requete_garage requete_client;

	requete_chef_vers_mecanicien requete_meca;
	requete_meca.type = 1;

	notification notif;


   	srand(time( 0 )); //srant init


   	key_t cle_chef_meca;
   	ssize_t nb_lus;


	while(1) 
	{
		// Attend/reçoit requête d'un client
		couleur(JAUNE);
		printf("CHEF_%d - En attente de requête client...\n", numero_ordre);
		nb_lus = msgrcv(file_mess, &requete_client, sizeof(requete_client)-sizeof(long int), 1, 0); // bloquant
		isWorking = 1;

		couleur(JAUNE);
		printf("CHEF_%d - Récupération d'une requête à la file %d\n", numero_ordre, file_mess);


		// FAUT METTRE TOUT CA DANS UN THREAD (Chef - Mecanicien)
		// Envoi requête au mécanicien (durée & outils nécéssaires dans chaque catégories)
		requete_meca.nb_outil[0] = rand() % (nb_outil_1_max+1);
		requete_meca.nb_outil[1] = rand() % (nb_outil_2_max+1);
		requete_meca.nb_outil[2] = rand() % (nb_outil_3_max+1);
		requete_meca.nb_outil[3] = rand() % (nb_outil_4_max+1);
		
		couleur(JAUNE);
		printf("CHEF_%d - Outils nécéssaire pour la tache demandée par le client : %d, %d, %d & %d\n", numero_ordre, requete_meca.nb_outil[0], requete_meca.nb_outil[1], requete_meca.nb_outil[2], requete_meca.nb_outil[3]);


		// séléction du mécanicien puis envois de la requête 
		choosen_mecanicien = rand() % nb_mecanicien;
		file_mess_meca = getMecaFile(choosen_mecanicien);
		nb_lus = msgsnd(file_mess_meca, &requete_meca, sizeof(requete_meca)-sizeof(long int), 0);
		couleur(JAUNE);
		printf("CHEF_%d - Envoi une requête (%d) au MECANICIEN_%d sur la file %d!\n", numero_ordre, nb_lus, choosen_mecanicien, file_mess_meca);


		// attente de la fin du travail du mécanicien
		printf("CHEF_%d - Attend qu'un mécanicien donne une réponse!\n", numero_ordre, nb_lus, choosen_mecanicien, notif.temps_seconde);
		nb_lus = msgrcv(file_mess_meca, &notif, sizeof(notification)-sizeof(long int), 2, 0); // bloquant
		couleur(JAUNE);
		printf("CHEF_%d - Reçoit la réponse (%d) du MECANICIEN_%d qui a mit %d seconde(s)!\n", numero_ordre, nb_lus, choosen_mecanicien, notif.temps_seconde);


		// notification du résultat de la requête au client
		nb_lus = msgsnd(file_mess, &notif, sizeof(notification)-sizeof(long int), 0);
		couleur(JAUNE);
		printf("CHEF_%d - Envoi de la notification (%d) au client!\n", numero_ordre, nb_lus);
		isWorking = 0;
	}

	endChefAtelier();

}
