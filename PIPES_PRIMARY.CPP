#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/wait.h>

int main(){
    int cpp_to_py[2]; //create a pipe for cpp --> py
    int py_to_cpp[2]; //create a pipe for py --> cpp

    //create the pipes
    if (pipe(cpp_to_py) == -1 || pipe(py_to_cpp) == -1){
        std::cerr << "Pipe failed" << std::endl;
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1){
        std::cerr << "Fork failed" << std::endl;
        return 1;
    }

    if (pid == 0) { // Child Process (Python)
        // Redirect std in to read from cpp_to_py
        dup2(cpp_to_py[0], STDIN_FILENO);
        close(cpp_to_py[1]); //Close write end
        close(cpp_to_py[0]); //Close read end after dup

        //Execute python program
        execlp("python3", "python3", "PIPES_SECONDARY.py", NULL);
        std::cerr << "Failed to execute Python" << std::endl;
        return 1;
    } else { //Parent Process (C++)
        //close unused ends
        close(cpp_to_py[0]); //Close read end
        close(py_to_cpp[1]); //Close write end

        //Read data from python continously
        char buffer[256];
        int bytes_read;
        int iterations = 0;
        while (iterations < 10 && (bytes_read = read(py_to_cpp[0], buffer, sizeof(buffer))) > 0){
            buffer[bytes_read] = '\0';
            std::cout << "C++ received: " << buffer << std::endl;
            iterations++;
        }

        //Cleanup
        close(py_to_cpp[0]);
        close(cpp_to_py[1]);
        wait(NULL);
        std::cout << "C++ finished" << std::endl;
    }
    return 0;    
}


