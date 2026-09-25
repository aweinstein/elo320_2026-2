/*
 * Tarea 1 ELO320 2026-1
 * Pregunta 1
 * Lista enlazada de eventos ordenados por severidad
 *
 * Nodo:
 *   - key:         severidad del evento (real entre 0 y 100, 4 decimales)
 *   - timestamp:   fecha y hora del evento, string en formato ISO 8601
 *   - descripcion: descripción del evento, string de máximo 30 caracteres
 *
 * Formato de entrada de ejemplo (campos separados por coma):
 *   2026-03-14T08:12:03,web-03,4.1754
 *   2026-03-14T08:12:05,db-01,38.6037
 *   2026-03-14T08:19:19,net-03,27.4512
 *   2026-03-14T08:23:55,web-01,98.2139
 *   2026-03-14T08:27:15,hd-01,9.1863
 *
 * (a) Lista enlazada simple:
 *   - add_node:          agrega un nodo al inicio de la lista.
 *   - extract_max_node:  retorna el nodo de mayor severidad y lo remueve.
 *
 * (b) Lista enlazada ordenada (de mayor a menor severidad):
 *   - sorted_add_node:          agrega un nodo manteniendo el orden.
 *   - sorted_extract_max_node:  retorna el nodo de mayor severidad y lo remueve.
 *
 * Verificación (para cada tipo de lista):
 *   1. Crear la lista con los datos del ejemplo.
 *   2. Imprimir la lista recorriéndola de principio a fin.
 *   3. Extraer e imprimir el nodo de mayor severidad.
 *   4. Volver a imprimir la lista.
 *
 * Por simplicidad:
 * - No se manejan los casos en que malloc retorna NULL
 * - No se validan los rangos de los datos
 * - Asumimos que los strings cumplen con el largo definido
 * - Usamos la misma estructura List para ambos tipos de lista
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    float key;
    struct Node *next;
    char timestamp[20];
    char desc[31];
} Node;

typedef struct List {
    Node *head;
} List;

/* Prototipo de funciones */
List *makelist(void);
Node *create_node(float key, char *ts, char *ds);
void add_node(List *list, float key, char *ts, char *ds);
Node *extract_max_node(List *list);
void display(List *list);
void sorted_add_node(List *list, float key, char *ts, char *ds);
Node *sorted_extract_max_node(List *list);

int main(void)
{
    printf("ELO320 Tarea 1 2026-2 Pregunta 1\n\n");
    ////////////////////////// (a) ////////////////////////
    printf("a) Lista enlazada simple. Los nodos se agregan al inicio de la lista\n");
    printf("Creamos las lista y la mostramos\n");
    printf("El orden de los nodos debe ser el inverso con el que se crearon\n");
    List *a = makelist();
    add_node(a, 4.1754, "2026-03-14T08:12:03", "web-03");
    add_node(a, 38.6037, "2026-03-14T08:12:05", "db-01");
    add_node(a, 27.4512, "2026-03-14T08:19:19", "net-03");
    add_node(a, 98.2139, "2026-03-14T08:23:55", "web-01");
    add_node(a, 9.1863, "2026-03-14T08:27:15", "hd-01");
    display(a);

    Node *max_node = extract_max_node(a);
    printf("\nNodo con maxima severidad\n");
    printf("%.4f %s %s \n\n", max_node->key, max_node->timestamp, max_node->desc);
    printf("Volvemos a mostrar la lista (el nodo de maxima severidad no debe estar) \n");
    display(a);

    ////////////////////////// (b) ////////////////////////
    printf("\n\nb) Lista enlazada ordenada. La lista esta ordenada de mayor a menor\n");
    printf("Creamos las lista y la mostramos\n");
    printf("La lista debe estar ordenada por severidad\n");
    List *b = makelist();
    sorted_add_node(b, 4.1754, "2026-03-14T08:12:03", "web-03");
    sorted_add_node(b, 38.6037, "2026-03-14T08:12:05", "db-01");
    sorted_add_node(b, 27.4512, "2026-03-14T08:19:19", "net-03");
    sorted_add_node(b, 98.2139, "2026-03-14T08:23:55", "web-01");
    sorted_add_node(b, 9.1863, "2026-03-14T08:27:15", "hd-01");
    display(b);

    max_node = sorted_extract_max_node(b);
    printf("\nNodo con maxima severidad\n");
    printf("%.4f %s %s \n\n", max_node->key, max_node->timestamp, max_node->desc);
    printf("Volvemos a mostrar la lista (el nodo de maxima severidad no debe estar) \n");
    display(b);

    return 0;
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
void add_node(List *list, float key, char *ts, char *ds)
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
Node *extract_max_node(List *list)
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

// Agrega un nodo manteniendo el orden
void sorted_add_node(List *list, float key, char *ts, char *ds)
{
    if(list->head == NULL) // Caso en que la lista esta vacia
        list->head = create_node(key, ts, ds);
    else {
        Node *current = list->head;
        if(key > current->key) {  // agregamos el nodo en la primera posicion
            list->head = create_node(key, ts, ds);
            list->head->next = current;
            return;
        }
        Node *prev = current;
        current = current->next;
        while(current!= NULL) { // ya estamos en el segundo nodo
            if(key > current->key) {
                prev->next = create_node(key, ts, ds);
                prev->next->next = current;
                return;
            }
            prev = current;
            current = current->next;
        }  // llegamos al ultimo nodo, el nodo nuevo es el mas pequeño de la lista
        prev->next = create_node(key, ts, ds);
    }
}

// Retorna el nodo de mayor severidad y lo remueve.
Node *sorted_extract_max_node(List *list)
{
    // como la lista esta ordenada de mayor a menor retornamos y removemos
    // el primer nodo
    if(list->head == NULL) {
        return NULL; // lista vacia
    } else {
        Node * ret_node = list->head;
        list->head = list->head->next;
        return ret_node;
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
