#include <iostream>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/timeb.h>

#define MAX_CLIENTS 5
#define BUFFER_SIZE 256
#define MAXHOSTNAME 256
#define FAILURE -1


/*
 * prints a given error system call msg
 */
void print_error(std::string text) {
    fprintf(stderr, "system error: %s\n", text.c_str());
}


/*
 * Client - create a client. Stages of client:
 * 1. open a socket
 * 2. connect to a listening socket
 * 3. send MSG
 */
int Client(unsigned short portnum, char *cmd) {
    struct sockaddr_in sa;
    struct hostent *hp;
    char buffer[BUFFER_SIZE];
    char myname[MAXHOSTNAME+1];
    int client_fd;

    //initialize variables
    memset(&sa,0,sizeof(sa));
    memset(buffer, 0, sizeof(buffer));

    //get host name
    if (gethostname(myname, MAXHOSTNAME) == FAILURE) {
        print_error("gethostname failed at server.");
        exit(1);
    }

    // get host details
    if ((hp= gethostbyname (myname)) == nullptr) {
        print_error("gethostbyname failed at client.");
        exit(1);
    }

    memcpy((char *)&sa.sin_addr , hp->h_addr , 	       hp->h_length);
    sa.sin_family = hp->h_addrtype;
    sa.sin_port = htons((u_short)portnum);

    //create a socket
    if ((client_fd = socket(hp->h_addrtype, SOCK_STREAM,0)) == FAILURE) {
        print_error("creating socket failed at client.");
        exit(1);
    }

    if (connect(client_fd, (struct sockaddr *)&sa , sizeof(sa)) < 0) {
        print_error("connect failed.");
        close(client_fd);
        exit(1);
    }

    //write msg to server
    int i = 0;
    while(i < BUFFER_SIZE) {
        buffer[i] = cmd[i];
        if (cmd[i] == '/0')
            break;
        i++;
    }

    if (write(client_fd, buffer, sizeof(buffer)) == FAILURE) {
        print_error("write failed.");
        close(client_fd);
        exit(1);
    }

    close(client_fd);
    return 1;
}

/*
 * Server - create a server. Stages of server:
 * 1. open a socket
 * 2. give the socket an address to listen on
 * 3. wait for calls to that socket
 * 4. read recieved MSGs
 */
int Server(unsigned short portnum) {
    char myname[MAXHOSTNAME+1];
    char buffer[BUFFER_SIZE];
    int server_fd, client_fd, valread;
    struct sockaddr_in sa;
    struct hostent *hp;

    //initialize variables
    memset(&sa, 0, sizeof(struct sockaddr_in));

    //get host name
    if (gethostname(myname, MAXHOSTNAME) == FAILURE) {
        print_error("gethostname failed at server.");
        exit(1);
    }

    //get host details
    hp = gethostbyname(myname);
    if (hp == nullptr) {
        print_error("gethostbyname failed at server, couln't find host.");
        exit(1);
    }

    //set host details to sockaddrr_in
    memcpy(&sa.sin_addr, hp->h_addr, hp->h_length);
    sa.sin_family = hp->h_addrtype;
    sa.sin_port= htons(portnum);

    // create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        print_error("creating socket failed at server.");
        exit(1);
    }

    //bind the socket to an adrress
    if (bind(server_fd , (struct sockaddr *)&sa , sizeof(struct sockaddr_in)) == FAILURE) {
        print_error("binding socket failed.");
        close(server_fd);
        exit(1);
    }

    // wait for calls
    if (listen(server_fd, MAX_CLIENTS) == FAILURE) {
        print_error("listen socket failed.");
        close(server_fd);
        exit(1);
    }

    while (true) {
        if ((client_fd = accept(server_fd,NULL,NULL)) == FAILURE) {
            print_error("accept socket failed.");
            close(server_fd);
            exit(1);
        }

        // get cmd from client
        //read_data(client_fd, buffer); //todo: understand it
        valread = read(client_fd, buffer, BUFFER_SIZE);
        if (valread == FAILURE) {
            print_error("read failed.");
            close(server_fd);
            exit(1);
        }

        //run cmd in terminal
        if (system(buffer) == FAILURE) {
            print_error("system command failed.");
            close(server_fd);
            exit(1);
        }
    }

    close(server_fd);   //todo: is it needs to be client or server
}


int main(int argc, char* argv[]) {

    unsigned short portnum = atoi(argv[2]);
    if (!strcmp(argv[1], "client"))
        Client(portnum, (argv[3]));
    else
        Server(portnum);
    //client <port> <terminal_command_to_run>
    //server <port>


    return 0;
}
