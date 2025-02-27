#ifndef AED_H
#define AED_H

typedef struct
{
    double alpha;
    double beta;
    double gamma;
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

int aed_vrp_tw (int num_poblacion, int num_generaciones, int num_clientes /*, faltan algunos valores*/);


#endif // AED_H
