#!/bin/bash

# Verificar si se proporcionaron los parámetros de letra, inicio y fin
if [ $# -ne 3 ]; then
    echo "Uso: $0 <letra> <numero_inicio> <numero_fin>"
    exit 1
fi

# Parámetros de entrada
letra=$1
numero_inicio=$2
numero_fin=$3

# Comprobar si el número de inicio es menor que el de fin
if [ "$numero_inicio" -gt "$numero_fin" ]; then
    echo "El número de inicio debe ser menor o igual al número final."
    exit 1
fi

# Iterar sobre el rango de números
for ((i=numero_inicio; i<=numero_fin; i++))
do
    # Generar el nombre del archivo usando la letra y el número actual
    archivo="${letra}${i}_(25).txt"

    # Verificar si el archivo existe
    if [ ! -f "$archivo" ]; then
        echo "No se encontró el archivo $archivo."
        continue
    fi

    # Buscar la línea en la que comienza el cliente 51 en el archivo
    linea_cliente=$(grep -n "^ *26" "$archivo" | cut -d: -f1)

    if [ -z "$linea_cliente" ]; then
        echo "No se encontró el cliente 26 en el archivo $archivo."
        continue
    fi

    # Eliminar todas las líneas desde el cliente 51 en adelante
    head -n $(($linea_cliente - 1)) "$archivo" > temp.txt

    # Sobrescribir el archivo original con las líneas seleccionadas
    mv temp.txt "$archivo"
    echo "Archivo $archivo recortado correctamente desde el cliente 26 en adelante."
done
