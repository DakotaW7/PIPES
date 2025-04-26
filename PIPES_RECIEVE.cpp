#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <cstring>  // Added for strerror
#include <cerrno>   // Added for errno

int main() {
    std::cout << "C++: Starting..." << std::endl;

    // Create FIFOs if they don't exist
    const char* cmd_fifo = "/tmp/cpp_to_python";
    const char* resp_fifo = "/tmp/python_to_cpp";
    if (access(cmd_fifo, F_OK) != 0) {
        if (mkfifo(cmd_fifo, 0666) == -1) {
            std::cerr << "C++: Failed to create " << cmd_fifo << ": " << std::strerror(errno) << std::endl;
            return 1;
        }
        std::cout << "C++: Created " << cmd_fifo << std::endl;
    }
    if (access(resp_fifo, F_OK) != 0) {
        if (mkfifo(resp_fifo, 0666) == -1) {
            std::cerr << "C++: Failed to create " << resp_fifo << ": " << std::strerror(errno) << std::endl;
            return 1;
        }
        std::cout << "C++: Created " << resp_fifo << std::endl;
    }

    // Launch Python script
    std::cout << "C++: Launching Python script..." << std::endl;
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "C++: Failed to fork: " << std::strerror(errno) << std::endl;
        return 1;
    } else if (pid == 0) {
        // Child process: execute Python script
        execlp("python3", "python3", "PIPES_SEND.py", nullptr);
        std::cerr << "C++: Failed to execute Python script: " << std::strerror(errno) << std::endl;
        exit(1);
    }

    // Parent process: wait briefly for Python to start
    std::cout << "C++: Waiting for Python to start..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Open command FIFO for writing
    std::cout << "C++: Opening " << cmd_fifo << " for writing..." << std::endl;
    int cmd_fd = open(cmd_fifo, O_WRONLY);  // Removed O_NONBLOCK
    if (cmd_fd == -1) {
        std::cerr << "C++: Failed to open " << cmd_fifo << " for writing: " << std::strerror(errno) << std::endl;
        kill(pid, SIGTERM);
        return 1;
    }
    std::ofstream cmd_stream(cmd_fifo);
    if (!cmd_stream.is_open()) {
        std::cerr << "C++: Failed to open cmd_stream for " << cmd_fifo << std::endl;
        close(cmd_fd);
        kill(pid, SIGTERM);
        return 1;
    }
    cmd_stream.rdbuf()->pubsetbuf(nullptr, 0); // Disable buffering
    std::cout << "C++: Opened " << cmd_fifo << " for writing" << std::endl;

    // Open response FIFO for reading
    std::cout << "C++: Opening " << resp_fifo << " for reading..." << std::endl;
    std::ifstream resp_stream(resp_fifo);
    if (!resp_stream.is_open()) {
        std::cerr << "C++: Failed to open " << resp_fifo << " for reading: " << std::strerror(errno) << std::endl;
        cmd_stream.close();
        close(cmd_fd);
        kill(pid, SIGTERM);
        return 1;
    }
    std::cout << "C++: Opened " << resp_fifo << " for reading" << std::endl;

    // Send commands and read responses for 200 iterations
    for (int i = 0; i < 200; ++i) {
        // Send command
        std::cout << "C++: Sending SEND command..." << std::endl;
        cmd_stream << "SEND\n";
        cmd_stream.flush();
        if (cmd_stream.fail()) {
            std::cerr << "C++: Failed to write to " << cmd_fifo << std::endl;
            break;
        }

        // Read response
        std::string line;
        std::getline(resp_stream, line);
        if (!line.empty()) {
            std::cout << "C++: Received coordinates: " << line << std::endl;
        } else {
            std::cerr << "C++: Failed to read coordinates or empty response" << std::endl;
            break;
        }

        // Wait 5 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // Cleanup
    std::cout << "C++: Cleaning up..." << std::endl;
    cmd_stream.close();
    resp_stream.close();
    kill(pid, SIGTERM);
    unlink(cmd_fifo);
    unlink(resp_fifo);
    std::cout << "C++: Done" << std::endl;

    return 0;
}
