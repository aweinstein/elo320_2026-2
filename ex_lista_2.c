#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int key;
    struct Node *next;
} Node;

typedef struct List {
    Node *head;
} List;

/* Prototipo de funciones */
List *makelist(void);
Node *create_node(int key);
void add(List *list, int key);
void delete(List *list, int key);
void display(List *list);

int main(void)
{
    printf("Lista enlazada simple\n");
    List *a = makelist();
    List *b = makelist();
    printf("Lista vacia\n");
    display(a);
    printf("Lista con un nodo\n");
    add(a, 12);
    display(a);
    printf("Lista con varios nodos\n");
    add(a, 34);
    add(a, 56);
    add(a, 128);
    add(a, 43);
    display(a);
    printf("Eliminamos el nodo con key 128\n");
    delete(a, 128);
    display(a);
    printf("Eliminamos el primer nodo\n");
    delete(a, 12);
    display(a);
    free(a);
//    printf("--------\n");
//    add(b, 123);
//    add(b, 456);
//    add(b, 789);
//    display(b);
    return 0;
}

List *makelist(void)
{
    List *list = malloc(sizeof(List)); // Falta manejar el caso en que malloc retorna NULL
    list->head = NULL;
    return list;
}

Node *create_node(int key)
{
    Node *new_node = malloc(sizeof(Node)); // Falta manejar el caso en que malloc retorna NULL
    new_node->key = key;
    new_node->next = NULL;
    return new_node;
}

// Agrega un nodo al final de la lista
void add(List *list, int key)
{
    Node *current = NULL;
    if(list->head == NULL) // Caso en que la lista esta vacia
        list->head = create_node(key);
    else {
        current = list->head;
        while(current->next != NULL)
            current = current->next;
        current->next = create_node(key);
    }
}

void display(List *list)
{
    Node *current = list->head;
    while(current != NULL) {
        printf("%d \n", current->key);
        current = current->next;
    }
    printf("NULL\n");
}

// Remueve de la lista el nodo en donde key aperece por primera vez
void delete(List *list, int key)
{
    Node *current, *previous;
    current = list->head;
    while(current != NULL)
    {
        if(current->key == key)
        {
            if(current == list->head) // Actualizar head is eliminamos el primer nodo
                list->head = current->next;
            else
                previous->next = current->next;
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}