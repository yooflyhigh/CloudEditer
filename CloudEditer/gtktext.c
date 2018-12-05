#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "gtktext.h"

void print_usage(void){
	fprintf(stderr, "%s: usage: %s [file]\n",NAME,NAME);
}

void gtk_notepad_set_title(const char* filename){
	char *fn = malloc(strlen(filename) + 1);
	strcpy(fn, filename);

	int i = 0;
	int index = 0;
	char *slash = strrchr(filename, '/');
	if(slash){
		index = (int)(slash - filename);

		for(i = 0; i <= index ; i++, fn++);
	}

	char *_title = malloc(strlen
}
