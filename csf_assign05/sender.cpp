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


  Connection conn;
  conn.connect(server_hostname, server_port);
  if (!conn.is_open()) {
    fprintf(stderr, "Error: couldn't connect to server\n");
    return 1; 
  }

  Message login_message = {TAG_SLOGIN, username};
  
  if(!conn.send(login_message)){
    conn.close();
    fprintf(stderr, "Error: username too long");
    exit(1);
  }

  if(!conn.receive(login_message)){
    std::cerr << login_message.data; 
    exit(1);
  }
  //login_message.print(std::cout);



  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate
  std::string temp;
  Message send_message;

  //wait for user to type in a join line
  do{
    printf("> "); 
    getline(std::cin, temp);; 
    //temp.erase(0,1);
    if(temp.substr(1, temp.find(" ") - 1) == TAG_JOIN && temp[0] == '/'){
        send_message = {TAG_JOIN, temp.substr(temp.find(" ")+1)};
        if(!conn.send(send_message)){
          fprintf(stderr, "Error: unable to send message");
          return 1;
        }
        if(!conn.receive(send_message)){
          if (send_message.tag == TAG_ERR) {
            std::cerr << send_message.data;
          } else {
            fprintf(stderr, "Error: server did not accept join request\n");
            return 1;
          }
        }
    } else if (temp.substr(1, temp.find(" ") - 1) == TAG_LEAVE && temp[0] == '/') {
      send_message = {TAG_LEAVE, "bye"};
        if(!conn.send(send_message)){
          fprintf(stderr, "Error: unable to send message");
          return 1;
        }
        if(!conn.receive(send_message)){
          if (send_message.tag == TAG_ERR) {
            std::cerr << send_message.data;
          } else {
            fprintf(stderr, "Error: unable to leave room");
            return 1;
          }
        }
    } else if (temp.substr(1, temp.find(" ") - 1) == TAG_QUIT && temp[0] == '/') {
      send_message = {TAG_QUIT, "bye"};
        if(!conn.send(send_message)){
          fprintf(stderr, "Error: unable to send quit message");
          return 1;
        }
        if(!conn.receive(send_message)){
          if (send_message.tag == TAG_ERR) {
            std::cerr << send_message.data;
          } else {
            fprintf(stderr, "Error: server did not accept quit message");
            return 1;
          }
        }
        conn.close(); 
        return 0; 
    } else {
      send_message = {TAG_SENDALL, temp};
      if(!conn.send(send_message)){
        fprintf(stderr, "Error: unable to send message");
      }
      if(!conn.receive(send_message)){
        if (send_message.tag == TAG_ERR) {
          std::cerr << send_message.data;
        } else {
          fprintf(stderr, "Error: unable to process message");
          return 1;
        }
      }
    }
  } while(true);

}
