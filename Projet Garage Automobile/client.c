#include <stdio.h>
#include <stdlib.h>

#include "verif.h"
#include "types.h"



#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <assert.h>
#include <sys/sem.h>
#include <sys/shm.h>


int choose_chef_atelier(int nb_chefs_atelier)
{
	int i;
	int choosen_chef;
	int occupation_chefs[nb_chefs_atelier];
	key_t sem_key_chef = ftok(FICHIER_CLE_SEM_OCCUPATION, 1);
	int sem_id_chef = semget(sem_key_chef, 0, 066|IPC_EXCL); // récupération du sémaphore contenant l'occupation des chefs :

	for(i = 0; i< nb_chefs_atelier; i++)
	{
		occupation_chefs[i] = semctl(sem_id_chef, i, GETVAL);
	}

	int location = 0;
    int min = occupation_chefs[location];  


  for (i = 1; i < nb_chefs_atelier; i++) {
    if (occupation_chefs[i] < min) {
      location = i;
    }
	}



    struct sembuf p[] = {{0,-1,0}, {1,-1,0}};
    semop(sem_id_chef, p, location);

	return location;
}

void free_chef_atelier(int chef_atelier_numero, int nb_chefs_atelier)
{
		key_t sem_key_chef = ftok(FICHIER_CLE_SEM_OCCUPATION, 1);
		int sem_id_chef = semget(sem_key_chef, 0, 066|IPC_EXCL); // récupération du sémaphore contenant l'occupation des chefs :

	    struct sembuf v[] = {{0,+1,0}, {1,+1,0}};
    	semop(sem_id_chef, v, chef_atelier_numero);

}


int main(int argc, char *argv[])
{
	int i;
	ssize_t nb_lus;
	int choosen_chef_atelier_ordre;

	// VERIFICATION DES ARGUMENTS
	if(argc < 1) 
	{
		couleur(VERT);
		printf("Client incorrect!\n");

		return EXIT_FAILURE;
	}

	int nb_chef_atelier = (int)strtol(argv[1],NULL,0);

	// Le nombre d'argument dépend du nombre de chef d'atelier
	if(argc != nb_chef_atelier+2) 
	{
		couleur(ROUGE);
				printf("./%s %s %s %s \n", argv[0],argv[1],argv[2],argv[3]);

		printf("Vous devez avoir autant de clé que de chef d'atelier (%d)!\n", argc);
		return EXIT_FAILURE;
	}

	// Vérification de si les arguments sont bien des entiers
	for(i = 1; i < argc; i++) 
	{
		if(isNumber(argv[i]) == 0) 
		{
			couleur(ROUGE);
			printf("Les arguments du client doivent être des entiers ! (%s)\n", argv[i]);
			return EXIT_FAILURE;
		}
	}

	// Sémaphores pour savoir le taux d'occupation d'un chef d'atelier
	key_t sem_key_chef = ftok(FICHIER_CLE_SEM_OCCUPATION, 'a');
	int sem_id_chef = semget(sem_key_chef, 0, 066|IPC_EXCL);




	// FIN VERIFICATION DES ARGUMENTS

	// CONVERTION DES ARGUMENTS
	key_t cle_ipc_chef[nb_chef_atelier];
	// Enregistrement des clés des files IPC de chaque chefs d'atelier
	for(i = 0; i < nb_chef_atelier; i++) 
	{
		cle_ipc_chef[i] = (int)strtol(argv[i+2],NULL,0);
	}

	couleur(VERT);
	printf("CLIENT - Nouveau client!\n");


	notification notif; // Notification que le client reçoit quand le travail est terminé

	// Créer la requête
	requete_garage requete;
	requete.type = 1;

	// Séléction du chef le moins occupé :
	choosen_chef_atelier_ordre = choose_chef_atelier(nb_chef_atelier); // cherche le numéro d'ordre du chef d'ateleir le moins occupé
	key_t choosen_cle_ipc_chef = cle_ipc_chef[choosen_chef_atelier_ordre]; // On prend récupère sa clé
	int file_mess = msgget(choosen_cle_ipc_chef, 0);

	couleur(VERT);
	printf("CLIENT - Le client choisi le CHEF_%d avec la clé : %d & la file : %d\n", choosen_chef_atelier_ordre, choosen_cle_ipc_chef, file_mess);


	// Sembuf opérations pour libérer ou rendre le chef d'atelier occupé
	struct sembuf v[] = {{choosen_chef_atelier_ordre, +1, 0}};
	struct sembuf p[] = {{choosen_chef_atelier_ordre, -1, 0}};

	semop(sem_id_chef, v, 1); // augmenter l'occupation de l'outil

	



	// transmettre la requête au chef d'atelier le moins occupé
	assert(msgsnd(file_mess, &requete, sizeof(requete)-sizeof(long int), 0) != -1);
	couleur(VERT);
	printf("CLIENT - Envoi d'un signal au CHEF_%d\n", nb_lus, file_mess, choosen_chef_atelier_ordre);


	// récupérer la réponse du chef d'atelier
	printf("CLIENT - Attend un signal du CHEF_%d\n", choosen_chef_atelier_ordre);
	assert(msgrcv(file_mess, &notif, sizeof(notification)-sizeof(long int), 2, 0) != -1); // bloquant

	// afficher le résultat
	free_chef_atelier(choosen_chef_atelier_ordre, nb_chef_atelier);
	couleur(VERT);
	printf("CLIENT - Le client est satisfait après %d seconde(s) d'attente!\n", notif.temps_seconde);
	
	semop(sem_id_chef, p, 1); // libérer le chef d'atelier


	return EXIT_SUCCESS;



}
