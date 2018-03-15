#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int tokenize(char* buf, int bufLen, char** wordArr, int wordArrLen);
void clearBuffer(char* str, int len);


int main(int argc, char* argv[]){
	printf("%s\n", argv[0]);

	
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

void removeDuplicates(){
	//assume that the array that's passed in is already sorted
	
	
	
	
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



