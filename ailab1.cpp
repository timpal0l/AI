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

	void reset(vector<VanInfo>& Vans, map<int,vector<pair<int,int>>>& Instructions, vector<DeliveryInfo>& waitingDeliveries, vector<DeliveryInfo>& activeDeliveries, vector<std::pair<int,int>>& completedDeliveries, wstring& output, vector<vector<int>>& Edges){
		Vans.clear();
		Instructions.clear();
		waitingDeliveries.clear();
		activeDeliveries.clear();
		completedDeliveries.clear();
		output.clear();
		Edges.clear();
}

std::vector<std::pair<int,int>> createRoute(Location initialPosition, Location targetPosition) {
	std::vector<std::pair<int,int>> route;
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
	for(int i = 0; i < route.size(); i++){
		cout << route[i].first << ":" << route[i].second << endl;
	}
	return route;
}

int _tmain(int argc, _TCHAR* argv[])
{

	const std::wstring group = L"LeGroup4";							// Group name for DM_Client()
	bool OK;
	std::wstring output;											// Output string for startGame()
	
	DM_Client dmc = DM_Client(group, OK);							// Initiate DM client
	
	static const std::wstring hws = dmc.getHighayString();
	static const std::wstring s = dmc.getSuburbanString();
	static const std::wstring b = dmc.getBusinessDistrictString();
	static const std::wstring cb = dmc.getCentralBusinessDistrictString();

	
	std::vector<std::vector<std::wstring>> nodes;					// create one vector to contain the node vectors
	


	int Time = 360;													// Create all variables needed for getInformation()
	std::vector<std::vector<int>> Edges;							
	std::vector<VanInfo> Vans;
	std::vector<DeliveryInfo> waitingDeliveries;
	std::vector<DeliveryInfo> activeDeliveries;
	std::vector<std::pair<int,int>> completedDeliveries;
	//std::map<int,std::vector<std::pair<int,int>>> instructionsT;

	

	dmc.startGame(nodes, output);
	dmc.getInformation(Time, Edges, Vans, waitingDeliveries, activeDeliveries, completedDeliveries, output);


	while (true){													// Get info, send instructions

		dmc.getInformation(Time, Edges, Vans, waitingDeliveries, activeDeliveries, completedDeliveries, output);


		//wcout << "Van number: " << Vans[0].Number << "Van instructions: " << Vans[0].instructions[0].first << Vans[0].instructions[0].second << endl;	
		//wcout << Vans[0].Number << Vans[0].location.first << Vans[0].location.second << endl;		
		//std::pair <int, int> directions(40,20);
		//route.push_back(directions);
		//Instructions.insert(std::make_pair(0, route));
		//cout << Instructions[0][0].second << endl;
		std::vector<std::pair<int,int>> route;



		std::map <int,std::vector<std::pair<int,int>>> Instructions;		



		if (!waitingDeliveries.empty()){
			cout << "Waiting deliveries: " << endl;
			cout << waitingDeliveries[0].pickUp.first << " " << waitingDeliveries[0].pickUp.second << endl;
			if (waitingDeliveries.size() > 1){
				cout << waitingDeliveries[1].pickUp.first << " " << waitingDeliveries[1].pickUp.second << endl;
			}
		}
		dmc.sendInstructions(Instructions, output);

		// Clear everything before next iteration
		reset(Vans, Instructions, waitingDeliveries, activeDeliveries, completedDeliveries, output, Edges);

		}


		return 0;
}
