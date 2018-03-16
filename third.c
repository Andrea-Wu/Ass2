#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct _node{
	int data;
	struct _node *  next;
} node;



int main(int argc, char* argv[]){
	node** hashTable = (node**)malloc(sizeof(node) * 10000); //hashtable is the pointer to the first node...
	
	FILE* fp = fopen("file2.txt", "r");

	while(!feof(fp)){
		char c; 
		fscanf(fp, "%c", &c);

		if(feof(fp)){//there's probably a better way to do this
			break;
		}

		int d;
		fscanf(fp, "%d\n", &d);

		
		if(c == 'i'){//insert
			insert(hashTable, d);
		}else{
			search(hashTable, d);
		}


	}

	


}

node* create(int d){
	node* newNode = (node*)malloc(sizeof(node));
	(*newNode).data = d;
	(*newNode).next = NULL;

	return newNode;
}

int  insert(node** hashTable, int d){ //returns 1 if successful, 0 if not 
		//passing in node** because its a pointer to an array of pointers.
	int bucket = d % 10000;


	node* newNode = create(d); //gets a node pointer. 

	node* head = *(hashTable); //gets a node pointer

	if(head == NULL){
		*(hashTable +  bucket) = newNode; //hashtable is a pointer of pointers. dereference ht -> a pointer. add bucket -> should get node that u want
		printf("inserted\n");
		return 1;
	}//if head is a null pointer, redirect it to the new ptr

	//search
	node* temp;
	while(head != NULL){ //node* = nll
		int n = (*head).data;
		if(n == d){
			printf("duplicate\n");
			return 0;
		}
	
		temp = head;	
		head = (*head).next;
	}
	
	//new node should be inserted between temp and head. head is null. essentially, just insert after temp.
	(*temp).next = newNode;
	printf("inserted\n");
	return 1;
	
}

int search(node ** hashTable, int d){  //returns 0 if not found, 1 if found
	 //passing in node** because its a pointer to an array of pointers.
	
	int bucket = d % 10000;
	node* temp = *(hashTable + bucket);//get head of whatever is in bucket

	if(temp == NULL){	
		printf("absent\n");
		return 0;
	}
	
	while(temp != NULL){
		int n =  (*temp).data; //get the value inside temp node 
		if(n == d){
			printf("present\n");
			return 1; 
		}
		
		temp = (*temp).next;
	}
	return 0;
	
}
