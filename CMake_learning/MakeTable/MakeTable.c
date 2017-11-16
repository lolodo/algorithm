/*************************************************************************
    > File Name: MakeTable.c
    > Author: Alex
    > Mail: fangyuan.liu@nio.com 
    > Created Time: Wed 15 Nov 2017 04:20:36 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[])
{
	int i;
	double result;

	// make sure we have enough arguments
	if (argc < 2) {
		return 1;
	}

	// open the output file
	FILE *fout = fopen(argv[1], "w");
	if (!fout) {
		return 1;
	}

	//create a source file with a table of square roots
	fprintf(fout, "double sqrtTable[] = {\n");
	for (i = 0; i < 10; i++) {
		result = sqrt(i);
		fprintf(fout, "%g, \n", result);
	}

	fprintf(fout, "0}; \n");
	fclose(fout);

	return 0;
}
