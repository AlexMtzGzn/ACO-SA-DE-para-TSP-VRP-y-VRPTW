#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "asignacion_memoria.h"
#include "algoritmo_evolutivo_diferencial.h"
#include "tsp_ant.h"


void tsp_ant(individuo * ind, double ** instancia_feromona,double ** intancia_distancias,int tamanio_instancia){

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("%0.2f ", instancia_feromona[i][j]);
        }
        printf("\n");
    }
}
