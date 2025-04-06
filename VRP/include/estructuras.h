#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

// --------------------- NODO Y LISTA DE RUTA ---------------------
// Estructura que representa un nodo en la ruta de un vehículo
typedef struct nodo_ruta
{
    int cliente;                 // ID del cliente en la ruta
    struct nodo_ruta *siguiente; // Puntero al siguiente nodo de la ruta
} nodo_ruta;

// Estructura que representa una lista de rutas, la cual es la secuencia de nodos
typedef struct lista_ruta
{
    nodo_ruta *cabeza; // Primer nodo en la ruta
    nodo_ruta *cola;   // Último nodo en la ruta
} lista_ruta;

// --------------------- VEHÍCULOS ---------------------
// Estructura que representa un vehículo en la flota
typedef struct vehiculo
{
    int id_vehiculo;            // ID único del vehículo
    double capacidad_maxima;    // Capacidad máxima del vehículo
    double capacidad_acumulada; // Capacidad actual acumulada en el vehículo
    double velocidad;           // Velocidad del vehículo
    int clientes_contados;      // Contador de clientes atendidos por el vehículo
    lista_ruta *ruta;           // Ruta que sigue el vehículo
    double fitness_vehiculo;    // Medida de rendimiento del vehículo
} vehiculo;

// --------------------- NODO Y LISTA DE VEHÍCULOS ---------------------
// Estructura que representa un nodo en la lista de vehículos
typedef struct nodo_vehiculo
{
    vehiculo *vehiculo;              // Puntero a un vehículo
    struct nodo_vehiculo *siguiente; // Puntero al siguiente nodo de vehículos
} nodo_vehiculo;

// Estructura que representa la lista de vehículos (flota de vehículos)
typedef struct lista_vehiculos
{
    nodo_vehiculo *cabeza; // Primer vehículo en la flota
    nodo_vehiculo *cola;   // Último vehículo en la flota
} lista_vehiculos;

// --------------------- HORMIGA ---------------------
// Estructura que representa una hormiga en el algoritmo ACO
typedef struct hormiga
{
    int id_hormiga;                 // ID único de la hormiga
    int *tabu;                      // Arreglo que indica las ciudades visitadas por la hormiga (tabú)
    int tabu_contador;              // Contador de la longitud de la lista tabu
    int *posibles_clientes;         // Arreglo de clientes posibles para visitar
    int posibles_clientes_contador; // Contador de clientes posibles a visitar
    double *probabilidades;         // Probabilidades de elegir un cliente siguiente
    double suma_probabilidades;     // Suma de las probabilidades de todos los clientes
    int vehiculos_necesarios;       // Número de vehículos necesarios para la hormiga
    int vehiculos_maximos;          // Número máximo de vehículos que puede utilizar la hormiga
    double fitness_global;          // Medida de rendimiento de la hormiga
    lista_vehiculos *flota;         // Flota de vehículos asignada a la hormiga
} hormiga;

// --------------------- CLIENTES ---------------------
// Estructura que representa un cliente en el VRP
typedef struct cliente
{
    int id_cliente;           // ID único del cliente
    double coordenada_x;      // Coordenada X del cliente
    double coordenada_y;      // Coordenada Y del cliente
    double demanda_capacidad; // Demanda de capacidad del cliente
} cliente;

// --------------------- CONFIGURACIÓN VRP ---------------------
// Estructura que contiene la configuración del problema VRP con ventanas de tiempo
typedef struct vrp_configuracion
{
    int num_clientes;            // Número total de clientes en el VRP
    int num_vehiculos;           // Número total de vehículos disponibles
    int num_capacidad;           // Capacidad de cada vehículo
    cliente *clientes;           // Arreglo de clientes en el VRP
    double tiempo_ejecucion;     // Tiempo de ejecucion del codigo
    char *archivo_instancia; // Nombre del archivo
} vrp_configuracion;

// --------------------- INDIVIDUO ---------------------
// Estructura que representa un individuo en la población de soluciones
typedef struct individuo
{
    double alpha;       // Parámetro alpha para el algoritmo ACO
    double beta;        // Parámetro beta para el algoritmo ACO
    double rho;         // Factor de evaporación de feromona
    int numHormigas;    // Número de hormigas en la población
    int numIteraciones; // Número de iteraciones del algoritmo
    double fitness;     // Medida de rendimiento del individuo
    hormiga *hormiga;   // Puntero a la hormiga asociada al individuo
} individuo;

#endif // ESTRUCTURAS_H
