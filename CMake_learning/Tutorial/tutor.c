/*************************************************************************
    > File Name: tutorial.c
    > Author: Alex
    > Mail: fangyuan.liu@nio.com 
    > Created Time: Wed 15 Nov 2017 12:09:12 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main (int argc, char *argv[])
{
	if (argc < 2) {
		return 1;
	}
	
	double inputValue = atof(argv[1]);
	double outputValue = sqrt(inputValue);
	fprintf(stdout,"The square root of %g is %g\n", inputValue, outputValue);
	
	return 0;
}
