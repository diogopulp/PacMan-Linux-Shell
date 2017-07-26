#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

//ligacao ssh para testar multiplayer

#define WIDTH 57
#define HEIGHT 24

#define WIDTH_MENU 27
#define HEIGHT_MENU 14

// FUNCOES...
int menu();
void jogo();
void print_game(WINDOW *game_win, int highlightx, int highlighty);
void print_menu(WINDOW *menu_win, int highlight);
void *musica(void *dados);
void *glutao(void *cord);
int login(char user[10], char passwd[10]);
// ...FUNCOES

// ESTRUTURAS...

typedef struct pos{

	int x;
	int y;


}pos;
// ...ESTRUTURAS

int startx = 0;
int starty = 0;

int tamx = 22, tamy =19;
char mat[22][19];

WINDOW *game_win;
WINDOW *score_win;

char *choices[] = {"Login","Help","Exit"};
int n_choices = sizeof(choices) / sizeof(char *);

int login(char user[10], char passwd[10]){

	FILE* f;
	char campo1[10], campo2[10];

	f = fopen("users.txt","rt");

	if (f == NULL) {
		 printf("Erro na abertura do ficheiro!\n");
		 exit(1);
	}

	while(!feof(f)){

	fscanf(f,"%s %s", campo1, campo2);

		if((strcmp(campo1,user)==0)&&(strcmp(campo2,passwd)==0)){
			printf("Login Efetuado com Sucesso!");
			sleep(1);
			return 1;

		}else if((strcmp(campo1,user)!=0)||(strcmp(campo2,passwd)!=0)){

			printf("Username ou Password Incorretos!");
			sleep(1);

		}


	}


	return 0;


}

void *musica(void *dados){

  int i;
  long max;

  max = (long) dados;
  system("afplay Disorder.m4a");
  pthread_exit(NULL);

}

void *glutao(void *cord){

 int i, choise, x, y;
 char mascara = 'n';

 pos copia;

 memcpy(&copia, cord, sizeof(pos));

 x=copia.x;
 y=copia.y;

 mat[x][y] = mascara;


 while(1){

  //print_game(game_win, x, y);
	choise=rand()%4;
	usleep(100000);
	//print_game2(game_win);
	//printf("[%d %d]", x, y);
	//printf(" %d ",choise);

	switch(choise){
		case 0: //UP
			if(mat[x-1][y] == '*')break;
			else {
				mat[x][y] = ' ';
				mat[x-1][y] = mascara;
				x--;
				break;

			}

		case 1: //DOWN
			if(mat[x+1][y] == '*')break;
			else {
				mat[x][y] = ' ';
				mat[x+1][y] = mascara;
				x++;
				break;
			}
		case 2: //LEFT
			if(mat[x][y-1] == '*')break;
			else {
				mat[x][y] = ' ';
				mat[x][y-1] = mascara;
				y--;
				break;
			}
		case 3: //RIGHT
			if(mat[x][y+1] == '*')break;
			else {
				mat[x][y] = ' ';
				mat[x][y+1] = mascara;
				y++;
				break;
			}
		default:

			refresh();
			break;
	}




 }



 pthread_exit(NULL);

}

int menu(){

	WINDOW *menu_win;
	int highlight = 1;
	int choice = 0;
	int c;

	initscr();
	clear();
	noecho();
	cbreak();	/* Line buffering disabled. pass on everything */
	startx = (80 - WIDTH_MENU) / 2;
	starty = (24 - HEIGHT_MENU) / 2;

	menu_win = newwin(HEIGHT_MENU, WIDTH_MENU, starty, startx);
	keypad(menu_win, TRUE);

	refresh();
	print_menu(menu_win, highlight);

	while(1)
	{
		c = wgetch(menu_win);
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
				mvprintw(24, 0, "Charcter pressed is = %3d Hopefully it can be printed as '%c'", c, c);
				refresh();
				break;
		}
		print_menu(menu_win, highlight);

		if(choice == 1)return 1;

		if(choice == 2)return 2;

		if(choice == 3)	break;
	}
	//mvprintw(23, 0, "You chose choice %d with choice string %s\n", choice, choices[choice - 1]);
	clrtoeol();
	refresh();
	endwin();
	return 0;
}

void print_menu(WINDOW *menu_win, int highlight){

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

void jogo(){

			initscr();
			clear();
			noecho();
			cbreak();
			system("clear");
			clrtoeol();
			refresh();
			endwin();

			//pthread_t tarefa;

			//pthread_create(&tarefa, NULL, musica, (void*) limite); // MUSICA THREAD


			int x,y,num = 10;
			int c;
			int highlightx = 16, highlighty = 9; // PACMAN - POSICAO INICIAL



			int i,j;

			for(i=0;i<tamx;i++){
				for(j=0;j<tamy;j++){
					mat[i][j] = ' ';
				}
			}

			for(i=0;i<tamx;i++){
				for(j=0;j<tamy;j++){
					if(mat[i][j] == ' ')mat[i][j] = '.';
				}

			}

			mat[highlightx][highlighty]='c';


			for(x=0;x<tamx;x++){ // PREECHE MATRIZ
				for(y=0;y<tamy;y++){

					// LADOS

					for(i=0;i<tamx;i++)
					{

					for(j=0;j<1;j++)if(x==i&&y==j){

					mat[x][y]='*';

					}

					}
					for(i=0;i<1;i++){for(j=1;j<tamy;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=1;i<tamx;i++){for(j=tamy-1;j<tamy;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=tamx-1;i<tamx;i++){for(j=1;j<tamy-1;j++)if(x==i&&y==j)mat[x][y]='*';}

					// CAMPO

					for(i=2;i<4;i++){for(j=2;j<4;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=2;i<4;i++){for(j=5;j<8;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=1;i<4;i++){for(j=9;j<10;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=2;i<4;i++){for(j=11;j<14;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=2;i<4;i++){for(j=15;j<17;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=5;i<6;i++){for(j=2;j<4;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=7;i<8;i++){for(j=1;j<4;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=8;i<9;i++){for(j=3;j<4;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=5;i<6;i++){for(j=2;j<4;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=5;i<10;i++){for(j=5;j<6;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=7;i<8;i++){for(j=6;j<8;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=5;i<6;i++){for(j=7;j<12;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=6;i<8;i++){for(j=9;j<10;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=5;i<10;i++){for(j=13;j<14;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=7;i<8;i++){for(j=11;j<13;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=5;i<6;i++){for(j=15;j<17;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=7;i<8;i++){for(j=15;j<18;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=8;i<9;i++){for(j=15;j<16;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=9;i<10;i++){for(j=1;j<4;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=9;i<10;i++){for(j=15;j<18;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=9;i<10;i++){for(j=7;j<9;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=9;i<10;i++){for(j=10;j<12;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=10;i<11;i++){for(j=7;j<8;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=10;i<11;i++){for(j=11;j<12;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=11;i<12;i++){for(j=1;j<4;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=11;i<14;i++){for(j=5;j<6;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=11;i<12;i++){for(j=7;j<12;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=11;i<14;i++){for(j=13;j<14;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=11;i<12;i++){for(j=15;j<18;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=12;i<14;i++){for(j=3;j<4;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=12;i<13;i++){for(j=15;j<16;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=13;i<14;i++){for(j=1;j<4;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=13;i<14;i++){for(j=7;j<12;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=13;i<14;i++){for(j=15;j<18;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=14;i<16;i++){for(j=9;j<10;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=15;i<16;i++){for(j=2;j<4;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=15;i<16;i++){for(j=5;j<8;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=15;i<16;i++){for(j=11;j<14;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=15;i<16;i++){for(j=15;j<17;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=16;i<18;i++){for(j=3;j<4;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=16;i<18;i++){for(j=15;j<16;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=17;i<18;i++){for(j=1;j<2;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=17;i<18;i++){for(j=17;j<18;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=17;i<19;i++){for(j=5;j<6;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=17;i<18;i++){for(j=7;j<12;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=17;i<19;i++){for(j=13;j<14;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=18;i<20;i++){for(j=9;j<10;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=19;i<20;i++){for(j=2;j<8;j++)if(x==i&&y==j)mat[x][y]='*';}
					for(i=19;i<20;i++){for(j=11;j<17;j++)if(x==i&&y==j)mat[x][y]='*';}

					// ESPECIAIS

					for(i=8;i<9;i++){for(j=0;j<3;j++)if(x==i&&y==j)mat[x][y]=' ';}
					for(i=8;i<9;i++){for(j=16;j<tamy;j++)if(x==i&&y==j)mat[x][y]=' ';}
					for(i=12;i<13;i++){for(j=0;j<3;j++)if(x==i&&y==j)mat[x][y]=' ';}
					for(i=12;i<13;i++){for(j=16;j<tamy;j++)if(x==i&&y==j)mat[x][y]=' ';}
					for(i=10;i<11;i++){for(j=8;j<11;j++)if(x==i&&y==j)mat[x][y]=' ';}
					for(i=9;i<10;i++){for(j=9;j<10;j++)if(x==i&&y==j)mat[x][y]='_';}

					for(i=10;i<11;i++){for(j=0;j<1;j++)if(x==i&&y==j)mat[x][y]='.';}
					for(i=10;i<11;i++){for(j=tamy-1;j<tamy;j++)if(x==i&&y==j)mat[x][y]='.';}

					for(i=3;i<4;i++){for(j=1;j<2;j++)if(x==i&&y==j)mat[x][y]='o';}
					for(i=3;i<4;i++){for(j=tamy-2;j<tamy-1;j++)if(x==i&&y==j)mat[x][y]='o';}
					for(i=16;i<17;i++){for(j=1;j<2;j++)if(x==i&&y==j)mat[x][y]='o';}
					for(i=16;i<17;i++){for(j=tamy-2;j<tamy-1;j++)if(x==i&&y==j)mat[x][y]='o';}


				}
			}


			initscr();
			clear();
			noecho();
			cbreak();

			startx = (80 - WIDTH) /2;
			starty = (24 - HEIGHT) /2;

			game_win = newwin(HEIGHT, WIDTH, starty, startx); // CRIA JANELA DE JOGO
			//score_win = newwin(HEIGHT,10,10,10); // CRIA JANELA DE SCORE


			keypad(game_win, TRUE);
			refresh();



			pos cord;

			// POSICAO INICIAL DO GLUTAO
			cord.x = 10;
			cord.y = 9;
      cord.win = game_win;


			// JOGO
			print_game(game_win, highlightx, highlighty);

			pthread_t glutao1, glutao2, glutao3, glutao4, glutao5;

			pthread_create(&glutao1, NULL, glutao, &cord); // GLUTAO 1
			pthread_create(&glutao2, NULL, glutao, &cord); // GLUTAO 2
      pthread_create(&glutao3, NULL, glutao, &cord); // GLUTAO 2
      pthread_create(&glutao4, NULL, glutao, &cord); // GLUTAO 2
      pthread_create(&glutao5, NULL, glutao, &cord); // GLUTAO 2

      print_game(game_win, highlightx, highlighty);

			while(1){

			c = wgetch(game_win);
				switch(c){
					case KEY_UP:
						if(mat[highlightx-1][highlighty]=='*')break; // DETETA PAREDE
						if(mat[highlightx-1][highlighty]=='n'){clrtoeol();refresh();endwin();exit(0);}
						if(highlightx == 0)
							highlightx=tamx-1;
						else{

							mat[highlightx][highlighty] = ' ';
							mat[highlightx-1][highlighty] = 'c';
							--highlightx;

							}
						break;

					case KEY_DOWN:
						if(mat[highlightx+1][highlighty]=='*')break; // DETETA PAREDE
						if(mat[highlightx+1][highlighty]=='_')break; // DETETA PORTA
						if(mat[highlightx+1][highlighty]=='n'){clrtoeol();refresh();endwin();exit(0);}
						if(highlightx == tamx-1)
							highlightx = 0;
							else{

								mat[highlightx][highlighty] = ' ';
								mat[highlightx+1][highlighty] = 'c';
								++highlightx;

								}
						break;

					case KEY_LEFT:

						if(highlighty == 0)
							highlighty = tamy;
						if(mat[highlightx][highlighty-1]=='*')break; // DETETA PAREDE
						if(mat[highlightx][highlighty-1]=='n'){clrtoeol();refresh();endwin();exit(0);}
						else{

							mat[highlightx][highlighty] = ' ';
							mat[highlightx][highlighty-1] = 'c';
							--highlighty;

							}
						break;
					case KEY_RIGHT:

						if(highlighty == tamy-1)
							highlighty = -1;
						if(mat[highlightx][highlighty+1]=='*')break; // DETETA PAREDE
						if(mat[highlightx][highlighty+1]=='n'){clrtoeol();refresh();endwin();exit(0);}
						else{

							mat[highlightx][highlighty] = ' ';
							mat[highlightx][highlighty+1] = 'c';
							++highlighty;

							}
						break;
					default:

						refresh();
						break;
				}
				print_game(game_win, highlightx, highlighty);

			}

			clrtoeol();
			refresh();
			endwin();

			pthread_exit(NULL);

}



void print_game(WINDOW *game_win, int highlightx, int highlighty){
		int i, j;
		int x=1, y=1;


		box(game_win, 0, 0);
		box(score_win,0,0);

		mvwin(score_win,0,0);
		mvwprintw(score_win,0,0,"Score: \n");

		for(i = 0; i < tamx; i++){
			for(j = 0;j< tamy; j++){
				if(highlightx == i && highlighty == j){ /* High light the present choice */

					//wattron(game_win, A_REVERSE);
					wattron(game_win,COLOR_PAIR(2));
					mvwprintw(game_win, y, x, "%c", mat[i][j]);
					wattroff(game_win,COLOR_PAIR(2));
					//wattroff(game_win, A_REVERSE);

				}else{

					if(mat[i][j]=='*'){

						start_color();
						init_pair(1,COLOR_BLUE,COLOR_BLACK);
						wattron(game_win,COLOR_PAIR(1));
						mvwprintw(game_win, y, x, "%c", mat[i][j]);
						wattroff(game_win,COLOR_PAIR(1));
					}else
					if(mat[i][j]=='o'){

						start_color();
						init_pair(2,COLOR_YELLOW,COLOR_BLACK);
						wattron(game_win,COLOR_PAIR(2));
						mvwprintw(game_win, y, x, "%c", mat[i][j]);
						wattroff(game_win,COLOR_PAIR(2));
					}else
					if(mat[i][j]=='n'){

						start_color();
						init_pair(3,COLOR_MAGENTA,COLOR_BLACK);
						wattron(game_win,COLOR_PAIR(3));
						mvwprintw(game_win, y, x, "%c", mat[i][j]);
						wattroff(game_win,COLOR_PAIR(3));
					}else
					mvwprintw(game_win, y, x, "%c", mat[i][j]);



				}
				x+=3;
			}
			x=1;
			y++;
		}

		wrefresh(game_win);
}
