#include "queue.h"

struct Queue *createQueue(unsigned capacity)
{
    struct Queue *queue = (struct Queue *)malloc(
        sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;

    queue->rear = capacity - 1;
    queue->array = (int *)malloc(
        queue->capacity * sizeof(int));
    queue->min = INT16_MAX;
    queue->max = INT16_MIN;
    return queue;
}

int isFull(struct Queue *queue)
{
    return (queue->size == queue->capacity);
}

int isEmpty(struct Queue *queue)
{
    return (queue->size == 0);
}

int dequeue(struct Queue *queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

void enqueue(struct Queue *queue, int item)
{
    if (isFull(queue))
    {
        dequeue(queue);
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    for (size_t i = 0; i < queue->size; i++)
    {
        int value = queue->array[i];
        if (queue->min > value)
            queue->min = value;
        if (queue->max < value)
            queue->max = value;
    }
}

int front(struct Queue *queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}

int rear(struct Queue *queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}
