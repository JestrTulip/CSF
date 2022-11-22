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

  // TODO: connect to server
  //does buf and need to be cleared before every use?
  int fd = open_clientfd(server_hostname.c_str(), argv[2]);
  if (fd < 0) {
    fprintf(stderr, "Error: couldn't connect to server");
    return 1; 
  }

  // TODO: send rlogin and join messages (expect a response from
  //       the server for each one)

  //send and recieve login message
  Message login_message = {TAG_RLOGIN, username};
  //check if username too long?
  char buf[255];
  login_message.output_message(buf);
  rio_writen(fd, buf , login_message.message_size()); // send message to server
  rio_t rio; 
  rio_readinitb(&rio, fd); // read response from server
  ssize_t n = rio_readlineb(&rio, buf, sizeof(buf));
  Message output;
  output.parse_message(buf);
  if(output.tag == TAG_ERR){
    close(fd);
    fprintf(stderr, "Error: %s\n", output.data.c_str());
    return 1;
  } else {
    output.output_message(buf);
    printf("%s\n", buf);
  }

  std::string temp;
  Message join_message;
  while(join_message.tag != TAG_JOIN){
    std::cin >> temp;
    join_message.parse_message(temp);
  }
  //check if room name too long?
  join_message.output_message(buf);
  rio_writen(fd, buf , join_message.message_size()); // send message to server
  rio_readinitb(&rio, fd); // read response from server
  n = rio_readlineb(&rio, buf, sizeof(buf));
  output.parse_message(buf);
  if(output.tag == TAG_ERR){
    close(fd);
    fprintf(stderr, "Error: %s\n", output.data.c_str());
    return 1;
  } else {
    output.output_message(buf);
    printf("%s\n", buf);
  }


  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)
  Message delivery_message;
  while(delivery_message.data != TAG_ERR){
    rio_readinitb(&rio, fd); // read response from server
    char buf[255];
    ssize_t n = rio_readlineb(&rio, buf, sizeof(buf));
    Message delivery_message;
    delivery_message.parse_message(buf);
    if(delivery_message.tag == TAG_DELIVERY){
      fprintf(stderr, "%s/n", output.data.c_str());
    }
  }
  
  close(fd);
  fprintf(stderr, "Error: %s\n", delivery_message.data.c_str());
  return 1;

  return 0;
}
