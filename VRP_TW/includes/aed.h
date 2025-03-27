#ifndef AED_H
#define AED_H

#include "../includes/estructuras.h"

// Prototipos de funciones
double calcular_Distancia(struct vrp_configuracion *vrp, int cliente_origen, int cliente_destino);
void inicializar_Visibilidad(double **instancia_visibilida, struct vrp_configuracion *vrp);
void inicializar_Ventana_Tiempo(double **instancia_ventanas_tiempo, struct vrp_configuracion *vrp);
void inicializar_Distancias(double **instancia_distancias, struct vrp_configuracion *vrp);
void inicializar_Feromona(struct vrp_configuracion *vrp, double **instancia_feromona);
void evaluaFO_AED(struct individuo *ind, double **instancia_feromona, double **instancia_visibilidad, double **instancia_distancias, double **instancia_ventanas_tiempo, struct vrp_configuracion *vrp);
double generaAleatorio(double minimo, double maximo);
void construyeRuidosos(struct individuo *objetivo, struct individuo *ruidoso, int poblacion);
void construyePrueba(struct individuo *objetivo, struct individuo *ruidoso, struct individuo *prueba, int poblacion);
void seleccion(struct individuo *objetivo, struct individuo *prueba, int poblacion);
void inicializaPoblacion(struct individuo *objetivo, int poblacion);
int aed_vrp_tw(int num_poblacion, int num_generaciones, char *archivo_instancia);

#endif // AED_H
