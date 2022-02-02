#include <stdio.h>
#include <stdlib.h>

#include "verif.h"


int main(int argc, char *argv[])
{
	int i;


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


	// transmettre la requête au chef d'atelier le moins occupé et attendre qu'il soit disponnible



	// récupérer la réponse du chef d'atelier


	// afficher le résultat

	return EXIT_SUCCESS;



}
