#ifndef INVERTEDINDEX_H_
#define INVERTEDINDEX_H_

 #define BUFSIZE 5000
 int tokenize(char* fileName, char ** wordArr);
 void clearBuffer(char* str, int len);
 node* createNode(char* str, int count, node* next);
 void sort(char** allStrings, int wordCount);
 node* removeDuplicates(char** allwords, int len);
 void printHLL(hNode* head);
 void PrintLL (node* head);
 void traverseDir(hNode** hashTable, char* path);
 void insertRecords(hNode** hashTable, node* head, char* fileName);
 int hashFunction(char* str);

#endif
