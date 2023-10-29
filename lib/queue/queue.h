#ifndef QUEEUE_H
#define QUEEUE_H

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

struct Queue
{
    int front, rear, size, min, max;
    unsigned capacity;
    int *array;
};

struct Queue *createQueue(unsigned capacity);
int isFull(struct Queue *queue);
int isEmpty(struct Queue *queue);
int dequeue(struct Queue *queue);
void enqueue(struct Queue *queue, int item);
int front(struct Queue *queue);
int rear(struct Queue *queue);

#endif