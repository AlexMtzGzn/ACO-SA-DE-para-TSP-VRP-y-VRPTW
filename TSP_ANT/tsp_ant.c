#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "asignacion_memoria.h"
#include "algoritmo_evolutivo_diferencial.h"
#include "tsp_ant.h"

void inializacionHormiga(hormiga * hor, int tamanio_instancia,int numHormigas){
    for(int i = 0; i < tamanio_instancia; i++){
        hor[i].ruta = asignacion_memoria_arreglo(tamanio_instancia+1);
        hor[i].tabu = asignacion_memoria_arreglo(tamanio_instancia);
        hor[i].fitness = 0.0;
    }
}

void tsp_ant(individuo * ind, double ** instancia_feromona,double ** instancia_distancias,int tamanio_instancia){
    hormiga * hor = asignar_memoria_arreglo_estructura_hormiga(ind->numHormigas);
    inializacionHormiga(hor,tamanio_instancia,ind->numHormigas);
}
