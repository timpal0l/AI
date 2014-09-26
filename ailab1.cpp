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
// GLOBAL VARIABLES ETC
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
struct Node{
	Node *parent;
	Location location;
	int f, g, h;
	pair<int, int> edgeToParent;
};
// Returns Edge
pair<int, int> goDown(Location loc){
	loc.first = 2 * (loc.first) + 1;
	return loc;
}
pair<int, int> goUp(Location loc){
	loc.first = 2 * (loc.first) - 1;
	return loc;
}
pair<int, int> goLeft(Location loc){
	loc.first = 2 * (loc.first);
	loc.second = loc.second - 1;
	return loc;
}
pair<int, int> goRight(Location loc){
	loc.first = 2 * (loc.first);
	loc.second = loc.second;
	return loc;
}
int getManhattan(Location location, Location target){
	int manhattanDistance = ((abs(location.first - target.first)) + (abs(location.second - target.second)));
	return manhattanDistance;
}	
vector<pair<int, int>> getAStarRoute(Node *node){
	vector<pair<int, int>> route;
	while (node->parent != NULL){
		route.push_back(node->edgeToParent);
		node = node->parent;
	}
	reverse(route.begin(), route.end());
	return route;
}
bool shouldWeAddNode(Node *node, vector<Node*> open, vector<Node*> closed){
	int openSize = open.size();
	for (int x=0; x<openSize; x++){
		if ((open[x]->location == node->location) && (open[x]->f < node->f)){
			return false;
		}
	}
	int closedSize = closed.size();
	for (int x=0; x<closedSize; x++){
		if ((closed[x]->location == node->location) && (closed[x]->f < node->f)){
			return false;
		}
	}
	return true;
}
void reset(){
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
			tempDistance = getManhattan(loc, Vans[i].location);
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
	return route;
}
// createNode takes parent, mainGoal and location of a neighbor,
// and returns a Node with all fields filled out correctly.
// Nodes are to be added to the open-list in aStarRoute()
Node* createNeighborNode(Node *parent, Location neighborLocation, Location mainGoal){
	Node *newNode = new Node;
	if (parent->location.first < neighborLocation.first){	
		newNode->g = parent->g + Edges[goDown(parent->location).first][goDown(parent->location).second];
	}
	else if (parent->location.first > neighborLocation.first){
		newNode->g = parent->g + Edges[goUp(parent->location).first][goUp(parent->location).second];
	}
	else if (parent->location.second < neighborLocation.second){
		newNode->g = parent->g + Edges[goRight(parent->location).first][goRight(parent->location).second];
	}
	else if (parent->location.second > neighborLocation.second){
		newNode->g = parent->g + Edges[goLeft(parent->location).first][goLeft(parent->location).second];
	}
	newNode->h = getManhattan(neighborLocation, mainGoal);
	newNode->f = newNode->g + newNode->h;
	newNode->parent = parent;
	newNode->location = neighborLocation;
	vector<pair<int,int>> singleEdgeRoute = createRoute(parent->location, neighborLocation);
	newNode->edgeToParent = singleEdgeRoute[0];
	//
	return newNode;
}
vector<pair<int, int>> aStarRoute(Location start, Location goal){
	// Initialize the open and the closed vectors
	vector<Node*> open, closed;
	vector<pair<int, int>> route;
	Node *currentNode = new Node;

	currentNode->location = start;
	currentNode->h = 0;
	currentNode->g = 0;
	currentNode->f = 0;
	currentNode->parent = NULL;
	currentNode->edgeToParent = make_pair(0,0);
	vector<Node*> neighborList;
	int leastFNodeIndex = 0;
	// put the starting node on the open list
	open.push_back(currentNode);
	// While open is not empty
	while(!open.empty()){
		int leastFValueYet = 10000;
		// Find the Node in Open[] with the least f value, call it nodeWithLeastF
		for (int i=0; i<open.size(); i++){
			if (open[i]->f < leastFValueYet){
				leastFValueYet = open[i]->f;
				leastFNodeIndex = i;
			}
		}
		Node *nodeWithLeastF = new Node;
		nodeWithLeastF = open[leastFNodeIndex];
		// Delete nodeWithLeastF from open[]
		open.erase(open.begin() + leastFNodeIndex);
		// Generate all neighbors for nodeWithLeastF
		// and set their parents to nodeWithLeastF.
		neighborList.clear();

		if (nodeWithLeastF->location.first == 0){}
		else {
			Node *upNode = new Node;
			vector<pair<int, int>> upEdge = createRoute(nodeWithLeastF->location, make_pair(nodeWithLeastF->location.first -1, nodeWithLeastF->location.second));
			upNode->location = make_pair(nodeWithLeastF->location.first -1, nodeWithLeastF->location.second);
			upNode->edgeToParent = upEdge[0];
			upNode->g = nodeWithLeastF->g + Edges[upEdge[0].first][upEdge[0].second];
			upNode->h = getManhattan(make_pair(nodeWithLeastF->location.first -1, nodeWithLeastF->location.second), goal);
			upNode->f = upNode->g + upNode->h;
			upNode->parent = nodeWithLeastF;
			neighborList.push_back(upNode);
		}
		// down neighbor
		if (nodeWithLeastF->location.first == 40){}
		else{
			Node *downNode = new Node;
			vector<pair<int, int>> downEdge = createRoute(nodeWithLeastF->location, make_pair(nodeWithLeastF->location.first +1, nodeWithLeastF->location.second));
			downNode->location = make_pair(nodeWithLeastF->location.first +1, nodeWithLeastF->location.second);
			downNode->edgeToParent = downEdge[0];
			downNode->g = nodeWithLeastF->g + Edges[downEdge[0].first][downEdge[0].second];
			downNode->h = getManhattan(make_pair(nodeWithLeastF->location.first +1, nodeWithLeastF->location.second), goal);
			downNode->f = downNode->g + downNode->h;
			downNode->parent = nodeWithLeastF;
			neighborList.push_back(downNode);
		}
		// left neighbor
		if (nodeWithLeastF->location.second == 0){}
		else{
			Node *leftNode = new Node;
			vector<pair<int, int>> leftEdge = createRoute(nodeWithLeastF->location, make_pair(nodeWithLeastF->location.first, nodeWithLeastF->location.second -1));
			leftNode->location = make_pair(nodeWithLeastF->location.first, nodeWithLeastF->location.second -1);
			leftNode->edgeToParent = leftEdge[0];
			leftNode->g = nodeWithLeastF->g + Edges[leftEdge[0].first][leftEdge[0].second];
			leftNode->h = getManhattan(make_pair(nodeWithLeastF->location.first, nodeWithLeastF->location.second -1), goal);
			leftNode->f = leftNode->g + leftNode->h;
			leftNode->parent = nodeWithLeastF;
			neighborList.push_back(leftNode);
		}
		// right neighbor
		if (nodeWithLeastF->location.second == 40){}
		else{
			Node *rightNode = new Node;
			vector<pair<int, int>> rightEdge = createRoute(nodeWithLeastF->location, make_pair(nodeWithLeastF->location.first, nodeWithLeastF->location.second +1));
			rightNode->location = make_pair(nodeWithLeastF->location.first, nodeWithLeastF->location.second +1);
			rightNode->edgeToParent = rightEdge[0];
			rightNode->g = nodeWithLeastF->g + Edges[rightEdge[0].first][rightEdge[0].second];
			rightNode->h = getManhattan(make_pair(nodeWithLeastF->location.first, nodeWithLeastF->location.second +1), goal);
			rightNode->f = rightNode->g + rightNode->h;
			rightNode->parent = nodeWithLeastF;
			neighborList.push_back(rightNode);
		}
		// up neighbor
		// neighborList = getNeighbors(nodeWithLeastF);
		for (int i=0; i<neighborList.size(); i++){
			bool addToOpen = true;
			int openSize = open.size();
			int closedSize = closed.size();
			for (int j=0; j<openSize; j++){
				if (neighborList[i]->location == goal){
					route = getAStarRoute(neighborList[i]);
					cout << "Route is: " << route[0].first << " etc etc " << endl;
					return route;
				}
				if (neighborList[i]->location == open[j]->location && neighborList[i]->f > open[j]->f){
					addToOpen = false;
				}
			}
			for (int j=0; j<closedSize; j++){
				if (neighborList[i]->location == closed[j]->location && neighborList[i]->f > closed[j]->f){
					addToOpen = false;
				}
			}
			if (addToOpen){
				open.push_back(neighborList[i]);
			}
		}
		closed.push_back(nodeWithLeastF);
	}
	return route;
}
	const std::wstring group = L"LeGroup4";	// Group name for DM_Client()
	bool OK;
	DM_Client dmc = DM_Client(group, OK);	// Initiate DM client
	// Why?
	static const std::wstring hws = dmc.getHighayString();
	static const std::wstring s = dmc.getSuburbanString();
	static const std::wstring b = dmc.getBusinessDistrictString();
	static const std::wstring cb = dmc.getCentralBusinessDistrictString();
	// Start game, get info
	// 0 = not busy, 1 = picking up parcel, 2 = dropping off parcel
	int busyVans[5] = {0,0,0,0,0};
	int lastCargoNumber[5] = {-1,-1,-1,-1,-1};
	int latestCargoBooked = -1;
	pair <int, int> loc;
	pair <int, int> target;
	vector<pair<int,int>> route;

int _tmain(int argc, _TCHAR* argv[])
{
	dmc.startGame(nodes, output);
	dmc.getInformation(Time, Edges, Vans, waitingDeliveries, activeDeliveries, completedDeliveries, output);
	srand(time(NULL));
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
		cout << "Time: " << Time << endl;
		cout << " " << endl;
		cout << "Latest cargo booked: " << latestCargoBooked << endl;
		dmc.getInformation(Time, Edges, Vans, waitingDeliveries, activeDeliveries, completedDeliveries, output);

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
					route = aStarRoute(loc, target);	
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
				cout << "{" << activeDeliveries[i].dropOff.first << "," << activeDeliveries[i].dropOff.second << "} ";
				for(int x = 0; x < 5; x++){
					if(busyVans[x] == 1 && Vans[x].cargo == activeDeliveries[i].Number){
						loc = Vans[x].location;
						target = activeDeliveries[i].dropOff;
						route = aStarRoute(loc, target);	
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
			cout << "Completed deliveries: " << completedDeliveries.size() << endl;
			for(int i = 0; i < completedDeliveries.size(); i++){
				for(int x = 0; x < 5; x++){
					if(lastCargoNumber[x] == completedDeliveries[i].first){
						busyVans[x] = 0;
						if (waitingDeliveries.empty()){
								loc = Vans[x].location;
								target.first = 20;
								target.second = 20;
								route = createRoute(loc, target);
						}
						lastCargoNumber[x] = -1;
					}
				}
			}
			cout << endl;
			if(completedDeliveries.size() == 20){
				cout << "ALL PARCELS DELIVERED!" << endl;
				break;
			}
		}
		cout << "Position of van0: " << Vans[0].location.first << " " << Vans[0].location.second << endl;
		cout << "Position of van1: " << Vans[1].location.first << " " << Vans[1].location.second << endl;
		cout << "Position of van2: " << Vans[2].location.first << " " << Vans[2].location.second << endl;
		cout << "Position of van3: " << Vans[3].location.first << " " << Vans[3].location.second << endl;
		cout << "Position of van4: " << Vans[4].location.first << " " << Vans[4].location.second << endl;
		cout << "vans status: " << busyVans[0] << " " << busyVans[1] << " " << busyVans[2] << " " << busyVans[3] << " " << busyVans[4] << endl;
		// Clear everything before next iteration
		if (!Instructions.empty()){
			dmc.sendInstructions(Instructions, output);
		}
		reset();
	}
	return 0;
}
