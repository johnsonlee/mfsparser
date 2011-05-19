#include <stddef.h>
#include <stdlib.h>

#ifndef __STACK_H__
#define __STACK_H__

typedef struct _Stack Stack;

struct _Stack
{
	void **elements;
	unsigned int capacity;
	unsigned int size;
};

#ifdef __cplusplus
extern "C" {
#endif

extern Stack* stack_new();

extern Stack* stack_new_with_capacity(unsigned int capacity);

extern int stack_is_empty(Stack *stack);

extern void* stack_peek(Stack *stack);

extern int stack_push(Stack *stack, void *data);

extern void* stack_pop(Stack *stack);

extern void stack_free(Stack **stack, void (*cleanup)(void*));

#ifdef __cplusplus
}
#endif

#endif /* __STACK_H__ */
