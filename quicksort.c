#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "queue.h"
#include "stack.h"
#include "msgLib.h"


int listSize;
int *list;

int* getList(char *fileName){
	FILE *file = fopen(fileName, "r");
	int size = 0;
	char temp;
	while(!feof(file)){
		temp = fgetc(file);
		if(temp == '\n') {size++; listSize++;}
	}
	int *list = (int*)malloc(sizeof(int) * size);
	fseek(file, 0, SEEK_SET);
	
	int counter = 0;
	while(size-- > 0){
		fscanf(file, "%d", &list[counter++]);
	}
	fclose(file);
	return list;
}

void writeList(char *fileName, int *list){
	int counter = 0;
	FILE *file = fopen(fileName, "w");
	while(counter < listSize){
		fprintf(file, "%d\n", list[counter]);
		counter++;
	}
}

int partition(int* A, int p, int r){
	//int z = (rand() % (r - p + 1)) + p;
	//printf("z is %d\n", z);
	int x = A[r];
	int i = p - 1;
	int j = -1;
	int temp = 0;
	for(j = p; j < r; j++){
		if(A[j] <= x){
			i += 1;
			temp = A[i];
			A[i] = A[j];
			A[j] = temp;
		}
	}

	i += 1;
	temp = A[i];
	A[i] = A[r];
	A[r] = temp;

	return i;
}

void master(int msize, char *input, char *output){

	//get array access to structs using pointers
	struct workRet wr;
	int *wrv = &wr;

	struct qnode w;
	int *wv = &w;

	MPI_Status status;

	listSize = 0;
	initQueue();
	initStack();

	list = getList(input);
	insert(0, listSize - 1); //printf("pushing %d %d\n", 0, listSize-1);

	int counter = 1;
	int cur = -1;
	int messageSize = -1;
	for(; counter < msize; counter++){
		push(counter);
	}

	do{	
		if(stackSize == msize - 1 && next == 0) {printf("breaking\n");break;}

		if(stackSize == 0 || next == 0){ //can't send work
			//printf("receiving\n");
			MPI_Recv(wrv, 4, MPI_INT, MPI_ANY_SOURCE, paramTag, MPI_COMM_WORLD, &status);
		//printf("received from %d %d %d %d\n", wr.id, wr.start, wr.q, wr.end);
			messageSize = (wr.end - wr.start) + 1;
			MPI_Recv(&list[wr.start], messageSize, MPI_INT, wr.id, dataTag, MPI_COMM_WORLD, &status);
		//	printf("q received is: %d\n" , wr.q);
			if(wr.q + 1 < wr.end){ //if there is more work, add it to the queue
				insert(wr.q + 1, wr.end);
			} 
			if(wr.start < wr.q - 1){
				insert(wr.start, wr.q - 1);
			}
		//	printf("pushing %d\n", wr.id);
			push(wr.id); //in any case, this node is now available		

		} else if(stackSize != 0 && next != 0){ //can send work

				cur = pop();
				w = popq(); //printf("popped %d %d for %d\n", w.start, w.end, cur);
			//	printf("new work: %d %d\n", w.start, w.end);
				MPI_Send(wv, 2, MPI_INT, cur, paramTag, MPI_COMM_WORLD);
				messageSize = (w.end - w.start) + 1;
				MPI_Send(&list[w.start], messageSize, MPI_INT, cur, dataTag, MPI_COMM_WORLD);
	//printf("send to %d the following range: %d %d\n", cur, w.start, w.end);

		}//printf("stack size: %d msize: %d queuSize: %d\n", stackSize, msize, next);
	} while(1);

	//kill them all!!
	w.start = -1;
	w.end = 0;
	for(counter = 1; counter < msize; counter++){
		MPI_Send(wv, 2, MPI_INT, counter, paramTag, MPI_COMM_WORLD);
	}

	//finished
	writeList(output, list);
	free(list);
	
	destroyQueue();
	killStack();

}


void slave(int rank){
	
	//get array access to structs using pointers
	struct workRet wr;
	wr.id = rank;
	int *wrv = &wr;

//printf("%d\n", wrv[3]);

	struct qnode w;
	int *wv = &w;

	MPI_Status status;
	int messageSize = 0;
	int *data;

	do{
		MPI_Recv(wv, 2, MPI_INT, 0, paramTag, MPI_COMM_WORLD, &status);
	//	printf(" start %d/%d end/%d %d\n", wv[0], w.start, wv[1], w.end);
		
		if(w.start < 0){
			break;
		}

		messageSize = (w.end - w.start) + 1;
		data = malloc(sizeof(int) * messageSize);
		MPI_Recv(data, messageSize, MPI_INT, 0, dataTag, MPI_COMM_WORLD, &status);

		wr.start = w.start;
		wr.end = w.end;		
		wr.q = partition(data, 0, messageSize - 1) + w.start;
		//printf("q is %d\n", wr.q);

		MPI_Send(wrv, 4, MPI_INT, 0, paramTag, MPI_COMM_WORLD);
		MPI_Send(data, messageSize, MPI_INT, 0, dataTag, MPI_COMM_WORLD);

		free(data);
	} while(1);
}


/*
command line args: infile, outfile
*/
int main(int argc, char** argv){

	int msize;
	int rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &msize);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//printf("msize is: %d\n", msize);
	if(rank == 0){
		master(msize, argv[1],argv[2]);
	} else {
		slave(rank);
	}

	MPI_Finalize();

	return 0;
}
