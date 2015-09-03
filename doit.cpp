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
 * To exit the shell, use the <code>exit</code> command.
 * 
 *
 * This shell is missing many of the functions and conveniences that normal
 * shells have. For instance, this shell cannot immediately print statistics 
 * for a background task when it actually finishes, something that zsh can 
 * do. Zsh most likely uses pthreads to achieve that, which is outside
 * of the scope of this project. Normal shells also have support for the 
 * pipe (|) and directing output to files (>>), neither of which are supported
 * by doit. The biggest feature missing (and most annoying) is the fact that 
 * normal shells allow for users to cycle through previously input commands
 * using the arrow keys. This shell does not support that. It also has a 
 * fixed prompt line, whereas most other shells allow users to customize
 * the prompt.
 *
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
/**
 * This struct holds information about a running process. It holds
 * the pid of the process, the command that was run, and the time
 * (wall clock time) that the process was started at.
 */
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
 * This is the main function of the doit program. This function determines
 * which mode to run the shell in, and then executes that mode. It does this
 * by determining how many arguments the program has. If there is only one
 * argument, then the user is attempting to run the program in shell mode. If 
 * there are more than one, then the user wants the program to simply run the
 * input command, print the stats, and then exit.
 *
 * @param argc The argument count.
 * @param argv Program arguments as a string.
 */
int main(int argc, char **argv)
{
    if (argc == 1)
    {
        cout << "Executing as shell..." << endl;
        vector<process> children;

        while (1)
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
            
            //This section of code generates the prompt line. I wanted it to look like a standard shell prompt, so this extracts the 
            //username, the hostname, and the currently running path and uses those to construct the prompt.
            struct passwd *passwd;
            passwd = getpwuid(getuid());
            char hostname[128];
            gethostname(hostname, sizeof hostname);
            char currentpath[256];
            getcwd(currentpath, sizeof currentpath);
            cout << passwd->pw_name << "@" << hostname << ":" << currentpath << "$ ";
            
            //Create an array of 32 commands to fit assignment spec
            char *argvNew[32];
            string cmd;
            getline(cin, cmd); //Get a line of input from user
            istringstream is(cmd); //Create stringstream for parsing into arguments
            string part;
            int arg = 0;
            int bg = 0;
            if (cmd.length() >= 1)
            {
                while (getline(is, part, ' ')) //Using getline to parse string, delimited by space characters
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
                if (strncmp(argvNew[0], "exit", 4) == 0) //parse out exit command
                {
                    if (children.size() > 0) //if there are any children left, wait for them to complete.
                    {
                        cout << "Waiting for background processes to complete..." << endl;
                        for (unsigned long i = 0; i < children.size(); i++) //loop through children and wait each of them
                        {
                            int procStatus;
                            pid_t result = waitpid(children.at(i).pid, &procStatus, 0);
                            if (result == 0) //Child running
                            {
                            }
                            else if (result < 0) //Error
                            {
                            }
                            else //Child quit, print statistics
                            {
                                cout << "[" << i + 1 << "] " << children.at(i).pid << " " << children.at(i).command << " [Finished]" << endl;
                                printProcStats(children.at(i).startTime);
                                children.erase(children.begin() + i);
                            }
                        }
                    }
                    exit(0); //terminate program
                }
                else if (strncmp(argvNew[0], "cd", 2) == 0) //Parse out cd command
                {
                    int result = chdir(argvNew[1]); //Change directory to first argument of cd command
                    if (result < 0)
                    {
                        cerr << "Error changing directory!" << endl;
                    }
                }
                else if (strncmp(argvNew[0], "jobs", 4) == 0) //Parse out jobs command
                {
                    if (children.size() == 0)
                    {
                        cout << "No jobs running." << endl;
                    }
                    else
                    {
                        for (unsigned long i = 0; i < children.size(); i++) //If there are any children, print out their names and information.
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
                    long start_ms = start.tv_sec * 1000 + start.tv_usec / 1000; //Calculate start time
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
                        if (bg != 1) //if not requested to be backgrounded, wait for process and then print stats.
                        {
                            wait(0);
                            printProcStats(start_ms);
                        }
                        else
                        {
                            process proc = {pid, cmd, start_ms};
                            children.push_back(proc); //Add new process to list of jobs.
                            cout << "[" << children.size() << "] " << children.back().pid << " " << children.back().command << endl; //Print that child has started.
                        }
                    }
                }
            }
        }
    }
    else if (argc > 1) //Execute input command and quit
    {
        int pid;
        struct timeval start;
        gettimeofday(&start, NULL);
        long start_ms = start.tv_sec * 1000 + start.tv_usec / 1000; //calculate start time of child
        //Exec given command
        if ((pid = fork()) < 0) //fork failed
        {
            cerr << "Fork error!" << endl;
        }
        else if (pid == 0)   //is child
        {
            char *argvNew[argc];
            for (int i = 1; i < argc; i++) //Copies argv into a new array without ./doit to feed into exec
            {
                argvNew[i - 1] = argv[i];
            }
            argvNew[argc - 1] = NULL; //argv must be null terminated
            if (execvp(argvNew[0], argvNew) < 0)
            {
                cerr << "Execvp error!" << endl;
                exit(1);
            }
        }
        else //is parent
        {
            wait(0); //wait for child to finish before printing stats
            printProcStats(start_ms);
        }
    }
}

