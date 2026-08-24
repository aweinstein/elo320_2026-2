/*
Ejemplo de lectura de un archivo CSV.

El archivo no contiene encabezado. Cada linea del archivo tiene tres campos: dos strings seguidos por un
numero real. El largo maximo de cada linea es de 128 caracteres.

Ejemplo de una linea:

2026-01-13T14:38:16,svc-39,85.14

Los datos del archivo CSV quedan disponibles en un arreglo de estructuras.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *time;
    char *name;
    float x;
} data;

#define LINE_LENGHT 128
int main(void)
{
    FILE *fp;
    char line[LINE_LENGHT];
    data *records = NULL;
    size_t count = 0;
    size_t capacity = 0;

    fp = fopen("ejemplo.csv", "r");
    if (fp == NULL) {
        perror("Error opening file");
        return 1;
    }

    while (fgets(line, sizeof(line), fp)) {
        char t[LINE_LENGHT], n[LINE_LENGHT];
        float x;

        // Intentamos de analizar tres parametros
        if (sscanf(line, "%31[^,],%31[^,],%f", t, n, &x) == 3) {
            // Ajustar el tamanio si es necesario
            if (count >= capacity) {
                capacity = (capacity == 0) ? 10 : capacity * 2;
                data *tmp = realloc(records, capacity * sizeof(data));
                if (tmp == NULL) {
                    perror("Error al asignar memoria");
                    free(records);
                    fclose(fp);
                    return 1;
                }
                records = tmp;
            }
            records[count].time = malloc(strlen(t) + 1);
            strcpy(records[count].time, t);
            records[count].name = malloc(strlen(n) + 1);
            strcpy(records[count].name, n);
            records[count].x = x;
            count++;
        }
    }

    fclose(fp);

    // Imprimimos los primeros y ultimos 10 datos para verificar
    printf("Primeros 10 datos:\n");
    for (size_t i = 0; i < 10; i++) {
        printf("%d time: %s, name: %s, x: %.2f\n", i+1, records[i].time, records[i].name, records[i].x);
    }
    printf("Ultimos 10 datos:\n");
    for (size_t i = count - 10; i < count; i++) {
        printf("%d time: %s, name: %s, x: %.2f\n", i+1, records[i].time, records[i].name, records[i].x);
    }

    free(records);
    return 0;
}