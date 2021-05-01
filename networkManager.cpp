#include "networkManager.hpp"

using namespace std;

NetworkManager::NetworkManager()
{
}

void NetworkManager::execute_command(string command) {
    stringstream s_stream(command);
    istream_iterator <std::string> begin(s_stream);
    istream_iterator <std::string> end;
    vector <std::string> command_tokens(begin, end);
    if (command_tokens[0] == "MySwitch") {
        // age kamtar az 3 ta argument ya bishtar bud error bedim
        create_switch(stoi(command_tokens[1]), stoi(command_tokens[2]));
    }
    else if (command_tokens[0] == "MySystem") {
        // age kamtar az 2 ta argument ya bishtar bud error bedim
        create_system(stoi(command_tokens[1]));
    }
    // else if (command_tokens[0] == "Connect")
    // else if (command_tokens[0] == "Send")
    // else if (command_tokens[0] == "Receive")    
    return;
}

void NetworkManager::create_switch(int num_of_ports, int switch_num) {
    Switch new_switch_info = Switch();

    string pipe_name = "./switch_pipe_" + to_string(switch_num);
    char* named_pipe_path = new char[pipe_name.length() + 1];
    strcpy(named_pipe_path, pipe_name.c_str());
    mkfifo(named_pipe_path, 0666);

    new_switch_info.number_of_ports = num_of_ports;
    new_switch_info.switch_number = switch_num;
    new_switch_info.pipe_path = named_pipe_path;
    cout << "Switch Created.\nNumber of ports: " << new_switch_info.number_of_ports << "\n";
    cout << "Switch Number: " << new_switch_info.switch_number << "\n";
    cout << "Switch pipe path: " << new_switch_info.pipe_path << "\n";
    switches.push_back(new_switch_info);
    
    pid_t switch_pid;
    switch_pid = fork();
    if(switch_pid == 0) {
        char **argv = new char*[5];
        
        string port_num_str = to_string(num_of_ports);
        char *port_num_ = new char[port_num_str.length() + 1];
        strcpy(port_num_, port_num_str.c_str());
        
        string switch_num_str = to_string(switch_num);
        char *switch_num_ = new char[switch_num_str.length() + 1];
        strcpy(switch_num_, switch_num_str.c_str());
        
        argv[0] = (char*) "./switch.out";
        argv[1] = switch_num_;
        argv[2] = port_num_;
        argv[3] = named_pipe_path;
        argv[4] = NULL;
        
        execv("./switch.out", argv);
        exit(0);
    }
    else if(switch_pid > 0) {
        // inja bayad ettelaat ro az named pipe e switch bekhunimo estefade konim
        // ya inke tush chizi benevisim

        // wait(NULL);
    }
    return;
}

void NetworkManager::create_system(int system_num) {
    System new_system_info = System();

    string pipe_name = "./system_pipe_" + to_string(system_num);
    char* named_pipe_path = new char[pipe_name.length() + 1];
    strcpy(named_pipe_path, pipe_name.c_str());
    mkfifo(named_pipe_path, 0666);

    new_system_info.system_number = system_num;
    new_system_info.pipe_path = named_pipe_path;
    cout << "System Created.\nSystem Number: " << new_system_info.system_number << "\n";
    cout << "System pipe path: " << new_system_info.pipe_path << "\n";
    systems.push_back(new_system_info);

    pid_t system_pid;
    system_pid = fork();
    if(system_pid == 0) {
        char **argv = new char*[4];

        string system_num_str = to_string(system_num);
        char *system_num_ = new char[system_num_str.length() + 1];
        strcpy(system_num_, system_num_str.c_str());
        
        argv[0] = (char*) "./system.out";
        argv[1] = system_num_;
        argv[2] = named_pipe_path;
        argv[3] = NULL;
        
        execv("./system.out", argv);
        exit(0);
    }
    else if(system_pid > 0) {
        // inja bayad ettelaat ro az named pipe e system bekhunimo estefade konim
        // ya inke tush chizi benevisim

        // wait(NULL);
    }

    return;
}
