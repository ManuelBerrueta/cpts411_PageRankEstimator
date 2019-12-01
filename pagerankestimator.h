#include <stdio.h>
#include <string.h>
#include <omp.h>

typedef struct data{
    int pagerank;
    int hyperlinks[64];
}DATA;