#ifndef ENTRADA_SALIDA_DATOS_H
#define ENTRADA_SALIDA_DATOS_H
#include <stdbool.h>
#include "AED.h"

void imprimir_instancia(double **matriz_instancia, int tamanio_instancia, char *texto_instancia);
void imprimir_individuo(individuo *ind, int tamanio_instancia,int poblacion,bool bandera);

#endif // ENTRADA_SALIDA_DATOS_H