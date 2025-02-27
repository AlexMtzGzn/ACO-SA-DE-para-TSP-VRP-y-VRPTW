#include <stdio.h>
#include <stdlib.h>
//#include <time.h>
#include "AED.h"
//#include "TSP_ACO.h"
#include "entrada_salida_datos.h"
#include "control_memoria.h"

int aed_vrp_tw (int num_poblacion, int num_generaciones, int num_clientes /*, faltan algunos valores*/)
{

   individuo *objetivo = asignar_memoria_individuos(num_poblacion);
   individuo *ruidoso = asignar_memoria_individuos(num_poblacion);
   individuo *prueba = asignar_memoria_individuos(num_poblacion);

}