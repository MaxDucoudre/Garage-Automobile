#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>


void* somme(void * args){

	int * arg =(int*)args;

	for(int i = 0; i < arg[1]; i++) {
		arg[3] += arg[0]+(arg[2]*i);
	}
}


int main(int argc, char *argv[]) {


	int nbThread = atoi(argv[1]);
	int nbEntier = atoi(argv[2]);
    pthread_t threads[nbThread];


    int args[4];
    args[1] = nbEntier/nbThread;
    args[2] = nbThread;
    args[3] = 0;

	for(int i = 0; i < nbThread; i++) {
		args[0] = i+1;
		pthread_create(&threads[i], NULL, somme, &args);
	}

	printf("%d threads ont tournés pour calculer la somme des %d premiers entiers\n Résultat : %d\n", nbThread, nbEntier, args[3]);
}