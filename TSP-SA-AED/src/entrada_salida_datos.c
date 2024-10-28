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
        printf("\nTemperatuara Inicial: %.2lf, Temperatura Final: %.2lf, Factor Enfriamiento: %.2lf, Iteraciones: %d\n", ind->temperatura_inicial, ind->temperatura_final, ind->enfriamiento, ind->numIteraciones);
        if (bandera == true)
        {
            printf("Fitness: %.2lf\nRuta : ", ind[i].fitness);
            for (int j = 0; j <= tamanio_instancia; j++)
            {
                if (j < tamanio_instancia)
                    printf("%d -> ", ind[i].ruta[j] + 1);
                else
                    printf("%d\n", ind[i].ruta[j] + 1);
            }
        }
    }
}

void imprimir_solucion(solucion *solucion, int tamanio_instancia)
{
    printf("Fitness: %.2lf\nRuta : ", solucion->fitness);
    for (int j = 0; j <= tamanio_instancia; j++)
    {
        if (j < tamanio_instancia)
            printf("%d -> ", solucion->solucion[j] + 1);
        else
            printf("%d\n", solucion->solucion[j] + 1);
    }
}

void imprimir_ind(individuo *ind, int tamanio_instancia)
{

    printf("\nTemperatuara Inicial: %.2lf, Temperatura Final: %.2lf, Factor Enfriamiento: %.2lf, Iteraciones: %d\n", ind->temperatura_inicial, ind->temperatura_final, ind->enfriamiento, ind->numIteraciones);

    printf("Fitness: %.2lf\nRuta : ", ind->fitness);
    for (int j = 0; j <= tamanio_instancia; j++)
    {
        if (j < tamanio_instancia)
            printf("%d -> ", ind->ruta[j] + 1);
        else
            printf("%d\n", ind->ruta[j] + 1);
    }
}

void imprimir_Archivo(generacion *datos, double tiempo, int poblacion, int generaciones, const char *nombre_archivo)
{
    char filename[256];
    snprintf(filename, sizeof(filename), "%s", nombre_archivo);

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        fprintf(stderr, "Error: No se pudo abrir el archivo %s\n", filename);
        return;
    }

    fprintf(file, "%lf %d %d\n", tiempo, poblacion, generaciones);

    for (int i = 0; i < poblacion * generaciones; i++)
    {
        fprintf(file, "%d,%d,%.lf\n",
                datos[i].poblacion,
                datos[i].generacion,
                datos[i].fitness);
    }

    fclose(file);
    printf("\nDatos guardados exitosamente en: %s\n", filename);
}
