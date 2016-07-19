#include <ncurses.h>
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
   ofstream outFile;
   char outputFilename[80];
   sprintf(outputFilename, "interactive/taker.%s",argv[1]);
   outFile.open(outputFilename, ios::out);
   int ch;
   int ch_prev=0;	
   initscr();		
   //nodelay(stdscr,TRUE);
   raw();
   keypad(stdscr, TRUE);
   //noecho();			
   while(1){
      ch = getch();
      switch(ch)
      {
	 case KEY_UP:
	    outFile << 0 << endl;
	    clear();
	    printw("UP");
	    break;
	 case KEY_DOWN:
	    outFile << 1 << endl;
	    clear();
	    printw("DOWN");
	    break;
	 case KEY_RIGHT:
	    outFile << 2 << endl;
	    clear();
	    printw("RIGHT");
	    break;
	 case KEY_LEFT:
	    outFile << 3 << endl;
	    clear();
	    printw("LEFT");
	    break;
	 case ' ':
	    if (ch_prev != ERR)
	       outFile << 4 << endl;
	    clear();
	    printw("STOP");
	    break;
	 case 27:
	    outFile.close();
	    endwin();
	    return 0;
	    break;
	 default:
	    break;
      }
      ch_prev=ch;
      refresh();
   }
   outFile.close();
}
