#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/aed.h"

int main(int argc, char *argv[])
{
    // Verificamos que los parámetros estén completos
    if (argc != 5)
    {
        fprintf(stderr, "Uso: <num_poblacion> <num_generaciones> <archivo_instancia> <tamanio_instancia>\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL)); // Inicializamos la semilla

    (void)argc;
    int num_poblacion = atoi(argv[1]);     // Tamaño de la población
    int num_generaciones = atoi(argv[2]);  // Número de generaciones
    int tamanio_instancia = atoi(argv[4]); // Tamaño de la instancia

    // Verificamos que el número de población no sea menor a 3
    while (num_poblacion < 3)
    {
        printf("\nLa población debe ser mayor a 2 !!!\nIngresa de nuevo el valor de la población: ");
        if (scanf("%d", &num_poblacion) != 1)
        {
            fprintf(stderr, "Error al leer el valor de la población.\n");
            exit(EXIT_FAILURE);
        }
    }

    // Verificamos que el número de generaciones no sea menor a 1
    while (num_generaciones < 1)
    {
        printf("\nLa generación debe ser mayor a 0 !!!\nIngresa de nuevo el valor de la generación: ");
        if (scanf("%d", &num_generaciones) != 1)
        {
            fprintf(stderr, "Error al leer el valor de las generaciones.\n");
            exit(EXIT_FAILURE);
        }
    }

    // Verificamos que el tamaño de la instancia sea 25, 50 o 100
    while (tamanio_instancia != 25 && tamanio_instancia != 50 && tamanio_instancia != 100)
    {
        printf("\nEl tamaño de la instancia debe ser 25, 50 o 100 !!!\nIngresa de nuevo el tamaño de la instancia: ");
        if (scanf("%d", &tamanio_instancia) != 1)
        {
            fprintf(stderr, "Error al leer el tamaño de la instancia.\n");
            exit(EXIT_FAILURE);
        }
    }

    // Construimos el nombre del archivo de instancia
    char archivo_instancia[100];
    snprintf(archivo_instancia, sizeof(archivo_instancia), "%s_(%s)", argv[3], argv[4]);

    // Llamamos al algoritmo principal
    aed_vrp_tw(num_poblacion, num_generaciones, tamanio_instancia, archivo_instancia);

    return 0;
}
