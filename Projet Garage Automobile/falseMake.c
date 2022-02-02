#include <stdlib.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
	
	system("gcc verif.c -o verif.o -c");

	system("gcc main.c -o main.o -c");
	system("gcc -o garage main.o verif.o -pthread");

	system("gcc client.c -o client.o -c");
	system("gcc -o client client.o verif.o");


		system("gcc chef_atelier.c -o chef_atelier.o -c");
	system("gcc -o chef_atelier chef_atelier.o verif.o");


		system("gcc mecanicien.c -o mecanicien.o -c");
	system("gcc -o mecanicien mecanicien.o verif.o");

	return 0;
}