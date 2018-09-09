
#ifndef AREA_H
#define AREA_H

#include <vector>
#include "common.h"

class Area {
  std::vector< std::vector <Place> > tiles; 
  std::vector< Loc > locations; 
  bool in_range (Loc loc);

public: 
  Area(std::vector< std::vector<Place> >, std::vector<Loc>);
  
  int get_rows ();
 
  int get_cols ();
  
  Place inspect(Loc loc);
  
  Place inspect(int row, int col);
  
  Loc locate(int id);

};

#endif
