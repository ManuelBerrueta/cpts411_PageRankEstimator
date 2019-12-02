#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

typedef struct data{
    int pagerank;
    int hyperlinks[64];
}DATA;