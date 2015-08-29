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
    } else if (argc > 1)
    {
        cout << "Executing command..." << endl;
        //Exec given command
    }
}

