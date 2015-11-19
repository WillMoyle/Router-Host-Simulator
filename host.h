// Programming Lab 2: Networks
// Author: Will Moyle
// Last updated: 17/11/14

// HOST header file

#ifndef host_h
#define host_h

#include <iostream>
#include <cstdlib>
#include <string>
#include <list>

using namespace std;


// Forward declare classes:
class Host;
class Router;


// DEFINE TYPES:
typedef Host *HostPtr;
typedef list<Host*> HostList;
typedef HostList::iterator HostIterator;


// CLASS DECLARATION: Host
class Host {
  
 private:
  int address;
  list<Router*> connectedRouters;
  bool isValid;

  static list<int> allHostAddresses;
  static HostList allHosts;

  // checks the static parameter allHostAddresses for another Host with 
  // this Host's current address and prompts the user to input another address
  // if the current address is taken. Then adds the address to allHostAddresses
  // and adds the Host to allHosts
  void checkAddress(); 

 
 public:
  
  Host(int number) : address(number) {
    checkAddress();
  };
  
  // returns the host's validity as a boolean
  bool giveValidity() const {return isValid;};

  // returns the host's address as an integer
  int giveAddress() const {return address;};

  // returns a list of router pointers to all of the host's connected routers
  list<Router*> giveConnectedRouters() {return connectedRouters;};

  // sets the connectedRouters to the std::list  RouterList 'routers'
  void setConnectedRouters(list<Router*> routers) {connectedRouters = routers;};
    
  // creates a Message with string content 'messageContent' 
  // and with target host integer address 'targetHost'
  // to connected routers to get to targetHost
  void send(int targetHost,string messageContent);

  // receives a Message 'package' from a connected router 
  // and prints out an acknowledgement of the receipt to the screen
  void receive(Message package);
};

#endif
