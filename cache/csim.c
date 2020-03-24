/* 
 * csim - A simulator for cache
 * 
 * <Put your name and login ID here>
 * Joshua Demoss -- joshdemoss
 */

#include "cachelab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXLINE 1024 /* max line size */

int isNumber(char * number) {
	int i;
	int test = 1;
	for(i = 0; number[i] != '\0'; i++) {
		test = test && isdigit(number[i]);
	}

	return test;
}

void printHelpMsg() {
	printf("./csim-ref: Missing required command line argument\n"
			"Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n"
			"Options:\n"
			"  -h         Print this help message.\n"
			"  -v         Optional verbose flag.\n"
			"  -s <num>   Number of set index  bits.\n"
			"  -E <num>   Number of lines per set.\n"
			"  -b <num>   Number of block offset bits.\n"
			"  -t <file>  Trace file.\n\n"

			"Examples:\n"
			"  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n"
			"  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

int main(int argc, char **argv)
{	
	int hits = 0, misses = 0, evictions = 0;
	int valid = 1; //to check if input is valid
	int v = 0; //used to determin if -v flag is set
	if(strcmp("-v", argv[1]) == 0) v = 1;
	
	//open file
	FILE *fp = fopen((const char *)argv[v + 8], "r");
	if(fp == NULL) {
		printHelpMsg();
		exit(1);
	}

	//parse file
	char buf[100];
	char mode[100];
	
	if(!(argc == 9 || argc == 10)) { //check number of arguments
		printHelpMsg();
		exit(0);
	}


	//checking validity of flags
	int flag = (strcmp("-s", argv[v + 1]) == 0);
	valid &= flag;
	flag = (strcmp("-E", argv[v + 3]) == 0);
	valid &= flag;
	flag = (strcmp("-b", argv[v + 5]) == 0);
	valid &= flag;
	flag = (strcmp("-t", argv[v + 7]) == 0); 
	valid &= flag;

	//check validity of numbers
	int nums = isNumber(argv[v + 2]);
	valid &= nums;
	nums = isNumber(argv[v + 4]);
	valid &= nums;
	nums = isNumber(argv[v + 6]);
	valid &= nums;

	if (valid) {

		int s, E, b;
		char *p; //used for strtol function
		//convert input to ints
		s = (int)strtol(argv[v + 2], &p, 10);
		E = (int)strtol(argv[v + 4], &p, 10);
		b = (int)strtol(argv[v + 6], &p, 10);

		// make simulated cache structure as an array[s][E]
		unsigned long cache[1 << s][E]; //used to store tag of address
		int cacheC[1 << s][E]; //used to indicate coldness of a cache block
		int cacheV[1 << s][E]; //says if cache line is valid or not

		//initialize arrays:
		int i;
		int j;
		for(i = 0; i < (1 << s); i++) {
			for(j = 0; j < E; j++){
				cache[i][j] = 0;
				cacheC[i][j] = 0;
				cacheV[i][j] = 0;
			}
		}
		// int test = 0;
		//parsing the addresses and storing them in cache
		while ((fscanf(fp, "%s %s",mode, buf)) != EOF) {
			// if(test == 1000) break;
			// test++;
			//address gets address from buff
			char *comma = strchr(buf, ','); //get location of comma after address
			if (comma != 0) *comma = 0; //change comma to string terminator
			unsigned long address = strtol(buf, &p, 16); //convert address to long
			
			
			//if the instruction is an M we will add one hit even if it is not in cache
			if(strcmp(mode, "M") == 0)
				hits++;
			if(strcmp(mode, "I") != 0) {

				//split up address
				unsigned long tag = address >> (s + b);
				unsigned long setMask = ~((-1 >> s) << s);
				int setNum = (int)((address >> b) & setMask);
				int bMask = ~((-1 >> b) << b);
				int bOffset = (int)(address & bMask);

				printf("%s %lx -> %lx %x %x -- ", mode, address, tag, setNum, bOffset);
				if (v) printf("%s %s  ", mode, buf);
				
				//checking cache for tag
				int found = 0; //used to see if there is a hit
				int LRU = cacheC[setNum][0]; //used to find LRU cache line in set
				int LRUindex = 0;
				int full = 1;
				for (i = 0; i < E; i++) { //look in set for tag and find LRU along the way
					full &= cacheV[setNum][i]; //checks to see any free cache lines
					if(cacheC[setNum][i] > LRU) {
						LRU = cacheC[setNum][i]; //update LRU
						LRUindex = i; //update LRUindex
					}
					if (tag == cache[setNum][i] && (cacheV[setNum][i] == 1)) { //check current line against tag
						found = 1;
						break;
					}
				}
				
				if(found) {
					if(v) printf("hit\n");
					printf("hit\n");
					hits++;
					cacheC[setNum][i] = -1;
				} else {
					if(v) printf("miss ");
					printf("miss ");
					misses++;

					//check line needs to be evicted or just empty cache line
					if(full == 1) {
						if(v) printf("eviction");
						printf("eviction");
						evictions++;
						cacheC[setNum][LRUindex] = 0;
					} else {
						cacheV[setNum][LRUindex] = 1;
					}
					if(v) printf("\n");
					printf("\n");
					cacheC[setNum][LRUindex] = -1;
					cache[setNum][LRUindex] = tag;
				}

			//increase all elements in cacheC[setNum] by 1 to distinguish LRU
			for(i = 0; i < E; i++){
				cacheC[setNum][i]++;
			}
			// int j;
			// for(i = 0; i < s; i++){
			// 	for(j = 0; j < E; j++) {
			// 		printf("\t%d: %lx ", i, cache[i][j]);
			// 	}
			// 	printf("\n");
			// }
			}

		}
		printSummary(hits, misses, evictions);

	} else {
		printHelpMsg();
	}

	fclose(fp);

	return 0;
}
