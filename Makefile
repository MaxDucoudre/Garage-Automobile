#
# CONSTANTES
#
CC = gcc
EXECUTABLES = ${RUNEXECNAME} client mecanicien chef_atelier
OFILES = verif.o

RUNEXECNAME = garage

#
# REGLES
#
all :  garage client chef_atelier mecanicien


verif : verif.c verif.h
	$(cc) verif.c -o verif.o -c 

mecanicien : mecanicien.c types.h verif.o
	$(CC) mecanicien.c -o mecanicien verif.o

chef_atelier : chef_atelier.c types.h verif.o
	$(CC) chef_atelier.c -o chef_atelier verif.o

client : client.c types.h verif.o
	$(CC) client.c -o client verif.o

${RUNEXECNAME} : main.c types.h verif.o main.o
	$(CC) main.c -o ${RUNEXECNAME} verif.o

#
# CONSTANTES
#
run :
	./${RUNEXECNAME} 2 3 4 3 9 2

#
# CLEAN
#
clean :
	rm -f $(EXECUTABLES)
	rm -f ${OFILES}
	rm -f ./files_cle/cle.*
