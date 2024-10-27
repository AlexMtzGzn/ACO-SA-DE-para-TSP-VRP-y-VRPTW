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
        solucion_inicial->fitness += instancia_distancias[solucion_inicial->solucion[i]][solucion_inicial->solucion[i + 1]];
    solucion_inicial->fitness += instancia_distancias[solucion_inicial->solucion[tamanio_instancia - 1]][solucion_inicial->solucion[0]]; // regreso a la ciudad inicial
}

void copiaSolucion(solucion *solucion_original, solucion *solucion_copia, int tamanio_instancia)
{
    for (int i = 0; i < tamanio_instancia; ++i)
        solucion_copia->solucion[i] = solucion_original->solucion[i];

    solucion_copia->solucion[tamanio_instancia] = solucion_copia->solucion[0];
    solucion_copia->fitness = solucion_original->fitness;
}

void intercambiarCiudades(solucion *solucion, int tamanio_instancia)
{

    int ciudad1 = rand() % tamanio_instancia;
    int ciudad2 = rand() % tamanio_instancia;

    while (ciudad1 == ciudad2)
        ciudad2 = rand() % tamanio_instancia;

    int temp = solucion->solucion[ciudad1];
    solucion->solucion[ciudad1] = solucion->solucion[ciudad2];
    solucion->solucion[ciudad2] = temp;

    solucion->solucion[tamanio_instancia] = solucion->solucion[0];
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

    while (temperatura > ind->temperatura_final)
    {

        for (int i = 0; i < ind->numIteraciones; ++i)
        {
            generaVecino(solucion_inicial, solucion_vecina, tamanio_instancia);
            evaluaFO(solucion_vecina, instancias_distancias, tamanio_instancia);
            double delta = solucion_vecina->fitness - solucion_inicial->fitness;

            if (delta <= 0 || (rand() / (double)RAND_MAX) < exp(-delta / temperatura))
                copiaSolucion(solucion_vecina, solucion_inicial, tamanio_instancia);

            if (solucion_inicial->fitness < mejor_solucion->fitness)
                copiaSolucion(solucion_inicial, mejor_solucion, tamanio_instancia);
        }
        temperatura *= ind->enfriamiento;
    }

    ind->fitness = mejor_solucion->fitness;
    ind->ruta = asignar_memoria_ruta(tamanio_instancia + 1);

    for (int i = 0; i <= tamanio_instancia; ++i)
        ind->ruta[i] = mejor_solucion->solucion[i];
}

void generaSolInicial(solucion *solucion_inicial, int tamanio_instancia)
{
    for (int i = 0; i < tamanio_instancia; ++i)
        solucion_inicial->solucion[i] = i;

    for (int i = tamanio_instancia - 1; i > 0; --i)
    {
        int j = rand() % (i + 1);
        int temp = solucion_inicial->solucion[i];
        solucion_inicial->solucion[i] = solucion_inicial->solucion[j];
        solucion_inicial->solucion[j] = temp;
    }

    solucion_inicial->solucion[tamanio_instancia] = solucion_inicial->solucion[0];
}

void tsp_sa(individuo *ind, double **instancias_distancias, int tamanio_instancia)
{
    solucion *solucion_inicial = (solucion *)malloc(sizeof(solucion));
    solucion *solucion_vecina = (solucion *)malloc(sizeof(solucion));
    solucion *mejor_solucion = (solucion *)malloc(sizeof(solucion));

    solucion_inicial->solucion = asignar_memoria_ruta(tamanio_instancia + 1);
    solucion_vecina->solucion = asignar_memoria_ruta(tamanio_instancia + 1);
    mejor_solucion->solucion = asignar_memoria_ruta(tamanio_instancia + 1);

    generaSolInicial(solucion_inicial, tamanio_instancia);
    sa(ind, solucion_inicial, solucion_vecina, mejor_solucion, instancias_distancias, tamanio_instancia);

    liberar_soluciones(solucion_inicial, true);
    liberar_soluciones(solucion_vecina, true);
    liberar_soluciones(mejor_solucion, true);
}
