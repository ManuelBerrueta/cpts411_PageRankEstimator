#include "pagerankestimator.h"

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
    int i=0;
    int j=0;
    int K = atoi(argv[1]);
    //double D = atof(argv[2]);
    double D = -1.0;
    sscanf(argv[2], "%lf", &D);
    char fileName[64] = {0};
    strcpy(fileName, argv[3]);
    int t = atoi(argv[4]);
    int nSize = atoi(argv[5]);
    //DATA myGraph[nSize+1];
    DATA *myGraph = (DATA *)malloc(sizeof(DATA) * (nSize + 2));


    for (i = 0; i < nSize+1; i++)
    {
        myGraph[i].pagerank = 0;
        for (j = 0; j < 3600; j++)
        {
            myGraph[i].hyperlinks[j] = 0;
        }
    }
        


    if (DEBUG) {
        printf("K=%d  |  D=%lf  |  fileName=%s  |  t=%d  |  nSize=%d\n", K, D, fileName, t, nSize);
    }

    FILE *fp = fopen(fileName, "r");
    if (fp == NULL) {
        printf("\n-=0={ERROR: Failed to Open %s}=0=-\n", fileName);
        return -1;
    }

    char buf[512] = {0};
    int node = -1;
    int hyperlink = -1;
    i=0;
    j=0;
    
    while (fgets(buf, 512, fp)) {
        if(buf[0] == '#'){ //*If the first char in the line is a '#' ignore it
            if(DEBUG){
                puts("testing");
                sscanf(buf, "%d %d", &node, &hyperlink);
                printf("node=%d  |  hyperlink=%d\n", node, hyperlink);
            } //!Skip lines with no data
        } else {
            i=0;
            sscanf(buf, "%d %d", &node, &hyperlink);
            //iterate to find the next empty hyperlink array spot to add new link
            while (myGraph[node].hyperlinks[i]) {
                i++; 
            }
            myGraph[node].hyperlinks[i] = hyperlink;//Add to the end

            if (DEBUG) {
                printf("myGraph[node=%d].hyperlinks[i=%d]  =  hyperlink=%d\n", node, i, myGraph[node].hyperlinks[i]);
            }
        }//* if next line is last line then keep node as size
    }

    //Print out contents of the first 18 links of the graph
    if (DEBUG) {
        i=0, j=0;
        while(i < 24){
            j=0;
            while(myGraph[i].hyperlinks[j] != 0){
                printf("myGraph[%d].hyperlinks[%d] = %d\n", i, j, (myGraph[i].hyperlinks[j]));
                j++;
            }
            j=0;
            i++;
        }
    }

    
    
    //!For each node do a walk of size K visits
    i=0, j=0;
    double dampingRatio = (1 - D);
    omp_set_num_threads(t);

    while(i < nSize-1){
        j=0;
        //TODO: figure out size of array
        int x=0;
        while(myGraph[i].hyperlinks[x] != 0){
            x++;
        }
        x--;//Subtract the last increment
        while(j < K-1){
            //TODO: Keep track of the top 5
            //      Possible: Use an array to keep track of the top 5 performers6
            int seed = time(NULL);
            //* Flip the coin, that is, use the Damping Ratio
            double coinToss = ((double)rand_r(&seed) / (double)RAND_MAX);
            if(coinToss >= dampingRatio){
                int randomNode = (rand_r(&seed) % (nSize + 1 - 0) + 0);
                myGraph[randomNode].pagerank++;
            }
            else
            {
                //*Choose a TargetNode from this node
                int myTargetNode = (rand_r(&seed) % (x + 1 - 0) + 0);
                //*Increment TargetNode
                myGraph[myGraph[i].hyperlinks[myTargetNode]].pagerank++;

                if(DEBUG){
                    if(myGraph[i].hyperlinks[myTargetNode] == 0){
                        puts("-=0=-={ERROR: Went out of bounds on array using rand");
                    }
                    printf("TargetNode==%d | myGraph[myGraph[%d].hyperlinks[%d]].pagerank++ == %d (current value)\n", myGraph[i].hyperlinks[myTargetNode], i, myTargetNode, myGraph[myGraph[i].hyperlinks[myTargetNode]].pagerank);
                }
                j++;   
            }
        }
        j=0;
        i++;
    }

    //Generating random numbers in a range: rand() % (upper - lower + 1)) + lower
    return 0;
}
