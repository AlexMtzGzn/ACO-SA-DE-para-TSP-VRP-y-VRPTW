#include <stdio.h>
#include <stdlib.h>
#include "entrada_salida_datos.h"

void imprimir_instancia(double **matriz_instancia, int tamanio_instancia, char *texto_instancia)
{
    printf("\n\n %s\n\n",texto_instancia);
    for (int i = 0; i < tamanio_instancia; i++)
    {
        for (int j = 0; j < tamanio_instancia; j++)
        {
            printf(" %.2lf ", matriz_instancia[i][j]);
        }
        printf("\n");
    }
}