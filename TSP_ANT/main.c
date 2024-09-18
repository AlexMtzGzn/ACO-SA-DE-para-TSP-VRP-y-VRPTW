// main.c
#include <stdio.h>
#include <stdlib.h>
#include "algoritmo_evolutivo_diferencial.h"

void tsp(int poblacion){
    a_v_d(poblacion);  
}

int main (int argc, char * argv[]){
    int poblacion = atoi(argv[1]);
    tsp(poblacion); 
    return 0;
}
