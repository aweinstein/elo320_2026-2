#include <stdlib.h>
#include <stdio.h>

typedef struct {
    int read;
    int write;
    int n; // Numero actual de elementos
    int size; // Tamanioo del arreglo
    int *data; // En este ejemplo usamos memoria dinamica
} buffer;

buffer *make_buffer(int size);
int get_buffer(buffer *b);
void put_buffer(buffer *b, int a);
int is_empty_buffer(buffer *b);
int is_full_buffer(buffer *b);
void display_buffer(buffer *b);

int main(void)
{
    printf("Buffer circular \n");
    buffer *b = make_buffer(10);

    put_buffer(b, 12); // debiese verificar primero que hay espacio!
    put_buffer(b, 34);
    display_buffer(b);
    return 0;
}

buffer *make_buffer(int size)
{
    buffer *b = malloc(sizeof(buffer));
    b->read = 0;
    b->write = 0;
    b->n = 0;
    b->data = malloc(size * sizeof(int));
    b->size = size;
    return b;
}

// quien llama a la funcion es reponsable de verificar que el buffer tiene datos
int get_buffer(buffer *b)
{
    int d;
    d = b->data[b->read];
    b->read = (b->read + 1) % b->size;
    b->n--;
    return d;
}


// quien llama a la funcion es reponsable de verificar que el buffer tiene espacio libre
void put_buffer(buffer *b, int a)
{
    b->data[b->write] = a;
    b->n++;
    b->write = (b->write + 1) % b->size;
}

int is_empty_buffer(buffer *b)
{
    return (b->n == 0);
}

int is_full_buffer(buffer *b)
{
    return (b->n == b->size);
}


void display_buffer(buffer *b)
{
    int i, cursor;
    cursor = b->read; // Lo copiamos para no sobreescribir read
    for(i=0; i < b->n; i++) {
        printf("%d ", b->data[cursor]);
        cursor = (cursor + 1) % b->size;
    }
    printf("\n");
}