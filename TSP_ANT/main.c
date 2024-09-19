// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algoritmo_evolutivo_diferencial.h"

void tsp(int poblacion, int tamanio_instancia,char * instancia){
    a_v_d(poblacion,tamanio_instancia,instancia);  
}

int main (int argc, char * argv[]){
    int poblacion = atoi(argv[1]);
    int tamanio_instancia = atoi(argv[2]);
    char instancia[20];
    strcpy(instancia,argv[3]);
    tsp(poblacion,tamanio_instancia,instancia); 
    return 0;
}
