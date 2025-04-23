#ifndef AED_H
#define AED_H

// Incluimos las bibliotecas necesarias para trabajar con las estructuras de datos del VRP
#include "../include/estructuras.h"

// Prototipos de funciones

// Calcula la distancia entre dos clientes dados en la configuración del VRP
double calcular_Distancia(struct vrp_configuracion *vrp, int cliente_origen, int cliente_destino);

// Inicializa la matriz de visibilidad, que representa la "facilidad" de mover de un cliente a otro
void inicializar_Visibilidad(double **instancia_visibilida, struct vrp_configuracion *vrp);

// Inicializa la matriz de distancias, que contiene la distancia entre cada par de clientes
void inicializar_Distancias(double **instancia_distancias, struct vrp_configuracion *vrp);

// Inicializa la matriz de feromonas, que representa la cantidad de feromona en cada arista
void inicializar_Feromona(struct vrp_configuracion *vrp, double **instancia_feromona);

// Evalúa la función objetivo (FO) del algoritmo evolutivo diferencial (AED) usando la instancia de feromona, visibilidad y distancias
void evaluaFO_AED(struct individuo *ind, double **instancia_feromona, double **instancia_visibilidad,double **instancia_distancias,struct vrp_configuracion *vrp);

// Genera un número aleatorio dentro de un rango especificado (mínimo, máximo)
double generaAleatorio(double minimo, double maximo);

// Construye una solución ruidosa a partir de un individuo objetivo y una población
void construyeRuidosos(struct individuo *objetivo, struct individuo *ruidoso, struct rangos *rango, int poblacion);

// Construye una solución de prueba a partir de un individuo objetivo y una población
void construyePrueba(struct individuo *objetivo, struct individuo *ruidoso, struct individuo *prueba, int poblacion);

// Realiza la selección de la mejor solución entre el individuo objetivo y el de prueba
void seleccion(struct individuo *objetivo, struct individuo *prueba, int poblacion);

// Inicializa una población de individuos para el algoritmo evolutivo
void inicializaPoblacion(struct individuo *objetivo, struct vrp_configuracion *vrp, struct rangos *rango, int poblacion);

// Función principal del algoritmo Evolutivo Diferencial (AED) para resolver el VRP, que recibe el número de individuos, generaciones y el archivo de instancia
void aed_vrp(int num_poblacion, int num_generaciones,int tamanio_instancia, char *archivo_instancia);

#endif // AED_H
