#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/aed.h"

int main(int argc, char *argv[])
{
    // Verificación de parámetros
    if (argc != 5)
    {
        fprintf(stderr, "Uso: <num_poblacion> <num_generaciones> <archivo_instancia> <tamanio_instancia>\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    int num_poblacion = atoi(argv[1]);
    int num_generaciones = atoi(argv[2]);
    int tamanio_instancia = atoi(argv[4]);

    // Validación de población
    while (num_poblacion < 3)
    {
        printf("La población debe ser mayor a 2.\nIngresa de nuevo el valor de la población: ");
        scanf("%d", &num_poblacion);
    }

    // Validación de generaciones
    while (num_generaciones < 1)
    {
        printf("La generación debe ser mayor a 0.\nIngresa de nuevo el valor de la generación: ");
        scanf("%d", &num_generaciones);
    }

    // Validación del tamaño de la instancia
    while (tamanio_instancia != 25 && tamanio_instancia != 50 && tamanio_instancia != 100)
    {
        printf("El tamaño de la instancia debe ser 25, 50 o 100.\nIngresa de nuevo el tamaño de la instancia: ");
        scanf("%d", &tamanio_instancia);
    }

    // Preparar el nombre del archivo
    char archivo_instancia[100];
    snprintf(archivo_instancia, sizeof(archivo_instancia), "%s_(%d)", argv[3], tamanio_instancia);

    // Ejecutar el algoritmo
    aed_tsp(num_poblacion, num_generaciones, tamanio_instancia, archivo_instancia);

    return 0;
}
