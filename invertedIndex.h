#ifndef INVERTEDINDEX_H_
#define INVERTEDINDEX_H_


 typedef struct node{
	char* str;
	int count;
	struct node* next;
 }node;

 typedef struct kNode{
	node* fileList;
	char* keyWord;
	struct kNode* next;
 } kNode;

 char** tokenize(char* fileName, char ** wordArr, int* wordCountPtr);
 void clearBuffer(char* str, int len);
 node* createNode(char* str, int count, node* next);
 void sort(char** allStrings, int wordCount);
 node* removeDuplicates(char** allwords, int len);
 void traverseDir(kNode** hashTable, char* path);
 void insertRecords(kNode** hashTable, node* head, char* fileName);
 int hashFunction(char* str);

 kNode* mergeSortKw(kNode* head);
 node* mergeSortRecords(node* head);

 void freeNodes(node* head);
 void freeKNodes(kNode* head);

#endif
