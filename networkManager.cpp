#include "networkManager.hpp"
using namespace std;

NetworkManager::NetworkManager()
{
}

bool NetworkManager::check_arguments_num(int vector_size, int correct_num) {
    if (vector_size != correct_num + 1) {
        cout << "Wrong number of arguments!\n";
        return false;
    }
    else return true;
}

bool NetworkManager::find_system(int system_num) {
    for(int i = 0; i < systems.size(); i++)
        if (systems[i].system_number == system_num)
            return true;

    return false;
}

int NetworkManager::find_switch(int switch_num) {
    for(int i = 0; i < switches.size(); i++)
        if (switches[i].switch_number == switch_num)
            return switches[i].number_of_ports;

    return -1;
}

bool NetworkManager::check_port_availability(int switch_num, int port_num) {
    bool is_busy = false;
    for(int i = 0; i < connected_ports.size(); i++) {
        if(connected_ports[i][0] == switch_num && connected_ports[i][1] == port_num) {
            is_busy = true;
            break;
        } 
    }
    if (is_busy) {
        cout << "Port is busy! Connection unavailable.\n";
        return false;
    }
    else return true;
}

void NetworkManager::execute_command(string command) {
    stringstream s_stream(command);
    istream_iterator <std::string> begin(s_stream);
    istream_iterator <std::string> end;
    vector <std::string> command_tokens(begin, end);
    if (command_tokens.size() < 1)
        return;
    if (command_tokens[0] == "myswitch") {
        if(!check_arguments_num(command_tokens.size(), 2))
            return;

        int switch_number = stoi(command_tokens[1]);
        int number_of_ports = stoi(command_tokens[2]);
        if(find_switch(switch_number) != -1) {
            cout << "System with this number already exists!\n";
            return;
        }
        if (number_of_ports < 0) {
            cout << "Unacceptable number of ports!\n";
            return;
        }
        create_switch(switch_number, number_of_ports);
    }
    else if (command_tokens[0] == "mysystem") {
        if(!check_arguments_num(command_tokens.size(), 1))
            return;
            
        int new_system_number = stoi(command_tokens[1]);
        if(find_system(new_system_number)) {
            cout << "System with this number already exists!\n";
            return;
        }
        create_system(new_system_number);
    }
    else if (command_tokens[0] == "connect") {
        if(!check_arguments_num(command_tokens.size(), 3))
            return;

        int system_num = stoi(command_tokens[1]);
        int switch_num = stoi(command_tokens[2]);
        int port_num = stoi(command_tokens[3]);
        if(!find_system(system_num)) {
            cout << "No systems found with the entered number!\n";
            return;
        }
        int ports_count = find_switch(switch_num);
        if(ports_count == -1) {
            cout << "No switches found with the entered number!\n";
            return;
        }
        else if(ports_count < port_num || port_num < 1) {
            cout << "Port number out of range!\n";
            return;
        }
        
        if (!(check_port_availability(switch_num, port_num)))
            return;
        connect(system_num, switch_num, port_num);
    }
    else if(command_tokens[0] == "connectswitch") {
        if(!check_arguments_num(command_tokens.size(), 4))
            return;

        int switch1_num = stoi(command_tokens[1]);
        int switch2_num = stoi(command_tokens[3]);
        int port1_num = stoi(command_tokens[2]);
        int port2_num = stoi(command_tokens[4]);
        int first_ports_count = find_switch(switch1_num);
        int second_ports_count = find_switch(switch2_num);
        
        if(first_ports_count == -1 || second_ports_count == -1) {
            cout << "No switches found with the entered number!\n";
            return;
        }
        else if(first_ports_count < port1_num || port1_num < 1 || second_ports_count < port2_num || port2_num < 1) {
            cout << "Port number out of range!\n";
            return;
        }
        if (!(check_port_availability(switch1_num, port1_num) && check_port_availability(switch2_num, port2_num)))
            return;
        
        connect_switches(switch1_num, port1_num, switch2_num, port2_num);
    }
    else if (command_tokens[0] == "send") {
        // file, source, destination
        if(!check_arguments_num(command_tokens.size(), 3))
            return;
        
        int source = stoi(command_tokens[2]);
        int destination = stoi(command_tokens[3]);
        bool source_found = false;
        bool destination_found = false;

        for(int i = 0; i < systems.size(); i++) {
            if (systems[i].system_number == source)
                source_found = true;
            if (systems[i].system_number == destination)
                destination_found = true;
        }

        if(!(source_found && destination_found)) {
            cout << "No systems found with the entered number!\n";
            return;
        }
        send(command_tokens[1], command_tokens[2], command_tokens[3]);
    }
    else if (command_tokens[0] == "receive") {

    }
    else {
        cout << "Command not found!\n";
        return;
    }
    return;
}
void NetworkManager::write_on_pipe(string pipe, string message) {
    int fd = open(pipe.c_str(), O_TRUNC | O_WRONLY );
    write(fd, message.c_str(), message.size()+ 1);
    close(fd);
}

void NetworkManager::connect(int system_number, int switch_number, int port_num) {
    // Informing system that will be connected to the mentioned switch
    string system_pipe = "./manager_system_" + to_string(system_number) + ".pipe";
    string system_reading_pipe = "./switch_" + to_string(switch_number) + "_port_" + to_string(port_num) + ".pipe";
    string message = "CONNECTED_TO_SWITCH " + system_reading_pipe;
    write_on_pipe(system_pipe, message);
    cout << "Message to " << system_pipe << " : " << message << "\n";
    

    // Informing switch that will be connected to the mentioned system
    string switch_pipe = "./manager_switch_" + to_string(switch_number) + ".pipe";
    string switch_reading_pipe = "./system_" + to_string(system_number) + "_switch_" + to_string(switch_number) + "_port_" + to_string(port_num) + ".pipe";
    string message2 = "CONNECTED_TO_SYSTEM " + switch_reading_pipe;
    write_on_pipe(switch_pipe,message2);
    cout << "Message to " << switch_pipe << " : " << message2 << "\n";
    vector<int> new_busy_port = {switch_number, port_num};
    // new_busy_port.push_back(switch_number);
    // new_busy_port.push_back(port_num);
    connected_ports.push_back(new_busy_port);
    return;
}

void NetworkManager::connect_switches(int switch1, int port1, int switch2, int port2) {
    // Informing first switch that will be connected to the second switch
    string switch_pipe = "./manager_switch_" + to_string(switch1) + ".pipe";
    string first_switch_reading_pipe = "./switch_" + to_string(switch2) + "_port_" + to_string(port2) + ".pipe";
    string message = "CONNECTED_TO_SWITCH " + first_switch_reading_pipe;
    write_on_pipe(switch_pipe, message);
    cout << "Message to " << switch_pipe << " : " << message << "\n";

    // Informing second switch that will be connected to the first switch
    string switch_pipe2 = "./manager_switch_" + to_string(switch2) + ".pipe";
    string second_switch_reading_pipe = "./switch_" + to_string(switch1) + "_port_" + to_string(port1) + ".pipe";
    string message2 = "CONNECTED_TO_SWITCH " + second_switch_reading_pipe;
    write_on_pipe(switch_pipe2,message2);
    cout << "Message to " << switch_pipe2 << " : " << message2 << "\n";
    vector<int> first_busy_port = {switch1, port1};
    vector<int> second_busy_port = {switch2, port2};
    connected_ports.push_back(first_busy_port);
    connected_ports.push_back(second_busy_port);
	return;
}

void NetworkManager::send(string file_path, string source, string destination) {
    string source_pipe = "./manager_system_" + source + ".pipe";
    string message = "SEND " + file_path  + " " + destination;
    write_on_pipe(source_pipe, message);
    cout << "Message to " << source_pipe << " : " << message << "\n";
    return;
}

void NetworkManager::create_switch(int switch_num, int num_of_ports) {
    Switch new_switch_info = Switch();

    string pipe_name = "./manager_switch_" + to_string(switch_num) + ".pipe";
    mkfifo(pipe_name.c_str(), 0666);

    new_switch_info.number_of_ports = num_of_ports;
    new_switch_info.switch_number = switch_num;
    new_switch_info.pipe_path = pipe_name.c_str();
    
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
        
        argv[0] = (char*) "./switch.out";
        argv[1] = switch_num_;
        argv[2] = port_num_;
        argv[3] = NULL;
        
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

    string pipe_name = "./manager_system_" + to_string(system_num) + ".pipe";
    mkfifo(pipe_name.c_str(), 0666);
    
    new_system_info.system_number = system_num;
    new_system_info.pipe_path = pipe_name;

    systems.push_back(new_system_info);

    pid_t system_pid;
    system_pid = fork();
    if(system_pid == 0) {
        char **argv = new char*[3];

        string system_num_str = to_string(system_num);
        char *system_num_ = new char[system_num_str.length() + 1];
        strcpy(system_num_, system_num_str.c_str());
        
        argv[0] = (char*) "./system.out";
        argv[1] = system_num_;
        argv[2] = NULL;
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
