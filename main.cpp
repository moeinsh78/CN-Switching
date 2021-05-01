#include <iostream>
#include <dirent.h>
#include <fstream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <unistd.h>

#include "networkManager.hpp"

using namespace std;

int main() {
    string command;
    NetworkManager network_manager = NetworkManager();
    while(getline(cin, command)) 
        network_manager.execute_command(command);
    return 0;
}