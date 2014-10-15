// LAB2_WC.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "CrocGame.h" 
#pragma comment(lib, "wherescrocengine") 
#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <cmath>

using namespace std;

vector<pair<double,double>> calcium; // For each waterhole:  mean, std dev
vector<pair<double,double>> salinity; // For each waterhole:  mean, std dev
vector<pair<double,double>> alkalinity; // For each waterhole: mean, std dev

struct Node{
	int item;
	Node *parentNode;
};

vector<Node*> findNeighbors(int start, Node* parentNode, vector<vector<int>> paths){
	vector<Node*> neighbors;
	for(int i = 0; i < paths[start-1].size(); i++){
		Node *node = new Node;
		node->item = paths[start-1][i];
		node->parentNode = parentNode;
		neighbors.push_back(node);
	}
	return neighbors;
}

vector<int> pathFinder(int start, int goal, vector<vector<int>> paths){ // should probably use a queue for this shit for quicker enqueue and dequeue
	vector<Node*> frontier;
	vector<Node*> visited;

	Node *rootNode = new Node;
	rootNode->item = start;
	rootNode->parentNode = NULL;

	frontier.push_back(rootNode);

	while(!frontier.empty()){ // continue until all nodes has been visited
		// pop front of vector
		Node *currentNode = frontier[0]; 
		frontier.erase(frontier.begin());
		// push to visited
		visited.push_back(currentNode);

		// if goal then return path
		if(currentNode->item == goal){
			cout << "WE FOUND THE GOAL NODE, NOW LETS ROUTE THIS SHIT" << endl;
			vector<int> route;
			
			while(currentNode->parentNode != NULL){
				route.push_back(currentNode->item);
				currentNode = currentNode->parentNode;
			}
			reverse(route.begin(), route.end());
			cout << "Path from start " << start << " to goal " << goal << endl;
			for(int i = 0; i < route.size(); i++){
				cout << route[i] << ", ";
			}
			cout << endl;
			return route; // TO REMEMBER: if start == goal then an empty route will be returned, DEAL WITH IT!
		}
		// else enqueue neighbors that has not yet have been visited
		else{
			// find currentNodes neighbors
			vector<Node*> neighbors = findNeighbors(currentNode->item, currentNode, paths);
			
			for(int i = 0; i < neighbors.size(); i++){
				// check if they already are visisted
				bool isVisited = false;
				for(int x = 0; x < visited.size(); x++){
					if(visited[x]->item == neighbors[i]->item){
						isVisited = true;
						break;
					}
				}
				// if not then add to frontier
				if(isVisited != true){
					frontier.push_back(neighbors[i]);
				}
			}
		}
	}

	cout << "ERROR! No node found and no route established" << endl;
}

void clearVariables(){
	calcium.clear();
	salinity.clear();
	alkalinity.clear();
}

int _tmain(int argc, _TCHAR* argv[]){

	const wstring group = L"Group4";
	bool OK = true;
	CrocSession game = CrocSession(group, OK);
	game.StartGame();
	const std::vector<std::vector<int>> paths = game.getPaths();

	int score;
	int playerLocation;
	int bp1activity;
	int bp2activity;

	double calciumReading;
	double salineReading;
	double alkalinityReading;

	game.GetGameState(score, playerLocation, bp1activity, bp2activity, calciumReading, salineReading, alkalinityReading);
	game.GetGameDistributions(calcium, salinity, alkalinity);

	cout << "Playerloc: " << playerLocation << endl;
	cout << "BP1 Loc : " << bp1activity << endl;
	cout << "BP2 Loc : " << bp2activity << endl;

	std::wstring playerMove = L"32";
	std::wstring playerMove2 = L"34";

	int outScore;

	bool state = game.makeMove(playerMove, playerMove2, outScore);

	cout << "State: " << state << endl;
	cout << "Outscore: " << outScore << endl;

	game.GetGameState(score, playerLocation, bp1activity, bp2activity, calciumReading, salineReading, alkalinityReading);

	//cout << "Playerloc: " << playerLocation << endl;
	//cout << "---------" << endl;
	//cout << "Croc - Calcium: " << calciumReading << endl;
	//cout << "Croc - Saline: " << salineReading << endl;
	//cout << "Croc - Alkalinity: " << alkalinityReading << endl;
	//cout << "--------" << endl;

	//double c, s, a, tmp;

	//for(int i= 1; i < 35; i++){

	//	c = abs( (calciumReading - calcium[i].first) / calcium[i].second );
	//	s = abs( (salineReading - salinity[i].first) / salinity[i].second );
	//	a = abs( (alkalinityReading - alkalinity[i].first) / alkalinity[i].second );

	//	tmp = ((c + s + a) / 3);
	//	cout << "Z-Score for waterhole " << i << "\n" << endl;
	//	cout << tmp << endl;
	//	cout << "\n" << endl;
	//}
	cout << "\n" << endl;
	vector <int> viktor = pathFinder(1,5, game.getPaths());
	wcout << viktor[0] << endl;
	return 0;
}
