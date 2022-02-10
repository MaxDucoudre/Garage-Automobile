#include<stdlib.h>
#include<stdio.h>
#include <ctype.h> 

// Fonction pour vérifier si une chaine de caractère contient bien un nombre
// return 1 si oui, 0 si non
int isNumber(char string[])
{
	int i;
    for (i = 0; string[i]!= '\0'; i++)
    {
        if (isdigit(string[i]) == 0)
        {
           return 0;
        }
    }
    return 1;
}

