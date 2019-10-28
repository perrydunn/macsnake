/*

This is a simple, old school snake game (as found on the Nokia),
written in C++ and using ncurses.

Enjoy!

Peregrine Dunn
11th May 2017

*/

#include <ncurses.h> 
#include <string.h>
#include <iostream> 
#include <unistd.h>

using namespace std;
 
#define WIDTH 30
#define HEIGHT 10 

int startx = 0;
int starty = 0;

const char *choices[] = { 
			"Play Game",
			"Settings",
			"Exit",
		  };
int n_choices = sizeof(choices) / sizeof(char *);

void print_menu(WINDOW *menu_win, int highlight)
{
	int x, y, i;	

	x = 2;
	y = 2;
	box(menu_win, 0, 0);
	for(i = 0; i < n_choices; ++i)
	{	if(highlight == i + 1) /* High light the present choice */
		{	wattron(menu_win, A_REVERSE); 
			mvwprintw(menu_win, y, x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		++y;
	}
	wrefresh(menu_win);
}

bool gameOver = false, restart = false;
const int width = 20, height = 20;
int zarray[100];
int x, y, fruitX, fruitY, score;
int tailX[100], tailY[100];
int nTail = 0;
enum eDirection {STOP = 0, LEFT, RIGHT, UP, DOWN};
eDirection dir;

void Setup()
{
	
	WINDOW *menu_win;
	int highlight = 1;
	int choice = 0;
	int c;

	initscr();
	clear();
	noecho();
	cbreak();	/* Line buffering disabled. pass on everything */
	startx = (80 - WIDTH) / 2;
	starty = (24 - HEIGHT) / 2;
		
	menu_win = newwin(HEIGHT, WIDTH, starty, startx);
	keypad(menu_win, TRUE);
	mvprintw(0, 0, "Use arrow keys to go up and down, Press enter to select a choice");
	refresh();
	print_menu(menu_win, highlight);
	while(1)
	{	c = wgetch(menu_win);
		switch(c)
		{	case KEY_UP:
				if(highlight == 1)
					highlight = n_choices;
				else
					--highlight;
				break;
			case KEY_DOWN:
				if(highlight == n_choices)
					highlight = 1;
				else 
					++highlight;
				break;
			case 10:
				choice = highlight;
				break;
			default:
				break;
		}
		print_menu(menu_win, highlight);
		if(choice != 0)	/* User did a choice come out of the infinite loop */
			break;
	}	
	mvprintw(23, 0, "You chose choice %d with choice string %s\n", choice, choices[choice - 1]);
	clrtoeol();

	if (choice == 1 | choice == 2 | choice == 3)
	{
		clear();
		refresh();
	}
	
	dir = STOP;
	x = width/2;
	y = height/2;
	fruitX = (rand() % width)+1;
	fruitY = (rand() % height)+1;
	score = 0;
}

void Draw()
{
	clear();
	
	for (int i = 0; i < width+2; i++)
		mvprintw(0,i,"#");
	
	for (int i = 0; i < height+2; i++)                         
	{
		for (int j = 0; j < width+2; j++)
		{
			if (i == 0 | i == 21)
				mvprintw(i,j,"#");
			else if (j == 0 | j == 21)
				mvprintw(i,j,"#");
			else if (i == y && j == x)
				mvprintw(i,j,"O");
			else if (i == fruitY && j == fruitX)
				mvprintw(i,j,"F");
			else
			{
				for (int k = 0; k < nTail; k++)
				{
					if (tailX[k] == j && tailY[k] == i)
						mvprintw(i,j,"o");
				}
			}
		}	
	}
	
	mvprintw(22,0,"Score: %d",score);
	
	refresh();
	
}

void Input()
{
	keypad(stdscr, TRUE);
	halfdelay(1);
	//cbreak();
	
	int c = getch();
	
	switch(c)
	{
		case KEY_LEFT:
			dir = LEFT;
			break;
		case KEY_RIGHT:
			dir = RIGHT;
			break;
		case KEY_UP:
			dir = UP;
			break;
		case KEY_DOWN:
			dir = DOWN;
			break;
		case 113:				// q is ASCII 113
			gameOver = true;
			break;
	}
}

void Logic()
{
	int prevX = tailX[0];
	int prevY = tailY[0];
	int prev2X, prev2Y;
	tailX[0] = x;
	tailY[0] = y;
	
	for (int i = 1; i < nTail; i++)
	{
		prev2X = tailX[i];
		prev2Y = tailY[i];
		tailX[i] = prevX;
		tailY[i] = prevY;
		prevX = prev2X;
		prevY = prev2Y;
	}
	
	switch (dir)
	{
		case LEFT:
			x--;
			break;
		case RIGHT:
			x++;
			break;
		case UP:
			y--;
			break;
		case DOWN:
			y++;
			break;
		default:
			break;
	}
	
	if (x > width || x < 1 || y > height || y < 1 )
	{
		gameOver = true;
	}
	
	for (int i = 0; i < nTail; i++)
		if (tailX[i] == x && tailY[i] == y)
		{
			gameOver = true;
		}
	
	if (x == fruitX && y == fruitY)
	{
		score += 10;
		fruitX = (rand() % width)+1;
		fruitY = (rand() % height)+1;
		nTail++;
	}
}

int main()
{
	while (!restart)
	{
		initscr();
		Setup();
		while (!gameOver)
		{
			Draw();
			Input();
			Logic();
			//sleep function to slow the game if wanted
		}
		clear();
		printw("Would you like to play again? Tap 'y' if yes");
		refresh();
		cbreak();
		int c = getch();
		
		if (c == 121) // ASCII for y
		{
			gameOver = false;
			
			for (int i = 0; i < 101; i++) 
				tailX[i] = tailY[i] = 0;
			
			nTail = 0;
		}
		else
			restart = true;

		nocbreak();
		clear();
	}
	
	printw("Your last score was %d", score);
	refresh();
	usleep(2000000);
	endwin();
	
	return 0;
}