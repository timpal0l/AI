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
	loc.second = loc.second + 1;
return loc;
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

	int lastPickedUpCargo = -1;
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
	bool sendInstr = false;


	// Main loop. Get info, send instructions
	while (true){			

		cout << " " << endl;
		cout << "Last picked up cargo: " << lastPickedUpCargo << endl;
		cout << "Latest cargo booked: " << latestCargoBooked << endl;
		dmc.getInformation(Time, Edges, Vans, waitingDeliveries, activeDeliveries, completedDeliveries, output);
		
		cout << "Time: " << Time << endl;

		if (!waitingDeliveries.empty()){
			cout << "Waiting delivery at node: " << waitingDeliveries[0].pickUp.first << " " << waitingDeliveries[0].pickUp.second << endl;
		}

		cout << "Position of van0: " << Vans[0].location.first << " " << Vans[0].location.second << endl;
		cout << "Position of van1: " << Vans[1].location.first << " " << Vans[1].location.second << endl;
		cout << "Position of van2: " << Vans[2].location.first << " " << Vans[2].location.second << endl;
		cout << "Position of van3: " << Vans[3].location.first << " " << Vans[3].location.second << endl;
		cout << "Position of van4: " << Vans[4].location.first << " " << Vans[4].location.second << endl;

		cout << "busyVans 0 to 5 status: " << busyVans[0] << " " << busyVans[1] << " " << busyVans[2] << " " << busyVans[3] << " " << busyVans[4] << endl;

		for (int i=0; i<5; i++){
			if (!Vans[i].instructions.empty()){
				cout << "Instructions for van" << i << ": " << Vans[i].instructions[0].first << " " << Vans[i].instructions[0].second << endl;
			}
			else{
				busyVans[i] = 0;
			}
		}

		// Is the createRoute function doing weird stuff? Instructions gets cleared for vans 2 and 3 once they have moved along the y-axis

		if (!waitingDeliveries.empty()){
			cout << "Waiting deliveries number: " << waitingDeliveries[0].Number << endl;
			// Check if nobody has been assigned this package
			if (waitingDeliveries[0].Number > latestCargoBooked){
				for (int x=0; x < waitingDeliveries.size()-1; x++){
					cout << "Waiting delivery to be picked up at " << waitingDeliveries[x].pickUp.first << " " << waitingDeliveries[x].pickUp.second << endl;
					for (int i=0; i<5; i++){
						if (busyVans[i] == 0){
							if (waitingDeliveries[waitingDeliveries.size()-1].Number > latestCargoBooked){
								// send Van[i] to pick up parcel
								loc = Vans[i].location;
								target = waitingDeliveries[x].pickUp;
								route = createRoute(loc, target);	
								Instructions.insert(make_pair(i, route));
								sendInstr = true;
								busyVans[i] = 1;
								latestCargoBooked = waitingDeliveries[0].Number;
								break;
							}
						}
					}
				}
			}
		}
		if (!activeDeliveries.empty()){
			lastPickedUpCargo = activeDeliveries[activeDeliveries.size()-1].Number;
			for (int i=0; i<=activeDeliveries.size()-1; i++){
				if (Vans[i].cargo == lastPickedUpCargo){
					if (busyVans[i] == 2){
						break;
					}
					busyVans[i] = 2;
					loc = Vans[i].location;
					for (int j=0; j<activeDeliveries.size()-1; j++){
						if (lastPickedUpCargo == activeDeliveries[j].Number){
							target = activeDeliveries[j].dropOff;
							break;
						}
					route = createRoute(loc, target);
					Instructions.insert(make_pair(i, route));
					sendInstr = true;
					}
				}
			}
			cout << "Active deliveries: Delivery #1 to be dropped off at " << activeDeliveries[0].dropOff.first << " " << activeDeliveries[0].dropOff.second << endl;
			cout << "Instructions[0][0]: inside activeDeliveries loop " <<  Instructions[0][0].first << " " << Instructions[0][0].second << endl;
		}
			
		if (!completedDeliveries.empty()){
			cout << "Completed delivery! " << endl;
		}
		// Clear everything before next iteration
		if (sendInstr){
			dmc.sendInstructions(Instructions, output);
		}
		sendInstr = false;
		reset(Vans, Instructions, waitingDeliveries, activeDeliveries, completedDeliveries, output, Edges);
	}
		return 0;
}

