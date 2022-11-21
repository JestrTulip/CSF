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
  int fd = open_clientfd(server_hostname, server_port);
  if (fd < 0) { fatal("Couldn't connect to server"); }

  // TODO: send rlogin and join messages (expect a response from
  //       the server for each one)

  //send and recieve login message
  Message login_message = {TAG_RLOGIN, username};
  //check if username too long?
  char buf[255];
  message_output(buf, login_message);
  rio_writen(fd, buf , message_size(login_message)); // send message to server
  rio_t rio; 
  rio_readinitb(&rio, fd); // read response from server
  char buf[255];
  ssize_t n = rio_readlineb(&rio, buf, sizeof(buf));
  Message output = parse_message(buf);
  if(output.tag == TAG_ERR){
    close(fd);
    fprintf(stderr, "Error: %s\n", output.data);
    return 1;
  } else {
    fprintf("%s\n", output_message(output));
  }

  std::string temp;
  Message join_message;
  while(join_message != TAG_JOIN){
    cin >> temp;
    join_message = parse_message(temp);
  }
  //check if room name too long?
  char buf[255];
  message_output(buf, join_message;
  rio_writen(fd, buf , message_size(join_message)); // send message to server
  rio_t rio; 
  rio_readinitb(&rio, fd); // read response from server
  char buf[255];
  ssize_t n = rio_readlineb(&rio, buf, sizeof(buf));
  Message output = parse_message(buf);
  if(output.tag == TAG_ERR){
    close(fd);
    fprintf(stderr, "Error: %s\n", output.data);
    return 1;
  } else {
    fprintf("%s\n", output_message(output));
  }


  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)
  Message delivery_message;
  while(delivery_message.data != TAG_ERR){
    rio_readinitb(&rio, fd); // read response from server
    char buf[255];
    ssize_t n = rio_readlineb(&rio, buf, sizeof(buf));
    Message delivery_message = parse_message(buf);
    if(delivery_message.tag == TAG_DELIVERY){
      fprintf(stderr, "%s/n", output.data);
    }
  } else {
    close(fd);
    fprintf(stderr, "Error: %s\n", delivery_message.data);
    return 1;
  }

  return 0;
}
