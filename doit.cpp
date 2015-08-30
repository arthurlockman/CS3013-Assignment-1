/**
 * Doit.cpp
 * Doit is a basic exec shell that supports background tasks.
 *
 * @author Arthur Lockman <ajlockman@wpi.edu>
 */

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <iomanip>
#include <time.h>

using namespace std;

/**
 * Print a stat returned from the getrusage() function.
 */
void printStat(char const *stat, long val)
{
    cout << left << setw(29) << stat << right << setw(10) << val << endl;
}

/**
 * Main function.
 * @param argc The argument count.
 * @param argv The arguments as text.
 */
int main(int argc, char **argv)
{
    if (argc == 1)
    {
        cout << "Executing as shell..." << endl;
        //Exec as shell
    }
    else if (argc > 1)
    {
        int pid;
        struct timeval start;
        gettimeofday(&start, NULL);
        long start_ms = start.tv_sec * 1000 + start.tv_usec / 1000;
        //Exec given command
        if ((pid = fork()) < 0) //fork failed
        {
            cerr << "Fork error!" << endl;
        }
        else if (pid == 0)   //is child
        {
            char *argvNew[argc];
            for (int i = 1; i < argc; i++)
            {
                argvNew[i - 1] = argv[i];
            }
            argvNew[argc - 1] = NULL;
            if (execvp(argvNew[0], argvNew) < 0)
            {
                cerr << "Execvp error!" << endl;
                exit(1);
            }
        }
        else //is parent
        {
            struct rusage childUsage;
            wait(0);
            cout << "Child finished." << endl;
            struct timeval end;
            gettimeofday(&end, NULL);
            long end_ms = end.tv_sec * 1000 + end.tv_usec / 1000;
            getrusage(RUSAGE_CHILDREN, &childUsage);
            printStat("Wall Clock Time:", end_ms - start_ms); 
            printStat("User CPU Time:", 345678);
            printStat("System CPU Time:", 134134);
            printStat("Max RSS:", childUsage.ru_maxrss);
            printStat("Integral Shared Memory Size:", childUsage.ru_ixrss);
            printStat("Integral Unshared Data Size:", childUsage.ru_idrss);
            printStat("Integral Unshared Stack Size:", childUsage.ru_isrss);
            printStat("Page Reclaims:", childUsage.ru_minflt);
            printStat("Page Faults:", childUsage.ru_majflt);
            printStat("Swaps:", childUsage.ru_nswap);
            printStat("Block Input Operations:", childUsage.ru_inblock);
            printStat("Block Output Operations:", childUsage.ru_oublock);
            printStat("IPC Messages Sent:", childUsage.ru_msgsnd);
            printStat("IPC Messages Received:", childUsage.ru_msgrcv);
            printStat("Signals Received:", childUsage.ru_nsignals);
            printStat("Voluntary Context Switches:", childUsage.ru_nvcsw);
            printStat("Involuntary Context Switches:", childUsage.ru_nivcsw);

        }
    }
}

