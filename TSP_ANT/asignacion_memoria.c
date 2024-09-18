// asignacion_memoria.c
#include <stdlib.h>
#include "asignacion_memoria.h"

individuo *asignar_memoria_arreglo(int poblacion) {
    individuo *ind = (individuo *)malloc(sizeof(individuo) * poblacion);
    return ind;
}
