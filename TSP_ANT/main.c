// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algoritmo_evolutivo_diferencial.h"

int main(int argc, char *argv[])
{
    srand(time(NULL));
    int poblacion=atoi(argv[1]);
    int generaciones = atoi(argv[2]);
    int tamanio_instancia = atoi(argv[3]);
    char archivo_instancia[20];
    strcpy(archivo_instancia, argv[4]);
    algoritmo_evolutivo_diferencial(poblacion,generaciones,tamanio_instancia, archivo_instancia);
    return 0;
}
