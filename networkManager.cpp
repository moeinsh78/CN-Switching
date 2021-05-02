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
    else if (command_tokens[0] == "Connect") {
        connect(stoi(command_tokens[1]), stoi(command_tokens[2]), stoi(command_tokens[3]))
    }
    else if(command_tokens[0] == "ConnectSwitches") {
        //port swtich to port switch 
        connect_switches(stoi(command_tokens[1]), stoi(command_tokens[2]), stoi(command_tokens[3]), stoi(command_tokens[4]));
    }
    else if (command_tokens[0] == "Send") {
        send(command_tokens[1], stoi(command_tokens[2]), stoi(command_tokens[3]));
    }
    // else if (command_tokens[0] == "Receive")    
    return;
}

void NetworkManager::connect(int system_number, int switch_number, int port_num) {
    //each system connects to a system with a pipe named system_id_switch_id_port_id
    string new_pipe = "./system" + to_string(system_number) + "_switch_" + to_string(switch_number) + "_port_" + to_string(port_num);
    char* named_pipe_path = new char[new_pipe.length() + 1];
    strcpy(named_pipe_path, new_pipe.c_str());
    mkfifo(named_pipe_path, 0666);

    string temp = "./manager_system_" + to_string(system_number);
    char* system_pipe = new char[temp.length()];
    string msg = "CONNECTED TO SWITCH " + to_string(switch_number) + " WITH PORT " + to_string(port_num);
    char* message = new char[msg.length()];
    strcpy(message,msg.c_str());
    int _sys = open(system_pipe,O_WRONLY);
    write(_sys, message, strlen(message)+1);
    close(_sys);    

    string temp = "./manager_switch_" + to_string(switch_number);
    char* switch_pipe = new char[temp.length()];
    string msg2 = "CONNECTED TO SYSTEM " + to_string(system_number) + " WITH PORT " + to_string(port_num);
    char* message2 = new char[msg2.length()];
    strcpy(message2,msg2.c_str());
    int _switch = open(switch_pipe,O_WRONLY);
    write(_switch, message2, strlen(message2)+1);
    close(_switch); 

}
void NetworkManager::connect_switches(int port1, int switch1, int port2, int switch2) {
    string old_name_1 = "./switch_" + to_string(switch1) + "_port_" + to_string(port1);
    string old_name_2 = "./switch_" + to_string(switch2) + "_port_" + to_string(port2);
	string new_name_1 = "./swtich_" + to_string(switch1) + "_port_" + to_string(port1) + "_switch_" + to_string(switch2) + "_port_" + to_string(port2);
    string new_name_2 = "./swtich_" + to_string(switch2) + "_port_" + to_string(port2) + "_switch_" + to_string(switch1) + "_port_" + to_string(port1);


	rename((char*) old_name_1.c_str(), (char*) new_name_1.c_str());
    rename((char*) old_name_2.c_str(), (char*) new_name_2.c_str());

    string temp = "./manager_switch_" + to_string(switch1);
    char* switch_pipe = new char[temp.length()];
    string msg = "CONNECTED TO SWITCH " + to_string(switch2) + " WITH PORT " + to_string(port1);
    char* message = new char[msg.length()];
    strcpy(message,msg.c_str());
    int _switch = open(switch_pipe,O_WRONLY);
    write(_switch, message, strlen(message)+1);
    close(_switch);

    string temp2 = "./manager_switch_" + to_string(switch2);
    char* switch_pipe2 = new char[temp2.length()];
    string msg2 = "CONNECTED TO SWITCH " + to_string(switch1) + " WITH PORT " + to_string(port2);
    char* message2 = new char[msg2.length()];
    strcpy(message2,msg2.c_str());
    int _switch = open(switch_pipe2,O_WRONLY);
    write(_switch, message2, strlen(message2)+1);
    close(_switch); 
	
}
void NetworkManager::send(string file_path, int source, int destination) {
    string temp = "./manager_system_" + to_string(source);
    char* source_pipe = new char[temp.length()];
    string msg = "SEND " + file_path + " TO " + to_string(destination);
    char* message = new char[msg.length()];
    strcpy(message,msg.c_str());
    int named_pipe = open(source_pipe,O_WRONLY);
    write(named_pipe, message, strlen(message)+1);
    close(named_pipe);
}

void NetworkManager::create_switch(int num_of_ports, int switch_num) {
    Switch new_switch_info = Switch();

    string pipe_name = "./manager_switch_" + to_string(switch_num);
    char* named_pipe_path = new char[pipe_name.length() + 1];
    strcpy(named_pipe_path, pipe_name.c_str());
    mkfifo(named_pipe_path, 0666);

    //each port has a pipe named switch_id_port_id
    for(int i = 0 ; i < num_of_ports ; i++) {
        string new_pipe = "./switch_" + to_string(switch_num) + "_port_" + to_string(i+1);
        char* named_pipe_path = new char[new_pipe.length() + 1];
        strcpy(named_pipe_path, new_pipe.c_str());
        mkfifo(named_pipe_path, 0666);
    }

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
        char **argv = new char*[4];
        
        string port_num_str = to_string(num_of_ports);
        char *port_num_ = new char[port_num_str.length() + 1];
        strcpy(port_num_, port_num_str.c_str());
        
        string switch_num_str = to_string(switch_num);
        char *switch_num_ = new char[switch_num_str.length() + 1];
        strcpy(switch_num_, switch_num_str.c_str());
        
        argv[0] = (char*) "./switch";
        argv[1] = switch_num_;
        argv[2] = port_num_;
        argv[3] = NULL;
        
        execv("./switch", argv);
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

    string pipe_name = "./manager_system_" + to_string(system_num);
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
        char **argv = new char*[3];

        string system_num_str = to_string(system_num);
        char *system_num_ = new char[system_num_str.length() + 1];
        strcpy(system_num_, system_num_str.c_str());
        
        argv[0] = (char*) "./system";
        argv[1] = system_num_;
        argv[2] = NULL;
        
        execv("./system", argv);
        exit(0);
    }
    else if(system_pid > 0) {
        // inja bayad ettelaat ro az named pipe e system bekhunimo estefade konim
        // ya inke tush chizi benevisim

        // wait(NULL);
    }

    return;
}
