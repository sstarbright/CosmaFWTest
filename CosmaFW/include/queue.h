#ifndef CFW_QUEUE_H
#define CFW_QUEUE_H

#define DEFINE_QUEUE(queueType, queueStruct)\
typedef struct queueStruct queueStruct;\
struct queueStruct {\
queueType* array;\
int front;\
int size;\
int capacity;\
}

#define INIT_QUEUE(queueType, queueName, queueCapacity)\
queueName.capacity = queueCapacity;\
queueName.array = malloc(sizeof(queueType)*queueCapacity);\
queueName.front = 0;\
queueName.size = 0;

#define ENQUEUE(queueName, queueElement)\
if (queueName.size == queueName.capacity) {\
    printf("Queue queueName is full!\n");\
} else {\
    queueName.array[(queueName.front + queueName.size) % queueName.capacity] = queueElement;\
    queueName.size += 1;\
}

#define DEQUEUE(queueName, returnVariable)\
if (queueName.size == 0) {\
    printf("Queue queueName is empty!\n");\
} else {\
    returnVariable = queueName.array[queueName.front];\
    queueName.front = (queueName.front + 1) % queueName.capacity;\
    queueName.size -= 1;\
}

#define RESET_QUEUE(queueName)\
queueName.front = 0;\
queueName.size = 0;

#define RESIZE_QUEUE(queueType, queueName, queueCapacity)\
free(queueName);\
INIT_QUEUE(queueType, queueName, queueCapacity);

#define FREE_QUEUE_ELEMENTS(queueName, iteratorName)\
for (int iteratorName = queueName.front; iteratorName < queueName.size+queueName.front; iteratorName++) {\
    if (queueName.array[iteratorName % queueName.capacity])\
        free(queueName.array[iteratorName % queueName.capacity]);\
}

#endif
