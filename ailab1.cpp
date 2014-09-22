// Code with Maries coordinate-functions

// ailab1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DeliveryManClient.h"
#pragma comment(lib,"DeliveryManClient")
#include <iostream>
#include <vector>


using namespace std;



	// Instruct van with edge coordinates
	Location below(VanInfo& van){
		return (std::make_pair(2*van.location.first+1, van.location.second));
	}
	Location above(VanInfo& van){
		return ( std::make_pair(2*van.location.first-1, van.location.second));
	}
	Location left(VanInfo& van){
		return ( std::make_pair(2*van.location.first, van.location.second-1));
	}
	Location right(VanInfo& van){
		return ( std::make_pair(2*van.location.first, van.location.second));
	}



int _tmain(int argc, _TCHAR* argv[])
{
	const std::wstring group = L"group4";							// Group name for DM_Client()
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



		pair<int, int> edgeInstruction = left(Vans[0]);
		cout << edgeInstruction.first << " " << edgeInstruction.second << endl;
		//wcout << "Van number: " << Vans[0].Number << "Van instructions: " << Vans[0].instructions[0].first << Vans[0].instructions[0].second << endl;	
		

		//wcout << Vans[0].Number << Vans[0].location.first << Vans[0].location.second << endl;		
		//std::pair <int, int> directions(40,20);

		std::vector<std::pair<int,int>> route;

		//route.push_back(directions);

		std::map <int,std::vector<std::pair<int,int>>> Instructions;		

		//Instructions.insert(std::make_pair(0, route));

		//cout << Instructions[0][0].second << endl;

		if (!waitingDeliveries.empty()){
			cout << "Waiting deliveries: " << endl;
			cout << waitingDeliveries[0].pickUp.first << " " << waitingDeliveries[0].pickUp.second << endl;
			if (waitingDeliveries.size() > 1){
				cout << waitingDeliveries[1].pickUp.first << " " << waitingDeliveries[1].pickUp.second << endl;
			}
		}
		dmc.sendInstructions(Instructions, output);
		// Clear everything before next iteration
		Vans.clear();
		Instructions.clear();
		waitingDeliveries.clear();
		activeDeliveries.clear();
		completedDeliveries.clear();
		output.clear();
		Edges.clear();
		}


		return 0;
}
