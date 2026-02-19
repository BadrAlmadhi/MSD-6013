#include <unistd.h>     // fork(), execvp(), pipe(), dup2()
#include <sys/wait.h>   // waitpid()
#include <sys/types.h>  // pid_t
#include <iostream>
#include <string>
#include <vector>
#include "shelpers.hpp"
#include <cstdlib>      // exit, getenv
#include <cstdio>       // perror

using namespace std;

int main(int argc, char* argv[]) {

    string line;

    // keep running forever
    while (true) {

        // ----------------------------
        // prevent zombie processes (reap finished background jobs)
        // ----------------------------
        while (true) {
            int status = 0;
            pid_t done = waitpid(-1, &status, WNOHANG);

            if (done > 0) {
                cout << "[bg] pid " << done << " done\n";
                continue;
            }

            // done == 0  -> no finished bg jobs right now
            // done == -1 -> no children exist (also fine)
            break;
        }

        // print every time
        cout << "myshell> " << flush;

        if (!getline(cin, line)) break;
        if (line == "exit") break;

        // split input safely
        auto tokens = tokenize(line);

        // build commands (may be 1 command, or multiple with pipes)
        auto commands = getCommands(tokens);

        if (commands.empty()) {
            continue;
        }

        bool runInBackground = commands.back().background;

        // ----------------------------
        // builtin: cd (must run in parent)
        // ----------------------------
        if (commands.size() == 1 && commands[0].execName == "cd") {

            int realArgs = (int)commands[0].argv.size() - 1; // because nullptr at end
            const char* target = nullptr;

            if (realArgs == 1) { // just "cd"
                target = getenv("HOME");
                if (!target) {
                    cerr << "cd: HOME not set\n";
                    continue;
                }
            }
            else if (realArgs == 2) { // "cd path"
                target = commands[0].argv[1];
            }
            else {
                cerr << "cd: too many arguments\n";
                continue;
            }

            if (chdir(target) < 0) {
                perror("cd");
            }
            continue;
        }

        // clones process now we have parent and child two processes running
        vector<pid_t> children;
        children.reserve(commands.size());

        // fork one child per command
        for (Command &cmd : commands) {

            pid_t pid = fork();

            if (pid < 0) {
                perror("fork");
                exit(1);
            }

            // --------------------
            // child
            // --------------------
            if (pid == 0) {

                // input redirection / pipe input
                if (cmd.inputFd != STDIN_FILENO) {
                    if (dup2(cmd.inputFd, STDIN_FILENO) < 0) {
                        perror("dup2 input");
                        _exit(1);
                    }
                }

                // output redirection / pipe output
                if (cmd.outputFd != STDOUT_FILENO) {
                    if (dup2(cmd.outputFd, STDOUT_FILENO) < 0) {
                        perror("dup2 output");
                        _exit(1);
                    }
                }

                // close ALL pipe fds so processes don't hang
                for (Command &c : commands) {
                    if (c.inputFd  != STDIN_FILENO)  close(c.inputFd);
                    if (c.outputFd != STDOUT_FILENO) close(c.outputFd);
                }

                // run program
                execvp(cmd.execName.c_str(),
                       const_cast<char* const*>(cmd.argv.data()));

                perror("execvp");
                _exit(1);
            }

            // --------------------
            // parent
            // --------------------
            children.push_back(pid);
        }

        // parent must close pipe fds too
        for (Command &c : commands) {
            if (c.inputFd  != STDIN_FILENO)  close(c.inputFd);
            if (c.outputFd != STDOUT_FILENO) close(c.outputFd);
        }

        // background: do NOT wait
        if (runInBackground) {
            cout << "[bg] started pid " << children.back() << "\n";
            continue;
        }

        // foreground: wait for all children
        for (pid_t p : children) {
            int status = 0;
            if (waitpid(p, &status, 0) < 0) {
                perror("waitpid");
                exit(1);
            }
        }
    }

    return 0;
}