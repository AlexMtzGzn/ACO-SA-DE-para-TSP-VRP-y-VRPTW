#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AED.h"

int main (int argc, char *argv[]){

    int num_poblacion = atoi(argv[1]); //Ingresa el tamaño de la poblacion
    int num_generaciones = atoi(argv[2]); //Ingresa el numero de generacones
    char archivo_instancia[100];
    snprintf(archivo_instancia, sizeof(archivo_instancia), "../VRP_Soolomon/%s", argv[3]);
    aed_vrp_tw(num_poblacion,num_generaciones,archivo_instancia);
    
    return 0;
} 