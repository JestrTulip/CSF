#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cerr << "Usage: ./sender [server_address] [port] [username]\n";
    return 1;
  }

  std::string server_hostname;
  int server_port;
  std::string username;

  server_hostname = argv[1];
  server_port = std::stoi(argv[2]);
  username = argv[3];

  // TODO: connect to server
  Connection conn;
  conn.connect(server_hostname, server_port);
  if (!conn.is_open()) {
    fprintf(stderr, "Error: couldn't connect to server");
    return 1; 
  }

  // TODO: send slogin message
  Message login_message = {TAG_SLOGIN, username};
  
  if(!conn.send(login_message)){
    conn.close();
    fprintf(stderr, "Error: username too long");
    return 1;
  }

  if(!conn.receive(login_message)){
    conn.close();
    fprintf(stderr, "Error: %s\n", login_message.data.c_str());
    return 1;
  }
  //login_message.print(std::cout);



  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate
  std::string temp;
  Message send_message;

  //wait for user to type in a join line
  do{
    getline(std::cin, temp);
    temp.erase(0,1);
    if(temp.substr(0, temp.find(" ")) == TAG_JOIN){
        send_message = {TAG_JOIN, temp.substr(temp.find(" ")+1)};
        if(!conn.send(send_message)){
          fprintf(stderr, "Error: unable to send message");
          return 1;
        }
        if(!conn.receive(send_message)){
          fprintf(stderr, "Error: server did not accept join request");
          return 1;
        }
    } else if (temp.substr(0, temp.find(" ")) == TAG_LEAVE) {
      send_message = {TAG_LEAVE, temp.substr(temp.find(" ")+1)};
        if(!conn.send(send_message)){
          fprintf(stderr, "Error: unable to send message");
          return 1;
        }
        if(!conn.receive(send_message)){
          fprintf(stderr, "Error: unable to leave room");
        }
    } else if (temp.substr(0, temp.find(" ")) == TAG_QUIT) {
      send_message = {TAG_LEAVE, temp.substr(temp.find(" ")+1)};
        conn.send(send_message);
        conn.close();
        return 0;
    } else {
      send_message = {TAG_DELIVERY, temp};
      if(!conn.send(send_message)){
        fprintf(stderr, "Error: unable to send message");
      }
      if(!conn.receive(send_message)){
        fprintf(stderr, "Error: unable to leave room");
      }
    }
  } while(true);

}
