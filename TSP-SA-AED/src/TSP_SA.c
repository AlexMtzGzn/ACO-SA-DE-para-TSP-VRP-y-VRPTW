#include <stdio.h>
#include <stdlib.h>
#include "TSP_SA.h"
#include "AED.h"
#include "entrada_salida_datos.h"
#include "control_memoria.h"

void generaSolInicial(int *solucion_inicial, int tamanio_instacia){

}

void tsp_sa(individuo *ind , double ** instancias_distancias, int tamanio_instancia){

    int * solucion_inicial=asignar_memoria_ruta(tamanio_instancia+1);
    int * solucion_vecina=asignar_memoria_ruta(tamanio_instancia+1);
    generaSolInicial(solucion_inicial,tamanio_instancia);

}