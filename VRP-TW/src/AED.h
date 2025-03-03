#ifndef AED_H
#define AED_H

#include "vrp_tw_setting.h"

typedef struct individuo
{
    double alpha;
    double beta;
    double gamma;
    double rho;
    int numHormigas;
    int numIteraciones;
    double fitness;
} individuo;

// Prototipos de funciones con las correcciones
double calcular_distancia(vrp_configuracion *vrp, int cliente_origen, int cliente_destino);
void inicializar_Visibilidad(double **instancia_visibilida, vrp_configuracion *vrp);
void inicializar_Feromona(vrp_configuracion *vrp, double **instancia_feromona, individuo *ind);
void evaluaFO_AED(struct individuo *ind, double **instancia_feromona, double ** instancia_visibilidad,struct vrp_configuracion *vrp);
double generaAleatorio(double minimo, double maximo);
void construyeRuidosos(individuo *objetivo, individuo *ruidoso, int poblacion);
void construyePrueba(individuo *objetivo, individuo *ruidoso, individuo *prueba, int poblacion);
void seleccion(individuo *objetivo, individuo *prueba, int poblacion);
void inicializaPoblacion(individuo *objetivo, int poblacion, int tamanio_instancia);int aed_vrp_tw(int num_poblacion, int num_generaciones, char *archivo_instancia);

#endif // AED_H
