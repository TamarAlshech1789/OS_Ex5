#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sys/stat.h>


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
    std::string dir("/sys");
    mkdir(dir.c_str(), 0755);
    dir += "/fs";
    mkdir(dir.c_str(), 0755);
    dir += "/cgroup";
    mkdir(dir.c_str(), 0755);
    dir += "/pids";
    mkdir(dir.c_str(), 0755);






}

int child(void *) {
    //todo: test this!!
    char hostname[MAX_HOST_NAME];
    memcpy(hostname, arg[1], sizeof(hostname));
    char *root_dir;
    memcpy(root_dir, arg[2], sizeof(root_dir));
    int numprocess = atoi(arg[3]);
    char *program;
    memcpy(program, arg[4], sizeof(program));
    char *const *args; //todo: need also to include the program

    //change host name
    if (sethostname(hostname, strlen(hostname)) == FAILURE) {
        print_error("sethostname failed");
        exit(1);
    }

    //change root directory
    if (chroot(root_dir) == FAILURE) {
        print_error("chroot failed");
        exit(1);
    }

    //limit number of processers

    //change working directory
    if (chdir(root_dir) == FAILURE) {
        print_error("chdir failed");
        exit(1);
    }

    //mount the new procfs
    if (mount("proc", "/proc", "proc", 0, 0) == FAILURE) {
        print_error("mount failed");
        exit(1);
    }

    //run the program
    if(execvp(program, args) == FAILURE) {
        print_error("execv failed");
        exit(1);
    }


    return 0;
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
