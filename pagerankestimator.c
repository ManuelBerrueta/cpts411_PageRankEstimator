#include "pagerankrestimator.h"

//TODO: To compile: gcc -o <execname> -fopenmp <source file names>

//*** Inputs listed in sequence
//* K:<length of random walk>, D:<damping ratio>, input:<input file>, t:<# of threads>
int DEBUG = 1;

int main(int argc, char const *argv[])
{
    if(argc < 6){
        puts("Please enter the required input:");
        puts("K:<length of random walk>, D:<damping ratio>, input:<input file>, t:<# of threads>, nSize:<Highest node value>");
        getchar();
        return -1;
    }

    int K = atoi(argv[1]);
    //double D = atof(argv[2]);
    double D = -1.0;
    sscanf(argv[2], "%lf", &D);
    char fileName[64] = {0};
    strcpy(fileName, argv[3]);
    int t = atoi(argv[4]);
    int nSize = atoi(argv[5]);
    DATA myGraph[nSize];
    
    if(DEBUG){
        printf("K=%d  |  D=%lf  |  fileName=%s  |  t=%d  |  nSize=%d\n", K, D, fileName, t, nSize);
    }

    FILE *fp = fopen(fileName, "r");
    if(fp == NULL){
        printf("\n-=0={ERROR: Failed to Open %s}=0=-\n", fileName);
        return -1;
    }

    char buf[512] = {0};
    int node = -1;
    int hyperlink = -1;
    int i=0;
    int j=0;
    while(j < 12){//feof(fp)){
        fgets(buf, 512, fp);

        if(buf[0] == '#'){
            if(DEBUG){
                puts("testing");
                sscanf(buf, "%d %d", &node, &hyperlink);
                printf("node=%d  |  hyperlink=%d\n", node, hyperlink);
            }
            //!Skip lines with no data
        }
        else{
            i=0;
            sscanf(buf, "%d %d", &node, &hyperlink);
            printf("node=%d  |  hyperlink=%d\n", node, hyperlink);
            while(myGraph[j].hyperlinks[i]){
               i++; 
            }
            myGraph[node].hyperlinks[i] = hyperlink;//TODO: Add the end

            //TODO: use random on the arraysize-1 to choose an index randomly to jump to
        }
        //* if next line is last line then keep node as size
        j++;
    }

    if(DEBUG){
        i=0, j=0;
        while(myGraph[i].hyperlinks != 0){
            j=0;
            while(myGraph[i].hyperlinks[j] = 0){
                printf("myGraph[%d].hyperlinks[%d] = %d", i, j, (myGraph[i].hyperlinks[j]));
                j++;
            }
            j=0;
            i++;
        }
    }

    //TODO: Make sure file exists, that is make sure fopen was succesful
    //TODO: if they first char in a line is a '#' then trash that line and read a new line, else put line in data structure

    omp_set_num_threads(t);
    
    return 0;
}
