// Programming Lab 2: Networks
// Author: Will Moyle
// Last updated: 17/11/14

// ROUTER FUNCTION DEFINITIONS file

#include <iostream>
#include <cstdlib>
#include <string>
#include <list>

using namespace std;

#include "message.h"
#include "host.h"
#include "router.h"

list<int> Router::allRouterAddresses;
RouterList Router::allRouters;


//---------------------------------------------------------------------------------------
// ROUTER FUNCTIONS: CONNECTION TO A HOST


// START OF FUNCTION Router::connectTo(Host)
void Router::connectTo(Host &newHost)
{
  /* check the Router is valid */
  if (!isValid)
    {
      cout << "\nThis Router is not valid, so cannot be connected to a Host.\n";
      return;
    }
  
  /* check the Host is valid */
  if (!newHost.giveValidity())
    {
      cout << "\nThis Router is not valid, so cannot be connected to a Host.\n";
      return;
    }  

  /* check there is currently no direct connection between the router and the host */
  if (!isNeighbour(newHost))
    {
      Route newRoute(&newHost);

      routeTable.push_front(newRoute);
      
      RouterList tempList = newHost.giveConnectedRouters();
      tempList.push_back(this);
      newHost.setConnectedRouters(tempList);
        
      connectedHosts.push_back(&newHost);
      
      informOfConnection(newHost.giveAddress(), newRoute.giveRouteToHost());
    }
  checkDuplicateRoutes();
}
// END OF FUNCTION


// START OF FUNCTION Router::informOfConnection
void Router::informOfConnection(int newHostAddress, IntList directions)
{
  bool routeContainsRouter(false);
  int currentAddress;
  IntList newDirections = directions;
  newDirections.push_front(address);

  if (connectedRouters.size() == 0) {return;}
  
  // For each router connected to this
  for (RouterIterator it1 = connectedRouters.begin(); it1 != connectedRouters.end(); ++it1)
    {
      currentAddress = (*it1)->giveAddress();
      
      // Check that the route does not contain itself, to avoid loops
      for (IntList::iterator it2 = newDirections.begin(); it2 != newDirections.end(); ++it2)
	{
	  if(*it2 == currentAddress)
	    {
	      routeContainsRouter = true;
	    }
	}

      // Otherwise, add the route to the neighbour's route table and inform the neighbour's neighbours
      if (!routeContainsRouter)
	{
	  RouteList tempTable = (*it1)->giveRouteTable();
	  Route tempRoute;
	  tempRoute.setConnectedHost(newHostAddress);
	  bool routeAdded(false);

	  tempRoute.setRouteToHost(newDirections);
	  for (RouteIterator it3 = tempTable.begin(); it3 != tempTable.end(); ++it3)
	    {
	      if ((*it3).giveRouteToHost().size() > newDirections.size())
		{
		  tempTable.insert(it3,tempRoute);
		  routeAdded = true;
		}
	    }
	     
	  if (!routeAdded)
	    {
	      tempTable.push_back(tempRoute);
	    }
	  
	  (*it1)->setRouteTable(tempTable);
	  (*it1)->informOfConnection(newHostAddress, newDirections);
	}
      routeContainsRouter = false;
    }
}
// END OF FUNCTION


//---------------------------------------------------------------------------------------
// ROUTER FUNCTIONS: CONNECTION TO OTHER ROUTERS


// START OF FUNCTION Router::connectTo(Router)
void Router::connectTo(Router &newRouter)
{
  /* check the Routers are valid */
  if (!isValid || !newRouter.giveValidity())
    {
      cout << "\nThis Router is not valid, so cannot be connected to another Router.\n";
      return;
    }


  /* if the router isn't connected to newRouter */
  if (this != &newRouter && !isNeighbour(newRouter))
    {
      connectedRouters.push_back(&newRouter);
      RouterList tempRouterList = newRouter.giveConnectedRouters();
      tempRouterList.push_back(this);
      newRouter.setConnectedRouters(tempRouterList);
      
      shareInformation(newRouter);
      newRouter.shareInformation(*this);
    }
  
  checkDuplicateRoutes();
}
// END OF FUNCTION


// START OF FUNCTION Router::shareInformation
void Router::shareInformation(Router &neighbour)
{
  RouteIterator it1, it3;
  RouteList tempRouteTable = neighbour.giveRouteTable();
  IntList::iterator it2;
  IntList tempRouteToHost;
  Route tempRoute;
  bool routeContainsRouter(false), duplicateRouteCheck(false);
  
  for (it1 = tempRouteTable.begin(); it1 != tempRouteTable.end(); ++it1)
    {
      tempRoute = *it1;
      tempRouteToHost = tempRoute.giveRouteToHost();
      

      /* Check that the route does not contain itself, to avoid loops */
      for (it2 = tempRouteToHost.begin(); it2 != tempRouteToHost.end(); ++it2)
	{
	  if (*it2 == this->giveAddress())
	    {
	      routeContainsRouter = true;
	    }
	}
        
      /* Add the route to the route table */
      if (!routeContainsRouter)
	{
	  tempRouteToHost.push_front(neighbour.giveAddress());

	  int tempHostAddress = tempRoute.giveConnectedHost();
	  Route newRoute;
	  newRoute.setConnectedHost(tempHostAddress);
	  bool routeAdded(false);
        
	  newRoute.setRouteToHost(tempRouteToHost);
        
	  /* Check that the route doesn't already exist in the route table to avoid duplicates */
	  for (it3 = routeTable.begin(); it3 != routeTable.end(); ++it3)
	    {
	      if (newRoute.isDuplicate(*it3))
		{
		  duplicateRouteCheck = true;
		}
	    }
	  
	  if (!duplicateRouteCheck)
	    {
        
	      for (RouteIterator it3 = routeTable.begin(); it3 != routeTable.end(); ++it3)
		{
		  if ((*it3).giveRouteToHost().size() > tempRouteToHost.size())
		    {
		      routeTable.insert(it3,newRoute);
		      routeAdded = true;
		    }
		}
	     
	      if (!routeAdded)
		{
		  routeTable.push_back(newRoute);
		}
	      informOfConnection(tempHostAddress,tempRouteToHost);
	    }
	}
      
      routeContainsRouter = false;
      duplicateRouteCheck = false;
    }
}
// END OF FUNCTION


//---------------------------------------------------------------------------------------
// ROUTER FUNCTIONS: DISCONNECTION FROM HOSTS


// START OF FUNCTION Router::disconnect(Host)
void Router::disconnect(Host &oldHost)
{
  /* Do nothing is the router isn't connected to the host */
  if (!isConnected(oldHost))
    {
      return;
    }
  RouteIterator it1;
  RouterIterator it2;
  HostIterator it3;

  /* delete the router from the host's list of connected routers */
  RouterList tempRouterList = oldHost.giveConnectedRouters();
  for (it2 = tempRouterList.begin(); it2 != tempRouterList.end(); it2++)
    {
      if ((*it2)->giveAddress() == address)
        {
	  tempRouterList.erase(it2);
	  it2--;
        }
    }
  oldHost.setConnectedRouters(tempRouterList);
    
  /* delete the router from the host's list of connected routers */
  for (it3 = connectedHosts.begin(); it3 != connectedHosts.end(); it3++)
    {
      if ((*it3)->giveAddress() == oldHost.giveAddress())
        {
	  connectedHosts.erase(it3);
	  it3--;
        }
    }    
    
  /* run through the route table and delete any direct routes going to the host */
  for (it1 = routeTable.begin(); it1 != routeTable.end(); ++it1)
    {
      if ((*it1).giveConnectedHost() == oldHost.giveAddress())
	{
	  if ((*it1).giveRouteToHost().size() == 0)
	    {
	      routeTable.erase(it1);
	      it1--;
	    }
	}
    }

  /* inform the router's neighbours of the disconnection */
  informOfDisconnect(oldHost, *this);
    
  checkDuplicateRoutes();
}
// END OF FUNCTION


// START OF FUNCTION ROUTER::informOfDisconnect(Host,Router)
void Router::informOfDisconnect(Host& oldHost, Router& oldRouter)
{
  if (connectedRouters.size() == 0)
    {
      return;
    }
  
  RouteIterator it1;
  RouterIterator it2;
  bool isConnected(true);
  

  /* run through this' connected routers */
  for (it2 = connectedRouters.begin(); it2 != connectedRouters.end(); ++it2)
    {
      /* if there isn't a route between the connected route and the host then do nothing */
      if (!(*it2)->isConnected(oldHost))
	{
	  isConnected = false;
	}
      
      /* otherwise disconnect any routes to oldHost from 
         this' route table that ends with oldRouter-oldHost */
      while (isConnected)
	{
	  RouteList tempRouteTable = (*it2)->giveRouteTable();
	  for (it1 = tempRouteTable.begin(); it1 != tempRouteTable.end(); ++it1)
	    {
	      if ((*it1).giveConnectedHost() == oldHost.giveAddress())
		{
		  if ((*it1).giveRouteToHost().back() == oldRouter.giveAddress())
		    {
		      tempRouteTable.erase(it1);
		      it1--;
		    }
		}
	    }
	  isConnected = false;
	  (*it2)->setRouteTable(tempRouteTable);
	  
	  /* inform neighbours of this disconnect */
	  (*it2)->informOfDisconnect(oldHost, oldRouter);

	}
      isConnected = true;
    }
  
}
// END OF FUNCTION


//---------------------------------------------------------------------------------------
// ROUTER FUNCTIONS: DISCONNECTION FROM OTHER ROUTERS


// START OF FUNCTION Router::disconnectFrom(Router)
void Router::disconnectFrom(Router& neighbour)
{
  /* do nothing if the routers aren't connected */
  if (!isNeighbour(neighbour))
    {
      return;
    }

  RouterIterator it1;
  RouterList tempRouterList = neighbour.giveConnectedRouters();
  RouteIterator it2;
  RouteList tempRouteTable = neighbour.giveRouteTable();
  IntList tempRouteToHost;

  /* adjust both connectedRouters lists */
  for (it1 = connectedRouters.begin(); it1 != connectedRouters.end(); it1++)
    {
      if ((*it1)->giveAddress() == neighbour.giveAddress())
	{
	  connectedRouters.erase(it1);
	  it1--;
	}
    }
  for (it1 = tempRouterList.begin(); it1 != tempRouterList.end(); it1++)
    {
      if ((*it1)->giveAddress() == address)
	{
	  tempRouterList.erase(it1);
	  it1--;
	}
    }
  neighbour.setConnectedRouters(tempRouterList);

  /* adjust both route tables */
  for (it2 = routeTable.begin(); it2 != routeTable.end(); it2++)
    {
      tempRouteToHost = (*it2).giveRouteToHost();

      if (tempRouteToHost.size() > 0)
	{
	  int firstStop = tempRouteToHost.front();
	  if (firstStop == neighbour.giveAddress())
	    {
	      routeTable.erase(it2);
	      it2--;
	    }
	}

    }
  for (it2 = tempRouteTable.begin(); it2 != tempRouteTable.end(); it2++)
    {
      tempRouteToHost = (*it2).giveRouteToHost();

      if (tempRouteToHost.size() > 0)
	{
	  int firstStop = tempRouteToHost.front();
	  if (firstStop == address)
	    {
	      routeTable.erase(it2);
	      it2--;
	    }
	}
    }
  neighbour.setRouteTable(tempRouteTable);

  /* inform neighbours of the disconnection */
  informOfDisconnect(neighbour, *this);
  neighbour.informOfDisconnect(neighbour, *this);

  checkDuplicateRoutes();
}
// END OF FUNCTION



// START OF FUNCTION ROUTER::informOfDisconnect(Router,Router)
void Router::informOfDisconnect(Router& router1, Router& router2)
{
  /* Do nothing if the router isn't connected to any other routers */
  if (connectedRouters.size() == 0)
    {
      return;
    }

  RouterIterator it2;
  IntList::iterator it3, it4;
  IntList tempRouteToHost;
  RouteIterator it1;
  RouteList tempRouteTable;
  bool disconnectsMade(false);

  /* for each connected router: */
  for (it2 = connectedRouters.begin(); it2 != connectedRouters.end(); ++it2)
    {
      tempRouteTable = (*it2)->giveRouteTable();
      /* for each route in it's route table: */
      for (it1 = tempRouteTable.begin(); it1 != tempRouteTable.end(); ++it1)
	{
	  tempRouteToHost = (*it1).giveRouteToHost();
	  /* check the route isn't direct: */
	  if (tempRouteToHost.size() > 1)
	    {
	      it3 = tempRouteToHost.begin();
	      it4 = it3;
	      it4++;
	      /* run through the route to host and check for router1 and router2 next to each other */
	      for ( ; it4!=tempRouteToHost.end() ; it4++)
		{
		  if ((*it3 == router1.giveAddress()
		       && *it4 == router2.giveAddress())
		      || (*it3 == router2.giveAddress()
			  && *it4 == router1.giveAddress()))
		    {
		      tempRouteTable.erase(it1);
		      it1--;
		      disconnectsMade = true;
		    }
		  it3++;
		}
	    }	  
	}    
      (*it2)->setRouteTable(tempRouteTable);
      if (disconnectsMade)
	{
	  (*it2)->informOfDisconnect(router1, router2);
	}
      disconnectsMade = false;
    }
  
}
// END OF FUNCTION

//---------------------------------------------------------------------------------------
// ROUTER FUNCTIONS: MESSAGE TRANSFER

// START OF FUNCTION Router::transferMessage
bool Router::transferMessage(Message package)
{
  RouteIterator it;
  RouterIterator it3;
  HostIterator it2;
  IntList tempRouteToHost;
  RouterPtr tempRouter = this;
  int iteratedHost;
  bool messageReceived(false);

  /* find the intended message receipient in the route table */
  for (it = routeTable.begin(); it != routeTable.end(); it++)
    {
      iteratedHost = (*it).giveConnectedHost();
      tempRouteToHost = (*it).giveRouteToHost();
      if (iteratedHost == package.giveDestination())
	{
	  /* CASE 1: the router is directly connnected to the receipient */
	  if (tempRouteToHost.size() == 0)
	    {
	      for (it2 = connectedHosts.begin(); it2 != connectedHosts.end() ; it2++)
		{
		  if ((*it2)->giveAddress() == package.giveDestination() && !messageReceived)
		    {
		      (*it2)->receive(package);
		      messageReceived = true;
		    }
		}
	      return true;
	    }

	  /* CASE 2: the router is connected to the receipient via another router */
	  else
	    {
	      for (it3 = connectedRouters.begin(); it3 != connectedRouters.end() ; it3++)
		{
		  if ((*it3)->giveAddress() == tempRouteToHost.front())
		    {
		      tempRouter = *it3;
		    }
		}

	      cout << "\nRouter " << address << " forwarded a message";
	      cout << " to router " << tempRouter->giveAddress() << ".\n";

	      tempRouter->transferMessage(package);

	      return true;
	    }
	}
    }

  /* CASE 3: there's no route between the router and the receipient */
  cout << "\nRouting of message failed at Router " << address << ".\n";
  return false;
}
// END OF FUNCTION


//---------------------------------------------------------------------------------------
// ROUTER FUNCTIONS: TEST PRINTS


// START OF FUNCTION Router::printConnectedRouters()
void Router::printConnectedRouters()
{
  cout << "\n\tPRINTING CONNECTED ROUTERS FOR ROUTER " << address << ":";
  if (connectedRouters.size() == 0)
    {
      cout << "\n\t\tRouter " << address << " is not connected to any hosts.";
    }
  else
    {
      cout << "\n\t\tRouter " << address << " is connected to routers: ";
      for (RouterIterator it = connectedRouters.begin(); it != connectedRouters.end(); ++it)
        {
	  cout << (*it)->giveAddress() << " ";
        }
    }
}
// END OF FUNCTION


// START OF FUNCTION Router::printRouteTable
void Router::printRouteTable()
{
  cout << "\n\tPRINTING ROUTE TABLE FOR ROUTER " << address << ":";
  if (routeTable.size() > 0)
    {
      cout << "\n\t\tRouter " << address << " is connected to the following hosts: ";
      for (RouteIterator it = routeTable.begin(); it != routeTable.end(); ++it)
        {
	  cout << "\n\t\t\tHost " << (*it).giveConnectedHost() << " | Route:";
	  IntList tempRouterList = (*it).giveRouteToHost();
	  for (IntList::iterator it1 = tempRouterList.begin(); it1 != tempRouterList.end(); ++it1)
            {
	      cout << " " << *it1;
            }
        }
    }
  return;
  
}
// END OF FUNCTION


// START OF FUNCTION Router::printAllRouteTables()
void Router::printAllRouteTables()
{
  cout << "\nPRINT ALL ROUTE TABLES:";
  for (RouterIterator it = allRouters.begin(); it != allRouters.end(); ++it)
    {
      (*it)->printConnectedRouters();
      (*it)->printRouteTable();
    }
  cout << endl;
}
// END OF FUNCTION


//---------------------------------------------------------------------------------------
// ROUTER FUNCTIONS: AUXILLIARY FUNCTIONS


// START OF FUNCTION Router::checkAddress
void Router::checkAddress()
{
  list<int>::iterator it;
  for (it = allRouterAddresses.begin(); it != allRouterAddresses.end(); ++it)
    {
      // checks if the address is taken
      if (*it == address)
	{
	  cout << "\nThere already exists a Router with this address. This router is invalid.\n";
	  isValid = false;
	  return;
	}
    }
  // if address is not taken, make Router valid and add to list of all Routers
  allRouterAddresses.push_back(address);
  allRouters.push_back(this);
  isValid = true;
}
// END OF FUNCTION


// START OF FUNCTION Router::isConnected(Router)
bool Router::isNeighbour(const Router &newRouter)
{
  bool isConnected(false);
  
  for (RouterIterator it = connectedRouters.begin(); it != connectedRouters.end(); ++it)
    {
      if ((*it)->giveAddress() == newRouter.giveAddress())
        {
	  isConnected = true;
        }
    }
  return isConnected;
}
// END OF FUNCTION


// START OF FUNCTION Router::isConnected(Host)
bool Router::isConnected(const Host &newHost)
{
  if (stepsToHost(newHost.giveAddress()) >= 0) {return true;}
  else {return false;}
}
// END OF FUNCTION


// START OF FUNCTION Router::isNeighbour(Host)
bool Router::isNeighbour(const Host &newHost)
{
  if (stepsToHost(newHost.giveAddress())==0) {return true;}
  else {return false;}
}
// END OF FUNCTION


// START OF FUNCTION Router::stepsToHost
int Router::stepsToHost(int hostAddress)
{
  int distance(-1);
  RouteList tempRouteTable = giveRouteTable();
  for (RouteIterator it = tempRouteTable.begin(); it != tempRouteTable.end(); ++it)
    {
      if (it->giveConnectedHost() == hostAddress)
        {
	  distance = static_cast<int>(it->giveRouteToHost().size());
        }
    }
  return distance;
}
// END OF FUNCTION


// START OF FUNCTION Router::checkDuplicateRoutes
void Router::checkDuplicateRoutes()
{
  RouterIterator it;
  RouteIterator it1, it2;
  RouteList tempRouteTable;
  IntList::iterator it3, it4;
  IntList tempRouteToHost1, tempRouteToHost2;
  bool deleteRoute(true);
    
  for(it = allRouters.begin(); it != allRouters.end(); it++)
    {
      tempRouteTable = (*it)->giveRouteTable();
      for (it1 = tempRouteTable.begin(); it1 != tempRouteTable.end(); it1++)
	{
	  tempRouteToHost1 = (*it1).giveRouteToHost();
	  it2 = it1;
	  it2++;
	  for ( ; it2 != tempRouteTable.end(); it2++)
	    {
	      if (it2 == it1) {it2++;}
            
	      tempRouteToHost2 = (*it2).giveRouteToHost();

	      /* check if both routes go to the same host */
	      if(it1->giveConnectedHost() != it2->giveConnectedHost())
		{
		  deleteRoute = false;
		}
            
	      /* check if both routes are of the same length */
	      if(tempRouteToHost1.size() != tempRouteToHost2.size())
		{
		  deleteRoute = false;
		}
            
	      /* check the steps in the routes */
	      it3 = tempRouteToHost1.begin();
	      it4 = tempRouteToHost2.begin();
	      for ( ; it3 != tempRouteToHost1.end() ; it3++)
		{
		  if (*it3 != *it4)
		    {
		      deleteRoute = false;
		    }
		  it4++;
		}

	      /* delete the route if it is a duplicate */
	      if(deleteRoute)
		{
		  routeTable.erase(it2);
		  it2--;
		}
	      deleteRoute = true;
	    }
	  (*it)->setRouteTable(tempRouteTable);
	}
    }
}
// END OF FUNCTION


//---------------------------------------------------------------------------------------
// ROUTE FUNCTIONS

// START OF FUNCTION Route::isDuplicate
bool Route::isDuplicate(Route otherRoute)
{
  if ((connectedHost != otherRoute.giveConnectedHost())
      || (routeToHost.size() != otherRoute.giveRouteToHost().size()))
    {
      return false;
    }
  if (routeToHost.size() == 0)
    {
      return true;
    }
  if (routeToHost.size() == 1)
    {
      if (*(routeToHost.begin()) == *(otherRoute.giveRouteToHost().begin()))
        {
	  return true;
        }
      else
        {
	  return false;
        }
    }
  bool isDuplicate(true);
  IntList tempList = otherRoute.giveRouteToHost();
  IntList::iterator it1 = routeToHost.begin();
  IntList::iterator it2 = tempList.begin();
  for ( ; it2 != tempList.end() || it1 != routeToHost.end() ; ++it2)
    {
      if(*it2 != *it1)
	{
          isDuplicate = false;
	}
      it1++;
    }
    
  return isDuplicate;
}
// END OF FUNCTION
