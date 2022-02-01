#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void print_int(void* arg) {
	int entier = (*(int*)arg);
	printf("%d\n", entier);

}
void print_string(void* arg) {
	char* carac = (*(char**)arg);
	printf("%s\n", carac);
}

void map(void *base, size_t n, size_t size_elem, void(*f)(void *)){
	int i;
	char *ptr = (char*)base; 
	for(i=0;i<n;i++) 
		f((void*)(ptr+i*size_elem)); 
}


int cmp_int(const void * arg1, const void * arg2) {
	return 	*((int*)arg1) - *((int*)arg2);
}

int cmp_string(const void * arg1, const void * arg2) {

	return strcmp(*(char**)arg1, *(char**)arg2 );

}




int main(){
	int t1[10]={12,-7,1,-16,3,19,7,1,5,0};
	char * t2[]={"chou","joujou","bijou","genou",
	"caillou","hibou","pou"};

	map(t1,10,sizeof(int),print_int);
	printf("\n");
	map(t2,7,sizeof(char*),print_string);

	qsort(t1,10,sizeof(int), cmp_int);
	qsort(t2,7,sizeof(char * ), cmp_string);
	map(t1,10,sizeof(int),print_int);
	map(t2,7,sizeof(char*),print_string);

}
