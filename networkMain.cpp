// MSc C++ Programming - Assessed Exercise No. 2 - NETWORK SIMULATOR
// Author: William Moyle
// Last modified: 17/11/14
// Main program file

// The functions in this program were written by Will Moyle as part of his MSc
// Degree in Computing Science from Imperial College, London

// The code in this main file was written by the academic staff at the university
// and is designed to test the functions written by Will Moyle

// The program simulates a network containing Routers and Hosts

#include <iostream>

#include "message.h"
#include "host.h"
#include "router.h"

using namespace std;

int main()
{    
  Host h1(1);
  Router r1(1);
  
  r1.connectTo(h1);
    
  Host h2(2);
  Router r2(2);
  
  r1.connectTo(r2);
  r2.connectTo(h2);
    
  h1.send(2, "This is a test");
  
  Router r3(3);
  Host h3(3);
  r3.connectTo(h3);
    
  Router r4(4);
  Host h4(4);
  r4.connectTo(h4);
    
  Router r5(5);
  
  r3.connectTo(r4);
  r3.connectTo(r5);
    
  Host h5(5);
  
  r5.connectTo(h5);
    
  h4.send(5, "This is another test");
    
  r5.connectTo(r4);
    
  h4.send(5, "Hello world");
  
  r2.connectTo(r3);
        
  h1.send(3, "What a wonderful world");
    
  r4.disconnect(h4);

  h2.send(4, "Is anybody out there?");
  
  r3.disconnectFrom(r5);

  h3.send(5, "How many links must a packet walk down?");
      
  return 0;
  
}

