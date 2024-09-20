// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algoritmo_evolutivo_diferencial.h"

void tsp(int tamanio_instancia,char * instancia){
    a_v_d(tamanio_instancia,instancia);  
}

int main (int argc, char * argv[]){
    int tamanio_instancia = atoi(argv[1]);
    char instancia[20];
    strcpy(instancia,argv[2]);
    tsp(tamanio_instancia,instancia); 
    return 0;
}
