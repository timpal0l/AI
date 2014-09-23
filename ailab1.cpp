// ailab1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DeliveryManClient.h"
#pragma comment(lib,"DeliveryManClient")
#include <iostream>
#include <vector>


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

// Print instructions for each van, just to get a clue
void printInstructions(vector<VanInfo>& Vans)

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
	return route;
}


int _tmain(int argc, _TCHAR* argv[])
{

	const std::wstring group = L"LeGroup4";							// Group name for DM_Client()
	bool OK;
	
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

	// Start game, get info
	dmc.startGame(nodes, output);
	dmc.getInformation(Time, Edges, Vans, waitingDeliveries, activeDeliveries, completedDeliveries, output);

	pair <int, int> loc = Vans[0].location;
	pair <int, int> target(30,4);
	vector<pair<int,int>> route0 = createRoute(loc, target);
	map <int,std::vector<std::pair<int,int>>> Instructions;	
	Instructions.insert(make_pair(0, route0));
	dmc.sendInstructions(Instructions, output);

	// Main loop. Get info, compute route, send instructions
	while (true){													
		

		if (!waitingDeliveries.empty()){
			cout << "Waiting deliveries: Delivery #1: to be picked up at " << waitingDeliveries[0].pickUp.first << " " << waitingDeliveries[0].pickUp.second << endl;
		}
		if (!activeDeliveries.empty()){
			cout << "Vans[0].cargo = " << Vans[0].cargo << endl;
			cout << "Active deliveries: Delivery #1 to be dropped off at " << activeDeliveries[0].dropOff.first << " " << activeDeliveries[0].dropOff.second << endl;
			pair <int, int> loc = Vans[0].location;
			pair <int, int> target = activeDeliveries[0].dropOff;
			vector<pair<int,int>> route1 = createRoute(loc, target);
			Instructions.clear();
			Instructions.insert(make_pair(0, route1));
		}
			
		if (!completedDeliveries.empty()){
			cout << "Completed delivery! " << endl;
		}
		// Clear everything before next iteration
		dmc.sendInstructions(Instructions, output);
		reset(Vans, Instructions, waitingDeliveries, activeDeliveries, completedDeliveries, output, Edges);
	}
		return 0;
}


		// wcout << "Van number: " << Vans[0].Number << "Van instructions: " << Vans[0].instructions[0].first << Vans[0].instructions[0].second << endl;	
		// wcout << Vans[0].Number << Vans[0].location.first << Vans[0].location.second << endl;		
		// std::pair <int, int> directions(40,20);
		// route.push_back(directions);
		// Instructions.insert(std::make_pair(0, route));
		// cout << Instructions[0][0].second << endl;
