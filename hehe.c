#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


//indecisive about whether to make this 1 or 2 different types of node
//cuz technically they're the same structure
//although considering the extremely fucked up way i implemented this, 
//i'll probably need like 3 different kinds of node
typedef struct node{
	char* str;
	int count;
	struct node* next;
} node;


//stands for "hash node"
typedef struct hNode{
	node* fileList;
	char* keyWord;
	struct hNode*  next;
} hNode;
	

int tokenize(char* buf, int bufLen, char** wordArr, int wordArrLen);
void clearBuffer(char* str, int len);
node* createNode(char* str, int count, node* next);
void sort(char** allStrings, int wordCount);
node* removeDuplicates(char** allWords, int len, char* fileName);
void printLL(node* head);
void insertHash(hNode** hashTable, hNode* insertMe);
int hashFunction(char* str);

int main(int argc, char* argv[]){
	/*
	//testing removeDuplicates()
	char** testArr = (char**)malloc(10 * sizeof(char*));
	
	int i = 0;
	while(i < 10){
		testArr[i] = (char*)malloc(20 * sizeof(char));

		i++;
	}

	for(i = 0; i < 5; i++){
		testArr[i] = "helo";
	}

	for(i = 5; i < 10; i++){
		testArr[i] = "worl";
	}

	node* head = removeDuplicates(testArr, 10, "file.txt");
	printLL(head);

	//end testing removeDuplicates()
	*/

	/*	
	//testing insertHash
	//an array of hNode* 
	hNode** hashTable = (hNode**)malloc(sizeof(hNode*) * 10000);

	hNode* test = (hNode*)malloc(sizeof(hNode));
	test -> fileList = NULL;
	test -> keyWord = (char*)malloc(sizeof(char) * 10);
	test -> keyWord = "assholes";

	int bucket = hashFunction(test -> keyWord);
	printf("bucket is %d\n", bucket);
	test -> next = NULL;
	
	insertHash(hashTable, test);
	printf("%s\n", (*(hashTable + 769)) -> keyWord);
	//end testing insertHash
	*/


	return 0;
}

//inserts a single hNode into the hashTable
void insertHash(hNode** hashTable, hNode* insertMe){
	char* keyWord = insertMe -> keyWord;
	int bucket = hashFunction(keyWord);

	//gets linked list present at hashtable[bucket]
	hNode* head = *(hashTable + bucket);

	//if there's no LL
	if(head == NULL){
		*(hashTable + bucket) = insertMe;
		return;
	}
	//else
	hNode* temp;
	while(head != NULL){
		temp = head;
		head = head -> next;
	}

	//when inserting, just append to end of list...will merge sort later
	temp -> next = insertMe;

}


//this is a shitty & temp hash function
//we will change it later depending on how many buckets we want,
//or if we make the hashmap resizable
int hashFunction(char* str){
	int strLen = strlen(str);

	//smallest word is "a", which is -97
	int bucket = -97;	

	int i = 0;
	while(i < strLen){
		bucket = bucket + str[i];
		i++;
	}

	bucket = bucket % 1000;
	return bucket;
}

void sort(char** allStrings, int wordCount){
	//insertion sort on word list
	//copied from Ass0
	
	int pos, i;
	char* temp;
	for(i = 1; i < wordCount; i++){
		temp = allStrings[i];
		pos = i-1;
		while(pos >= 0 && strcmp(temp, allStrings[pos]) < 0){
			allStrings[pos+1] = allStrings[pos];
			pos--;
		}
		allStrings[pos+1] = temp;
	}
}

node* removeDuplicates(char** allWords, int len, char* fileName){
	//len = length of array
	//assume that the array that's passed in is already sorted
	
	//algorithm: for every position, check how many dupes come after
	//need 2 pointers: 1 for original and 1 to keep track of dupes
	//need 2 counters, 1 to keep track of arr pos and 1 to keep track 
	//of the number of words
	//store counted words & the count in a linked list. 
	//this reminds me of expression eval for some reason
	
	int i = 0;

	node* head = NULL;
	node* tempNode;

	//every unique word gets an iteration of the while loop
	while(i < len){

		//IMPORTANT: this might break something b/c string not malloced?
		char* word = allWords[i];
		int count = 1;
		int temp = i + 1;

		//obviously some arr out of bounds will happen
		//when i = len-1
		while( temp < len && strcmp(word, allWords[temp]) == 0){
			count++;
			temp++;
		}		
		
		//incrementation, basically 
		i = temp;

		//puts nodes in Linked List
		if(head == NULL){
			head = createNode(word, count, NULL);
			tempNode = head;		
		}else{
			node* newNode = createNode(word, count, NULL);
			tempNode -> next = newNode;
			tempNode = tempNode -> next;	
		}
				

	}

	return head;
	
}

node* createNode(char* str, int count, node* next){
	
	//create permanent string
	int strLen = strlen(str);
	char* newStr= (char*)malloc((strLen+ 1) * sizeof(char));
	strcpy(newStr, str);

	node* newNode = (node*)malloc(sizeof(node));
	newNode -> str = newStr;
	newNode -> count = count;
	newNode -> next = next;
	
	return newNode;
	
}

int tokenize(char* buf, int bufLen, char** wordArr, int wordArrLen){ 
	//takes in complete file text, file text length,
	//an array which to place an unsorted list of words, 
	//and the original length of that array. 
	//returns the size of the full array
	
	
	//IMPORTANT: if this ends up seg faulting,
	//it's probably because strcpy() is creating ptrs to strings
	//that are being destroyed when the function is returned 
	//(i'm not sure how strcpy works, maybe this will not happen)
	//test this theory out...later
	
	int wordCount = 0;

	//assume this stuff is already done in another function
	//int wordArrLen = 20;
	//char** wordArr = (char**)malloc(sizeof(char**) * wordArrLen);
	
	int len = 0;
	int tokenBufLen = 20; //number of available spaces in tokenBuf
	char* tokenBuf = (char*)malloc(tokenBufLen* sizeof(char));


	int prevIsLetter = 0; //keep track of repeating delims

	int i;
	for(i = 0; i < bufLen; i++){
	
		//checks if char is letter
		if(isalpha(buf[i])){
			tokenBuf[len] = buf[i];

			//if word len approaches amount of space allocated
			if(len > tokenBufLen -2){
				tokenBufLen = tokenBufLen + 100;
				tokenBuf = (char*)realloc(tokenBuf, sizeof(char) * tokenBufLen);
			}

			prevIsLetter = 1;
			len = len + 1;
		}

		//char is not letter, word complete
		else{
			if(prevIsLetter == 0){
				continue;
			}

			char* newString = (char*)malloc(sizeof(char) * (len + 1));

			//strcpy buffer to permanent location
			newString = strcpy(newString, tokenBuf);
			wordArr[wordCount] = newString;

			//deal with resizing
			if(wordCount > wordArrLen - 2){
				wordArrLen = wordArrLen + 100;
				wordArr = (char**)realloc(wordArr, sizeof(char*) * wordArrLen);
			}

			len = 0;

			clearBuffer(tokenBuf, tokenBufLen);
			prevIsLetter = 0;
			wordCount++;
		}
	}

	if(prevIsLetter == 1){
		char* newString = (char*)malloc(sizeof(char) * (len+1));
		newString = strcpy(newString, tokenBuf);
		wordArr[wordCount] = newString;
		wordCount++;
	}

	return wordCount;
}


void clearBuffer(char* str, int len){
	int i;
	for(i = 0; i < len; i++){
		str[i] = '\0';
	}
}

void printLL(node* head){ //for testing only
	node* temp = head;
	while(temp != NULL){
		printf("str is %s, count is %d\n", temp -> str, temp -> count); 
		temp = temp -> next;
	}
}



