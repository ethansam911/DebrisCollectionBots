
#ifndef STATE_H
#define STATE_H

#include <cstdlib>
#include <iostream>
#include <vector>

#include "bot.h"
#include "outstream.h"

enum Game_status {RUNNING, SUCCESS, FAILURE};

struct Robot {Loc loc; int id; int wait; bool fixed;};

struct State {
  // dimensions
  int rows;
  int cols;
  // map
  std::vector <std::vector<Place> > map;

  std::vector <Robot> robots;
  
  std::vector<bool> prng_seq;
  int prng_index;

  int debris_placed;

  int round;
  double failure_prob;
  Game_status status;

  // UI
  bool play;
};

void init(State &st, int rows, int cols, int number, double debris_density, double failp);

void update(State &st, std::ostream &gamelog);

Area make_area(State &st);

int count_debris(State &st);
#endif
