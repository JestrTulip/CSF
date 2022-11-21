#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <string>

struct Message {
  // An encoded message may have at most this many characters,
  // including the trailing newline ('\n'). Note that this does
  // *not* include a NUL terminator (if one is needed to
  // temporarily store the encoded message.)
  static const unsigned MAX_LEN = 255;

  std::string tag;
  std::string data;

  Message() { }

  Message(const std::string &tag, const std::string &data)
    : tag(tag), data(data) { }

  // TODO: you could add helper functions
  Message parse_message(std::string input){

    std::string curr_tag;
    std::string curr_message;

    char * dup = strdup(input.c_str());

    char * token = strtok(dup, ":");
    if(token != NULL){ //check of existance of tag
      curr_tag = token.toString();
      token = strtok(NULL, ":");
      if (token != NULL) { //check for existance of message
        curr_message = token.toString();
      }
      curr_message = NULL;

    } else {
      curr_tag = NULL;
      curr_message = NULL;
    }

    return Message(curr_tag, curr_message);

  }

  Message parse_message(char * input){

    std::string curr_tag;
    std::string curr_message;

    char * token = strtok_r(input, ":");
    if(token != NULL){ //check of existance of tag
      curr_tag = token.toString();
      token = strtok_r(NULL, ":");
      if (token != NULL) { //check for existance of message
        curr_message = token.toString();
      }
      curr_message = NULL;

    } else {
      curr_tag = NULL;
      curr_message = NULL;
    }

    return Message(curr_tag, curr_message);
    
  }


  bool check_valid_message(Message message){
    if(message_size > MAX_LEN){
      return false;
    } else {
      return true;
    }
  }

  std::string output_message(char* buf, Message message){
    strcpy(buf, (tag + ":" + message + "\n").c_str()); //what if NULL message?
  }

  int message_size(Message message){
    return strlen(tag) + strlen(message) + 2;
  }
};

// standard message tags (note that you don't need to worry about
// "senduser" or "empty" messages)
#define TAG_ERR       "err"       // protocol error
#define TAG_OK        "ok"        // success response
#define TAG_SLOGIN    "slogin"    // register as specific user for sending
#define TAG_RLOGIN    "rlogin"    // register as specific user for receiving
#define TAG_JOIN      "join"      // join a chat room
#define TAG_LEAVE     "leave"     // leave a chat room
#define TAG_SENDALL   "sendall"   // send message to all users in chat room
#define TAG_SENDUSER  "senduser"  // send message to specific user in chat room
#define TAG_QUIT      "quit"      // quit
#define TAG_DELIVERY  "delivery"  // message delivered by server to receiving client
#define TAG_EMPTY     "empty"     // sent by server to receiving client to indicate no msgs available

#endif // MESSAGE_H
