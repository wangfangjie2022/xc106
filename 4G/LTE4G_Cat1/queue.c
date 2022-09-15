#include "queue.h"
#include <string.h>
#include <stdlib.h>

//队初始化
void    queue_init(Queue* q)
{
    memset(q,0,sizeof(Queue));
}

//入队
int    queue_push(Queue* q,void* d)
{
    queue_node* node = (queue_node*)malloc(sizeof(queue_node));
    if(!node)
        return 0;
    q->num++;
    node->data = d;
    if(q->top == NULL)
    {
        q->top = q->tail = node;
        q->top->next = NULL;
        q->top->pre = NULL;
    }
    else
    {
        q->tail->next = node;
        node->pre = q->tail;
        q->tail = node;
        q->tail->next = NULL;
    }
    return 1;
}

//访问队顶元素
void*    queue_top(Queue* q)
{
    if(!queue_empty(q))
        return q->top->data;
    return NULL;
}

//出队并返回值
void*    queue_pop(Queue* q)
{
    queue_node* node = NULL;
    void* data = NULL;
    if(queue_empty(q))
        return NULL;
    node = q->top;
    data = node->data;
    q->top = node->next;
    q->num--;
    free(node);
    return data;
}

//返回队是否为空
int     queue_empty(Queue* q)
{
    if(q->num > 0)
        return 0;
    return 1;
}

//清空
void    queue_clear(Queue* q)
{
    while(!queue_empty(q))
    {
        queue_pop(q);
    }
}
