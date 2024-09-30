#ifndef ALGORITMO_EVOLUTIVO_DIFERENCIAL_H
#define ALGORITMO_EVOLUTIVO_DIFERENCIAL_H

typedef struct {
    double alpha;
    double beta;
    double rho;
    int numHormigas;
    int numIteraciones;
    double fitness;
    int *ruta;
} individuo;


void inializacion_instancia_feromona(double **instancia_feromonas, int tamanio_instancia, individuo * ind);
void evaluaFO(individuo *ind, double **instancia_feromonas, double **instancia_distancias,int tamanio_instancia);
double generaAleatorio(double minimo, double maximo);
void inicializaPoblacion(individuo *objetivo, int poblacion,int tamanio_instancia);
void construyeRuidosos(individuo *objetivo, individuo *ruidoso, int poblacion);
void construyePrueba(individuo *objetivo, individuo *ruidoso, individuo *prueba, int poblacion);
void imprimePoblacion(individuo *objetivo, int poblacion);
void leer_instancia(double **instancia_distancias, int tamanio_instancia, char *archivo_instancia);
void imprimir_instancia(double ** instancia, int tamanio_instancia);
void algoritmo_evolutivo_diferencial(int poblacion,int generaciones,int tamanio_instancia, char *archivo_instancia);


#endif // ALGORITMO_EVOLUTIVO_DIFERENCIAL_H
