#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>

using namespace std;

#define STACK 8192
#define MAX_HOST_NAME 256
#define FAILURE -1

/*
 * prints a given error system call msg
 */
void print_error(std::string text) {
    fprintf(stderr, "system error: %s\n", text.c_str());
}

/*
 * Limiting the maximal number of processes to max_proc
 */
void limit_proccess(int max_proc) {

    // create a new directories
    std::string dir("/sys"), fileName;
    mkdir(dir.c_str(), 0755);
    dir += "/fs";
    mkdir(dir.c_str(), 0755);
    dir += "/cgroup";
    mkdir(dir.c_str(), 0755);
    dir += "/pids";
    mkdir(dir.c_str(), 0755);

    //write proccess id
    fileName = dir + "cgroup.procs";
    ofstream procID(fileName);
    procID << getpid();
    procID.close();

    //write limitation on number of proccess
    fileName = dir + "pids.max";
    ofstream maxProc(fileName);
    maxProc << max_proc;
    maxProc.close();
}

int child(void *arg) {

    char **params = static_cast<char **>(arg);
    int numprocess = atoi(params[3]);

    cout <<"start creating container"  <<endl;
    //change host name
    if (sethostname(params[1], strlen(params[1])) == FAILURE) {
        print_error("sethostname failed");
        exit(1);
    }

    cout << "set host name" <<endl;
    //change root directory
    if (chroot(params[2]) == FAILURE) {
        print_error("chroot failed");
        exit(1);
    }
    cout << "change root directory"  <<endl;

    //limit number of processers
    //limit_proccess(numprocess);

    //change working directory
    if (chdir(params[2]) == FAILURE) {
        print_error("chdir failed");
        printf("%s: %s\n",strerror(errno), params[2]);
        exit(1);
    }
    cout <<"change working directory"  <<endl;

    //mount the new procfs
    if (mount("proc", "/proc", "proc", 0, 0) == FAILURE) {
        print_error("mount failed");
        exit(1);
    }

    cout <<"mount" <<endl;
    //run the program
    char* argument_list[] = {params[4]};
    for (int i = 1;;i++) {
        if (!params[i + 4]) {
            argument_list[i] = NULL;
            break;
        }
        argument_list[i] = params[i + 4];
    }
    if(execvp(params[4], argument_list) == FAILURE) {
        print_error("execv failed");
        exit(1);
    }
    cout <<"run command"  <<endl;

    return 1;
}

int main(int argc, char* argv[]) {

    void* stack = malloc(STACK);
    if (!stack) {
        print_error("malloc failed");
        exit(1);
    }

    int child_pid = clone(child, stack+STACK, CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS | SIGCHLD, argv);
    if (child_pid == FAILURE) {
        print_error("clone stage failed.");
        exit(1);
    }

    //waits for a signal
    wait(NULL);

    if (umount("/proc") == FAILURE) {
        print_error("clone stage failed.");
        exit(1);
    }

    //delete all created files
}