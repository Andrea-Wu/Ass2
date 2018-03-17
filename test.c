#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int main(int argc, char* argv[]){
	FILE* writeFp = fopen("bob.txt", "w");
	char xml[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
        fwrite(xml, sizeof(char), strlen(xml), writeFp);

	
}
