#ifndef TSP_ANT_H
#define TSP_ANT_H
typedef struct
{
    double *ruta;
    double *tabu;
    double fitness;
}hormiga;

void inializacionHormiga(hormiga * hor, int tamanio_instancia,int numHormigas);
void tsp_ant(individuo * ind, double ** instancia_feromona,double ** instancia_distancias,int tamanio_instancia);

#endif // TSP_ANT_H