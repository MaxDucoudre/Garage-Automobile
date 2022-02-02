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



int main(int argc, char *argv[])
{
	int i;
	ssize_t nb_lus;


	// VERIFICATION DES ARGUMENTS
	if(argc < 1) 
	{
		printf("Client incorrect!\n");

		return EXIT_FAILURE;
	}

	int nb_chef_atelier = (int)strtol(argv[1],NULL,0);

	// Le nombre d'argument dépend du nombre de chef d'atelier
	if(argc != nb_chef_atelier+2) 
	{
		printf("Vous devez avoir autant de clé que de chef d'atelier!\n");
		return EXIT_FAILURE;
	}

	// Vérification de si les arguments sont bien des entiers
	for(i = 1; i < argc; i++) 
	{
		if(isNumber(argv[i]) == 0) 
		{

			printf("Les arguments du client doivent être des entiers !\n");
			return EXIT_FAILURE;
		}
	}
	// FIN VERIFICATION DES ARGUMENTS

	// CONVERTION DES ARGUMENTS
	key_t cle_ipc_chef[nb_chef_atelier];
	// Enregistrement des clés des files IPC de chaque chefs d'atelier
	for(i = 0; i < nb_chef_atelier; i++) 
	{
		cle_ipc_chef[i] = (int)strtol(argv[i+2],NULL,0);
	}


	printf("CLIENT - Nouveau client!\n");



	// Créer la requête
	requete_garage requete;

	// Séléction du chef le moins occupé :
	key_t choosen_cle_ipc_chef = cle_ipc_chef[0];

	// récupération de la file
	int file_mess = msgget(choosen_cle_ipc_chef, IPC_CREAT);

	printf("CLIENT - Récéption de la file : %d\n", file_mess);


	// transmettre la requête au chef d'atelier le moins occupé et attendre qu'il soit disponnible
	nb_lus = msgsnd(file_mess, &requete, sizeof(requete)-sizeof(long int), 0);
	
	printf("CLIENT - Envoi du message %d dans la file\n", nb_lus);


	// récupérer la réponse du chef d'atelier


	// afficher le résultat

	return EXIT_SUCCESS;



}
