#ifndef ALGORITMO_EVOLUTIVO_DIFERENCIAL_H
#define ALGORITMO_EVOLUTIVO_DIFERENCIAL_H

// Estructura de individuo
typedef struct
{
    double alpha;       // Influencia de la feromona
    double beta;        // Influencia del costo
    double rho;         // Tasa de evaporación de la feromona
    int numHormigas;    // Número de hormigas
    int numIteraciones; // Número de iteraciones
    double fitness;     // Valor de fitness
} individuo;

double evaluacionFO();
double generaAleatorio(double minimo, double maximo);
void inicializaPoblacion(individuo *objetivo, int poblacion);
void construyeRuidosos(individuo *objetivo, individuo *ruidoso, int poblacion);
void construyePrueba(individuo *objetivo, individuo *ruidoso, individuo *prueba, int poblacion);
void imprimePoblacion(individuo *objetivo, int poblacion);
void leer_instancia(double ** instancia_matriz,int tamanio_instancia, char * archivo );
void inializacion_instancia_feromona(double ** instancia_matriz,int tamanio_instancia);
void a_v_d(int tamanio_instancia,char *instancia);

#endif // ALGORITMO_EVOLUTIVO_DIFERENCIAL_H
