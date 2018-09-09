
#include <curses.h>
#include <string>
#include "output.h"
#include "area.h"

#define X(r,c) (c)*2 + 6
#define Y(r,c) (r)*1 + 3

#define GRAY_COLOR (A_NORMAL | COLOR_PAIR(1))

#define ROBOT_ON_EMPTY_COLOR (A_BOLD | COLOR_PAIR(4))
#define ROBOT_ON_DEBRIS_COLOR (A_BOLD | COLOR_PAIR(11))
#define BROKEN_ROBOT_COLOR (A_BOLD | COLOR_PAIR(10))

#define DEBRIS_COLOR (A_NORMAL | COLOR_PAIR(9))

void print_hint(std::string s, char c) {
  bool needs_highlight = true;
  attrset(A_NORMAL | COLOR_PAIR(1));
  addch('[');
  for(unsigned int i = 0; i<s.size(); ++i) {
    if (needs_highlight && s[i] == c) {
      attrset(A_BOLD | COLOR_PAIR(1));
      addch(c);
      attrset(A_NORMAL | COLOR_PAIR(1));
      needs_highlight = false;
    }
    else
      addch(s[i]);
  }
  addch(']');
}

void output(State &st, std::vector<std::string> &logbufdata) {
 
  int rows = st.rows;
  int cols = st.cols;


  // draw the labels
  for(int r = 0; r < rows; ++r) {

    if (r%5 != 0) continue;

    move(Y(r,-1), X(r,-1));
    printw("%i", r%10);
    
    if (r/10 > 0) {
      move(Y(r,-1), X(r,-1)-1);
      printw("%i", r/10);
    }
  }

  for(int c = 0; c < cols; ++c) {
    
    if (c%5 != 0) continue;
   
    /*
    move(Y(-1,c), X(-1,c));
    printw("%i", c%10);
    
    if (c/10 > 0) {
      move(Y(-1,c)-1, X(-1,c));
      printw("%i", c/10);
    }
    */
    move(Y(-1,c), X(-1,c));
    printw("%i", c);
  }

  // draw the map
  for(int r = 0; r < rows; ++r) {
    for(int c = 0; c < cols; ++c) {
      move(Y(r,c), X(r,c));
      
      switch(st.map[r][c]) {
        case EMPTY: 
          attrset(GRAY_COLOR);
          addch('.');
          addch(' ');
          break;
        case DEBRIS: 
          attrset(DEBRIS_COLOR);
          addch('.');
          addch(' ');
          break;
        default:
          addch(' ');
          addch(' ');
          break;
      }
    }
  }

  for(int id = 0; id < (int)st.robots.size(); ++id) {
    int r = st.robots[id].loc.r;
    int c = st.robots[id].loc.c;
    move(Y(r,c), X(r,c));
    if (st.robots[id].fixed) {
      if (st.map[r][c] == EMPTY)
        attrset(ROBOT_ON_EMPTY_COLOR);
      else
        attrset(ROBOT_ON_DEBRIS_COLOR);
    }
    else {
      attrset(BROKEN_ROBOT_COLOR);
    }
    if (id < 10) 
      addch('0'+id);
    else if (id < 10+26)  
      addch('A'+(id-10));
    else if (id < 10+26+26)
      addch('a'+(id-10-26));
    else
      addch('@');
      
    addch(' ');
  }

  // Draw the interface

  int yy = Y(rows + 1, 0);
  int xx = 2; //X(rows + 1, 0);

  attrset(A_NORMAL | COLOR_PAIR(1));
  move(yy, xx + 1);
  printw("Time: %i ", st.round);
  
  if (st.status == FAILURE) {
    move(yy+1, xx + 1);
    attrset(A_BOLD | COLOR_PAIR(3));
    printw("GAME OVER");
    attrset(A_NORMAL | COLOR_PAIR(1));
  }
  if (st.status == SUCCESS) {
    move(yy+1, xx + 1);
    attrset(A_BOLD | COLOR_PAIR(4));
    printw("WORK COMPLETE!");
  }

  move(yy+0, xx+25);
  print_hint("Quit", 'Q');
  
  move(yy+0, xx+35);
  if (st.play)
    print_hint("Pause", 'P');
  else {
    print_hint("Play", 'P');
    addch(' ');
  }

  move(yy+0, xx+45);
  print_hint("Step", 'S') ;
  
  move(yy+0, xx+55);
  print_hint("Fast-forward", 'F') ;
 
  /* Log */
  attrset(GRAY_COLOR);
  move(yy+2, xx);
  for (unsigned int i = 0; i < log_line_length; i++) {
    addch('-');
  }
  move(yy+2+9, xx);
  for (unsigned int i = 0; i < log_line_length; i++) {
    addch('-');
  }
  move(yy+2, xx + log_line_length - 25);
  addch('|');
  printw(" Programmer's log ");
  addch('|');

  for (unsigned int i = 0; i < logbufdata.size(); ++i) {
    move(yy+3 + i, xx);
    clrtoeol();
  }

  attrset(A_NORMAL | COLOR_PAIR(1));
  for (unsigned int i = 0; i < logbufdata.size(); ++i) {
    move(yy+3 + i, xx);
    printw("%s", logbufdata[i].c_str());
  }

  refresh();
}
