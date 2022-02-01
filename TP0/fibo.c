#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#define R 0 
#define W 1

long int fibo (long int n)
{
	return (n<=1)?n:fibo(n-1)+fibo(n-2);
}



int main(int argc, char *argv[])
{
	long int n;
	assert(argc > 1);
	n=strtol(argv[1],NULL,0);
	//printf("fibo(%ld) = %ld\n",n,fibo(n));


	int f1_to_p[2];
	int f2_to_p[2];
	
	pipe(f1_to_p);
	pipe(f2_to_p);

    pid_t pidf1;

    pid_t pid = fork();

    int n1;
    int n2;

    // PERE 1
    if(pid > 0) {

		close(f1_to_p[W]);
		close(f2_to_p[W]);

        wait(NULL); 
		wait(NULL);

        read(f1_to_p[R], &n1, sizeof(int));
        read(f2_to_p[R], &n2, sizeof(int));


		printf("fibo : %ld = %ld\n",n,n1+n2);


		close(f1_to_p[R]);
		close(f2_to_p[R]);

    }

    // FILS 1
    if(pid == 0) {

		close(f2_to_p[R]);
		close(f2_to_p[W]);

		close(f1_to_p[R]);


		n1 = fibo(n);

        write(f1_to_p[W], &n1, sizeof(int));


        pidf1 = fork();
		close(f1_to_p[W]);


	    // FILS 2
	    if(pidf1 == 0) {

			close(f1_to_p[R]);
			close(f1_to_p[W]);

			close(f1_to_p[R]);

			n2 = fibo(n-1);
	        write(f2_to_p[W], &n2, sizeof(int));

			close(f2_to_p[W]);

	    } 

    } 





	return 0;
}
