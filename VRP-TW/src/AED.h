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


#endif // AED_H
