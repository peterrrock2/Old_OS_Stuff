#include "multi-lookup.h"
#include "stack.h"


pthread_mutex_t msharedStk = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mfileStk = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mqfile = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t msfile = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cSpaceAvail  = PTHREAD_COND_INITIALIZER;
pthread_cond_t cDomainAvail  = PTHREAD_COND_INITIALIZER;
pthread_cond_t breakr = PTHREAD_COND_INITIALIZER;

int main(int argc, char *argv[])
{
/*
Order of arguments in argv:
    0 -> program name
    1 -> number requesters
    2 -> number resolvers
    3 -> requester log
    4 -> resolver log
    5-105 -> data files to be examined
*/

    clock_t startTime = clock();

    // Make all the mutexes
    // pthread_mutex_t mserviceFile;
    // pthread_mutex_t mresolvedFile;

    if(check_args(argc, argv) < 0)
    {
        clock_t stopTime = clock();
        printRuntime(argv[0], startTime, stopTime);
        return -1;
    }

    int nquestThreads = strtol(argv[1], NULL, 10);
    int nsolveThreads = strtol(argv[2], NULL, 10);

    // Get the files for the logs
    FILE* requestfp = fopen(argv[3], "w+");
    FILE* resolvefp = fopen(argv[4], "w+");

    // Arrays with the pthreads in them
    // Used https://www.youtube.com/watch?v=1ks-oMotUjc for reference on this part
    pthread_t questThreads[nquestThreads];
    pthread_t solveThreads[nsolveThreads];

    Stack *sharedStk = init_Stk(ARRAY_SIZE); //SHARED BY BOTH REQUESTER AND RESOLVER <==========================
    Stack *fileStk = init_Stk(argc - 5);

    sharedStk->nQuest = nquestThreads;

    // Make a stack to hold all of the files
    for(int i = argc - 1; i >= 5; i--) 
        pushStk(fileStk, argv[i]);

    // Handle requester thread structures ---------------------------------------------
    questData* qData = malloc(sizeof(questData));

    qData->stk = sharedStk;
    qData->fileStk = fileStk;
    qData->requestLogfp = requestfp;

    for(int i = 0; i < nquestThreads; i++) 
        pthread_create(&questThreads[i], NULL, request, qData);
    //--------------------------------------------------------------------------------



    // Handle resolver thread structure ----------------------------------------------
    solveData* sData = malloc(sizeof(solveData));

    pthread_mutex_lock(&msharedStk);
    sData->stk = sharedStk;
    pthread_mutex_unlock(&msharedStk);
    sData->resolvefp = resolvefp;



    for (int i = 0; i < nsolveThreads; i++) 
        pthread_create(&solveThreads[i], NULL, resolve, sData);
    //--------------------------------------------------------------------------------
    for (int i = 0; i < nquestThreads; i++)
        pthread_join(questThreads[i], NULL);

    for (int i = 0; i < nsolveThreads; i++)
        pthread_join(solveThreads[i], NULL);

    destroyStk(sharedStk);
    destroyStk(fileStk);
    free(qData);
    free(sData);
    fclose(requestfp);
    fclose(resolvefp);

    clock_t stopTime = clock();
    printRuntime(argv[0], startTime, stopTime);
    return 0;
}

void printRuntime(char *progName, clock_t startTime, clock_t stopTime) 
{
    double runTime = (double)(stopTime - startTime) / CLOCKS_PER_SEC;
    printf("%s: total time is %f seconds\n", progName, runTime); 
}

int check_args(int argc, char *argv[])
{
    if (argc < 6)
    {
        printf("Too few arguments entered. Expected at least 6 but got %d.\n", argc);
        printf("Expected command line format for running ./multi-lookup:\n\t./multi-lookup <# requester threads> <# resolver threads> <requester log> <resolver log> [<data file> ...]\n");
        return -1;
    }

    if (argc > 105)
    {
        printf("Too many arguments entered. Expected at most 105 but got %d.\n", argc);
        printf("Expected command line format for running ./multi-lookup:\n\t./multi-lookup <# requester threads> <# resolver threads> <requester log> <resolver log> [<data file> ...]\n");
        printf("At most 100 data files may be entered to be checked for any iteration of the function multi-lookup.\n");
        return -1;
    }

    if (strtol(argv[1], NULL, 10) <= 0)
    {
        fprintf(stderr, "Too few requester threads entered. Number of requester threads must be at least 1, and the number entered was %ld.\n", strtol(argv[1], NULL, 10));
        return -1;
    }

    if (strtol(argv[2], NULL, 10) <= 0)
    {
        fprintf(stderr, "Too few resolver threads entered. Number of resolver threads must be at least 1, and the number entered was %ld.\n", strtol(argv[1], NULL, 10));
        return -1;
    }

    if (strtol(argv[1], NULL, 10) > MAX_REQUESTER_THREADS)
    {
        fprintf(stderr, "Too many requester threads entered. Number of requester threads is at most %d, and the number entered was %ld.\n", MAX_REQUESTER_THREADS, strtol(argv[1], NULL, 10));
        return -1;
    }

    if (strtol(argv[2], NULL, 10) > MAX_RESOLVER_THREADS)
    {
        fprintf(stderr, "Too many resolver threads entered. Number of resolver threads is at most %d, and the number entered was %ld.\n", MAX_RESOLVER_THREADS, strtol(argv[2], NULL, 10));
        return -1;
    }


    return 0;
}

// The handling for this is the exact same as the chef-server problem from the midterm
void *request(void *qData)
{
    pthread_mutex_lock(&msharedStk);
    questData* temp = (questData*) qData; // Create temp variable so we don't directly access shared data
    Stack *files = temp->fileStk;
    Stack *sharedStk = temp->stk;
    pthread_mutex_unlock(&msharedStk);
    FILE *qLogFile = temp->requestLogfp;

    int serveCount = 0;
    char *fileName;

    pthread_mutex_lock(&mfileStk);
    int filesLeft = !testStkEmpty(files);
    pthread_mutex_unlock(&mfileStk);

    
    while(filesLeft)
    {
        pthread_mutex_lock(&mfileStk);
        fileName = popStk(files);
        pthread_mutex_unlock(&mfileStk);

        FILE *reqfile = fopen(fileName, "r");

        if(reqfile == NULL)
        {
            fprintf(stderr, "invalid file %s.\n", fileName);
        }
        else
        {
            serveCount += 1;
            char *domainName = NULL;
            size_t len = 0;
            ssize_t nread;

            while ((nread = getline(&domainName, &len, reqfile)) != -1)
            {

                if (domainName[strlen(domainName) - 1] == '\n')
                    domainName[strlen(domainName) - 1] = 0;

                if(nread > MAX_NAME_LENGTH)
                    fprintf(stderr, "In %s domain name of inappropriate length entered.\nExpected name of length at most 255 and got a name of length %ld.\n", fileName, nread);
                else
                {
                    pthread_mutex_lock(&msharedStk);
                    while(testStkFull(sharedStk))
                    {
                        // printf("Stack is full\n");
                        pthread_cond_wait(&cSpaceAvail, &msharedStk); // Block until shared stack is not full
                    }
                    pushStk(sharedStk, domainName);

                    printf("Pushing %s onto the stack. Here is the stack:\n", domainName);
                    // printStk(sharedStk);
                    pthread_cond_signal(&cDomainAvail); // Signal that the shared stack has something in it
                    pthread_mutex_unlock(&msharedStk); // Allow access to the shared stack


                    pthread_mutex_lock(&mqfile);
                    fprintf(qLogFile, "%s\n", domainName);
                    pthread_mutex_unlock(&mqfile);
                }
            }
            
            if(domainName)
                free(domainName);

            if(reqfile) fclose(reqfile);

            pthread_mutex_lock(&msharedStk);
            if(sharedStk->nqDone == sharedStk->nQuest)
            {
                printf("Said done.\n");
                sharedStk->finishedAdding = 1;
            }
            pthread_mutex_unlock(&msharedStk);
        }

        pthread_mutex_lock(&mfileStk);
        filesLeft = !testStkEmpty(files);
        pthread_mutex_unlock(&mfileStk);
    }
    pthread_mutex_lock(&msharedStk);
    sharedStk->finishedFiles = 1;
    sharedStk->nqDone += 1;
    if (sharedStk->nqDone == sharedStk->nQuest)
    {
        printf("Said done.\n");
        sharedStk->finishedAdding = 1;
    }
    pthread_mutex_unlock(&msharedStk);

    printf("Thread %ld serviced %d files.\n", pthread_self(), serveCount);
    // pthread_cond_signal(&cDomainAvail);
    // pthread_cond_signal(&cDomainAvail);
    // pthread_cond_signal(&cDomainAvail);
    // pthread_exit(0);
    return NULL;
}

void *resolve(void *sData)
{
    pthread_mutex_lock(&msharedStk);
    solveData *temp = (solveData *)sData; // Create temp variable so we don't directly access shared data
    Stack *sharedStk = temp->stk;
    pthread_mutex_unlock(&msharedStk); // Allow access to the shared stack
    FILE * sLogfile = temp->resolvefp;

    int solveCount = 0;

    while(1)
    {

        pthread_mutex_lock(&msharedStk); //Getting stuck here
        // if(!(testStkEmpty(sharedStk)))
        // {
        //     char *domainName = popStk(sharedStk);
        //     printf("Thred %ld Popped %s off the stack\n", pthread_self(), domainName);
        //     // printStk(sharedStk);

        //     pthread_cond_signal(&cSpaceAvail); // Tell producer that there is now a spot to put a domain
        //     pthread_mutex_unlock(&msharedStk);

        //     // char ip[MAX_IP_LENGTH];  // Make a spot for the ip address

        //     // if (dnslookup(domainName, ip, sizeof(ip)) == UTIL_FAILURE)
        //     // {
        //     //     fprintf(stderr, "DNS lookup could not find domain name: %s\n", domainName);
        //     //     strcpy(ip, "NOT_RESOLVED");
        //     // }

        //     pthread_mutex_lock(&msfile);
        //     fprintf(sLogfile, "%s, %s\n", domainName, "Thing");
        //     pthread_mutex_unlock(&msfile);
        // }
        // else
        // {
        //     pthread_mutex_unlock(&msharedStk);

        //     if (sharedStk->finishedAdding)
        //     {
        //         printf("Thread %ld resolved %d hostnames.\n", pthread_self(), solveCount);
        //         pthread_exit(0);
        //         return NULL;
        //     }
        // }
        

        if (testStkEmpty(sharedStk) && !(sharedStk->finishedFiles))
        {    
            while (testStkEmpty(sharedStk))
            {
                if (sharedStk->finishedAdding)
                {
                    pthread_mutex_unlock(&msharedStk);
                    printf("Thread %ld resolved %d hostnames.\n", pthread_self(), solveCount);
                    pthread_exit(0);
                    return NULL;
                }
                // printf("waiting for domain\n");
                printf("file finished %d\n", sharedStk->finishedFiles);
                printf("number q done: %d\n", sharedStk->nqDone);
                printf("Thread %ld resolved waiting \n", pthread_self());
                pthread_cond_wait(&cDomainAvail, &msharedStk); // Wait for producer to give me a domain
            }
        }
        else if (testStkEmpty(sharedStk) && (sharedStk->finishedFiles))
        {

        }

        char *domainName = popStk(sharedStk);
        printf("Thred %ld Popped %s off the stack\n", pthread_self(), domainName);
        // printStk(sharedStk);

        pthread_cond_signal(&cSpaceAvail);  // Tell producer that there is now a spot to put a domain
        pthread_mutex_unlock(&msharedStk);

        // char ip[MAX_IP_LENGTH];  // Make a spot for the ip address

        // if (dnslookup(domainName, ip, sizeof(ip)) == UTIL_FAILURE)
        // {
        //     fprintf(stderr, "DNS lookup could not find domain name: %s\n", domainName);
        //     strcpy(ip, "NOT_RESOLVED");
        // }

        pthread_mutex_lock(&msfile);
        fprintf(sLogfile, "%s, %s\n", domainName, "Thing");
        pthread_mutex_unlock(&msfile);
    }
}