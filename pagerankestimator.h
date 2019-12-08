#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>
#include <assert.h>

typedef struct data{
    int pagerank;
    int hyperlinks[3600];
}DATA;