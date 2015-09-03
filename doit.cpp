/**
 * Doit.cpp
 * Doit is a basic shell that supports background tasks.
 * 
 * Doit can be run in two modes. Simply calling <code>./doit [command]</code>
 * with a command will execute that command and then exit. When it exits, 
 * it will print statistics about the command that was just run. An example 
 * is given below:
 * 
 *     vagrant@vagrant-ubuntu-vivid-64:/vagrant$ ./doit ls
 *     bootstrap.sh  doit  doit.cpp  LICENSE  Makefile  myexec.cpp 
 *     Wall Clock Time:                      1
 *     User CPU Time:                        0
 *     System CPU Time:                      0
 *     Max RSS:                           2528
 *     Integral Shared Memory Size:          0
 *     Integral Unshared Data Size:          0
 *     Integral Unshared Stack Size:         0
 *     Page Reclaims:                      121
 *     Page Faults:                          0
 *     Swaps:                                0
 *     Block Input Operations:               0
 *     Block Output Operations:              0
 *     IPC Messages Sent:                    0
 *     IPC Messages Received:                0
 *     Signals Received:                     0
 *     Voluntary Context Switches:           8
 *     Involuntary Context Switches:         2
 * 
 * The second mode for doit is to run as a very basic shell. Executing 
 * <code>./doit</code> will bring up a standard-looking command prompt:
 * 
 *     vagrant@vagrant-ubuntu-vivid-64:/vagrant$
 * 
 * From there you can interact with doit like you would any standard 
 * shell, with the caveat that many shell functions (pipes, >>, etc.) 
 * will not work. Doit will run any command, and allows backgrounding 
 * of those commands as well by adding an ampersand (&) to the end of 
 * a command line, as shown below.
 * 
 *     vagrant@vagrant-ubuntu-vivid-64:/vagrant$ sleep 10 &
 *     Background requested.
 *     [1] 2357 sleep 10 &
 * 
 * To get a list of currently executing jobs, use the <code>jobs</code> command.
 * 
 *     vagrant@vagrant-ubuntu-vivid-64:/vagrant$ jobs
 *     [1] 2357 sleep 10 &
 * 
 * When a job completes, the command line will print the statistics for the 
 * completed job. This works for both foreground and background jobs.
 * 
 *     [1] 2357 sleep 10 & [Finished]
 *     Wall Clock Time:                  66286
 *     User CPU Time:                        0
 *     System CPU Time:                      0
 *     Max RSS:                           2024
 *     Integral Shared Memory Size:          0
 *     Integral Unshared Data Size:          0
 *     Integral Unshared Stack Size:         0
 *     Page Reclaims:                      168
 *     Page Faults:                          0
 *     Swaps:                                0
 *     Block Input Operations:               0
 *     Block Output Operations:              0
 *     IPC Messages Sent:                    0
 *     IPC Messages Received:                0
 *     Signals Received:                     0
 *     Voluntary Context Switches:           9
 *     Involuntary Context Switches:         6
 * 
 * To change directory in the shell, use the <code>cd</code> command.
 * 
 *     vagrant@vagrant-ubuntu-vivid-64:/vagrant$ cd stuff
 *     vagrant@vagrant-ubuntu-vivid-64:/vagrant/stuff$
 * 
 * To exit the shell, use the <code>exit</code> command, or feed the 
 * shell and <code>EOF</code> character.
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
#include <string.h>
#include <sstream>
#include <pwd.h>
#include <vector>

using namespace std;
typedef struct
{
    int pid;
    string command;
    long startTime;
} process;

/**
 * Print a stat returned from the getrusage() function.
 */
void printStat(char const *stat, long val)
{
    cout << left << setw(29) << stat << right << setw(10) << val << endl;
}

/**
 * Print out process stats.
 */
void printProcStats(long start_ms)
{
    struct rusage childUsage;
    struct timeval end;
    gettimeofday(&end, NULL);
    long end_ms = end.tv_sec * 1000 + end.tv_usec / 1000;
    getrusage(RUSAGE_CHILDREN, &childUsage);
    printStat("Wall Clock Time:", end_ms - start_ms);
    printStat("User CPU Time:", childUsage.ru_utime.tv_sec * 1000 + childUsage.ru_utime.tv_usec / 1000);
    printStat("System CPU Time:", childUsage.ru_stime.tv_sec * 1000 + childUsage.ru_stime.tv_usec / 1000);
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
        int halt = 0;
        vector<process> children;

        while (!halt)
        {
            for (unsigned long i = 0; i < children.size(); i++)
            {
                int procStatus;
                pid_t result = waitpid(children.at(i).pid, &procStatus, WNOHANG);
                if (result == 0) //Child running
                {
                }
                else if (result < 0) //Error
                {
                }
                else //Child quit
                {
                    cout << "[" << i + 1 << "] " << children.at(i).pid << " " << children.at(i).command << " [Finished]" << endl;
                    printProcStats(children.at(i).startTime);
                    children.erase(children.begin() + i);
                }
                //TODO: Check any child processes, find if any are dead (http://stackoverflow.com/questions/5278582/checking-the-status-of-a-child-process-in-c)
            }
            struct passwd *passwd;
            passwd = getpwuid(getuid());
            char hostname[128];
            gethostname(hostname, sizeof hostname);
            char currentpath[256];
            getcwd(currentpath, sizeof currentpath);
            cout << passwd->pw_name << "@" << hostname << ":" << currentpath << "$ ";

            char *argvNew[32];
            string cmd;
            getline(cin, cmd);
            istringstream is(cmd);
            string part;
            int arg = 0;
            int bg = 0;
            if (cmd.length() >= 1)
            {
                while (getline(is, part, ' '))
                {
                    char *cstr = strdup(part.c_str());
                    if (strncmp(cstr, "&", 1) == 0)
                    {
                        cout << "Background requested." << endl;
                        bg = 1;
                    }
                    else
                    {
                        argvNew[arg] = cstr;
                        arg++;
                    }
                }
                argvNew[arg] = NULL;
                if (strncmp(argvNew[0], "exit", 4) == 0)
                {
                    if (children.size() > 0)
                    {
                        cout << "Waiting for background processes to complete..." << endl;
                        for (unsigned long i = 0; i < children.size(); i++)
                        {
                            int procStatus;
                            pid_t result = waitpid(children.at(i).pid, &procStatus, 0);
                            if (result == 0) //Child running
                            {
                            }
                            else if (result < 0) //Error
                            {
                            }
                            else //Child quit
                            {
                                cout << "[" << i + 1 << "] " << children.at(i).pid << " " << children.at(i).command << " [Finished]" << endl;
                                printProcStats(children.at(i).startTime);
                                children.erase(children.begin() + i);
                            }
                        }
                    }
                    exit(0);
                }
                else if (strncmp(argvNew[0], "cd", 2) == 0)
                {
                    int result = chdir(argvNew[1]);
                    if (result < 0)
                    {
                        cerr << "Error changing directory!" << endl;
                    }
                }
                else if (strncmp(argvNew[0], "jobs", 4) == 0)
                {
                    if (children.size() == 0)
                    {
                        cout << "No jobs running." << endl;
                    }
                    else
                    {
                        for (unsigned long i = 0; i < children.size(); i++)
                        {
                            cout << "[" << i + 1 << "] " << children[i].pid << " " << children[i].command << endl;
                        }
                    }
                }
                else
                {
                    int pid;
                    struct timeval start;
                    gettimeofday(&start, NULL);
                    long start_ms = start.tv_sec * 1000 + start.tv_usec / 1000;
                    if ((pid = fork()) < 0) //fork failed
                    {
                        cerr << "Fork error!" << endl;
                    }
                    else if (pid == 0) //is child
                    {
                        if (execvp(argvNew[0], argvNew) < 0)
                        {
                            cerr << "Execvp error!" << endl;
                            exit(1);
                        }
                    }
                    else //is parent
                    {
                        if (bg != 1)
                        {
                            wait(0);
                            printProcStats(start_ms);
                        }
                        else
                        {
                            process proc = {pid, cmd, start_ms};
                            children.push_back(proc);
                            cout << "[" << children.size() << "] " << children.back().pid << " " << children.back().command << endl;
                        }
                    }
                }
            }
        }
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
            wait(0);
            printProcStats(start_ms);
        }
    }
}

