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

using namespace std;

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
        cout << "Executing command..." << endl;
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
        }
    }
}

