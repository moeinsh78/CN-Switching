#ifndef _MANAGER_
#define _MANAGER_

#include <iostream>
#include <vector>
#include <string.h>
#include <fstream>
#include <iterator>
#include <sstream>
#include <unistd.h>
#include <fcntl.h> 
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <sys/wait.h> 

typedef struct 
{
    int switch_number;
    int number_of_ports;
    std::string pipe_path;
} Switch;

typedef struct 
{
    int system_number;
    std::string pipe_path;
} System;


class NetworkManager 
{
public:
    NetworkManager();
    void execute_command(std::string);
    void create_switch(int number_of_ports, int switch_number);
    void create_system(int system_num);
    void connect(int system_number, int switch_number, int port);
    void connect_switches(int port1, int switch1, int port2, int switch2);
    void send(string file_path, int source, int destination);

private:
    std::vector<Switch> switches;    
    std::vector<System> systems;
    std::vector<std::vector<int>> switch_connections;
};

#endif