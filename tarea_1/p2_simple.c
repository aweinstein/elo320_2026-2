/*
 * Tarea 1 ELO320 2026-1
 * Pregunta 2 - Implementacion basada en lista enlazada simple
 *
 * Cola de prioridad con funciones:
 *   - queue_add:     agrega un nodo a la cola.
 *   - queue_extract: extrae el nodo de mayor severidad.
 *
 * Por simplicidad:
 * - Se implementan las versiones basadas en lista simple y ordenada en
 *   archivos por separado.
 * - Se mantienen todas las funciones en un archivo
 * - No se manejan los casos en que malloc retorna NULL
 * - No se validan los rangos de los datos
 * - Asumimos que los strings cumplen con el largo definido
 * - Usamos la misma estructura List para ambos tipos de lista
 * - No liberamos la memoria
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Node {
    float key;
    struct Node *next;
    char timestamp[20];
    char desc[31];
} Node;

typedef struct {
    char *time;
    char *name;
    float x;
} Data;

typedef struct List {
    Node *head;
} List;

/* Prototipo de funciones */
List *makelist(void);
Node *create_node(float key, char *ts, char *ds);
void display(List *list);
void queue_add(List *list, float key, char *ts, char *ds);
Node *queue_extract(List *list);
Data *read_csv(char *fn, size_t *n);

int main(void)
{
    printf("ELO320 Tarea 1 2026-2 Pregunta 2\n\n");
    printf("Implementacion basada en lista simple\n");
    printf("Validando con los datos del ejemplo\n");
    List *queue = makelist();
    queue_add(queue, 4.1754, "2026-03-14T08:12:03", "web-03");
    queue_add(queue, 38.6037, "2026-03-14T08:12:05", "db-01");
    queue_add(queue, 27.4512, "2026-03-14T08:19:19", "net-03");
    queue_add(queue, 98.2139, "2026-03-14T08:23:55", "web-01");
    queue_add(queue, 9.1863, "2026-03-14T08:27:15", "hd-01");
    display(queue);
    free(queue);

    // Leyendo el archivo CSV
    size_t n_data = 0;
    size_t n = 128000;
    char fn[30];
    sprintf(fn, "data_%d.csv", n);
    Data *csv_data = read_csv(fn, &n_data);
    printf("%d lineas leidas de %s\n", n_data, fn);
    /* printf("Verifiquemos mirando las primeras lineas\n"); */
    /* for(size_t i=0; i<5; i++) { */
    /*     printf("   %s %s %f\n", csv_data[i].time, csv_data[i].name, csv_data[i].x); */
    /* } */

    ////////////////// Microbenchmarking //////////////////
    const int repeticiones = 10;
    const int iner_rep = 1;
    double times[repeticiones];
    for(size_t r=0; r < repeticiones; r++) {
        clock_t start = clock();
        for(size_t ir=0; ir < iner_rep; ir++) {
            queue = makelist();
            for(size_t i=0; i<n_data; i++)
                queue_add(queue, csv_data[i].x, csv_data[i].time, csv_data[i].name);
            //printf("\nNodos con maxima severidad\n");
            volatile Node *max_node = NULL;
	    size_t n_max = (int)(0.5 * n);
            for(size_t i=0; i<n_max; i++) {
                max_node = queue_extract(queue);
                //printf("%.4f %s %s \n", max_node->key, max_node->timestamp, max_node->desc);
            }
        }
        clock_t end = clock();
        double elapsed = (double)(end - start) / CLOCKS_PER_SEC / iner_rep;
        times[r] = elapsed;
    }

    float mean = 0;
    for(size_t i=0; i < repeticiones; i++) {
        mean += times[i];
        printf("Iteracion %d: %.6f ms\n", i, 1000*times[i]);
    }
    mean /= repeticiones;
    printf("Promedio: %f ms\n", 1000*mean);
    return 0;
}

#define LINE_LENGHT 128
Data *read_csv(char *fn, size_t *n)
{
    FILE *fp;
    char line[LINE_LENGHT];
    Data *records = NULL;
    size_t count = 0;
    size_t capacity = 0;

    fp = fopen(fn, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return NULL;
    }

    while (fgets(line, sizeof(line), fp)) {
        char t[LINE_LENGHT], n[LINE_LENGHT];
        float x;

        // Intentamos de analizar tres parametros
        if (sscanf(line, "%31[^,],%31[^,],%f", t, n, &x) == 3) {
            // Ajustar el tamanio si es necesario
            if (count >= capacity) {
                capacity = (capacity == 0) ? 10 : capacity * 2;
                Data *tmp = realloc(records, capacity * sizeof(Data));
                if (tmp == NULL) {
                    perror("Error al asignar memoria");
                    free(records);
                    fclose(fp);
                    return NULL;
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
    *n = count;
    return records;
}

List *makelist(void)
{
    List *list = malloc(sizeof(List)); // Falta manejar el caso en que malloc retorna NULL
    list->head = NULL;
    return list;
}

Node *create_node(float key, char *ts, char *ds)
{
    Node *new_node = malloc(sizeof(Node)); // Falta manejar el caso en que malloc retorna NULL
    new_node->key = key;
    new_node->next = NULL;
    strcpy(new_node->timestamp, ts);
    strcpy(new_node->desc, ds);
    return new_node;
}

// Agrega un nodo al inicio de la lista
void queue_add(List *list, float key, char *ts, char *ds)
{
    if(list->head == NULL) // Caso en que la lista esta vacia
        list->head = create_node(key, ts, ds);
    else {
        Node *tmp = list->head;
        list->head = create_node(key, ts, ds);
        list->head->next = tmp;
    }
}

// Retorna el nodo de mayor severidad y lo remueve.
Node *queue_extract(List *list)
{
    // primero encontramos el valor maximo
    float max = 0; // Sabemos que 0 es el valor minimo de severidad
    Node *current = list->head;
    while(current != NULL) {
        if(current->key > max) {
            max = current->key;
        }
        current = current->next;
    }

    // luego eliminamos ese nodo
    Node *previous;
    current = list->head;
    while(current != NULL)
    {
        if(current->key == max)
        {
            if(current == list->head) // Actualizar head si eliminamos el primer nodo
                list->head = current->next;
            else
                previous->next = current->next;
            return current;
        }
        previous = current;
        current = current->next;
    }
}
void display(List *list)
{
    printf(">>>>>>>>\n");
    Node *current = list->head;
    while(current != NULL) {
        printf("%10.4f %s %s \n", current->key, current->timestamp, current->desc);
        current = current->next;
    }
    printf("<<<<<<<<\n");
}
