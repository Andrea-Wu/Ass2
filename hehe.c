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
void printHLL(hNode* head);
void printLL(node* head);
void insertKeyWord(hNode** hashTable, hNode* insertMe);
int hashFunction(char* str);
hNode* mergeSortKw(hNode* head);
node* mergeSortRecords(node* head);

int main(int argc, char* argv[]){
	

	//hNode** hashTable = (hNode**)malloc(sizeof(hNode*) * 1000);
	
/* error test for mergeSortRecords()
	node* first = (node*)malloc(sizeof(node));
	 node* second = (node*)malloc(sizeof(node));
	 node* third = (node*)malloc(sizeof(node));
	 node* fourth = (node*)malloc(sizeof(node));
	 node* fifth = (node*)malloc(sizeof(node));

	first -> count = 100;
	second -> count = 100;
	third -> count = 100;
	fourth -> count = 50;
	fifth -> count = 50;

	first -> str = "and";
	second -> str = "she";
	third -> str = "win";
	fourth -> str = "fan";
	fifth -> str = "sid";

	first -> next = second;
	second -> next = third;
	third -> next = fourth;
	fourth -> next = fifth;
	fifth -> next = NULL;

	node* hed = mergeSortRecords(first);
	printLL(hed);
	*/

	return 0;
}

void insertRecords(hNode** hashTable, node* head, char* fileName){
	//head is an LL with node containing keyword, count, and next.
	//we have to turn each node into a different node, containing filename, count, and next.
	//and then insert that into the sub-LL (I just made up a word)
	//just occurred to me that I'm inserting my nodes at the end of LL but it
	//but it's more efficient to insert in the front. later
	
	while(head != NULL){
		//copy fileName into permanent string
		int fnLen = sizeof(fileName);
		char* fn = (char*)malloc((fnLen + 1) * sizeof(char));
		strcpy(fn, fileName);


		//create new node to insert into sub-LL		
		node* newNode = createNode(fn, head -> count, NULL);

	// the following code inserts into sub-LL
		char* currWord = head -> str;
		int bucket = hashFunction(currWord);

			//stands for "keyword linked-list head"
		hNode* kwLLHead = hashTable[bucket];

			//gets hNode corresponding with current keyword
			//this is linear search...inefficient
		while(strcmp(kwLLHead -> keyWord, currWord) != 0 && kwLLHead != NULL){
			kwLLHead = kwLLHead -> next;
		}
	
		//word doesn't exist as a kw in hash table
		if(kwLLHead == NULL){
			hNode* kwNode = (hNode*)malloc(sizeof(hNode));

			//the list will only have 1 file, the one u currently parsed
			kwNode -> fileList = newNode;
			kwNode -> keyWord = currWord;

			//insert at front of linked list
			hNode* temp = hashTable[bucket];
			hashTable[bucket] = kwNode;
			kwNode -> next = temp;
		}
		//word exists as a kw
		else{
			//insert node at the front of sub-LL
			node* temp = kwLLHead -> fileList;
			kwLLHead -> fileList = newNode;
			newNode -> next = temp; 
		}
	
		head = head -> next;
	}
}

//inserts a single hNode into the hashTable
//this function is no longer necessary
void insertKeyWord(hNode** hashTable, hNode* insertMe){
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
		printf("str is %s, count is %d\n",temp -> str ,temp -> count); 
		temp = temp -> next;
	}
}


void printHLL(hNode* head){ //for testing only
        hNode* temp = head;
        while(temp != NULL){
                printf("str is %s\n", temp -> keyWord);
                temp = temp -> next;
        }
}


//takes in hNode head
//returns sorted hNode head
hNode* mergeSortKw(hNode* head){
	
	//base case
	if(head == NULL || head -> next == NULL){
		return head;
	}

	//split LL by iterating thru list with 2 ptrs, 
	//ptr2 2x faster than ptr1
	//temp trails behind ptr1 so we can properly split the LLs
		//by setting temp -> next = NULL
	hNode* temp;
	hNode* ptr1 = head;
	hNode* ptr2 = head;
	while(ptr2 != NULL && ptr2-> next != NULL){
		temp = ptr1;
		ptr1 = ptr1 -> next;
		ptr2 = ptr2 -> next -> next;
	}

	//detatch the 2 LLs
	temp -> next = NULL;

	//if odd # of nodes, the right side LL will have 1 extra node
	//head points to left LL, ptr1 points to right LL
	
	//recursively merge sort each side.
	ptr1 = mergeSortKw(ptr1);
	head = mergeSortKw(head);


	//merge ptr1 and head together
	hNode* newHead = NULL;
	hNode* newTemp;
	while(head != NULL && ptr1 != NULL){ 
		char* lString = head -> keyWord;
		char* rString = ptr1 -> keyWord;

		int cmp = strcmp(lString, rString);
		if(cmp < 0){ 
			//lString less than rString 
			//aka lString comes before rString in oxford dictionary
			if(newHead == NULL){
				newHead = head;
				newTemp = newHead;
				head = head -> next;
				newHead -> next = NULL;	
			}else{
				newTemp -> next = head;
				head = head -> next;
				newTemp = newTemp -> next;
				newTemp -> next = NULL;
			}
		}else if(cmp > 0){ //rString comes before lString in dict. 
			if(newHead == NULL){
				newHead = ptr1;
				newTemp = newHead;
				ptr1 = ptr1 -> next;
				newHead -> next = NULL;
			}else{
				newTemp -> next = ptr1;
				ptr1 = ptr1 -> next;
				newTemp = newTemp -> next;
				newTemp -> next = NULL;
			}
		} 

	}

	//append leftovers to end of list
	if(head != NULL){
		newTemp -> next = head;
	}else if(ptr1 != NULL){
		newTemp -> next = ptr1;
	}

	return newHead;	
}

//copied code from prev mergesort but with modifications
node* mergeSortRecords(node* head){ 
	//base case
	if(head == NULL || head -> next == NULL){
                return head;
        }
	//split LL by iterating thru list with 2 ptrs,
	//ptr2 2x faster than ptr1
	//temp trails behind ptr1 so we can properly split the LLs
	//by setting temp -> next = NULL
                             
	node* temp;
        node* ptr1 = head;
        node* ptr2 = head;
        while(ptr2 != NULL && ptr2 -> next != NULL){
                temp = ptr1;
                ptr1 = ptr1 -> next;
                ptr2 = ptr2 -> next -> next;
        }

	//detatch the 2 LLs
	temp -> next = NULL;
	         
	//if odd # of nodes, the right side LL will have 1 extra node
	//head points to left LL, ptr1 points to right LL
	
	//recursively merge sort each side.
	ptr1 = mergeSortRecords(ptr1);
        head = mergeSortRecords(head);

	//merge ptr1 and head together
	node* newHead = NULL;
        node* newTemp;
        while(head != NULL && ptr1 != NULL){
                int lFreq = head -> count;
                int rFreq = ptr1 -> count;

                if(lFreq > rFreq){
                       if(newHead == NULL){
                                newHead = head;
                                newTemp = newHead;
                                head = head -> next;
                                newHead -> next = NULL;
                        }else{
                                newTemp -> next = head;
                                head = head -> next;
                                newTemp = newTemp -> next;
                                newTemp -> next = NULL;
                        }
                }else if(rFreq > lFreq){
	                if(newHead == NULL){
                                newHead = ptr1;
                                newTemp = newHead;
                                ptr1 = ptr1 -> next;
                                newHead -> next = NULL;
                        }else{
                                newTemp -> next = ptr1;
                                ptr1 = ptr1 -> next;
                                newTemp = newTemp -> next;
                                newTemp -> next = NULL;
                        }
                }else if(rFreq == lFreq){ //both equal, then sort by fileName
			char* lString = head -> str;
			char* rString = ptr1 -> str;
			
			int cmp = strcmp(lString, rString);
			if(cmp < 0){ //iterate head
				if(newHead == NULL){
                               		newHead = head;
                                	newTemp = newHead;
                                	head = head -> next;
                                	newHead -> next = NULL;
                        	}else{
                                	newTemp -> next = head;
                                	head = head -> next;
                                	newTemp = newTemp -> next;
                                	newTemp -> next = NULL;
                        	}
			}else if(cmp > 0){ //iterate ptr1
				if(newHead == NULL){
                                	newHead = ptr1;
                                	newTemp = newHead;
                                	ptr1 = ptr1 -> next;
                                	newHead -> next = NULL;
                        	}else{
                                	newTemp -> next = ptr1;
                                	ptr1 = ptr1 -> next;
                                	newTemp = newTemp -> next;
                                	newTemp -> next = NULL;
                        	}		
			}else if(cmp == 0){
				printf("this shouldn't happen\n");
			}			
		}
        }

	//append leftovers
 	if(head != NULL){
                newTemp -> next = head;
        }else if(ptr1 != NULL){
                newTemp -> next = ptr1;
        }

        return newHead;

}




