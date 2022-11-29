#include <sstream>
#include <iostream>
#include <cctype>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "connection.h"

Connection::Connection()
  : m_fd(-1)
  , m_last_result(SUCCESS) {
}

Connection::Connection(int fd)
  : m_fd(fd)
  , m_last_result(SUCCESS) {
  rio_readinitb(&m_fdbuf, m_fd);
}

void Connection::connect(const std::string &hostname, int port) {
  m_fd = open_clientfd(hostname.c_str(), std::to_string(port).c_str());
  rio_readinitb(&m_fdbuf, m_fd);
}

Connection::~Connection() {
  // TODO: close the socket if it is open
  close(); 
}

bool Connection::is_open() const {
  // TODO: return true if the connection is open
  if(m_fd >= 0){ return true; }
  else { return false; }
}

void Connection::close() {
  // TODO: close the connection if it is open
  if(this->is_open()){
    ::close(m_fd); 
  }
}

bool Connection::send(const Message &msg) {
  // TODO: send a message
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
  if(!msg.check_valid_message()){
    m_last_result = INVALID_MSG;
    return false;
  }
  char buf[255];
  msg.output_message(buf);
  rio_writen(m_fd, buf , msg.message_size()); //return #of characters in the message if success
  //how to check if send is succesful?
  return true;
}

bool Connection::receive(Message &msg) {
  // TODO: receive a message, storing its tag and data in msg
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
  char buf[255];
  ssize_t n = rio_readlineb(&m_fdbuf, buf, sizeof(buf));
  if(n<=0) { 
    m_last_result = INVALID_MSG;
    return false;
  }
  msg.parse_message(buf);

  if (msg.tag == TAG_ERR){
    m_last_result = EOF_OR_ERROR;
    return false;
  }

  return true;

}
