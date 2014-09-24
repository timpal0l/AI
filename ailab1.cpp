// ailab1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DeliveryManClient.h"
#pragma comment(lib,"DeliveryManClient")
#include <iostream>
#include <vector>
#include <time.h>
#include <stdlib.h>

using namespace std;

Location goDown(Location loc){
	loc.first = 2 * (loc.first) + 1;
	return loc;
}
Location goUp(Location loc){
	loc.first = 2 * (loc.first) - 1;
	return loc;
}
Location goLeft(Location loc){
	loc.first = 2 * (loc.first);
	loc.second = loc.second - 1;
return loc;
}
Location goRight(Location loc){
	loc.first = 2 * (loc.first);
	loc.second = loc.second;
return loc;
}

vector<Location> aStarRoute(Node start, Node goal){

	/*initialize the open and the closed vectors */
	vector<Node> open;
	vector<Node> closed;

	open.push_back(start);

	while(!open.empty()){
		//Get the node off the open list with the lowest f and call it node_current

		if((start.location.first == 0) && (start.location.first == 0)){
			/* topLeft */
			/* check weight of down and right only */
			std::pair<int,int> rightEgde = goRight(start.location);
		}



		//find the best route of from the connecting edges

		/*Location oneUp = createRoute(start, goUp(start));
		Location oneRight = createRoute(start, goRight(start));
		Location oneDown = createRoute(start, goDown(start));
		Location oneLeft = createRoute(start, goLeft(start));*/

	}

}
void reset(vector<VanInfo>& Vans, map<int,vector<pair<int,int>>>& Instructions, vector<DeliveryInfo>& waitingDeliveries, 
	vector<DeliveryInfo>& activeDeliveries, vector<std::pair<int,int>>& completedDeliveries, wstring& output, vector<vector<int>>& Edges){
		Vans.clear();
		Instructions.clear();
		waitingDeliveries.clear();
		activeDeliveries.clear();
		completedDeliveries.clear();
		output.clear();
		Edges.clear();
}

int closestVan(Location loc, vector<VanInfo> Vans, int busyVans[]){
	int closestVan = -1;
	int closestDistance = 100000;
	int tempDistance = -1;
	for(int i = 0; i < 5; i++){
		if(busyVans[i] == 0){
			tempDistance = ((abs(loc.first - Vans[i].location.first)) + (abs(loc.second - Vans[i].location.second)));
			if (tempDistance < closestDistance){
				closestVan = Vans[i].Number;
				closestDistance = tempDistance;
			}
		}
	}
	return closestVan;
}


// Create route, returns a vector of locations
vector<pair<int,int>> createRoute(Location initialPosition, Location targetPosition) {
	vector<pair<int,int>> route;
	while(initialPosition.first != targetPosition.first || initialPosition.second != targetPosition.second){
		if(initialPosition.first < targetPosition.first){
			route.push_back(goDown(initialPosition));
			initialPosition.first += 1;
		}
		else if(initialPosition.first > targetPosition.first){
			route.push_back(goUp(initialPosition));
			initialPosition.first -= 1;
		}
		else if(initialPosition.second < targetPosition.second){
			route.push_back(goRight(initialPosition));
			initialPosition.second += 1;
		}
		else if(initialPosition.second > targetPosition.second){
			route.push_back(goLeft(initialPosition));
			initialPosition.second -= 1;
		}
	}
	cout << "Route is: " << endl;
	int routeLength = route.size()-1;
	for (int i=0; i<routeLength; i++){
		cout << route[i].first << " " << route[i].second << endl;
	}
	return route;
}


int _tmain(int argc, _TCHAR* argv[])
{

	const std::wstring group = L"LeGroup4";							// Group name for DM_Client()
	bool OK;

	srand (time(NULL));
	
	DM_Client dmc = DM_Client(group, OK);							// Initiate DM client
	
	// Why?
	static const std::wstring hws = dmc.getHighayString();
	static const std::wstring s = dmc.getSuburbanString();
	static const std::wstring b = dmc.getBusinessDistrictString();
	static const std::wstring cb = dmc.getCentralBusinessDistrictString();

	// Initiate a vector to contain the node vectors
	std::vector<std::vector<std::wstring>> nodes;					
									
	// Initiate everything needed for getInformation()
	int Time = 360;													
	vector<std::vector<int>> Edges;							
	vector<VanInfo> Vans;
	vector<DeliveryInfo> waitingDeliveries;
	vector<DeliveryInfo> activeDeliveries;
	vector<pair<int,int>> completedDeliveries;
	wstring output;		
	map <int,std::vector<std::pair<int,int>>> Instructions;	

	// Start game, get info
	dmc.startGame(nodes, output);
	dmc.getInformation(Time, Edges, Vans, waitingDeliveries, activeDeliveries, completedDeliveries, output);

	// 0 = not busy, 1 = picking up parcel, 2 = dropping off parcel
	int busyVans[5] = {0,0,0,0,0};
	int lastCargoNumber[5] = {-1,-1,-1,-1,-1};

	int latestCargoBooked = -1;
	pair <int, int> loc;
	pair <int, int> target;
	vector<pair<int,int>> route;

	// Spread out vans (vans 2 & 3 does not move correctly)

	loc = Vans[0].location;
	target.first = 10; 
	target.second = 10;
	route = createRoute(loc, target);
	Instructions.insert(make_pair(0, route));
	target.first = 30;
	target.second = 10;
	route = createRoute(loc, target);
	Instructions.insert(make_pair(1, route));
	target.first = 10;
	target.second = 30;
	route = createRoute(loc, target);
	Instructions.insert(make_pair(2, route));
	target.first = 30;
	target.second = 30;
	route = createRoute(loc, target);
	Instructions.insert(make_pair(3, route));
	dmc.sendInstructions(Instructions, output);

	// Main loop. Get info, send instructions
	while (true){			

		cout << " " << endl;
		cout << "Latest cargo booked: " << latestCargoBooked << endl;
		dmc.getInformation(Time, Edges, Vans, waitingDeliveries, activeDeliveries, completedDeliveries, output);
		
		cout << "Time: " << Time << endl;

		if (!waitingDeliveries.empty()){
			cout << "Waiting delivery at node: ";
			for(int i = 0; i < waitingDeliveries.size(); i++){
				cout << "{" << waitingDeliveries[i].pickUp.first << "," << waitingDeliveries[i].pickUp.second << "} ";
				if(waitingDeliveries[i].Number > latestCargoBooked){
					int x = closestVan(waitingDeliveries[i].pickUp, Vans, busyVans);
					if(x == -1){
						break;
					}
					loc = Vans[x].location;
					target = waitingDeliveries[i].pickUp;
					route = createRoute(loc, target);	
					Instructions.insert(make_pair(x, route));
					busyVans[x] = 1;
					latestCargoBooked = waitingDeliveries[i].Number;
				}
			}
			cout << endl;
		}

		if(!activeDeliveries.empty()){
			cout << "Drop off cargo at node: ";
			for(int i = 0; i < activeDeliveries.size(); i++){
				cout  << "{" << activeDeliveries[i].dropOff.first << "," << activeDeliveries[i].dropOff.second << "} "; 
				for(int x = 0; x < 5; x++){
					if(busyVans[x] == 1 && Vans[x].cargo == activeDeliveries[i].Number){
						loc = Vans[x].location;
						target = activeDeliveries[i].dropOff;
						route = createRoute(loc, target);	
						Instructions.insert(make_pair(x, route));
						busyVans[x] = 2;
						lastCargoNumber[x] = activeDeliveries[i].Number;
						break;
					}
				}
			}
			cout << endl;
		}

		if(!completedDeliveries.empty()){
			cout << "Completed delivery:  ";
			for(int i = 0; i < completedDeliveries.size(); i++){
				cout << completedDeliveries[i].first << ", ";
				for(int x = 0; x < 5; x++){
					if(lastCargoNumber[x] == completedDeliveries[i].first){
						busyVans[x] = 0;
						lastCargoNumber[x] = -1;
					}
				}
			}
			cout << endl;
			if(completedDeliveries.size() == 20){
				cout << "WE ARE FUCKING DONE BIAAACH" << endl;
				break;
			}
		}

		cout << "Position of van0: " << Vans[0].location.first << " " << Vans[0].location.second << endl;
		cout << "Position of van1: " << Vans[1].location.first << " " << Vans[1].location.second << endl;
		cout << "Position of van2: " << Vans[2].location.first << " " << Vans[2].location.second << endl;
		cout << "Position of van3: " << Vans[3].location.first << " " << Vans[3].location.second << endl;
		cout << "Position of van4: " << Vans[4].location.first << " " << Vans[4].location.second << endl;

		cout << "busyVans 0 to 5 status: " << busyVans[0] << " " << busyVans[1] << " " << busyVans[2] << " " << busyVans[3] << " " << busyVans[4] << endl;
	
		// Clear everything before next iteration
		if (!Instructions.empty()){
			dmc.sendInstructions(Instructions, output);
		}
		reset(Vans, Instructions, waitingDeliveries, activeDeliveries, completedDeliveries, output, Edges);
	}
		return 0;
}

