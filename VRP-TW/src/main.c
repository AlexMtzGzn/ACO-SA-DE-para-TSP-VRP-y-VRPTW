#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aed.h"

int main(int argc, char *argv[])
{

    int num_poblacion = atoi(argv[1]);    // Ingresa el tamaño de la poblacion
    int num_generaciones = atoi(argv[2]); // Ingresa el numero de generacones
    if (num_poblacion < 3)
    {
        do
        {

            printf("\n La poblacion debe ser mayor a 2 !!!\nIngresa de nuevo el valor de la poblacion: ");
            scanf("%i", &num_poblacion);

        } while (num_poblacion < 3);
    }
    char archivo_instancia[100];                                           // Declaramos nombre de la instacia a leer
    snprintf(archivo_instancia, sizeof(archivo_instancia), "%s", argv[3]); // Copiamos el nombre de la instacia a leer a del parametro argv[3];
    aed_vrp_tw(num_poblacion, num_generaciones, archivo_instancia);        // Enviamos al como parametro el numero de la poblacion , numero de generaciones , y el archivo de la intsncia

    return 0;
}