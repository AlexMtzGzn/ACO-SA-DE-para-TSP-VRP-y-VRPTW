#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

// Declaraciones adelantadas para evitar problemas de dependencias
struct vehiculo;

// --------------------- NODO Y LISTA DE RUTA ---------------------
typedef struct nodo_ruta
{
    int cliente;
    struct nodo_ruta *siguiente;
} nodo_ruta;

typedef struct lista_ruta
{
    nodo_ruta *cabeza;
    nodo_ruta *cola;
} lista_ruta;

// --------------------- VEHÍCULOS ---------------------
typedef struct nodo_vehiculo
{
    struct vehiculo *vehiculo;
    struct nodo_vehiculo *siguiente;
} nodo_vehiculo;

typedef struct lista_vehiculos
{
    nodo_vehiculo *cabeza;
    nodo_vehiculo *cola;
} lista_vehiculos;

typedef struct vehiculo
{
    int id_vehiculo;
    double capacidad_maxima;
    double capacidad_acumulada;
    double vt_actual;
    double vt_final;
    double vt_inicial;
    double velocidad;
    int clientes_contados;
    lista_ruta *ruta;
    double fitness_vehiculo;
} vehiculo;

// --------------------- HORMIGA ---------------------
typedef struct hormiga
{
    int id_hormiga;
    int *tabu;
    int tabu_contador;
    int *posibles_clientes;
    int posibles_clientes_contador;
    double *probabilidades;
    double suma_probabilidades;
    int vehiculos_necesarios;
    int vehiculos_maximos;
    double fitness_global;
    lista_vehiculos *flota;
} hormiga;

// --------------------- CLIENTES ---------------------
typedef struct cliente
{
    int id_cliente;
    double coordenada_x;  
    double coordenada_y;  
    double demanda_capacidad;
    double vt_inicial;
    double vt_final;
    double tiempo_servicio;
} cliente;

// --------------------- CONFIGURACIÓN VRP ---------------------
typedef struct vrp_configuracion
{
    int num_clientes;
    int num_vehiculos;
    int num_capacidad;
    cliente *clientes;
} vrp_configuracion;

// --------------------- INDIVIDUO ---------------------
typedef struct individuo
{
    double alpha;
    double beta;
    double gamma;
    double rho;
    int numHormigas;
    int numIteraciones;
    double fitness;
    hormiga *hormiga;
} individuo;

#endif // ESTRUCTURAS_H
