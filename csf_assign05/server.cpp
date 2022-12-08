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
struct ClientInfo {
  Server * server;
  int clientfd;
};

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

namespace {

void *worker(void *arg) {
  pthread_detach(pthread_self());

  // TODO: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)
  ClientInfo * info = (ClientInfo *) arg;
  Connection conn(info->clientfd);

  // TODO: read login message (should be tagged either with
  //       TAG_SLOGIN or TAG_RLOGIN), send response
  Message login_message;
  conn.receive(login_message);
  if(login_message.tag == TAG_SLOGIN){
    std::string username = login_message.data;
    conn.send({TAG_OK, "Sender accepted"});
    sender_handler(&conn, username, info->server);
  } else if (login_message.tag == TAG_RLOGIN){
    std::string username = login_message.data;
    conn.send({TAG_OK, "Sender accepted"});
    reciever_handler(&conn, username, info->server);
  } else {
    login_message = {TAG_ERR, "Invalid login message"};
    conn.send(login_message);
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
  pthread_mutex_init(&m_lock, NULL);
}

Server::~Server() {
  // TODO: destroy mutex
  pthread_mutex_destroy(&m_lock);
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
      ClientInfo * info = (ClientInfo *) malloc(sizeof(ClientInfo));
      *info = {this, client_fd};
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
  Guard guard(m_lock);
  if (m_rooms.find(room_name) == m_rooms.end()) {
    Room* newRoom = (Room *) malloc(sizeof(Room));
    new(newRoom) Room(room_name);
    m_rooms.insert({room_name, newRoom});
  } 
  guard.~Guard();
  return m_rooms[room_name];

}

void sender_handler(Connection * conn, std::string username, Server * server) {

  std::string room = "";

  Message incoming_message;
  while(incoming_message.tag != TAG_QUIT){
    conn->receive(incoming_message);
    /*
    if (!incoming_message.data.empty() && incoming_message.data[incoming_message.data.length()-1] == '\n') {
    incoming_message.data.erase(incoming_message.data.length()-1);
    }
    */ 
    if (incoming_message.tag == TAG_JOIN){
      room = incoming_message.data;
      conn->send(Message(TAG_OK, "room joined"));

    } else if(room == ""){
      conn->send(Message(TAG_ERR, "no room joined"));

    } else if(incoming_message.tag == TAG_LEAVE) {
      room = "";
      conn->send(Message(TAG_OK, "room left"));

    } else if(incoming_message.tag == TAG_SENDALL){
      //create broadcase message

      std::string final_payload = room + ":" + username + ":" + incoming_message.data;
      //remove all newlines from string (if any)
      size_t last_newline = final_payload.rfind('\n');
      if (last_newline != std::string::npos) {
        final_payload.erase(final_payload.begin(), final_payload.begin() + last_newline);
      }
      //broadcast message to all in room
      (server->find_or_create_room(room))->broadcast_message(username, final_payload);
      conn->send(Message(TAG_OK, "message sent"));
    } else if (incoming_message.tag == TAG_QUIT) {
      conn->send(Message(TAG_OK, "bye"));
      continue;
    } else {
      conn->send(Message(TAG_ERR, "invalid tag"));
    }
  }
}

void reciever_handler(Connection * conn, std::string username, Server * server) {
  User reciever(username);

  Message join_message;
  conn->receive(join_message); 
  if (join_message.tag == TAG_JOIN){
      (server->find_or_create_room(join_message.data))->add_member(&reciever);
      conn->send(Message(TAG_OK, "room joined"));
  }
  Message * incoming_message;
  while(true){
    //conn->receive(*incoming_message); 
    incoming_message = reciever.mqueue.dequeue();
    if(incoming_message != NULL){
      if(incoming_message->tag == TAG_QUIT){
        delete(incoming_message); 
        break;
      }
      if(!conn->send(*incoming_message)){
        delete(incoming_message); 
        break;
      }
    }
  }
  
  (server->find_or_create_room(join_message.data))->remove_member(&reciever);
}