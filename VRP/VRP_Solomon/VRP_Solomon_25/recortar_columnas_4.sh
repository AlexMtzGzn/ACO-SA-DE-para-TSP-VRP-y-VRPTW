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

    # Crear un archivo temporal
    temp_file=$(mktemp)

    # Procesar el archivo línea por línea
    while IFS= read -r line; do
        # Verificar si es una línea con "DEMAND", "READY TIME", "DUE DATE", "SERVICE" en el encabezado
        if [[ $line == *"DEMAND"* && $line == *"READY TIME"* && $line == *"DUE DATE"* && $line == *"SERVICE"* ]]; then
            # Eliminar esas columnas del encabezado
            header=$(echo "$line" | sed -E 's/DEMAND\s+READY TIME\s+DUE DATE\s+SERVICE\s+TIME\s*//g')
            echo "$header" >> "$temp_file"
        # Verificar si es una línea numérica de datos (busca patrones de números)
        elif [[ $line =~ [0-9]+[[:space:]]+[0-9]+[[:space:]]+[0-9]+ ]]; then
            # Extraer solo los primeros 3 campos (CUST NO, XCOORD, YCOORD)
            customer=$(echo "$line" | awk '{printf "%-10s %-10s %s", $1, $2, $3}')
            echo "$customer" >> "$temp_file"
        else
            # Mantener líneas que no son datos o encabezados (como títulos o espacios)
            echo "$line" >> "$temp_file"
        fi
    done < "$archivo"

    # Reemplazar el archivo original con el temporal
    mv "$temp_file" "$archivo"

    echo "Archivo $archivo recortado correctamente (eliminadas las columnas de DEMAND, READY TIME, DUE DATE y SERVICE TIME)."
done