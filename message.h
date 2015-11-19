// Programming Lab 2: Networks
// Author: Will Moyle
// Last updated: 17/11/14

// MESSAGE header file

#ifndef message_h
#define message_h

#include <iostream>
#include <string>

using namespace std;


// CLASS DECLARATION: Message
class Message {
  
 private:
  int source;
  int destination;
  string content;
  
 public:
  
  // constructor creates a Message with source integer 'start',
  // destination integer 'end' and string content 'message'
 Message(int start, int end, string message) : 
  source(start), destination(end), content(message) {};

  // returns the integer parameter 'source'
  int giveSource() const {return source;};

  // returns the integer parameter 'destination'
  int giveDestination() const {return destination;};

  // returns the string parameter 'content'
  string giveContent() const {return content;};
};

#endif
