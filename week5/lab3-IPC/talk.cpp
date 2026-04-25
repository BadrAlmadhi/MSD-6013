/*
 * Name: Badr Almadhi
 * Date: April 2026
 * Class: CS 6013
 * Assignment: Lab 3 - IPC
 * Overview:
 * Parent and child communicate using a pipe. The parent repeatedly asks
 * the user for a message and sends it to the child. The child reads and
 * prints each message, then acknowledges the parent using a signal.
 * The program ends when the user types "quit".
 */

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <cstdlib>
#include <csignal>

using namespace std;

// Global flag changed by signal handler.
// volatile sig_atomic_t is safe for signal communication.
volatile sig_atomic_t gotAck = 0;

// This function runs when parent receives SIGUSR1 from child.
void ackHandler(int sig) {
    gotAck = 1;
}

// Child reads messages from parent through the pipe.
void childCode(int readFd, pid_t parentPid) {
    cout << "Child: started\n";

    bool done = false;

    while (!done) {
        int length = 0;

        // Read message length first.
        if (read(readFd, &length, sizeof(length)) < 0) {
            perror("Child read length failed");
            exit(1);
        }

        char msg[1024];

        // Read actual message.
        if (read(readFd, msg, length) < 0) {
            perror("Child read message failed");
            exit(1);
        }

        cout << "Child thinks message has length of " << length << endl;
        cout << "Child: " << msg << endl;

        // Acknowledge parent after receiving message.
        if (kill(parentPid, SIGUSR1) < 0) {
            perror("Child signal failed");
            exit(1);
        }

        // Quit handling.
        if (string(msg) == "quit") {
            done = true;
        }
    }

    cout << "Child: exiting\n";
    exit(0);
}

// Parent repeatedly gets user input and sends it to child.
void parentCode(int writeFd) {
    cout << "Parent: started\n";

    bool done = false;

    while (!done) {
        string message;

        cout << "Parent: enter message: ";
        getline(cin, message);

        int len = message.length() + 1; // +1 for null terminator

        // Send message length first.
        if (write(writeFd, &len, sizeof(len)) < 0) {
            perror("Parent write length failed");
            exit(1);
        }

        // Send actual message.
        if (write(writeFd, message.c_str(), len) < 0) {
            perror("Parent write message failed");
            exit(1);
        }

        cout << "Parent: done sending message\n";

        // Wait until child sends acknowledgement signal.
        gotAck = 0;
        while (!gotAck) {
            pause();
        }

        cout << "Parent: child acknowledged message\n";

        if (message == "quit") {
            done = true;
        }
    }

    // Wait for child process to exit.
    int status;
    wait(&status);

    cout << "Parent: Child is done. Goodbye\n";
    exit(0);
}

int main(int argc, char *argv[]) {
    // This version is interactive, so no command-line message is needed.
    if (argc != 1) {
        cout << "Usage: " << argv[0] << endl;
        exit(1);
    }

    // Tell parent what function to run when SIGUSR1 arrives.
    signal(SIGUSR1, ackHandler);

    int fds[2];

    // Create pipe before fork so parent and child share it.
    if (pipe(fds) < 0) {
        perror("pipe failed");
        exit(1);
    }

    int readFd = fds[0];
    int writeFd = fds[1];

    pid_t rc = fork();

    if (rc < 0) {
        perror("fork failed");
        exit(1);
    } else if (rc == 0) {
        // Child only reads, so close write side.
        close(writeFd);
        childCode(readFd, getppid());
    } else {
        // Parent only writes, so close read side.
        close(readFd);
        parentCode(writeFd);
    }

    return 0;
}