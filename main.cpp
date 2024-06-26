#include "lib/lmpt.h"
#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <sstream>

void handleInput(Lamport* lamport){
    std::cout << "Available commands: REQUEST, EXIT, STATUS\n";
    std::string input;
    while(true){
        std::cout << ">";
        std::cin >> input;
        if(input == "REQUEST"){
            lamport->request();
        }
        if(input == "EXIT"){
            return;
        }
        if(input == "STATUS"){
            lamport->printConfig();
        }
    }
}

int main(int argc, char* argv[]) {
    int port = 0;
    int sys_id = 0;
    std::string filename;

    //input check for port and sys_id
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " -p <PORT NO> -i <SYS ID> -f <CONFIG FILE> \n";
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if ((arg == "-p") && i + 1 < argc) {
            port = std::stoi(argv[++i]); // Convert string to int
        } else if ((arg == "-i") && i + 1 < argc) {
            sys_id = std::stoi(argv[++i]); // Convert string to int
        } else if ((arg == "-f") && i + 1 < argc) {
            filename = argv[++i]; // Convert string to int
        } else {
            std::cerr << "Usage: " << argv[0] << " -p <PORT NO> -i <SYS ID>\n";
            return 1;
        }
    }
    
    Lamport* lamport = new Lamport(sys_id, port);

    // Handle config
    std::ifstream file(filename);

    std::vector<std::string> nodeList;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int id, port;
        if (iss >> port >> id) {
            lamport->addNode(id, "127.0.0.1", port);
        }
    }

    // Initialize the listener thread
    std::thread listenerThread(&Lamport::receive, lamport);
    // Initialize the queue handler thread
    std::thread queueHandlerThread(&Lamport::handleQueue, lamport);
    // Initialize the input handler thread
    std::thread inputHandlerThread(handleInput, lamport);

    // Wait for the listener thread to finish
    listenerThread.join();
    // Wait for the queue handler thread to finish
    queueHandlerThread.join();
    // Wait for the input handler thread to finish
    inputHandlerThread.join();

    return 0;
}