// LAB2_WC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CrocGame.h" 
#pragma comment(lib, "wherescrocengine") 
#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>

using namespace std;

vector<pair<double,double>> calcium; // For each waterhole:  mean, std dev
vector<pair<double,double>> salinity; // For each waterhole:  mean, std dev
vector<pair<double,double>> alkalinity; // For each waterhole: mean, std dev

void clearVariables(){
	calcium.clear();
	salinity.clear();
	alkalinity.clear();
}

int _tmain(int argc, _TCHAR* argv[])
{
	const wstring group = L"Tim and the league!";							// Group name for DM_Client()
	bool OK = true;
	CrocSession game = CrocSession(group, OK);

	game.StartGame();

	const std::vector<std::vector<int>> paths = game.getPaths();

	for(int i = 0; i < paths.size(); i++){
		cout << "{" << i+1 << ": ";
		for(int x = 0; x < paths[i].size(); x++){
			cout << paths[i][x] << ", ";
		}
		cout << "}" << endl;
	}

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
	cout << "Playerloc: " << playerLocation << endl;
	return 0;
}
