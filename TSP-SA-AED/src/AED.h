#ifndef AED_H
#define AED_H

typedef struct
{
    double temperatura_inicial;
    double temperatura_final;
    int numIteraciones;
    double enfriamiento;
    double fitness;
    int *ruta;
} individuo;

typedef struct 
{
    int generacion;
    double fitness;
    int poblacion;
} generacion;

void evaluaFO_AED(individuo *ind,double ** instancia_distacias,int tamanio_instancia);
double generaAleatorio(double minimo, double maximo);
void inicializaPoblacion(individuo *objetivo, int poblacion);
void construyeRuidosos(individuo *objetivo, individuo *ruidoso, int poblacion);
void construyePrueba(individuo *objetivo, individuo *ruidoso, individuo *prueba, int poblacion);
void aed(int poblacion, int generaciones, int tamanio_instancia, char *archivo_instancia);

#endif // AED_H
