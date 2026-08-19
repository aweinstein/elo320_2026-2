#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int key;
    struct Node *next;
} Node;

int main(void)
{
    printf("Implementacion basica de lista enlazada simple\n");
    Node *a, *b, *c;
    a = malloc(sizeof(Node));  // En caso practico debemos manejar caso en que malloc retorna NULL
    b = malloc(sizeof(Node));
    c = malloc(sizeof(Node));
    a->key = 12;
    a->next = b;
    b->key = 34;
    b->next = c;
    c->key = 56;
    c->next = NULL;

    // Recorramos la lista en orden
    Node *current = a;  // Magicamente sabemos que la lista parte en el nodo a
    int i = 0;
    while(current != NULL)
    {
        printf("Nodo %d tiene llave %d\n", ++i, current->key);
        current = current->next;
    }

    free(a);
    free(b);
    free(c);
    return 0;
}