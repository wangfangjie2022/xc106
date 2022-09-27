#ifndef _QUEUE_H_
#define _QUEUE_H_

//队列元素
typedef struct queue_node
{
    struct queue_node  *next;
    struct queue_node  *pre;
    void*   data;
}queue_node;

//队列定义
typedef struct
{
    queue_node* top;
    queue_node* tail;
    int num;
}Queue;

//队初始化
void    queue_init(Queue* q);

//入队
int    queue_push(Queue* q,void* d);

//访问队首元素
void*    queue_top(Queue* q);

//出队并返回值
void*    queue_pop(Queue* q);

//返回队是否为空
int     queue_empty(Queue* q);

//清空
void    queue_clear(Queue* q);

#endif
