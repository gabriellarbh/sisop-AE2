#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>


pid_t filho1, filho2;
int valorA, valorB;


int divide(int *src, int *arrA, int *arrB, int size) {
	int i, meio = size / 2;
	for(i = 0; i < size; i++){
		if(i < meio)
			*(arrA+i) = *(src+i);
		else
			*(arrB+i-meio) = *(src+i);
	}
	return 0;
}

int Maior(int *src, int size) {
	
	int i, maior = 0;
	for(i = 0; i < size; i++){
		if(*(src+i) > maior)
		  maior = *(src+i);
	}
	return maior;
}

int Menor(int *src, int size){
	int i, menor = 127;
	for(i = 0; i < size; i++) {
		if(*(src+i) < menor)
			menor = *(src+i);
	}
	return menor;
}


int main(int argc, char* argv[])
{

	int maxNivel, sizeOriginal = 0;
	char fileName[100];
	int bMaior;
	int i = 0;
   	int sizeA, sizeB;

	//Variáveis de listas
	int *listaOrig, *listaA, *listaB;
	//Ler os argumentos da linha de comando
	FILE *file;
	if (argc > 5){
		for(i = 1; i < argc; i++){
			if(strcmp(argv[i],"-l") == 0)
				maxNivel = atoi(argv[i+1]);	 
			else if (strcmp(argv[i],"-f") == 0)
				strcpy(fileName,argv[i+1]);
			else if (strcmp(argv[i],"-M") == 0)
				bMaior = 1;
			else if (strcmp(argv[i],"-m") == 0)
				bMaior = 0;
		}
	}
    else{
        printf("Not enough parameters\n");
	    exit(-1);
    }

	//Agora com todas as entradas resolvidas, Lê o arquivo e seta o numLista;
	file = fopen(fileName, "r");
    while (!feof (file)) {  
      fscanf (file, "%d", &i);
      sizeOriginal++;
    }
    //Alocando espaço para os inteiros do arquivo
    listaOrig = (int*)malloc(sizeOriginal*sizeof(int));
    sizeOriginal = 0;
    rewind(file);
    while (!feof (file)) {  
      fscanf (file, "%d", listaOrig+sizeOriginal);
      sizeOriginal++;
    }
    sizeOriginal--;
    fclose (file);
    if(sizeOriginal % 2 == 0){
        sizeA = sizeOriginal/2;
        sizeB = sizeOriginal/2;
    }
    else{
        sizeA = sizeOriginal/2;
        sizeB = sizeOriginal/2 + 1;
    }

    listaA = (int*)malloc(sizeA*sizeof(int));
    listaB = (int*)malloc(sizeB*sizeof(int));
    divide(listaOrig, listaA, listaB, sizeOriginal);
    //Com as listas divididas em A e B
    int pai = getpid();
    int n = 1;
    printf("PROC %d LEVEL %d\n", pai, n);
    while(n < maxNivel){
        filho1 = fork();
        if(filho1 == 0){
            printf("PROC %d LEVEL %d\n", getpid(), n+1);
            free(listaOrig);
            listaOrig = (int*)malloc(sizeA*(sizeof(int)));
            for(i=0; i < sizeA; i++)
                listaOrig[i] = listaA[i];
            sizeOriginal = sizeA;
            if(sizeOriginal % 2 == 0){
                sizeA = sizeOriginal/2;
                sizeB = sizeOriginal/2;
            }
            else{
                sizeA = sizeOriginal/2;
                sizeB = sizeOriginal/2 + 1;
            }
            divide(listaOrig, listaA, listaB, sizeOriginal);
        }
        else if (filho1 > 0) {
            filho2 = fork();
            if (filho2 == 0) {
                printf("PROC %d LEVEL %d\n", getpid(), n+1);
                
                free(listaOrig);
                listaOrig = (int*)malloc(sizeB*(sizeof(int)));
                for(i=0; i < sizeB; i++)
                    listaOrig[i] = listaB[i];
                sizeOriginal = sizeB;

                if(sizeOriginal % 2 == 0){
                    sizeA = sizeOriginal/2;
                    sizeB = sizeOriginal/2;
                }
                else{
                    sizeA = sizeOriginal/2;
                    sizeB = sizeOriginal/2 + 1;
                }
                divide(listaOrig, listaA, listaB, sizeOriginal);
            }
            else if (filho2 > 0){
                break;
            }
            else{
                printf("Error when creating new process");
                exit(-1);
            }
        }
        else {
            printf("Error when creating new process");
            exit(-1);
        }
        n++;
    }

    
    int status1 = -1, status2 = -1, saida = -1;
    waitpid(filho1, &status1, 0);
    waitpid(filho2, &status2, 0);
    if(status1 == -1 && status2 == -1) {
        if(bMaior)
        {
            saida = Maior(listaOrig, sizeOriginal);
        }
        else
            saida = Menor(listaOrig, sizeOriginal);
    }
    else {
		if(bMaior){
		    if(WEXITSTATUS(status1) > WEXITSTATUS(status2))
		        saida = WEXITSTATUS(status1);
		    else
		        saida = WEXITSTATUS(status2);
		}
		else {
		    if(WEXITSTATUS(status1) < WEXITSTATUS(status2))
		        saida = WEXITSTATUS(status1);
		    else
		        saida = WEXITSTATUS(status2);
		}
   	 }
    if(getpid() == pai){
        if(bMaior)
            printf("MAIOR %d\n", saida);
        else
            printf("MENOR %d\n", saida);
    	free(listaA);
    	free(listaB);
    	free(listaOrig);
        return 0;
    }
    exit(saida);

}

