// Programming Lab 2: Networks
// Author: Will Moyle
// Last updated: 17/11/14 

// HOST FUNCTION DEFINITIONS file

#include <iostream>
#include <cstdlib>
#include <string>
#include <list>

using namespace std;

#include "message.h"
#include "host.h"
#include "router.h"

list<int> Host::allHostAddresses;
HostList Host::allHosts;

// START OF FUNCTION Host::checkAddress
void Host::checkAddress()
{
  list<int>::iterator it;
  for (it = allHostAddresses.begin(); it != allHostAddresses.end(); ++it)
    {
      // checks if the address is taken
      if (*it == address)
	{
	  cout << "\nThere already exists a Host with this address. This Host is invalid.\n";
	  isValid = false;
	  return;
	}
    }
  // if address is not taken, make Host valid and add to list of all Hosts
  allHostAddresses.push_back(address);
  allHosts.push_back(this);
  isValid = true;
}
// END OF FUNCTION


// START OF FUNCTION Host::send
void Host::send(int targetHost, string messageContent)
{
  if(!isValid)
    {
      cout << "\nThis Host is invalid and cannot send a message.\n";
      return;
    }

  Message message(address, targetHost, messageContent);
  bool messageDelivered(false);
  RouterIterator it;

  cout << "\nHost " << message.giveSource() << " sent a message to Host ";
  cout << message.giveDestination() << ": " << message.giveContent() << endl;

  if (targetHost == address)
    {
      receive(message);
      return;
    }
      

  for (it = connectedRouters.begin(); 
       (it != connectedRouters.end()) && !messageDelivered ; 
       it++)
    {
      messageDelivered = (*it)->transferMessage(message);
    } 
}
// END OF FUNCTION


// START OF FUNCTION Host::receive
void Host::receive(Message package)
{  
  if (package.giveDestination() != address)
    {
      return;
    }
  cout << "\nHost " << address << " received a message from Host ";
  cout << package.giveSource() << ": " << package.giveContent() << endl;

}
// END OF FUNCTION
