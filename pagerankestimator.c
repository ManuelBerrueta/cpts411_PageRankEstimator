#include "pagerankestimator.h"

//To compile OpenMP: gcc -o <execname> -fopenmp <source file names>

//*** Inputs listed in sequence
//* K:<length of random walk>, D:<damping ratio>, input:<input file>, t:<# of threads>, nSize:<Highest node value>

//* Generating random numbers in a range: rand() % (upper - lower + 1)) + lower

int DEBUG = 0;

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
    double D = -1.0;
    sscanf(argv[2], "%lf", &D);
    char fileName[64] = {0};
    strcpy(fileName, argv[3]);
    int t = atoi(argv[4]);
    int nSize = atoi(argv[5]);
    DATA *myGraph = (DATA *)malloc(sizeof(DATA) * (nSize + 2));
    char buf[512] = {0};
    int node = -1;
    int hyperlink = -1;

    int thread_i = 0;
    int walks_i = 0;


    for (walks_i = 7; walks_i < 15; walks_i++) //K Walks multiples of 2
    {
        
        K = (int) pow(2, walks_i);

        //Test Num of threads
        for (thread_i = 0; thread_i < 4; thread_i++)
        {

            t = (int) pow(2, thread_i);



    omp_set_num_threads(t);


    for (i = 0; i < nSize+1; i++) {
        myGraph[i].pagerank = 0;
        for (j = 0; j < 3600; j++)
        {
            myGraph[i].hyperlinks[j] = -1;
        }
    }

    if (DEBUG) {
        puts("============[ DEBUG ] ============");
        printf("K=%d  |  D=%lf  |  fileName=%s  |  t=%d  |  nSize=%d\n", K, D, fileName, t, nSize);
    }

    FILE *fp = fopen(fileName, "r");
    if (fp == NULL) {
        printf("\n-=0={ERROR: Failed to Open %s}=0=-\n", fileName);
        return -1;
    }

    clock_t inputLoadStartTime = clock();
    while (fgets(buf, 512, fp)) {
        if (buf[0] == '#') { //*If the first char in the line is a '#' ignore it
            if (DEBUG) {
                puts("============[ DEBUG ] ============");
                puts("testing");
                sscanf(buf, "%d %d", &node, &hyperlink);
                printf("node=%d  |  hyperlink=%d\n", node, hyperlink);
            } //!Skip lines with no data
        } else {
            i=0;
            sscanf(buf, "%d %d", &node, &hyperlink);
            //iterate to find the next empty hyperlink array spot to add new link
            while (myGraph[node].hyperlinks[i] != -1) {
                i++; 
            }
            myGraph[node].hyperlinks[i] = hyperlink;//Add to the end

            if (DEBUG) {
                puts("============[ DEBUG ] ============");
                printf("myGraph[node=%d].hyperlinks[i=%d]  =  hyperlink=%d\n", node, i, myGraph[node].hyperlinks[i]);
            }
        }//* if next line is last line then keep node as size
    }
    clock_t endInputLoadTime = clock();
    double inputLoadTime = ((double)(endInputLoadTime - inputLoadStartTime)) / CLOCKS_PER_SEC;


    if (DEBUG) { //Print contents of the first 24 nodes and each of its links 
        i=0, j=0;
        puts("============[ DEBUG ] ============");
        while(i < 24){
            j=0;
            while(myGraph[i].hyperlinks[j] != -1){
                printf("myGraph[%d].hyperlinks[%d] = %d\n", i, j, (myGraph[i].hyperlinks[j]));
                j++;
            }
            j=0;
            i++;
        }
    }

    //!For each node do a walk of size K visits
/*     i=0, j=0;
    double dampingRatio = (1 - D);
    int seed = time(NULL);
    double coinToss = -1;
    int randomNode = -1;
    int myTargetNode = -1;
    int numLinks = 0; */

    #pragma omp parallel
    {
        assert(t == omp_get_num_threads());
        int rank = omp_get_thread_num();
        if (DEBUG)
        {
            printf("Number of threads: %d\n", t);
            printf("Rank=%d\t|\tRunning on %d threads", rank, t);
        }
    }
    
    int timeOfLinux = time(NULL);

    double startTime = omp_get_wtime();

    #pragma omp parallel default(none) shared(DEBUG, timeOfLinux, j, K, D, myGraph, nSize) private(i)
    {

    //!For each node do a walk of size K visits
    i=0, j=0;
    double dampingRatio = (1 - D);
    double coinToss = -1;
    int randomNode = -1;
    int myTargetNode = -1;
    int numLinks = 0;
    int myRank = omp_get_thread_num();
    int seed = myRank + timeOfLinux;

        #pragma omp for schedule(dynamic)
        for (i = 0; i <= nSize; i++) {
            j = numLinks = 0;
            //TODO: figure out # of links
            while (myGraph[i].hyperlinks[numLinks] != -1) {
                numLinks++;
            }
            numLinks--;//Subtract the last increment

            if (numLinks < 0) {
            } else {
                for (j = 0; j < K; j++) {
                    //TODO: Keep track of the top 5
                    //TODO: Possible: Use an array to keep track of the top 5 performers

                    //* Flip the coin, that is, use the Damping Ratio
                    coinToss = ((double)rand_r(&seed) / (double)RAND_MAX);
                    if(coinToss >= dampingRatio){
                        //seed = (myRank + timeOfLinux);
                        randomNode = (rand_r(&seed) % (nSize + 1 - 0) + 0);

                        myGraph[randomNode].pagerank++;

                        if (DEBUG) {
                            if (myGraph[randomNode].hyperlinks[0] == -1) {
                                puts("-=0=-={Found NonTargetable NODE: Empty links, treat as random page");
                            }
                            printf("Rank=%d\t\t|randomNode=%d\t\t| myGraph[%d].pagerank++ = %d (current value)\n", myRank, randomNode, randomNode, myGraph[randomNode].pagerank);
                        }  
                    } else {
                        //*Choose a TargetNode from this node
                        //seed = (myRank + timeOfLinux);
                        randomNode = (rand_r(&seed) % (numLinks + 1 - 0) + 0);
                        myTargetNode = myGraph[i].hyperlinks[randomNode];
                        //*Increment TargetNode pagerank
                        myGraph[myTargetNode].pagerank++;

                        if (DEBUG) {
                            if (myGraph[i].hyperlinks[randomNode] == -1) {
                                puts("-=0=-={ERROR: Went out of bounds on array using rand");
                            }
                            printf("Rank=%d\t\t|randomNode=%d \t\t|  Target=%d  \t|  myGraph[myGraph[%d].hyperlinks[%d]].pagerank++ = %d (current value)\n", myRank, randomNode, myTargetNode, i, randomNode, myGraph[myTargetNode].pagerank);
                        
                            if (i == 4038) {
                                puts("=====================[ DEBUG ]=====================");
                                printf("Debug Test at i = %d\n", i);
                                //getchar();                
                            }
                        }
                    }
                }
            }

            if (DEBUG) {
                if ((i % 10000) == 0) {
                    puts("=====================[ DEBUG ]=====================");
                    printf("Debug Test at i = %d\n", i);
                    //FACEBOOK TEST
                    //printf("%d\n", myGraph[3980].hyperlinks[54]);
                    //getchar();
                }
                if (i == 4038) {
                    puts("=====================[ DEBUG ]=====================");
                    printf("Debug Test at i = %d\n", i);
                    //getchar();                
                }
            }
        }


    }

    double duration = omp_get_wtime() - startTime;



    //! Find top 5 most ranked pages
    int topFive[5] = {0};
    int topNodes[5] = {0};
    int temp = 0;
    int lowestVal = 0;
    int lowValIndex = -1;

    //? Initialisze top 5
    for (i = 0; i < 5; i++) {
        topFive[i] = myGraph[i].pagerank;
        topNodes[i] = i;
        /* if (i == 0) {
            lowestVal = topFive[0];
            lowValIndex = 0;
        } else if (lowestVal > topFive[i]) {
            lowestVal = topFive[i];
            lowValIndex = i;
        } */
    }

    //Select top 5 Sites
    for (i = 5; i < nSize; i++) { // i = 5 for start
        
        //*Find curent lowest value in an array
        //lowestVal = topFive[0];
        //lowValIndex = 0;
        for (j = 0; j < 5; j++) { // j = 1 for start?
            /* if (topFive[j] < lowestVal) { 
                lowestVal = topFive[j];
                lowValIndex = j;
            } */
            if (j == 0) {
                lowestVal = topFive[0];
                lowValIndex = 0;
            } else if (lowestVal > topFive[j]) {
                lowestVal = topFive[j];
                lowValIndex = j;
            }
        }
        
        if (myGraph[i].pagerank >= lowestVal) {
            topFive[lowValIndex] = myGraph[i].pagerank;
            topNodes[lowValIndex] = i;
        }
    }

    double pagRank = -1.0;

    printf("}}=======![ Input Data Set = %s\n ]!======={{", fileName);
    printf("}======[ TOP 5 Nodes @ K=%d Threads=%d ]======={\n", K,t);
    for (i = 0; i < 5; i++) {
        pagRank = (double)topFive[i] / (double)(nSize * K);
        //pagRank = pagRank * 10.0;
        printf("\tNode=%d  \tPageHits=%d \tpageRank=%0.3lf\n", topNodes[i], topFive[i], pagRank);
    }

    printf("}===[ Time to execute threaded code: %0.3lf s ==={\n", duration);
    printf("}===[ Total input load time: %0.3lf s ==={\n", inputLoadTime);


    char outFileName[64] = {0};
    int copySize = strlen(fileName) - 4;
    strncpy(outFileName, fileName, copySize);
    strcat(outFileName, ".csv");
    FILE *outFile = fopen(outFileName, "a");

    fprintf(outFile, "FileName,K Walks,Num Of Threads\n");
    fprintf(outFile, "%s,%d,%d\n", fileName, K, t);
    fprintf(outFile, "}======[ TOP 5 Nodes ]======={\n");
    fprintf(outFile, "Node,NumberOfHits,PageRank\n");

    //! Loop to print out nodes
    for (i = 0; i < 5; i++) {
        pagRank = (double)topFive[i] / (double)(nSize * K);
        fprintf(outFile, "%d,%d,%0.3lf\n", topNodes[i], topFive[i], pagRank);
    }
    fprintf(outFile, "==================================================================\n\n");

    fclose(outFile);

    fclose(fp);

    }
    }

    return 0;
}
