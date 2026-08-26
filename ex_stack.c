#include <stdio.h>
#include <stdlib.h>
#define MAXSIZE 10

typedef struct {
    int n;  // cuantos elementos hay en el stack en cada momento
    int data[MAXSIZE];
} stack;

/* Prototipo de funciones */
stack *make_stack(void);
void push(stack *sp, int a);
int pop(stack *sp);
void display_stack(stack *sp);

int main(void)
{
    printf("Stack usando arreglos\n");
    stack *s = make_stack();
    push(s, 12);
    push(s, 34);
    push(s, 78);
    display_stack(s);
    printf("Pop: %d\n", pop(s));
    display_stack(s);
    push(s, 773);
    push(s, 42);
    display_stack(s);
    printf("Pop: %d\n", pop(s));
    display_stack(s);
    return 0;
}


stack *make_stack(void)
{
    stack *sp = malloc(sizeof(stack)); // Falta manejar el caso en que malloc retorna NULL
    sp->n = 0;
    return sp;
}

void push(stack *sp, int a)
{
    if(sp->n == MAXSIZE) {
        fputs("ERROR: Stack Overflow\n", stderr);
        exit(1);
    } else
        sp->data[sp->n++] = a;

}

int pop(stack *sp)
{
    if(sp->n == 0) {
        fputs("ERROR: Empty stack\n", stderr);
        exit(1);
    } else {
        return sp->data[--(sp->n)];
    }
}


void display_stack(stack *sp)
{
    printf("----------\n");
    for(int i=sp->n - 1; i >= 0; i--)
        printf("%d\n", sp->data[i]);
    printf("----------\n");
    printf("Inicio stack\n");
}

