#include <pthread.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <set>
#include <vector>
#include <cctype>
#include <cassert>
#include "message.h"
#include "connection.h"
#include "user.h"
#include "room.h"
#include "guard.h"
#include "server.h"

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////

// TODO: add any additional data types that might be helpful
//       for implementing the Server member functions

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

namespace {

void *worker(void *arg) {
  pthread_detach(pthread_self());

  // TODO: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)
  Connection * conn = (Connection *) arg;

  // TODO: read login message (should be tagged either with
  //       TAG_SLOGIN or TAG_RLOGIN), send response
  Message login_message;
  conn->receive(login_message);
  if(login_message.tag == TAG_SLOGIN){
    std::string username = login_message.data;
    login_message = {TAG_OK, "Sender accepted"};
    conn->send(login_message);
    sender_handler(conn, username);
  } else if (login_message.tag == TAG_RLOGIN){
    std::string username = login_message.data;
    login_message = {TAG_OK, "Reciever Accepted"};
    conn->send(login_message);
    reciever_handler(conn, username);
  } else {
    login_message = {TAG_ERR, "Invalid login message"};
    conn->send(login_message);
  }

  // TODO: depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities
  //       is a good idea)


  //dont forget to deconstruct and free arg
  return nullptr;
}

}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port)
  : m_port(port)
  , m_ssock(-1) {
  // TODO: initialize mutex
}

Server::~Server() {
  // TODO: destroy mutex
}

bool Server::listen() {
  // TODO: use open_listenfd to create the server socket, return true
  //       if successful, false if not
  m_ssock = open_listenfd(std::to_string(m_port).c_str());
  if (m_ssock < 0) { return false; }
  else { return true; }
}

void Server::handle_client_requests() {
  // TODO: infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client
  while (true) {
    int client_fd = Accept(m_ssock, NULL, NULL);
    if (client_fd < 0) { fprintf(stderr, "Error: could not connect to client"); }
    else {
      Connection * info = (Connection *) malloc(sizeof(Connection));
      new(info) Connection(client_fd);
      pthread_t thr_id;
      if (pthread_create(&thr_id, NULL, worker, info) != 0) {
        fprintf(stderr, "Error: pthread_create failed");
      }
    }
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
  if (m_rooms.find(room_name) == m_rooms.end()) {
    Room* newRoom = (Room *) malloc(sizeof(Room));
    new(newRoom) Room(room_name);
    m_rooms.insert({room_name, newRoom});
  } 

  return m_rooms[room_name];
  
}

void sender_handler(Connection * conn, std::string username) {
  User sender(username);
  Message join_message;
  conn->receive(join_message);
  if(join_message.tag == TAG_JOIN){

  } else {
    join_message = {TAG_ERR, "Invalid login message"};
    conn->send(join_message);
  }
}

void reciever_handler(Connection * conn, std::string username) {
  User reciever(username);
}
