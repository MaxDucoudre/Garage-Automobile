OBJS	= chef_atelier.o client.o main.o mecanicien.o
SRC	= %(willcard*.c)
OUT	= garage
CC	 = gcc	
FLAGS	 = -g -c 


all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) 

chef_atelier.o: chef_atelier.c
	$(CC) $(FLAGS) chef_atelier.c 

client.o: client.c
	$(CC) $(FLAGS) client.c 

mecanicien.o: mecanicien.c
	$(CC) $(FLAGS) mecanicien.c 

main.o: main.c
	$(CC) $(FLAGS) main.c 


clean:
	rm -f $(OBJS) $(OUT) 