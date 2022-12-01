#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];

  Connection conn;

  conn.connect(server_hostname, server_port);
  if (!conn.is_open()) {
    fprintf(stderr, "Error: couldn't connect to server");
    return 1; 
  }



  //send and recieve login message
  Message login_message = {TAG_RLOGIN, username};
  
  if(!conn.send(login_message)){
    conn.close();
    fprintf(stderr, "Error: username too long");
    exit(1);
  }

  if(!conn.receive(login_message)){
    std::cerr << login_message.data; 
    exit(1);
  }



  Message join_message = {TAG_JOIN, room_name};
  
  if(!conn.send(join_message)){
    conn.close();
    fprintf(stderr, "Error: username too long");
    exit(1); 
  }

  if(!conn.receive(join_message)){
    std::cerr << join_message.data; 
    exit(1);
  }


  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)
  Message delivery_message;
  while(true){ //can be done using conn last result?
    Message delivery_message;
    conn.receive(delivery_message);
    if(delivery_message.tag == TAG_DELIVERY){
      std::cout << delivery_message.data; 
    } else if (delivery_message.tag == TAG_ERR){
      std::cerr << delivery_message.data; 
    }
  }
  
  return 0;
}
