#ifndef INVERTEDINDEX_H_
#define INVERTEDINDEX_H_

  #define BUFSIZE 5000

  //record node
  typedef struct node{
    char* str;
    int count;
    struct node* next;
  }node;
  
  //hash node
  typedef struct hNode{
    node*fileList;
    char* keyword;
    struct hNode* next;
  }hNode;
  
  typedef struct hNode{
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
  hNode* mergeSortKw(hNode* head);
  node* mergeSortRecords(node* head);

#endif
