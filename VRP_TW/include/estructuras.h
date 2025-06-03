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

// --------------------- TIEMPO CLIENTE ---------------------
// Estructura que representa los tiempos de cada cliente

typedef struct tiempos_cliente
{
    int cliente;              // ID del cliente
    double tiempo_llegada;    // Tiempo de llegada del vehiculo al cliente
    double tiempo_espera;     // Tiempo de espera del vehiculo en el cliente
    double inicio_servicio;   // Tiempo de inicio del servicio al cliente
    double duracion_servicio; // Duración del servicio al cliente
    double tiempo_salida;     // Tiempo de salida del vehiculo del cliente

} tiempos_cliente;
// --------------------- VEHÍCULOS ---------------------
// Estructura que representa un vehículo en la flota
typedef struct vehiculo
{
    int id_vehiculo;                  // ID único del vehículo
    double capacidad_maxima;          // Capacidad máxima del vehículo
    double capacidad_acumulada;       // Capacidad actual acumulada en el vehículo
    double vt_actual;                 // Ventana de tiempo actual del vehículo
    double vt_final;                  // Ventana de tiempo final
    double vt_inicial;                // Ventana de tiempo inicial
    double tiempo_salida_vehiculo;    // Tiempo de salida del vehiculo
    double tiempo_llegada_vehiculo;   // Tiempo de llegada del vehiculo
    double velocidad;                 // Velocidad del vehículo
    int clientes_contados;            // Contador de clientes atendidos por el vehículo
    lista_ruta *ruta;                 // Ruta que sigue el vehículo
    double fitness_vehiculo;          // Medida de rendimiento del vehículo
    tiempos_cliente *tiempos_cliente; // Arreglo que lleva el control de de los timepos de servicio del vehiuculo con el cliente
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

// --------------------- METAL ---------------------
// Estructura que representa el metal en el algoritmo SA
typedef struct metal
{

    lista_vehiculos *mejor_solucion;   // Mejor solución
    double fitness_mejor_solucion;     // Fitness de la mejor solución
    lista_vehiculos *solucion_inicial; // Solución inicial
    double fitness_solucion_inicial;   // Fitness de la solución inicial
    lista_vehiculos *solucion_actual;  // Solución actual
    double fitness_solucion_actual;    // Fitness de la solución actual
    lista_vehiculos *solucion_vecina;  // Solución vecina
    double fitness_solucion_vecina;    // Fitness de la solución vecina

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
    double suma_probabilidades;     // Suma de las probabilidades de todos los clientes
    int vehiculos_necesarios;       // Número de vehículos necesarios para la hormiga
    int vehiculos_maximos;          // Número máximo de vehículos que puede utilizar la hormiga
    double fitness_global;          // Medida de rendimiento de la hormiga
    metal *metal;                   // Puntero a las soluciones de SA
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
    double vt_inicial;        // Ventana de tiempo inicial
    double vt_final;          // Ventana de tiempo final
    double tiempo_servicio;   // Tiempo necesario para servir al cliente
} cliente;

// --------------------- CONFIGURACIÓN VRP ---------------------
// Estructura que contiene la configuración del problema VRP con ventanas de tiempo
typedef struct vrp_configuracion
{
    int num_clientes;        // Número total de clientes en el VRP
    int num_vehiculos;       // Número total de vehículos disponibles
    int num_capacidad;       // Capacidad de cada vehículo
    int generaciones;        // Numero de generaciones
    int poblacion;           // Numero de poblacion
    cliente *clientes;       // Arreglo de clientes en el VRP
    double tiempo_ejecucion; // Tiempo de ejecucion del codigo
    char *archivo_instancia; // Nombre del archivo
} vrp_configuracion;
// --------------------- RANGOS ---------------------
// Estructura que contiene los rangos de los parámetros del algoritmo
typedef struct rangos
{
    double maxAlpha;               // Parámetro alpha máximo
    double minAlpha;               // Parámetro alpha mínimo
    double maxBeta;                // Parámetro beta máximo
    double minBeta;                // Parámetro beta mínimo
    double maxGamma;               // Parametro gamma minimo
    double minGamma;               // Parametro gamma maximo
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
    double gamma;               // Parametro gamma para el algoritmo ACO
    double rho;                 // Factor de evaporación de feromona para el algoritmo ACO
    int numHormigas;            // Número de hormigas en la población para el algoritmo ACO
    int numIteracionesACO;      // Número de iteraciones del algoritmo para el algoritmo ACO
    double temperatura_inicial; // Temperatura inicial para el algoritmo SA
    double temperatura_final;   // Temperatura final para el algoritmo SA
    double factor_enfriamiento; // Factor de enfriamiento para el algoritmo SA
    double factor_control;      // Factor de control para el algoritmo SA
    int numIteracionesSA;       // Número de iteraciones del algoritmo SA
    double fitness;             // Medida de rendimiento del individuo
    hormiga *hormiga;           // Puntero a la hormiga asociada al individuo
    //metal *metal;               // Puntero a las soluciones de SA
} individuo;

#endif // ESTRUCTURAS_H
