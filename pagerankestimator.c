#include <stdio.h>
#include <string.h>
#include <omp.h>

//TODO: To compile: gcc -o <execname> -fopenmp <source file names>

//*** Inputs listed in sequence
//* K:<length of random walk>, D:<damping ratio>, input:<input file>, t:<# of threads>
int DEBUG = 1;

int main(int argc, char const *argv[])
{
    if(argc < 5){
        puts("Please enter the required input:");
        puts("K:<length of random walk>, D:<damping ratio>, input:<input file>, t:<# of threads>");
        getchar();
        return -1;
    }

    int K = atoi(argv[1]);
    float D = atof(argv[2]);
    char fileName[64] = {0};
    strcpy(fileName, argv[3]);
    int t = atoi(argv[4]);
    
    if(DEBUG)
    {
        printf("K=%d  |  D=%f  |  fileName=%s  |  t=%d\n", K, D, fileName, t);
    }

    omp_set_num_threads(t);
    
    return 0;
}
