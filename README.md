# WPI CS3013 - Assignment 1
Assignment 1 for WPI CS3013.

## Doit
Doit is a basic shell that supports background tasks.

Doit can be run in two modes. Simply calling <code>./doit [command]</code> with a command will execute that command and then exit. When it exits, it will print statistics about the command that was just run. An example is given below:

    vagrant@vagrant-ubuntu-vivid-64:/vagrant$ ./doit ls
    bootstrap.sh  doit  doit.cpp  LICENSE  Makefile  myexec.cpp  myfork.cpp  README.md  stuff  Vagrantfile
    Wall Clock Time:                      1
    User CPU Time:                        0
    System CPU Time:                      0
    Max RSS:                           2528
    Integral Shared Memory Size:          0
    Integral Unshared Data Size:          0
    Integral Unshared Stack Size:         0
    Page Reclaims:                      121
    Page Faults:                          0
    Swaps:                                0
    Block Input Operations:               0
    Block Output Operations:              0
    IPC Messages Sent:                    0
    IPC Messages Received:                0
    Signals Received:                     0
    Voluntary Context Switches:           8
    Involuntary Context Switches:         2

The second mode for doit is to run as a very basic shell. Executing <code>./doit</code> will bring up a standard-looking command prompt:

    vagrant@vagrant-ubuntu-vivid-64:/vagrant$

From there you can interact with doit like you would any standard shell, with the caveat that many shell functions (pipes, >>, etc.) will not work. Doit will run any command, and allows backgrounding of those commands as well by adding an ampersand (&) to the end of a command line, as shown below.

    vagrant@vagrant-ubuntu-vivid-64:/vagrant$ sleep 10 &
    Background requested.
    [1] 2357 sleep 10 &

To get a list of currently executing jobs, use the <code>jobs</code> command.

    vagrant@vagrant-ubuntu-vivid-64:/vagrant$ jobs
    [1] 2357 sleep 10 &

When a job completes, the command line will print the statistics for the completed job. This works for both foreground and background jobs.

    [1] 2357 sleep 10 & [Finished]
    Wall Clock Time:                  66286
    User CPU Time:                        0
    System CPU Time:                      0
    Max RSS:                           2024
    Integral Shared Memory Size:          0
    Integral Unshared Data Size:          0
    Integral Unshared Stack Size:         0
    Page Reclaims:                      168
    Page Faults:                          0
    Swaps:                                0
    Block Input Operations:               0
    Block Output Operations:              0
    IPC Messages Sent:                    0
    IPC Messages Received:                0
    Signals Received:                     0
    Voluntary Context Switches:           9
    Involuntary Context Switches:         6

To change directory in the shell, use the <code>cd</code> command.

    vagrant@vagrant-ubuntu-vivid-64:/vagrant$ cd stuff
    vagrant@vagrant-ubuntu-vivid-64:/vagrant/stuff$

To exit the shell, use the <code>exit</code> command.

