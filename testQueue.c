#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "stack.h"
#include "queue.h"

int main(){

	initStack();

	push(1);
	//printf("%p\n", stackStart);
	push(2);
	push(3);

	int ret = 0;
	do{
		ret = pop();
		printf("%d\n", ret);
	}while(ret != -1);

/*
	initQueue();
	for(ret = 1; ret < 11; ret++){
		assert(insert(ret, ret + ret % 7) != ERR);
		printf("%d %d %d\n", ret, size, next - 1);
	}
	struct qnode *temp;
	temp = popq();
	while(temp != NULL){
		printf("%d %d %d\n", temp->end - temp->start, size, next - 1);
		free(temp);
		temp = popq();
	}	

	destroyQueue();
	*/killStack();
	

	return 0;
}
