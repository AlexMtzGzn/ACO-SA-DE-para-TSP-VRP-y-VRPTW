#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

// Estructura Cliente
typedef struct cliente {
    int id_cliente;
    double cordenada_x;
    double cordenada_y;
    double demanda_capacidad;
    double vt_inicial;  
    double vt_final;
    double tiempo_servicio;
} cliente;

// Estructura Vrp_configuracion
typedef struct vrp_configuracion {
    int num_clientes;
    int num_vehiculos;
    int num_capacidad;
    cliente *clientes; 
} vrp_configuracion;

//Estructura Individuo
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


#endif // ESTRUCTURAS_H
