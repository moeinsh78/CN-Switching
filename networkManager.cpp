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
    if (command_tokens[0] == "myswitch") {
        // age kamtar az 3 ta argument ya bishtar bud error bedim
        create_switch(stoi(command_tokens[1]), stoi(command_tokens[2]));
    }
    else if (command_tokens[0] == "mysystem") {
        // age kamtar az 2 ta argument ya bishtar bud error bedim
        create_system(stoi(command_tokens[1]));
    }
    else if (command_tokens[0] == "connect") {
        connect(stoi(command_tokens[1]), stoi(command_tokens[2]), stoi(command_tokens[3]));
    }
    else if(command_tokens[0] == "connectswitch") {
        //port swtich to port switch 
        connect_switches(stoi(command_tokens[1]), stoi(command_tokens[2]), stoi(command_tokens[3]), stoi(command_tokens[4]));
    }
    else if (command_tokens[0] == "send") {
        send(command_tokens[1], stoi(command_tokens[2]), stoi(command_tokens[3]));
    }
    // else if (command_tokens[0] == "Receive")    
    return;
}
void NetworkManager::write_on_pipe(string pipe, string message) {
    int fd = open(pipe.c_str(), O_WRONLY);
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
    cout << "Message to " << switch_pipe << " : " << message2 << "\n";
	return;
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
