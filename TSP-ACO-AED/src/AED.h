#ifndef AED_H
#define AED_H

typedef struct
{
    double alpha;
    double beta;
    double rho;
    int numHormigas;
    int numIteraciones;
    double fitness;
    int *ruta;
} individuo;

typedef struct
{
    int generacion;
    double fitness;
    int poblacion;
} generacion;
void inializacion_instancia_feromona(double **instancia_feromonas, int tamanio_instancia, individuo *ind);
void evaluaFO_AED(individuo *ind, double **instancia_feromonas, double **instancia_distancias, int tamanio_instancia);
double generaAleatorio(double minimo, double maximo);
void inicializaPoblacion(individuo *objetivo, int poblacion, int tamanio_instancia);
void construyeRuidosos(individuo *objetivo, individuo *ruidoso, int poblacion);
void construyePrueba(individuo *objetivo, individuo *ruidoso, individuo *prueba, int poblacion);
void aed(int poblacion, int generaciones, int tamanio_instancia, char *archivo_instancia);

#endif // AED_H
