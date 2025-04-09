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
    double suma_probabilidades;     // Suma de las probabilidades de todos los cliente
    double fitness_global;          // Medida de rendimiento de la hormiga
    lista_ruta *ruta;               // Ruta que sigue la hormiga
} hormiga;

// --------------------- CLIENTES ---------------------
// Estructura que representa un cliente en el tsp
typedef struct cliente
{
    int id_cliente;      // ID único del cliente
    double coordenada_x; // Coordenada X del cliente
    double coordenada_y; // Coordenada Y del cliente
} cliente;

// --------------------- CONFIGURACIÓN tsp ---------------------
// Estructura que contiene la configuración del problema tsp con ventanas de tiempo
typedef struct tsp_configuracion
{
    int num_clientes;        // Número total de clientes en el tsp
    cliente *clientes;       // Arreglo de clientes en el tsp
    double tiempo_ejecucion; // Tiempo de ejecucion del codigo
    char *archivo_instancia; // Nombre del archivo
} tsp_configuracion;

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
