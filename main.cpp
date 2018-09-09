
#include <unistd.h>
#include <stdio.h>
#include <curses.h>
#include <locale.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>

#include <stdlib.h>
#include <string.h>

#include "state.h"
#include "area.h"
#include "output.h"

int update_from_input(State &s, std::ostream &gamelog);
void finish(int sig);

/* Run the game */
void run (State &st) {

  buf buf;
  std::ostream gamelog (&buf); // game log

  output(st, buf.data);
  
  { // Bot logic init
    Area area = make_area(st);
    onStart(st.robots.size(), area.get_rows(), area.get_cols(), st.failure_prob, area, gamelog);
  }

  int k = 0;
  int finished = 0;
  while( !finished ) {
    k++;
    if (k>1) { 
      
      k=0;
      // run the bot's logic and update the state
      if (st.play) {
        update(st, gamelog);
      }

    }
    finished = update_from_input(st, gamelog);
    output(st, buf.data);
    usleep(5000);
  }
}

/* Helper function, puts the value v in the interval [min, max] */
int put_in_range(int v, int min, int max) {
  if (v < min)
    return min;
  else if (v > max)
    return max;
  else
    return v;
}

double put_in_range_double(double v, double min, double max) {
  if (v < min)
    return min;
  else if (v > max)
    return max;
  else
    return v;
}

/* Main function. Mostly boilerplate code of setting up the terminal. */
int main(int argc, char* argv[]){

  /*       1    2    3   4      5     6    */
  /* ARGS: ROWS COLS NUM DEBRIS FAILP SEED */
  /* Init the game */
  int rows = 20;
  int cols = 20;
  int num = 2;
  double debris = 0.5;
  double failp = 0.0002;

  if (argc > 4) {debris = put_in_range_double(atof(argv[4]), 0.0, 1.0);}
  if (argc > 5) {failp = put_in_range_double(atof(argv[5]), 0.0, 1.0);}
  // Random seed
  if (argc > 6) {
    srand(atoi(argv[6]));
  }
  else {
    srand(time(NULL));
    rows += -2 + rand() % 5;
    cols += -2 + rand() % 5;
    num += -1 + rand() % 4;
    debris = put_in_range_double( debris -0.1 + 0.2 * 0.001 * (double)(rand()%1000), 0.0, 1.0);
  }
  if (argc > 1) {rows = put_in_range(atoi(argv[1]), 5, 40);}
  if (argc > 2) {cols = put_in_range(atoi(argv[2]), 5, 40);}
  if (argc > 3) {num  = put_in_range(atoi(argv[3]), 1, 50);}
  
  // Skip the video simulation?
  bool fast = (argc > 7) && (strcmp(argv[7], "fast") == 0);
 
  // Init the game state
  State st;
  init(st, rows, cols, num, debris, failp);

  if (!fast) 
  {
    /* User interface initialization */
    signal(SIGINT, finish);

    /* ncurses initialization */
    setlocale(LC_ALL, "");
    initscr();     /* initialize the library and screen */
    cbreak();      /* put terminal into non-blocking input mode */
    nonl();        /* no NL -> CRNL on output */
    noecho();      /* turn off echo */
    start_color();
    curs_set(0);   /* hide the cursor */
    timeout(0);

    use_default_colors();
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);
    init_pair(6, COLOR_YELLOW, COLOR_BLACK);
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(8, COLOR_CYAN, COLOR_BLACK);
    
    init_pair(9, COLOR_BLACK, COLOR_BLUE);
    init_pair(10, COLOR_BLACK, COLOR_RED);
    init_pair(11, COLOR_GREEN, COLOR_BLUE);

    color_set(0, NULL);
    assume_default_colors(COLOR_WHITE, COLOR_BLACK);

    attrset(A_NORMAL | COLOR_PAIR(0));
    refresh();
    clear();
   
    /* Run the game */
    run(st);
  
    /* Restore the teminal state */
    echo();
    curs_set(1);
    clear();
    endwin();
  }
  else {

    buf buf;
    std::ostream gamelog (&buf); // game log

    // init bot logic
    Area area = make_area(st);
    onStart(st.robots.size(), area.get_rows(), area.get_cols(), st.failure_prob, area, gamelog);
    
    // run the game until it's over
    while(st.status == RUNNING) { update(st, gamelog); }

    int debris_placed = st.debris_placed;
    int debris_remains = count_debris(st);
    int debris_collected = debris_placed - debris_remains;

    if (argc > 8) {
      FILE *ff = fopen(argv[8], "w+");
      
      if (st.status == SUCCESS) 
        fprintf(ff, "success ");
      else
        fprintf(ff, "failure ");
      
      fprintf(ff, "%i ", st.round);
      fprintf(ff, "%i ", debris_placed);
      fprintf(ff, "%i ", debris_collected);
      fprintf(ff, "%i ", debris_remains);
        
      fprintf(ff, "\n");

      fclose(ff);
    }
    else {
      if (st.status == SUCCESS) 
        printf("success ");
      else
        printf("failure ");

      printf("%i ", st.round);
      printf("%i ", debris_placed);
      printf("%i ", debris_collected);
      printf("%i ", debris_remains);
        
      printf("\n");
    }
  }

  return 0;
}

int update_from_input(State &s, std::ostream &gamelog)
{
    int c;
    int finished=0;

    while ( !finished && (c=getch()) != ERR ) {

      switch(c){
        case 'q': case 'Q':
          finished = 1;
          break;
        case 'f': case 'F':
          for(int i = 0; i<500; ++i){
            update(s, gamelog);
          }
          break;
        case 's': case 'S':
          s.play = false;
          for(int i = 0; i<1; ++i){
            update(s, gamelog);
          }
          break;
        case 'p': case 'P':
          s.play = !s.play;
          break;
        default:;
      }

    }                
    return finished;
}

/* SIGINT */
void finish(int sig)
{
  echo();
  curs_set(1);
  clear();
  endwin();
  exit(0);
}
