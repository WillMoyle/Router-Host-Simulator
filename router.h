// Programming Lab 2: Networks
// Author: Will Moyle
// Last updated: 17/11/14

// ROUTER header file

#ifndef router_h
#define router_h

#include <iostream>
#include <cstdlib>
#include <string>
#include <list>

using namespace std;


// FORWARD DECLARATION:
class Route;
class Router;


// DEFINE TYPES: 
typedef Router *RouterPtr;
typedef list<RouterPtr> RouterList;
typedef RouterList::iterator RouterIterator;
typedef list<int> IntList;
typedef list<Route> RouteList;
typedef RouteList::iterator RouteIterator;


// CLASS DECLARATION: Route
class Route {
 private:
  int connectedHost;
  IntList routeToHost;
  
 public:
  
  // Route constructors overloaded to allow for 
  // different quantities of parameters to be set
  Route () : connectedHost(0) {return;}
  Route (HostPtr host) : connectedHost(host->giveAddress()) {}
  Route (HostPtr host, IntList route) : 
       connectedHost(host->giveAddress()), 
       routeToHost(route) {}
  
  // returns the connectedHost as an integer
  int giveConnectedHost() const {return connectedHost;};
  
  // sets connectedHost to the integer 'number'
  void setConnectedHost(int number) {connectedHost = number;};
  
  // returns the routeToHost as a std::list of integers
  IntList giveRouteToHost() {return routeToHost;};
  
  // sets an integer std::list, 'routeToHost;, to route parameter
  void setRouteToHost(IntList route) {routeToHost = route;};
  
  // returns true if the parameters of this Route contain the same 
  // values as the parameters of Route 'otherRoute'
  bool isDuplicate(Route otherRoute);
};


// CLASS DECLARATION: Router
class Router {
  
 private:
  int address;
  RouteList routeTable;
  RouterList connectedRouters;
  HostList connectedHosts;
  bool isValid;
  
  static list<int> allRouterAddresses;
  static RouterList allRouters;
  
  // function returns the Router's validity as a boolean
  bool giveValidity() const {return isValid;};

  // function returns the address of the router as an integer
  int giveAddress() const {return address;};
  
  // returns the router's route table as an std::list of Routes
  RouteList giveRouteTable() const {return routeTable;};
  
  // sets routeTable to the std::list of Routes 'newRouteTable'
  void setRouteTable(RouteList newRouteTable) {routeTable = newRouteTable;};
  
  // returns the std::list of Router pointers, 'connectedRouters'
  RouterList giveConnectedRouters() const {return connectedRouters;};
  
  // sets the list of connected routers to routerList
  void setConnectedRouters(RouterList routerList) {connectedRouters = routerList;};
  
  // returns the list of the router's neighbouring hosts 
  // as an std::list of pointers to Hosts
  HostList giveConnectedHosts() const {return connectedHosts;};
  
  // sets the list of 'connectedHosts' to the 
  // std::list of pointers to Hosts, 'hostList'
  void setConnectedHosts(HostList hostList) {connectedHosts = hostList;};
    
  // runs through this Router's list of connectedRouters and 
  // sets up a new route to the host newHostAddress via this router
  // with routeToHost as 'directions'
  void informOfConnection(int newHostAddress, IntList directions);

  // shares the routeTable information with another Router 'neighbour'
  // and supplements 'neighbour's' routeTable with new routes
  void shareInformation(Router &neighbour);
  
  // informs this' neighbouring routers that router1 has 
  // been disconnected from router1 and removes any routes from
  // the neighbour's route table containing a link between
  // router1 and router2
  void informOfDisconnect(Router& router1, Router& router2);

  // informs this' neighbouring routers that oldRouter has been 
  // disconnected from oldHost and removes any routes from the
  // neighbour's route table that used a link between
  // oldHost and oldRouter
  void informOfDisconnect(Host& oldHost, Router& oldRouter);
  
  // prints the list of the router's connected routers to the screen
  void printConnectedRouters();
  
  // prints the router's route table to the screen
  void printRouteTable();
  
  // Prints the route tables for all routers to the screen
  void printAllRouteTables();

  // checks the static parameter allRouterAddresses for another router with 
  // this Router's current address and prompts the user to input another address
  // if the current address is taken. Then adds the address to allRouterAddresses
  // and adds the Router to allRouters
  void checkAddress();
  
  // returns true if connectedRouters parameter contains a pointer to newRouter
  bool isNeighbour(const Router &newRouter);

  // returns true if routeTable parameter contains a Route to newHost, else returns false
  bool isConnected(const Host &newHost);

  // returns true if connectedHost parameter contains a pointer to newHost, else returns false
  bool isNeighbour(const Host &newHost);

  // if there is a route from the Router to a Host with address hostAddress, 
  // this function returns the number of elements in the shortest route.
  // The function returns -1 if they are not connected
  int stepsToHost(int hostAddress);
  
  // removes any duplicate routes from the router's route table
  void checkDuplicateRoutes();

 public:
  
 Router(int number) : address(number) {
    checkAddress();
  };
  
  // connects the router with newHost, by adding newHost to connectedHost and
  // adding this Router to the Host's connectedRouter table. Then informs the rest 
  // of the network of the connection, through the informOfConnection function
  // updating all route tables in the network
  void connectTo(Host &newHost);
  
  // connects the router with newRouter, adding router pointers to the respective
  // connectedRouters parameters, and shares the router's route tables with the
  // shareInformation function
  void connectTo(Router &newRouter);
    
  // disconnects the router from a host, updating the connectedHosts parameter and
  // the Host's connectedRouter parameter. The function also propagates this information
  // throughout the nextwork to ensure all routeTables are up to date using the
  // informOfDisconnect(Host,Router) function
  void disconnect(Host& oldHost);

  // disconnects this router from 'neighbour', updating the connectedRouters parameters for
  // each Router. The function also propagates this information
  // throughout the nextwork to ensure all routeTables are up to date using the
  // informOfDisconnect(Router,Router) function
  void disconnectFrom(Router& neighbour);

  // searches the routeTable parameter for the intended receipient of 'package'
  // If the Router is directly connected to the Host, then the function calls the Host::receive
  // function. If there exists a Route to the Host, then the next Router on the Route calls
  // calls transferMessage.
  // Function returns true if there exists some route from this Router to the receipient Host
  // Function returns false if there is no such route.
  bool transferMessage(Message package);

};

#endif
