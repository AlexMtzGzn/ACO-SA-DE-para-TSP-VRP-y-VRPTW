#include <stdio.h>
#include <stdlib.h>
#include "entrada_salida_datos.h"

void leer_instancia(double **instancia_distancias, int tamanio_instancia, char *archivo_instancia)
{
    FILE *instancia = fopen(archivo_instancia, "r");

    for (int i = 0; i < tamanio_instancia; i++)
        for (int j = 0; j < tamanio_instancia; j++)
            fscanf(instancia, "%lf", &instancia_distancias[i][j]);

    fclose(instancia);
}

void imprimir_instancia(double **matriz_instancia, int tamanio_instancia, char *texto_instancia)
{
    printf("\n\n%s\n\n", texto_instancia);
    for (int i = 0; i < tamanio_instancia; i++)
    {
        for (int j = 0; j < tamanio_instancia; j++)
            printf(" %.2lf ", matriz_instancia[i][j]);
        printf("\n");
    }
}

void imprimir_individuo(individuo *ind, int tamanio_instancia, int poblacion, bool bandera)
{
    for (int i = 0; i < poblacion; i++)
    {
        printf("\nIndividio:  %d", i + 1);
        printf("\nAlpha: %.2lf, Beta: %.2lf, Rho: %.2lf, Hormigas: %d, Iteraciones: %d\n", ind[i].alpha, ind[i].beta, ind[i].rho, ind[i].numHormigas, ind[i].numIteraciones);
        if (bandera == true)
        {
            printf("Fitness: %.2lf\nRuta : ", ind[i].fitness);
            for (int j = 0; j <= tamanio_instancia; j++)
            {
                if (j < tamanio_instancia)
                    printf("%d -> ", ind[i].ruta[j]+1);
                else
                    printf("%d\n", ind[i].ruta[j]+1);
            }
        }
    }
}

void imprimir_ruta_hormiga(hormiga *hor, int tamanio_instancia, int n_hormiga, int n_iteracion)
{
    printf("\nHormiga %d Iteracion %d : ", (n_hormiga+1), (n_iteracion+1));
    printf("Ruta : ");
    for (int i = 0; i <= tamanio_instancia; i++)
    {
        if (i < tamanio_instancia)
            printf("%d -> ", hor->ruta[i]+1);
        else
            printf("%d\n", hor->ruta[i]+1);
    }
    printf("fitness: %.2lf\n", hor->fitness);
}