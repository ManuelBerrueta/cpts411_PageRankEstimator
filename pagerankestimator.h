#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

#define DEBUG 1

typedef struct data{
    int pagerank;
    int hyperlinks[64];
}DATA;