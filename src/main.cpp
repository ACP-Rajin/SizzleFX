#include <ncurses.h>

int main(){
  // Initialize ncurses
  initscr();              // Start curses mode
  cbreak();               // Line buffering disabled
  noecho();               // Don't echo() while we do getch
  keypad(stdscr, TRUE);   // Enable F1, F2 etc
  curs_set(0);            // Hide cursor

  // Print something
  mvprintw(0,0,"Welcome to SizzleFX 🔥");
  mvprintw(1,0,"Press Q to quit.");

  int ch;
  while((ch=getch())!='q'){
  }

  // End ncurses mode
  endwin();
  return 0;
}
