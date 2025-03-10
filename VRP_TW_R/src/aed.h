#ifndef AED_H
#define AED_H

/*Bibleotecas*/
#include "configuracion_vrp_tw.h"

//Estructura individuo
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
double calcular_distancia(struct vrp_configuracion *vrp, int cliente_origen, int cliente_destino);
void inicializar_Visibilidad(double **instancia_visibilida, struct vrp_configuracion *vrp);
void inicializar_Distancias(double **instancia_distancias, struct vrp_configuracion *vrp);
void inicializar_Feromona(struct vrp_configuracion *vrp, double **instancia_feromona, struct individuo *ind);
void evaluaFO_AED(struct individuo *ind, double **instancia_feromona, double **instancia_visibilidad, struct vrp_configuracion *vrp);
double generaAleatorio(double minimo, double maximo);
void construyeRuidosos(struct individuo *objetivo, struct individuo *ruidoso, int poblacion);
void construyePrueba(struct individuo *objetivo, struct individuo *ruidoso, struct individuo *prueba, int poblacion);
void seleccion(struct individuo *objetivo, struct individuo *prueba, int poblacion);
void inicializaPoblacion(struct individuo *objetivo, int poblacion);
int aed_vrp_tw(int num_poblacion, int num_generaciones, char *archivo_instancia);

#endif // AED_H
