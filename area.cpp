
#include "area.h"

bool Area::in_range (Loc loc) {
  return (loc.r >= 0 && loc.c >= 0 && loc.r < (int)tiles.size() && loc.c < (int)tiles[0].size());
}
  
int Area::get_rows() {
  return (int)tiles.size();
}

int Area::get_cols(){
  if (tiles.size() > 0) 
    return (int)tiles[0].size();
  else
    return 0;
}

Area::Area(std::vector< std::vector <Place> > v, std::vector<Loc> loc) {

  int rows = v.size();
  int cols = v[0].size();

  tiles.resize(rows);
  for(int r = 0; r < rows; ++r) {
    tiles[r].resize(cols);
    for(int c = 0; c < cols; ++c) {
      tiles[r][c] = v[r][c];
    }
  }

  locations = loc;
}

Place Area::inspect(int row, int col) {
  Loc loc = {row, col};
  if (in_range(loc)) {
    return tiles[row][col];
  }
  return UNDEFINED;
}

Place Area::inspect(Loc loc) {
  if (in_range(loc)) {
    return tiles[loc.r][loc.c];
  }
  return UNDEFINED;
}

Loc Area::locate(int id) {
  if (0 <= id && id < (int)locations.size())
    return locations[id];
  else {
    Loc ans = {0, 0};
    return ans;
  }
}



