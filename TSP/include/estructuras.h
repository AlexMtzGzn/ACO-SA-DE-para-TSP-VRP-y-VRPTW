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

// --------------------- METAL ---------------------
// Estructura que representa el metal en el algoritmo SA
typedef struct metal
{

    lista_ruta *mejor_solucion;      // Mejor solución encontrada
    double fitness_mejor_solucion;   // Fitness de la mejor solución
    lista_ruta *solucion_inicial;    // Solución inicial generada
    double fitness_solucion_inicial; // Fitness de la solución inicial
    lista_ruta *solucion_actual;     // Solución actual generada
    double fitness_solucion_actual;  // Fitness de la solución actual
    lista_ruta *solucion_vecina;     // Solución vecina generada
    double fitness_solucion_vecina;  // Fitness de la solución vecina

} metal;

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

// --------------------- CONFIGURACIÓN TSP ---------------------
// Estructura que contiene la configuración del problema tsp
typedef struct tsp_configuracion
{
    int num_clientes;        // Número total de clientes en el tsp
    cliente *clientes;       // Arreglo de clientes en el tsp
    double tiempo_ejecucion; // Tiempo de ejecucion del codigo
    char *archivo_instancia; // Nombre del archivo
    int poblacion;           // Poblacion Usada
    int generaciones;        // Generaciones Usadas
    double tiempo_ejecucion; // Tiempo de ejecucion del codigo
    char *archivo_instancia; // Nombre del archivo
} tsp_configuracion;

// --------------------- RANGOS ---------------------
// Estructura que contiene los rangos de los parámetros del algoritmo
typedef struct rangos
{
    double maxAlpha;               // Parámetro alpha máximo
    double minAlpha;               // Parámetro alpha mínimo
    double maxBeta;                // Parámetro beta máximo
    double minBeta;                // Parámetro beta mínimo
    double maxRho;                 // Parámetro rho máximo
    double minRho;                 // Parámetro rho mínimo
    double maxNumHormigas;         // Número máximo de hormigas
    double minNumHormigas;         // Número mínimo de hormigas
    int maxNumIteracionesACO;      // Número máximo de iteraciones ACO
    int minNumIteracionesACO;      // Número mínimo de iteraciones ACO
    double maxTemperatura_inicial; // Temperatura inicial máxima
    double minTemperatura_inicial; // Temperatura inicial mínima
    double maxTemperatura_final;   // Temperatura final máxima
    double minTemperatura_final;   // Temperatura final mínima
    double maxFactor_enfriamiento; // Factor de enfriamiento máximo
    double minFactor_enfriamiento; // Factor de enfriamiento mínimo
    double maxFactor_control;      // Factor de control máximo
    double minFactor_control;      // Factor de control mínimo
    int maxIteracionesSA;          // Número máximo de iteraciones SA
    int minIteracionesSA;          // Número mínimo de iteraciones SA
} rangos;

// --------------------- INDIVIDUO ---------------------
// Estructura que representa un individuo en la población de soluciones
typedef struct individuo
{
    double alpha;               // Parámetro alpha para el algoritmo ACO
    double beta;                // Parámetro beta para el algoritmo ACO
    double rho;                 // Factor de evaporación de feromona para el algoritmo ACO
    int numHormigas;            // Número de hormigas en la población para el algoritmo ACO
    int numIteracionesACO;      // Número de iteraciones del algoritmo para el algoritmo ACO
    double temperatura_inicial; // Temperatura inicial para el algoritmo SA
    double temperatura_final;   // Temperatura final para el algoritmo SA
    double factor_enfriamiento; // Factor de enfriamiento para el algoritmo SA
    int numIteracionesSA;       // Número de iteraciones del algoritmo SA
    double fitness;             // Medida de rendimiento del individuo
    hormiga *hormiga;           // Puntero a la hormiga asociada al individuo
    metal *metal;               // Puntero a las soluciones de SA
} individuo;

#endif // ESTRUCTURAS_H
