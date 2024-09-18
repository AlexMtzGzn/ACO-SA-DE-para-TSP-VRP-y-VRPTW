#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
//include <windows.h>

typedef struct{
	double * arreglo;
	double fitness;
}vector;

double evaluaFO(double arreglo[], int nx){
    char buffer[128];
    double resultado;
  //recocido se corre con ./recocido temp_inicial temperatura_final enfriamiento iteraciones tamanio nombre_archivo
    snprintf(buffer, sizeof(buffer), "./recocido_edgecover %f %f %f %d 8 matriz_grafo.txt", 
             arreglo[0], arreglo[1], arreglo[2], (int)arreglo[3]);
		//arreglo0 es temp_inicial
		//arreglo1 es temp_final
		//arreglo2 es enfriamiento (actualizacion de temperatura)
		//arreglo3 es numero de iteraciones
    //abrimos un pipe para ejecutar el comando y leer su salida
    FILE *fp = popen(buffer, "r");//popen es para trabajar con pipes... en los s.o. los pipes son un medio de comunicacion entre procesos
 
    //leemos la salida del comando, que se supone es un número flotante
    if(fgets(buffer, sizeof(buffer), fp) != NULL)
        resultado = atof(buffer);  //convertimos la salida a un número flotante
    else
        resultado = -1.0;  //valor de error si no se pudo leer la salida
    //printf("\nResultado %f",resultado);
    //cerramos el pipe
    pclose(fp);
    return resultado;
}

double generaAleatorio(double minimo, double maximo){
	double aleatorio = (double) rand()/RAND_MAX;//aleatorio entre 0 y 1
	return minimo+aleatorio*(maximo-minimo);
}

void inicializaPoblacion(vector * objetivo, int dimension, int poblacion){
	for(int i=0;i<poblacion;++i)
		for(int j=0;j<dimension;++j){
			if(j==0)//valor para la temperatura inicial
				objetivo[i].arreglo[j]=generaAleatorio(900.0,1000.0);
			else if(j==1)//valor para la temperatura final
				objetivo[i].arreglo[j]=generaAleatorio(0.1,0.25);
			else if(j==2)//valor para actualizacion
				objetivo[i].arreglo[j]=generaAleatorio(0.9,0.99);
			else//valor para numero de iteraciones
				objetivo[i].arreglo[j]=generaAleatorio(5.0,20.0);
		}

}

void imprimePoblacion(vector * poblacion_vector, int dimension, int poblacion){
	for(int i=0;i<poblacion;++i){
		printf("\nSoy el elemento %d de la población: ",i+1);
		for(int j=0;j<dimension;++j){
			printf("%f ",poblacion_vector[i].arreglo[j]);
		}
		printf("\nCon fitness: %f",poblacion_vector[i].fitness);
	}
}
//los ruidosos son los mutados
//construyeRuidosos(objetivo,ruidoso,poblacion,dimension);
void construyeRuidosos(vector * objetivo, vector * ruidoso, int poblacion, int dimension){
	for(int i=0;i<poblacion;++i){
		int aleatorio1=0,aleatorio2=0,aleatorio3=0;
		while(aleatorio1==aleatorio2||aleatorio2==aleatorio3||aleatorio1==aleatorio3){
			aleatorio1=rand()%poblacion;
			aleatorio2=rand()%poblacion;
			aleatorio3=rand()%poblacion;
		}
		for(int j=0;j<dimension;++j){
			ruidoso[i].arreglo[j]=objetivo[aleatorio1].arreglo[j]+0.5*(objetivo[aleatorio2].arreglo[j]-objetivo[aleatorio3].arreglo[j]);
			if(j==0){//temp inicial
				if(ruidoso[i].arreglo[j]>1000.0)
					ruidoso[i].arreglo[j]=1000.0;
				if(ruidoso[i].arreglo[j]<900.0)
					ruidoso[i].arreglo[j]=900.0;
			}
			else if(j==1){//temp final
				if(ruidoso[i].arreglo[j]>0.25)
					ruidoso[i].arreglo[j]=0.25;
				if(ruidoso[i].arreglo[j]<0.1)
					ruidoso[i].arreglo[j]=0.1;
			}
			else if(j==2){//factor de actualizacion
				if(ruidoso[i].arreglo[j]>0.99)
					ruidoso[i].arreglo[j]=0.99;
				if(ruidoso[i].arreglo[j]<0.9)
					ruidoso[i].arreglo[j]=0.9;
			}
			else{
				if(ruidoso[i].arreglo[j]>20.0)
					ruidoso[i].arreglo[j]=20.0;
				if(ruidoso[i].arreglo[j]<5.0)
					ruidoso[i].arreglo[j]=5.0;
			}
		}
	}
}
//los de prueba son los cruzados
//construyePrueba(objetivo,ruidoso,prueba,dimension,poblacion);
void construyePrueba(vector * objetivo, vector * ruidoso, vector *prueba,int dimension, int poblacion){
	for(int i=0; i < poblacion;++i)//iteramos cada individuo de la poblacion
		for(int j=0;j<dimension;++j){//iteramos ahora cada elemento del individuo
			double aleatorio = (double) rand()/RAND_MAX;
			if(aleatorio <= 0.5)//0.5 es la tasa de cruza
				prueba[i].arreglo[j]=objetivo[i].arreglo[j];
			else
				prueba[i].arreglo[j]=ruidoso[i].arreglo[j];
		}
}

void seleccion(vector * objetivo, vector * prueba, int poblacion){
	//vamos a recorrer cada elemento de los vectores objetivo y prueba para remplazar
	for(int i=0;i<poblacion;++i){//si el fitness del de prueba es mejor, remplazamos
		if(objetivo[i].fitness > prueba[i].fitness){
			objetivo[i]=prueba[i];
		}
	}
		//en caso contrario, no hacemos nada
}

int main(int argc, char * argv[]){
	//para evolucion diferencial necesitamos trabajar con los parametros ya definidos
	srand(time(NULL));
	int poblacion=100;
	//generaciones 50
	//tasa de mutacion 0.5
	//tasa de cruza 0.5
	int dimension=atoi(argv[1]);
	vector * objetivo=(vector*)malloc(sizeof(vector)*poblacion);
	for(int i=0;i<poblacion;++i)
		objetivo[i].arreglo=(double *)malloc(sizeof(double)*dimension);
	vector * ruidoso=(vector*)malloc(sizeof(vector)*poblacion);
	for(int i=0;i<poblacion;++i)
		ruidoso[i].arreglo=(double *)malloc(sizeof(double)*dimension);
	vector * prueba=(vector*)malloc(sizeof(vector)*poblacion);
	for(int i=0;i<poblacion;++i)
		prueba[i].arreglo=(double *)malloc(sizeof(double)*dimension);
	
	inicializaPoblacion(objetivo,dimension,poblacion);
	int generacion=0;
	while(generacion<1000){
		construyeRuidosos(objetivo,ruidoso,poblacion,dimension);
		construyePrueba(objetivo,ruidoso,prueba,dimension,poblacion);
		for(int i=0;i<poblacion;++i){
			objetivo[i].fitness=evaluaFO(objetivo[i].arreglo,dimension);//evaluamos a los vectores objetivo
			prueba[i].fitness=evaluaFO(prueba[i].arreglo,dimension);//evaluamos a los vectores de prueba
		}
		seleccion(objetivo,prueba,poblacion);
		imprimePoblacion(objetivo,dimension,poblacion);
		//imprimePoblacion(ruidoso,dimension,poblacion);
		//imprimePoblacion(prueba,dimension,poblacion);
		generacion++;
		//getchar();
	}
}
