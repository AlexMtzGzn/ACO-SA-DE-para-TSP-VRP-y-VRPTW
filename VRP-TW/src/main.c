#include <stdio.h>

int main (int argc, char *argv[]){

    int poblacion = atoi(argv[1]); //Ingresa el tamaño de la poblacion
    int generaciones = atoi(argv[2]); //Ingresa el numero de generacones
    char archivo_instancia[200];
    snprintf(archivo_instancia, sizeof(archivo_instancia), "../Instancias/%s", argv[3]);
    
    return 0;
} 