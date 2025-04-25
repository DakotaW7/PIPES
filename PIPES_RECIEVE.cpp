#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <chrono> // Added for timing

int main() {
    const char* fifo_path = "/tmp/python_to_cpp";

    // Remove any existing pipe
    unlink(fifo_path);

    // Create the FIFO pipe
    if (mkfifo(fifo_path, 0666) != 0) {
        std::cerr << "Failed to create FIFO pipe" << std::endl;
        return 1;
    }

    std::cout << "C++ Reader: Created FIFO pipe at " << fifo_path << std::endl;

    // Launch the Python script
    std::cout << "C++ Reader: Launching Python script..." << std::endl;
    system("python3 PIPES_SEND.py &");

    // Open the FIFO for reading
    std::cout << "C++ Reader: Waiting for data..." << std::endl;
    std::ifstream fifo(fifo_path);

    if (!fifo.is_open()) {
        std::cerr << "Failed to open FIFO for reading" << std::endl;
        return 1;
    }

    std::cout << "C++ Reader: Connected to FIFO" << std::endl;

    // Timing variables
    std::chrono::steady_clock::time_point start_time, end_time;
    bool first_data_received = false;

    // Read messages
    std::string line;
    while (getline(fifo, line)) {
        if (line.empty()) {
            break;
        }
        // Record time for first data
        if (!first_data_received) {
            start_time = std::chrono::steady_clock::now();
            first_data_received = true;
        }
        std::cout << "Received: " << line << std::endl;

        // Update end time for each line
        end_time = std::chrono::steady_clock::now();
    }

    // Calculate and display time difference if data was received
    if (first_data_received) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        std::cout << "Time between first and last data: " << duration.count() << " milliseconds" << std::endl;
    } else {
        std::cout << "No data received" << std::endl;
    }

    std::cout << "End of stream" << std::endl;
    fifo.close();

    // Clean up the pipe
    unlink(fifo_path);
    return 0;
}
