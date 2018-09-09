
#ifndef BOT_H
#define BOT_H

#include <vector>
#include <iostream>
#include "area.h"

// Robot actions: collecting debris, four directional movement actions, and four repair actions
enum Action {COLLECT,  UP, DOWN, LEFT, RIGHT,  REPAIR_UP, REPAIR_DOWN, REPAIR_LEFT, REPAIR_RIGHT};


void onStart(int num, int rows, int cols, double mpr, Area &area, std::ostream &log);
// onStart: Called at the start, if you want to initialize certain global
// variables, or make something else before the actual simulation starts.
//
// Parameters:
//
// num:  number of robots
// rows: number of rows
// cols: number of columns
// mpr:  malfunction probability (between 0.0 and 1.0)
// area: information about surrounding area
//       area.inspect(loc): returns whether location is DEBRIS or EMPTY
//       area.locate(id):   returns location of the robot with specified ID
//
// log:  a cout-like programmer's log


Action onRobotAction(int id, Loc loc, Area &area, std::ostream &log);
// onRobotAction: Called on each step, choosing an action to perform by
// a debris collecting robot. Must return a valid Action (COLLECT, LEFT, RIGHT, etc.)
//
// Parameters:
//
// id:   robot ID
// loc:  current robot location, struct Loc that has two fields:
//       loc.r
//              its row
//       loc.c
//              its column
//
// area: information about surrounding area
//       area.inspect(loc): returns whether location is DEBRIS or EMPTY
//       area.locate(id):   returns location of the robot with specified ID
//
// log:  a cout-like programmer's log


//
//  Auxiliary functions for more advanced robot control
//

void onRobotMalfunction(int id, Loc loc, Area &area, std::ostream &log);
// Processing robot failure event

void onClockTick(int time, std::ostream &log);
// Processing clock tick event, you you need it


#endif
