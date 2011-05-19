#include <string.h>
#include <stack.h>
#include <log.h>

#define DEFAULT_STACK_CAPACITY 10

static void ensure_capacity(Stack *stack)
{
	if (stack->capacity <= stack->size) {
		void **buf = (void**) malloc(sizeof(void*) * stack->capacity * 2);

		memcpy(buf, stack->elements, sizeof(void*) * stack->size);
		free(stack->elements);

		stack->elements = buf;
		stack->capacity *= 2;
	}
}

Stack* stack_new()
{
	return stack_new_with_capacity(DEFAULT_STACK_CAPACITY);
}

Stack* stack_new_with_capacity(unsigned int capacity)
{
	Stack *stack = (Stack*) malloc(sizeof(Stack));

	stack->elements = (void**) malloc(sizeof(void*) * capacity);
	stack->capacity = capacity;
	stack->size = 0;

	return stack;
}

int stack_is_empty(Stack *stack)
{
	return NULL != stack && stack->size <= 0;
}

void* stack_peek(Stack *stack)
{
	if (stack_is_empty(stack))
		return NULL;

	return stack->elements[stack->size - 1];
}

int stack_push(Stack *stack, void *data)
{
	ensure_capacity(stack);
	stack->elements[stack->size++] = data;

	return 0;
}

void* stack_pop(Stack *stack)
{
	if (stack_is_empty(stack))
		return NULL;

	return stack->elements[--stack->size];
}

void stack_free(Stack **stack, void (*cleanup)(void*))
{
	while (!stack_is_empty(*stack)) {
		void *data = stack_pop(*stack);
		if (cleanup) {
			cleanup(data);
		}
	}

	free((*stack)->elements);
	free(*stack);

	*stack = NULL;
}
