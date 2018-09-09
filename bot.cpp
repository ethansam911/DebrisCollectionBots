/*
Author: Ethan Sam
Course: CSCI-135
Instructor: Genady Maryash
Assignment: Space Cleaning Program Project 3


This program dispatches several robots to clean debris. Within a grid, robots may malfunction 
during the cleaning process, they must find borken robots and fix them within an optimal time.
*/
#include <cstdlib>
#include <iostream>
#include <cmath>
#include "bot.h"

using namespace std;

const int MAX_ROBOT_NUM = 50;

int NUM;          // to remember number or robots
int ROWS, COLS;   // map dimensions

bool robotIsBroken[MAX_ROBOT_NUM]; //We create an array of booleans to represent the working/malfunctioned robots

/* Initialization procedure, called when the game starts: */
void onStart(int num, int rows, int cols, double mpr,
             Area &area, ostream &log)
{
	NUM = num;   // save the number of robots and the map dimensions
	ROWS = rows;
	COLS = cols;
	
	for(int i = 0; i < MAX_ROBOT_NUM; i++)
	{
		robotIsBroken[i] = false;
	}

	log << "Start!" << endl;
} 

/* Deciding robot's next move */
Action onRobotAction(int id, Loc loc, Area &area, ostream &log) {   
	int row = loc.r; // current row and column
	int col = loc.c;

	if (area.inspect(row, col) == DEBRIS)
		return COLLECT;
	else
	{
		int optCol = -1; // -1 so that the first col and row found are going to be the best 
		int optRow = -1;
	
		for(int searchRow = 0; searchRow < ROWS; searchRow++)//For Loop that searches for the debris
		{
			for(int searchCol = 0; searchCol < COLS; searchCol++)
			{
				if(area.inspect(searchRow,searchCol) == DEBRIS) //Returns debris 
				{ 
					if((optCol < 0 && optRow < 0) || (pow(searchCol-col,2) + pow(searchRow-row,2)) < (pow(optCol-col,2) + pow(optRow-row,2))
					{
						optCol = searchCol; //Set each evaluated value to one of least distance (optimal)
						optRow = searchRow; 
					}			
				}
			}
		}  
	
		bool pathToBroken = false; //We set a path boolean to false (Will not search if not broken)
		int inertRobotCol;
		int inertRobotRow;
		int largerDim;
		if(ROWS > COLS) //Simply sets "larger" to the larger dimension
		{
			largerDim = ROWS; 
		}
		else
		{
			largerDim = COLS;
		}
			
		int brokenRobotId = 0;
		for(int i = 0; i < NUM; i++) //This for-loop iterates through the total number of robots
		{
			inertRobotCol = area.locate(i).c; //We locate each and every robot (row, col)
			inertRobotRow = area.locate(i).r;
			//Below conditional compares the ID of a robot with an integer(ID must be different from the one being fixed); 
			if(i != id && robotIsBroken[i] && (sqrt(pow(inertRobotCol-col,2) + pow(inertRobotRow-row,2)) < (sqrt(pow(optCol-col,2) + pow(optRow-row,2))) + largerDim/2))//
			{
				optCol = inertRobotCol;//We know a robot is broken when "onRobotMalfunction" evaluates a value from the array to be true
				optRow = inertRobotRow;
				pathToBroken = true;
				brokenRobotId = i;
			}
		}
		
		if(pathToBroken == true) //Since pathToBroken (broken robot indication) is true we initiate the following
		{
			if(optCol == col) // For the case that the broken robot and the assisting robot are in the same column
			{
				if(optRow == row + 1) //If the fixing robot is above (row is the broken robot), then we fix below
				{
					robotIsBroken[brokenRobotId] = false;
					return REPAIR_DOWN;
				}
				if(optRow == row - 1)
				{
					robotIsBroken[brokenRobotId] = false;
					return REPAIR_UP;//If the fixing robot is below (row is the fixing robot; -1 means below), then we fix above (optRow is the row we want to fix)
				}
			}
			if(optRow == row) // For the case that the broken robot and the assisting robot are in the same row 
			{
				if(optCol == col + 1)
				{
					robotIsBroken[brokenRobotId] = false;
					return REPAIR_RIGHT; //If the fixing robot is to the left (col+1) (optCol is the broken robot), then we fix to the right
					
				}
				if(optCol == col - 1)
				{
					robotIsBroken[brokenRobotId] = false;
					return REPAIR_LEFT;//If the fixing robot is to the right (col-1) (optCol is the broken robot), then we fix to the left
				}
			}
		}

		if(optRow > row)
		{
			return DOWN;
		}
		if(optRow < row)
		{
			return UP;
		}
		if(optCol < col)
		{
			return LEFT;
		}
		if(optCol > col)
		{
			return RIGHT;
		}
		
		switch(rand() % 4) {
		case 0:
			return LEFT;
		case 1:
			return RIGHT; 
		case 2:
			return UP;
		default:
			return DOWN;
		}
	}
}


void onRobotMalfunction(int id, Loc loc, Area &area, ostream &log) {
	log << "Robot " << id << " is damaged." << endl;
	log << "Help is on the way!"<< endl;
	robotIsBroken[id] = true; // ID returned is the value of the robot (This return happens autonomously, even though voids arent meant to return)
}

void onClockTick(int time, ostream &log) {
	if (time % 100 == 0) log << time << " "; 
}
