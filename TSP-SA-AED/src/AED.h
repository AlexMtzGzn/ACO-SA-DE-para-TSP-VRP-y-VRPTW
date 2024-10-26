#ifndef AED_H
#define AED_H

typedef struct
{
    double temperatura_incial;
    double temperatura_final;
    double factor_enfriamiento;
    int numIteraciones;
    double fitness;
    int *ruta;
} individuo;

void evaluaFO_AED(individuo *ind,double ** instancia_distacias,int tamanio_instancia);
double generaAleatorio(double minimo, double maximo);
void inicializaPoblacion(individuo *objetivo, int poblacion, int tamanio_instancia);
void construyeRuidosos(individuo *objetivo, individuo *ruidoso, int poblacion);
void construyePrueba(individuo *objetivo, individuo *ruidoso, individuo *prueba, int poblacion);
void aed(int poblacion, int generaciones, int tamanio_instancia, char *archivo_instancia);

#endif // AED_H
