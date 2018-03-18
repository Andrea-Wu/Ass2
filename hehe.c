#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>


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
	
int tokenize(char* fileName, char** wordArr);
void clearBuffer(char* str, int len);
node* createNode(char* str, int count, node* next);
void sort(char** allStrings, int wordCount);
node* removeDuplicates(char** allWords, int len);
void printHLL(hNode* head);
void printLL(node* head);
void insertKeyWord(hNode** hashTable, hNode* insertMe);
void traverseDir(hNode** hashTable, char* path);
void insertRecords(hNode** hashTable, node* head, char* fileName);
int hashFunction(char* str);

hNode* mergeSortKw(hNode* head);
node* mergeSortRecords(node* head);

int main(int argc, char* argv[]){
	//an array of hash nodes
	
	if(argv[1] == NULL || argv[2] == NULL){
		printf("invalid input\n");
		return 1;
	}
	hNode** hashTable = (hNode**)malloc(sizeof(hNode*) * 1000);

	traverseDir(hashTable, argv[2]);

	//all of the required data is now in the hash table
	//time to print everything out!
		
	//remove all of the nodes from the hash table and turn into a LL
	hNode* head = NULL;
	hNode* temp;
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
		}
		i++;
	}

	head = mergeSortKw(head); 

	//now all the kws are in the correct order
	//for every keyword, merge sort sub-LL
	//this isn't necessary if we sort while inserting, change lateri
	//
	//IMPORTANT FREE EVERYTHING
	

	FILE* writeFp = fopen(argv[1], "w");

	char xml[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	fwrite(xml, sizeof(char), strlen(xml), writeFp);

	char fileIndex[] = "<fileIndex>\n";
	fwrite(fileIndex, sizeof(char), strlen(fileIndex), writeFp);
	
	//FOR FREE
	//hNode* freeLaterHead = head;

	while(head != NULL){
		char* kw = head -> keyWord;

		//write the xml thing to a file
		//final line of text will be stored in openWordText
		
		char openWordText[] = "\t<word text=\"";
		char closeWordText[] = "\">\n";		
		
		//this should be a separate function lol
		int newKwLen = strlen(kw) + strlen(openWordText) + strlen(closeWordText);
			
		char* newKwStr = (char*)malloc(sizeof(char) * (newKwLen + 1));

		int itr = 0;	
		int ITR = 0;

		while(itr < strlen(openWordText)){
			newKwStr[itr] = openWordText[itr];
			itr++;
			ITR++;
		}


		itr = 0;
                while(itr < strlen(kw)){
                        newKwStr[ITR] = kw[itr];
                        itr++;
                        ITR++;
                }

		itr = 0;
                while(itr < strlen(closeWordText)){
                        newKwStr[ITR] = closeWordText[itr];
                        itr++;
                        ITR++;
                }
		

		fwrite(newKwStr, sizeof(char), strlen(newKwStr), writeFp);
		
		node* fileList = head -> fileList;

		//FOR FREE
		//node* fileListHead = fileList;
		fileList = mergeSortRecords(fileList);
		
		//print fileList as XML to file
		while(fileList != NULL){
			char* str = fileList -> str;

			int count = fileList -> count;

			char frag1[] = "\t\t<file name=\"";
			char frag2[] = "\">";
			char frag3[] = "</file>\n";

			//assume the number won't be more than 20 digits...
			//...10 quintillion

			char countStr[20];
			sprintf(countStr, "%d", count);

			//malloc a string that's big enough
			int newLen = strlen(str) + strlen(countStr) + strlen(frag1) \
					+ strlen(frag2) + strlen(frag3);	
	
			char* newStr = (char*)malloc(sizeof(char) *(newLen + 1));
			
			int IT = 0;	
			int it = 0;
			while(it < strlen(frag1)){
				newStr[it] = frag1[it];
				it++;
				IT++;
			}
		
			it = 0;
			while(it < strlen(str)){
				newStr[IT] = str[it];
				it++;
				IT++;
			} 
	
                        it = 0;
                        while(it < strlen(frag2)){
                                newStr[IT] = frag2[it];
				it++;
				IT++;
                        }
			
			
                        it = 0;
                        while(it < strlen(countStr)){
                                newStr[IT] = countStr[it];
				it++;
				IT++;
                        }
	
			it = 0;
                        while(it < strlen(frag3)){
                                newStr[IT] = frag3[it];
                                it++;
                                IT++;
                        }
	 
			fwrite(newStr, sizeof(char), strlen(newStr), writeFp);

			fileList = fileList -> next;
		}

		char* word = "\t</word>\n";
		fwrite(word, sizeof(char), strlen(word), writeFp);
		
		
		head = head -> next;
	}

	char closeFI[] = "</fileIndex>\n";
	fwrite(closeFI, sizeof(char), strlen(closeFI), writeFp);	
	
	return 0;
}

//ERRNO
void traverseDir(hNode** hashTable, char* path){
	DIR* dirp;
	struct dirent* dp;
	char child[PATH_MAX]; //idk what path_max is 
	if(!(dirp = opendir(path))){ //this is file or not valid name
		//pretend all files are valid...
		//fix later
		
		

		
	}else{ //is directory
		
		while((dp = readdir(dirp)) != NULL){

 			//concat path & subdir name
                        snprintf(child, PATH_MAX, "%s%s", path, dp -> d_name);
                                  
			if(dp -> d_type == DT_REG){ //is regular file
				//create array pointer and tokenize into array
				//the tokenize() function does the mallocing
				char** wordArr = (char**)malloc(sizeof(char*) * 100);

				//pass by pointer? hopefully this works
				int numWords = tokenize(child,wordArr);
				
				//should have put an unsorted array into wordArr
				//sort.
				sort(wordArr, numWords);

				//sorted list has duplicates. remove. turn into linked list.
				node* head = removeDuplicates(wordArr, numWords);

				//insert each node into the master linked list
				insertRecords(hashTable, head, dp -> d_name);
			
				
			}else if(dp -> d_type == DT_DIR){ //this is dir

				//concat path & subdir name
				snprintf(child, PATH_MAX, "%s%s", path, dp -> d_name);
				
				//recurse on subdirectory, except . and ..
				if(strcmp(dp -> d_name, ".") != 0 && strcmp(dp ->d_name, "..") != 0){
					traverseDir(hashTable,child);
				}
			}else{
				printf("what the fresh fuck");
			}
		}
	}

	closedir(dirp);
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

	// the followsng code inserts into sub-LL
		char* currWord = head -> str;
		int bucket = hashFunction(currWord);

			//stands for "keyword linked-list head"
		hNode* kwLLHead = hashTable[bucket];

			//gets hNode corresponding with current keyword
			//this is linear search...inefficient
		while( kwLLHead != NULL && strcmp(kwLLHead -> keyWord, currWord) != 0){
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
			//word might already exists in different file with same name
			//then just increment count. 
			//iterate thru list to find out!!
			//this is highly inefficient b/c i'm already iterating thru list,
			//	might as well just sort it?? lmao rip merge sort. later 

			node* recordListHead = kwLLHead -> fileList;
			node* recordList= kwLLHead -> fileList; //iterator
			
			while(strcmp(recordList -> str, fileName) != 0 && recordList != NULL){
				recordList = recordList -> next;
			}
			if(recordList == NULL){ //this filename doesn't exist in record List
				//add to front of list
				
				kwLLHead -> fileList = newNode;
				newNode -> next = recordListHead;
			}
			else{ //filename exists in record List, where recordList is desired node 
				//increment count. 
				int addMe = newNode -> count;
				recordList -> count = (recordList -> count) + addMe;
			}
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

node* removeDuplicates(char** allWords, int len){
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

int tokenize(char* fileName, char** wordArr){ 
	//takes in complete file text, file text length,
	//an array which to place an unsorted list of words, 
	//and the original length of that array. 
	//returns the size of the full array
	
	//IMPORTANT: might have to concat path...
	FILE* fp = fopen(fileName, "r");
	
	int wordCount = 0;
	int maxWordCount = 100;
	//array already declared...

	while(1){
		char temp[100]; //this is bad. to be fair, fscanf is also bad
		fscanf(fp, "%s", temp);
	
		if(feof(fp)){
                        break;
                }

			
		wordArr[wordCount] = (char*)malloc(sizeof(char) * (strlen(temp)+1));

		strcpy(wordArr[wordCount], temp);
		wordCount++;

		

		//"don't use array that has been realloced"
		if(wordCount == maxWordCount -2){
			maxWordCount = maxWordCount * 2;
			wordArr = (char**)realloc(wordArr ,sizeof(char*) * maxWordCount);
		}		

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




