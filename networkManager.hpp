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
    void create_switch(int switch_num, int num_of_ports);
    void create_system(int system_num);
    void connect(int system_number, int switch_number, int port);
    void connect_switches(int switch1, int port1, int switch2, int port2);
    void write_on_pipe(std::string system_pipe, std::string message);
    void send(std::string file_path, std::string source, std::string destination);
    void receive(std::string destination, std::string file, std::string source);
    int find_switch(int switch_num);
    bool find_system(int system_num);
    bool check_arguments_num(int vector_size, int correct_num);
    bool check_port_availability(int switch_num, int port_num);
private:
    std::vector<Switch> switches;    
    std::vector<System> systems;
    // std::vector< std::vector<int> > switch_connections;
    std::vector< std::vector<int> > connected_ports;
};

#endif