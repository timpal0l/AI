// LAB2_WC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CrocGame.h" 
#pragma comment(lib, "wherescrocengine") 
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <stdlib.h>
#include <windows.h>
#include <algorithm>
using namespace std;

// Node = waterhole
struct Node{
	int item;
	Node *parentNode;
};
// Variables
vector<pair<double,double>> calcium; // For each waterhole:  mean, std dev
vector<pair<double,double>> salinity; // For each waterhole:  mean, std dev
vector<pair<double,double>> alkalinity; // For each waterhole: mean, std dev
double TPtable[35][35];

double pdf(double mean, double stdDev, double rawValue){
	double pi = 3.1415926535897;
	double value = (1/(stdDev*sqrt(2*pi))*exp(-pow(2,rawValue - mean)/pow(2,2*stdDev)));
	return value;
}
// Find neighbors method
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
// BFS method
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
			vector<int> route;

			while(currentNode->parentNode != NULL){
				route.push_back(currentNode->item);
				currentNode = currentNode->parentNode;
			}
			reverse(route.begin(), route.end());
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
// Clear water level variables
void clearVariables(){
	calcium.clear();
	salinity.clear();
	alkalinity.clear();
}

int _tmain(int argc, _TCHAR* argv[])
{

	/**
	TO DO: 
	- include backpacker location and activity in guesses (if bp JUST died -> Croc is at his last location)
	- matrix multiplication, do crocProb * transitionMatrix, store results in new crocProbTemp Vector
	- multiply this new vector into the old crocProb vector
	- use SPARSE probability table (i.e. set non-existing edges prob to 0)
	*/



	// Variables
	vector<double> crocProb;
	int score;
	int playerLocation;
	int bp1activity;
	int bp2activity;
	double calciumReading;
	double salineReading;
	double alkalinityReading;
	const wstring group = L"group4";							// Group name for DM_Client()
	bool OK = true;
	bool gameActive = true;
	int bp1lastLocation, bp2lastLocation;
	bool bp1JustDied = true, bp2JustDied = true;
	CrocSession game = CrocSession(group, OK);
	// Start game 
	game.StartGame();

	// Get edges
	const std::vector<std::vector<int>> paths = game.getPaths();

	// Create a vector for all edges;
	vector<pair<int,int>> allEdges;

	for (int i=0; i<paths.size(); i++){
		for (int j=0; j<paths[i].size(); j++){
			allEdges.push_back(make_pair(i+1, paths[i][j]));
		}
	}

	// Initialize crocProb
	for (int i=0; i<35; i++){
		crocProb.push_back((1/(double)35));
	}

	// Initialize sparse transision prob matrix
	for (int i=0; i<35; i++){
		for (int j=0; j<35; j++){
			if ((find(allEdges.begin(), allEdges.end(), make_pair(i+1,j+1)) != allEdges.end()) || i ==j){
				// If there is an edge, then prob is #neigbors + 1 (i.e. all possible transitions + case where croc does not move)
				TPtable[i][j] = (1/(double)(paths[i].size()+1));

			}
			else{
				TPtable[i][j] = 0;
			}
		}
	}

	// PRINT TPtable!!!
	for (int i=0; i<35; i++){
		cout << "TPtable row #" << i << ": ";
		for (int j=0; j<35; j++){
			cout << TPtable[i][j] << ", ";
		}
		cout << "\n" << endl;
	}
	Sleep(10000);


	// PDF values for water levels
	vector<double> PDFall; 
	vector<double> PDFcalcium;
	vector<double> PDFsalinity;
	vector<double> PDFalkalinity;	



	vector<pair<int, int>> edgeVector;
	game.GetGameDistributions(calcium, salinity, alkalinity);

	while(gameActive){
		game.GetGameState(score, playerLocation, bp1activity, bp2activity, calciumReading, salineReading, alkalinityReading);

		// For each waterhole:
		for(int i = 0; i < paths.size(); i++){
			// Get values
			double cmean = calcium[i].first;
			double cstddev = calcium[i].second;
			double smean = salinity[i].first;
			double sstddev = salinity[i].second;
			double amean = alkalinity[i].first;
			double astddev = alkalinity[i].second;
			// Calculate pdf
			double calciumPDF = pdf(cmean, cstddev, calciumReading);
			double salinityPDF = pdf(smean, sstddev, salineReading);
			double alkalinityPDF = pdf(amean, astddev, alkalinityReading);
			double allPDF = (calciumPDF * salinityPDF * alkalinityPDF);
			/*
			for(int x = 0; x < 35; x++){
				TPtable[i][x] = TPtable[i][x] * allPDF;
			}
			*/
		}

		// normSum is sum of all matrix elements, used for normalizing
		double normSum = 0;
		for(int i = 0; i < 35; i++){
			for(int j = 0; j < 35; j++){
				normSum += TPtable[i][j];
			}
		}

		cout << "normSum: " << normSum << endl;

		// normalize
		for(int i = 0; i < 35; i++){
			for(int j = 0; j < 35; j++){
				TPtable[i][j] = TPtable[i][j]/normSum;
			}
		}

		// find node with biggest probability
		double guessProb = 0;
		int guessNode = 0;
		for(int i = 0; i < 35; i++){
			if(crocProb[i] > guessProb){
				guessProb = crocProb[i];
				guessNode = i+1;
			}
			cout << "guessNode: " << guessNode << " and guessProb: " << guessProb << endl;
		}

		vector<int> route;
		
		route = pathFinder(playerLocation, guessNode+1, paths);
		bool crocActive = true;
		if(route.empty()){
			wstring move1 = L"S";
			wstring move2 = L"S";
			crocActive = game.makeMove(move1, move2, score);
		}
		else if(route.size() == 1){
			long long m1 = route[0];
			wstring move1 = to_wstring(m1);
			wstring move2 = L"S";
			crocActive = game.makeMove(move1, move2, score);
		}
		else{
			long long m1 = route[0];
			long long m2 = route[1];
			wstring move1 = to_wstring(m1);
			wstring move2 = to_wstring(m2);
			crocActive = game.makeMove(move1, move2, score);
		}
		if(crocActive == false){
			cout << "Score is: " << score << endl;
			cout << "WE ARE MOTHERFUCKING DONE!" << endl;
			gameActive = false;
		}
		cout << "PlayerLocation: " << playerLocation << endl;
		cout << "GuessedNode: " << guessNode << endl;
		if (bp1activity != 0){
			bp1lastLocation = bp1activity;
		}
		if (bp2activity != 0){
			bp2lastLocation = bp2activity;
		}
	}
	return 0;
}
