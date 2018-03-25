#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include "invertedIndex.h"


int main(int argc, char* argv[]){
	//an array of hash nodes
	
	if(argv[1] == NULL || argv[2] == NULL){
		printf("Error: invalid number of arguments\n");
		return 1;
	}

	if(argc > 3){
		printf("Warning: Extra argument will not be processed. Continuing.\n");
	}

	//checks if argv[1] is a directory
	DIR* dirp = opendir(argv[1]);
	if(dirp){
		printf("Error: First argument cannot be a directory\n");
		return 1;
	} 

	//checks if argv[1] already exists as a file, gives user option to overwrite or not 
	FILE* fp = fopen(argv[1], "r");
	if(fp){
		printf("Warning: The file you are writing to already exixts. Do you wish to overwrite? [y/n]\n");

		//allow user to be 99 extra characters of stupid
		//if it breaks b/c the user doesn't know how to enter y/n, it is their fault
		char* c = (char*)malloc(sizeof(char) * 100); //freed
		int again = 0;
		do{
			if(again){
				printf("Type 'y' or 'n'\n");
			}
			scanf("%s", c);
			again = 1;
		}while(strcmp("y", c) != 0 && strcmp("n", c) != 0);

		if(strcmp("n", c) == 0){
			return 1;
		}

		free(c);
	}
		


	kNode** hashTable = (kNode**)malloc(sizeof(kNode*) * 1000); //freed

	traverseDir(hashTable, argv[2]);

	//all of the required data is now in the hash table
	//time to print everything out!
		
	//remove all of the nodes from the hash table and turn into a LL
	kNode* head = NULL;
	kNode* temp;
	int i = 0;
	while(i < 1000){
		if(hashTable[i] != NULL){

			//concat to LL
			if(head == NULL){
				head = hashTable[i];
				temp = head;
			}else{
				temp -> next = hashTable[i];
			}
			//cmon what's the corner case
			if(temp == NULL){
				printf("whoops, corner case...should not happen");
				break;
			}

			//iterate until temp reaches end, so we can concat to the end next time
			while(temp -> next != NULL){
				temp = temp -> next;
			}
			hashTable[i] = NULL;	
		}
		i++;
	}

	head = mergeSortKw(head); 

	//now all the keywords are in the correct order
	//for every keyword, merge sort its LL 
	

	FILE* writeFp = fopen(argv[1], "w");

	char xml[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	fwrite(xml, sizeof(char), strlen(xml), writeFp);

	char fileIndex[] = "<fileIndex>\n";
	fwrite(fileIndex, sizeof(char), strlen(fileIndex), writeFp);
	
	//FOR FREE
	kNode* freeLaterHead = head;

	while(head != NULL){
		char* kw = head -> keyWord;

		//write the xml thing to a file
		//final line of text will be stored in openWordText
		
		fprintf(writeFp, "\t<word text=\"%s\">\n", kw);
		
		node* fileList = head -> fileList;

		//FOR FREE
		//node* fileListHead = fileList;
	
		//assume sorted already
		//fileList = mergeSortRecords(fileList);
		
		//print fileList as XML to file
		while(fileList != NULL){
			char* str = fileList -> str;

			int count = fileList -> count;

			//assume the number won't be more than 20 digits...
			//...10 quintillion

			char countStr[20];
			sprintf(countStr, "%d", count);

			fprintf(writeFp, "\t\t<file name=\"%s\">%s</file>\n", str,countStr);
			
			fileList = fileList -> next;
		}

		char* word = "\t</word>\n";
		fwrite(word, sizeof(char), strlen(word), writeFp);
		
		head = head -> next;
	}

	char closeFI[] = "</fileIndex>\n";
	fwrite(closeFI, sizeof(char), strlen(closeFI), writeFp);

	freeKNodes(freeLaterHead);	
	
	return 0;
}

void traverseDir(kNode** hashTable, char* path){
	DIR* dirp;
	struct dirent* dp;
	char child[PATH_MAX]; 
	if(!(dirp = opendir(path))){ //dirp cannot be opened, path is file or not valid name
	
		FILE* fp = fopen(path, "r");
		if(fp){ //is valid file
	
			//close for now
			fclose(fp);
			
			char** wordArr = (char**)malloc(sizeof(char*) * 100); //freed
			int numWords = 0;

			//this function will also change the value of numWords.
			wordArr = tokenize(path, wordArr, (int*)&numWords);
			sort(wordArr, numWords);
			node* head = removeDuplicates(wordArr, numWords);	
			insertRecords(hashTable, head, path);

			//free the LL malloc'd in removeDuplicates()
			freeNodes(head);

			//frees wordArr & strings in array
			int i;
			for(i = 0; i < numWords; i++){
				free(wordArr[i]);
			}
			free(wordArr);

		}else{ //invalid file name
			printf("error: invalid file name\n");
			exit(1);			
		}
		
		return;

	}else{ //is directory
		
		while((dp = readdir(dirp)) != NULL){

 			//concat path & subdir name
 			//note that this//path evaluates to this/path
                        snprintf(child, PATH_MAX, "%s/%s", path, dp -> d_name);
		
                                  
			if(dp -> d_type == DT_REG){ //is regular file
				//create array pointer and tokenize into array
				//the tokenize() function does the mallocing
				char** wordArr = (char**)malloc(sizeof(char*) * 100); //freed

				//pass by pointer? hopefully this works
				int numWords = 0;
				wordArr = tokenize(child, wordArr, (int*)&numWords);
				
				//should have put an unsorted array into wordArr
				//sort.
				sort(wordArr, numWords);

				//sorted list has duplicates. remove. turn into linked list.
				node* head = removeDuplicates(wordArr, numWords);

				//insert each node into the master linked list
				insertRecords(hashTable, head, dp -> d_name);

				//frees nodes created in removeDuplicates()
				freeNodes(head);	

				//frees wordArr & strings in array
				int j;
				for(j = 0; j < numWords; j++){
					free(wordArr[j]);
				}
				free(wordArr);
				
			}else if(dp -> d_type == DT_DIR){ //this is dir
				
				//recurse on subdirectory, except . and ..
				if(strcmp(dp -> d_name, ".") != 0 && strcmp(dp ->d_name, "..") != 0){
					traverseDir(hashTable,child);
				}
			}else{
				printf("this should not happen");
				break;
			}
		}
	}

	closedir(dirp);
}

void insertRecords(kNode** hashTable, node* head, char* fileName){
	//head is an LL with node containing keyword, count, and next.
	//we have to turn each node from head into a different node, containing filename, count, and next.
	//and then insert that into the sub-LL (I just made up a word)
	
	while(head != NULL){
		//copy fileName into permanent string
		int fnLen = strlen(fileName);
		char* fn = (char*)malloc((fnLen + 1) * sizeof(char)); //freed
		strcpy(fn, fileName);

		//create new node to insert into sub-LL		
		node* newNode = createNode(fn, head -> count, NULL);

		// the following code inserts into sub-LL
		char* currWord = head -> str;
		int bucket = hashFunction(currWord);

			//stands for "keyword linked-list head"
		kNode* kwLLHead = hashTable[bucket];

			//gets kNode corresponding with current keyword
			//this is linear search...inefficient
		while( kwLLHead != NULL && strcmp(kwLLHead -> keyWord, currWord) != 0){
			kwLLHead = kwLLHead -> next;
		}
	
		//word doesn't exist as a kw in hash table
		if(kwLLHead == NULL){
			kNode* kwNode = (kNode*)malloc(sizeof(kNode)); //freed

			//the list will only have 1 file, the one currently being parsed
			kwNode -> fileList = newNode;

			char* ass = (char*)malloc(sizeof(char) * (strlen(currWord) + 1));

			strcpy(ass, currWord);
			kwNode -> keyWord = ass;

			//insert at front of linked list
			kNode* temp = hashTable[bucket];
			hashTable[bucket] = kwNode;
			kwNode -> next = temp;
		}
		//word exists as a kw
		else{
			//if word already exists in different file with same name
			//then just increment count. 
			//iterate thru list to find out!!
			
			node* recordListHead = kwLLHead -> fileList;
			node* recordList= kwLLHead -> fileList;//iterator

			while(recordList != NULL && strcmp(recordList -> str, fileName) != 0){
				recordList = recordList -> next;
			}
			if(recordList == NULL){ //filename doesn't exist in record list
					//add to front of list
				kwLLHead -> fileList = newNode;
				newNode -> next = recordListHead;
			}
			else{//filename exists in record List, where recordList is desired node 
				//increment count. 
				int addMe = newNode -> count;
				recordList -> count = (recordList -> count) + addMe;
			}

			
			
		}
	
		head = head -> next;
	}

}

int hashFunction(char* str){
	int strLen = strlen(str);

	int bucket = 0;	

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

node* removeDuplicates(char** allWords, int len){
	//len = length of array
	//assume that the array that's passed in is already sorted
	
	//algorithm: for every position, check how many duplicates come after
	//need 2 pointers: 1 for original and 1 to keep track of dupes
	//need 2 counters, 1 to keep track of arr pos and 1 to keep track 
	//of the number of words
	//store counted words & the count in a linked list. 
	
	int i = 0;

	node* head = NULL;
	node* tempNode;

	//every unique word gets an iteration of the while loop
	while(i < len){

		
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

char** tokenize(char* fileName, char** wordArray, int* wordCountPtr){ 
	//takes in complete file text, file text length,
	//an array which to place an unsorted list of words, 
	//and the original length of that array. 
	//returns the size of the full array
	
	int fp = open(fileName, O_RDONLY);
	
	//array already declared
        //these variables are for putting separated strings into unsorted array
        int wordCount = *wordCountPtr;  
        //initially allow for 100 words before resizing
        //this array is declared in the function that calls it
        int wordArrLen = 100; 
    
	char* megaBuff = (char*)malloc(sizeof(char*) * 3); //freed
    
        //string buffer to read individual words
        int len = 0; //current word length
        char* buffer = (char*)malloc(20 * sizeof(char));  //freed
        int buffLength = 20; 

        //keep track of repeating deliminators
        int prevIsLetter = 0;
    
        while(read(fp,megaBuff,sizeof(char))){                                                  
                //checks if character is a letter
                if(isalpha(megaBuff[0]) || isdigit(megaBuff[0])){

			if(isalpha(megaBuff[0])){
				megaBuff[0] = tolower(megaBuff[0]);
			}
							
                        buffer[len] = megaBuff[0];   					

                        //if word length approaches the amount of space allocated
                        if(len > buffLength - 3){ 
                                buffLength = buffLength + 100;
                                buffer  = (char*)realloc(buffer, sizeof(char) * buffLength);
                        }
    
                        prevIsLetter = 1;
                        len = len+1;
                }
    
                //char is not letter, word complete.
                else{
                        if(prevIsLetter == 0){ 
                                continue;
                        }
    

                        char* newString = (char*)malloc(sizeof(char) * (len+1));   //freed

                        //strcpy buffer  to a permanent location
                        newString = strcpy(newString, buffer);
    
                        wordArray[wordCount] = newString;

                        //deal with resizing problems 
                        if(wordCount > wordArrLen - 3){
                                wordArrLen = wordArrLen + 100;
                                wordArray = (char**)realloc(wordArray, sizeof(char*) * wordArrLen);
                        }

                        len = 0;

                        clearBuffer(buffer, buffLength);

                        prevIsLetter = 0;

                        wordCount++;
                }

        }

        //non-alpha character usually triggers putting word into array
        //but last word might not end with delim.
        if(prevIsLetter == 1){
                char* newString = (char*)malloc(sizeof(char) * (len+1)); //freed
                newString = strcpy(newString,buffer);
                wordArray[wordCount] = newString;
                wordCount++;
        }

		
	//change value of wordCount, which is passed in by pointer
	*(wordCountPtr) = wordCount;

	free(buffer);
	free(megaBuff);
	return wordArray;

}


void clearBuffer(char* str, int len){
	int i;
	for(i = 0; i < len; i++){
		str[i] = '\0';
	}
}


//takes in kNode head of Linked List
//returns pointer to sorted Linked List
kNode* mergeSortKw(kNode* head){
	
	//base case
	if(head == NULL || head -> next == NULL){
		return head;
	}

	//split LL by iterating thru list with 2 ptrs, 
	//ptr2 2x faster than ptr1
	//temp trails behind ptr1 so we can properly split the LLs
		//by setting temp -> next = NULL
	kNode* temp;
	kNode* ptr1 = head;
	kNode* ptr2 = head;
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
	kNode* newHead = NULL;
	kNode* newTemp;
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


void freeKNodes(kNode* head){

	//base case
	if(head == NULL){
		return;
	}

	//free string
	free(head -> keyWord);
	
	//free fileList linked list 
	freeNodes(head -> fileList);	

	//recursively free LL
	freeKNodes(head -> next);
	
	free(head);
}

void freeNodes(node* head){

	if(head == NULL){
		return;
	}

	//free string
	free(head -> str);

	//recursively free next
	freeNodes(head -> next);
	
	free(head);
}































