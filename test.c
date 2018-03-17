#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int main(int argc, char* argv[]){

	DIR* dirp;
	struct dirent *dp;
	char child[PATH_MAX];

	if(!(dirp = opendir(argv[1]))){
		printf("this is not right\n");
	}else{
		printf("ok\n");
	}
	
}
