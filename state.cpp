
#include "state.h"

const int default_waiting = 8;
const int slow_down = 10;

bool in_range(State &st, int r, int c) {
  return (r >= 0 && r < st.rows && c >= 0 && c < st.cols);
}

int count_debris(State &st) {
  int count = 0;
  for(int r = 0; r < st.rows; ++r) {
    for(int c = 0; c < st.cols; ++c) {
      if (st.map[r][c] == DEBRIS)
        count += 1;
    }
  }
  return count;
}

bool operator==(Loc &a, Loc &b) {
  return a.r == b.r && a.c == b.c;
}

Area make_area(State &st) {
  std::vector<Loc> robot_locs;
  for(int id = 0; id < (int)st.robots.size(); ++id) {
    robot_locs.push_back(st.robots[id].loc);
  }
  Area area(st.map, robot_locs);
  return area;
}
  
bool is_free(State &st, Loc loc) {
  if (not in_range(st, loc.r, loc.c))
    return false;
  
  for(size_t id = 0; id < st.robots.size(); ++id) {
    if (st.robots[id].loc == loc)
      return false;
  }
  return true;
} 

void init(State &st, int rows, int cols, int number, double debris_density, double failp) {
  st.rows = rows;
  st.cols = cols;
  st.failure_prob = failp;
  st.status = RUNNING;
  st.round = 0;
  st.debris_placed = 0;
  st.play = true;

  // initialize the map
  std::vector<std::vector <Place> > tmp_map;

  st.map.resize(rows);
  tmp_map.resize(rows);
  for(int r = 0; r < rows; ++r) {
    st.map[r].resize(cols);
    tmp_map[r].resize(cols);
    for(int c = 0; c < cols; ++c) {
      st.map[r][c] = EMPTY;
      tmp_map[r][c] = EMPTY;
    }
  }
  
  // place debris
  for(int r = 0; r < rows; ++r) {
    for(int c = 0; c < cols; ++c) {
      if ((double)rand()/(double)RAND_MAX < 0.5) {
        tmp_map[r][c] = DEBRIS;
      }
    }
  }
  
  for(int r = 0; r < rows; ++r) {
    for(int c = 0; c < cols; ++c) {
      int count = 0;
      int radius = 2;
      for (int rr = r - radius; rr <= r+radius; ++rr) {
        for (int cc = c - radius; cc <= c+radius; ++cc) {
          int rrr = (rr + rows) % rows;
          int ccc = (cc + cols) % cols;
          if (tmp_map[rrr][ccc] == DEBRIS) {
            count ++;
          }
        }
      }
      double ratio = (double)count / (double)((2*radius+1)*(2*radius+1));
      if (ratio - 0.1 + 0.2*(double)rand()/(double)RAND_MAX < 0.8*debris_density ) {
        st.map[r][c] = DEBRIS;
      }
    }
  }
  
  for(int i = 0; i < (int)(3*number) + rand()%5; i++) {
    int r = rand() % rows;
    int c = rand() % cols;
    st.map[r][c] = EMPTY;
  }

  for(int i = 0; i < (int)((3+2*debris_density)*number) + rand()%5; i++) {
    int r = rand() % rows;
    int c = rand() % cols;
    st.map[r][c] = DEBRIS;
  }
   
  // count debris
  st.debris_placed = count_debris(st);

  // place robots
  int id = 0;
  for(int i = 0; i < number; ++i) {

    bool placed = false;
    int count = 0;
    do {
      count += 1;
      int r = rand() % rows;
      int c = rand() % cols;
      Loc loc = {r, c}; 

      // check if (r,c) is free
      if (not is_free(st, loc)) continue;

      int wait = rand() % (default_waiting * slow_down);
      bool fixed = true;
      Robot robot = {loc, id, wait, fixed};
      st.robots.push_back(robot);
      placed = true;
    } while(not placed && count < 10);
  }

  // prng sequence
  for (int i = 0; i < 100000; i++) {
    bool fail = ((double)rand() / (double)RAND_MAX) < st.failure_prob;
    st.prng_seq.push_back(fail);
  }
  st.prng_index = 0;
}

bool next_prng(State &st) {
  bool ans = st.prng_seq[st.prng_index];
  st.prng_index = (st.prng_index + 1) % st.prng_seq.size();
  return ans;
}

Loc translate_loc(Loc loc, Action dir) { 
  Loc ans = loc;
  switch(dir) {
    case UP:    ans.r -= 1; return ans;
    case DOWN:  ans.r += 1; return ans;
    case LEFT:  ans.c -= 1; return ans;
    case RIGHT: ans.c += 1; return ans;
    
    case REPAIR_UP:    ans.r -= 1; return ans;
    case REPAIR_DOWN:  ans.r += 1; return ans;
    case REPAIR_LEFT:  ans.c -= 1; return ans;
    case REPAIR_RIGHT: ans.c += 1; return ans;
    default:    return ans;
  }
}

void move(State &st, int id, Action dir) {
  Loc loc = st.robots[id].loc;
  Loc new_loc = translate_loc(loc, dir);
  // no wrapping 
  // new_loc.c = (new_loc.c + st.cols) % st.cols;
  if (in_range(st, new_loc.r, new_loc.c) && is_free(st, new_loc)) {
    st.robots[id].loc = new_loc;
  }
}

void repair(State &st, int id, Action dir) {
  Loc loc = st.robots[id].loc;
  Loc new_loc = translate_loc(loc, dir);
  // no wrapping
  // new_loc.c = (new_loc.c + st.cols) % st.cols;
  if (in_range(st, new_loc.r, new_loc.c)) {
    for(int id2 = 0; id2 < (int)st.robots.size(); ++id2) {
      if (st.robots[id2].loc == new_loc) {
        st.robots[id2].fixed = true;
        break;
      }
    }
  }
}

void update(State &st, std::ostream &gamelog) { 
  // don't do anything if the game had finished
  if (st.status == SUCCESS || st.status == FAILURE) return;

  st.round += 1;

  onClockTick(st.round, gamelog);

  for(size_t id = 0; id < st.robots.size(); id++) {
    
    st.robots[id].wait --;

    if (st.robots[id].wait <= 0) {

      // get action
      Area area = make_area(st);
      Action ac = onRobotAction(id, st.robots[id].loc, area, gamelog);
      
      // process the action
      switch(ac) {
        case COLLECT:

          // break accidentally 
          if (next_prng(st)) {
            st.robots[id].fixed = false; // BREAK the robot
            Area area = make_area(st);
            onRobotMalfunction(id, st.robots[id].loc, area, gamelog);
          }

          if (st.robots[id].fixed) {
            st.map[st.robots[id].loc.r][st.robots[id].loc.c] = EMPTY;
          }
          else {
            st.map[st.robots[id].loc.r][st.robots[id].loc.c] = DEBRIS;
          }
          break;
        case UP:
        case DOWN:
        case LEFT:
        case RIGHT:
          move(st, id, ac);
          break;
        case REPAIR_UP:
        case REPAIR_DOWN:
        case REPAIR_LEFT:
        case REPAIR_RIGHT:
          repair(st, id, ac);
          break;
        default:
          st.robots[id].fixed = false; // additionally BREAK the robot if the action is not understood
          Area area = make_area(st);
          onRobotMalfunction(id, st.robots[id].loc, area, gamelog);
          break;
      }
      
      // collect or release debris
      if (not st.robots[id].fixed) {
        st.map[st.robots[id].loc.r][st.robots[id].loc.c] = DEBRIS;
      }
      
      // update waiting time
      if (st.map[st.robots[id].loc.r][st.robots[id].loc.c] == EMPTY) {
        st.robots[id].wait += default_waiting;
      }
      else {
        st.robots[id].wait += default_waiting * slow_down;
      }

    }
  }

  int dirty = count_debris(st);
  if (dirty <= 0) {
    st.status = SUCCESS;
    return;
  }

  if (st.round * (int)st.robots.size() > 16 * default_waiting * st.rows * st.cols) {
    st.status = FAILURE;
  }
}


