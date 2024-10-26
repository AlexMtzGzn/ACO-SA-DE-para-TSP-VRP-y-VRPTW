#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "TSP_SA.h"
#include "AED.h"
#include "entrada_salida_datos.h"
#include "control_memoria.h"

void evaluaFO(solucion *solucion_inicial, double **instancia_distancias, int tamanio_instancia)
{
    solucion_inicial->fitness = 0.0;
    for (int i = 0; i < tamanio_instancia - 1; ++i)
    {
        solucion_inicial->fitness += instancia_distancias[solucion_inicial->solucion[i]][solucion_inicial->solucion[i + 1]];
    }
    solucion_inicial->fitness += instancia_distancias[solucion_inicial->solucion[tamanio_instancia - 1]][solucion_inicial->solucion[0]]; // regreso a la ciudad inicial
}

void copiaSolucion(solucion *solucion_original, solucion *solucion_copia, int tamanio_instancia)
{
    for (int i = 0; i < tamanio_instancia; ++i)
        solucion_copia->solucion[i] = solucion_original->solucion[i];
    solucion_copia->fitness = solucion_original->fitness;
}

void intercambiarCiudades(solucion *solucion, int tamanio_instancia)
{
    int ciudad1 = rand() % tamanio_instancia;
    int ciudad2 = rand() % tamanio_instancia;
    int temp = solucion->solucion[ciudad1];
    solucion->solucion[ciudad1] = solucion->solucion[ciudad2];
    solucion->solucion[ciudad2] = temp;
}

void generaVecino(solucion *solucion_inicial, solucion *solucion_vecina, int tamanio_instancia)
{
    copiaSolucion(solucion_inicial, solucion_vecina, tamanio_instancia);
    intercambiarCiudades(solucion_vecina, tamanio_instancia);
}

void sa(individuo *ind, solucion *solucion_inicial, solucion *solucion_vecina, solucion *mejor_solucion, double **instancias_distancias, int tamanio_instancia)
{
    evaluaFO(solucion_inicial, instancias_distancias, tamanio_instancia);
    copiaSolucion(solucion_inicial, mejor_solucion, tamanio_instancia);

    double temperatura = ind->temperatura_inicial;
    double mejorFO = mejor_solucion->fitness;

    while (temperatura > ind->temperatura_final)
    {
        for (int i = 0; i < ind->numIteraciones; ++i)
        {
            generaVecino(solucion_inicial, solucion_vecina, tamanio_instancia);
            evaluaFO(solucion_vecina, instancias_distancias, tamanio_instancia);
            double delta = solucion_vecina->fitness - solucion_inicial->fitness;

            if (delta <= 0 || (rand() / (double)RAND_MAX) < exp(-delta / temperatura))
            {
                copiaSolucion(solucion_vecina, solucion_inicial, tamanio_instancia);
            }

            if (solucion_inicial->fitness < mejorFO)
            {
                copiaSolucion(solucion_inicial, mejor_solucion, tamanio_instancia);
                mejorFO = mejor_solucion->fitness;
            }
        }
        temperatura *= ind->enfriamiento;
    }

    printf("Mejor distancia encontrada: %f\n", mejorFO);
    printf("Mejor ruta: ");
    for (int i = 0; i < tamanio_instancia; ++i)
    {
        printf("%d ", mejor_solucion->solucion[i]);
    }
    printf("\n");
}

void generaSolInicial(solucion *solucion_inicial, int tamanio_instancia)
{
    for (int i = 0; i < tamanio_instancia; ++i)
        solucion_inicial->solucion[i] = i;

    for (int i = 0; i < tamanio_instancia; ++i)
    {
        int j = rand() % tamanio_instancia;
        int temp = solucion_inicial->solucion[i];
        solucion_inicial->solucion[i] = solucion_inicial->solucion[j];
        solucion_inicial->solucion[j] = temp;
    }
}

void tsp_sa(individuo *ind, double **instancias_distancias, int tamanio_instancia)
{
    solucion *solucion_inicial = (solucion *)malloc(sizeof(solucion));
    solucion *solucion_vecina = (solucion *)malloc(sizeof(solucion));
    solucion *mejor_solucion = (solucion *)malloc(sizeof(solucion));

    solucion_inicial->solucion = asignar_memoria_ruta(tamanio_instancia);
    solucion_vecina->solucion = asignar_memoria_ruta(tamanio_instancia);
    mejor_solucion->solucion = asignar_memoria_ruta(tamanio_instancia);

    generaSolInicial(solucion_inicial, tamanio_instancia);
    sa(ind, solucion_inicial, solucion_vecina, mejor_solucion, instancias_distancias, tamanio_instancia);

    // Liberación de memoria
    free(solucion_inicial->solucion);
    free(solucion_vecina->solucion);
    free(mejor_solucion->solucion);
    free(solucion_inicial);
    free(solucion_vecina);
    free(mejor_solucion);
}
