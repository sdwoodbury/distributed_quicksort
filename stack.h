
#ifndef stack_h
#define stack_h

#include <stdlib.h>

struct tnode{
	int id;
	struct wnode *next;
	struct wnode *prev;
};

struct tnode *stackStart;
struct tnode *stackEnd; //doesn't get malloc'd. just points to last item in stack
int stackSize;

void initStack(){
	stackStart = NULL;
	stackEnd = NULL;
	stackSize = 0;
}

void push(int id){
	stackSize++;
	if(stackStart == NULL){
		stackStart = malloc(sizeof(struct tnode));
		
		stackStart->next = NULL;
		stackStart->prev = NULL;
		stackStart->id = id;

		stackEnd = stackStart;

		return;
	}
	struct tnode *q = stackEnd; //a temp variable
	struct tnode *n = malloc(sizeof(struct tnode)); //the new node
	n->id = id;
	n->prev = q;
	n->next = NULL;
	q->next = n;
	stackEnd = n;
}

int pop(){
	if(stackStart == NULL){
		return -1;
	}
	stackSize--;
	int ret = stackStart->id;
	if(stackEnd == stackStart){
		free(stackStart);
		stackStart = NULL;
		stackEnd = NULL;
		//printf("%p\n", stackEnd);
		return ret;
	}
	struct tnode *q = stackStart;
	stackStart = q->next;
	stackStart->prev = NULL;
	free(q);
	//printf("%p\n", stackEnd);
	return ret;
}

void killStack(){
	while(pop() != -1){/*keep popping*/}
}


#endif
