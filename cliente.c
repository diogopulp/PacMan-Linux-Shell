#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <ncurses.h>
#include <pthread.h>
#include <termios.h>

#define WIDTH 57
#define HEIGHT 24

struct timeval tempo;

typedef struct {

  int login;
  char nome[30], passwd[30];
  int  pid, posX, posY;
  char player[3]; // glu / pac

} PEDIDO;

typedef struct{

    int pacmanX, pacmanY;
    int pid;
    char mat[22][19];

} RESPOSTA_MAPA;

//POS
typedef struct{

  int pid;
	int x;
	int y;

}REFRESH_POS;

//VARIAVEIS GLOBAIS
int highlightx = 16, highlighty = 9;
int tamx = 22, tamy =19;
char mat[22][19];

char cp[20];   // DECLARA STRING "cp"

int startx = (80 - WIDTH) /2;
int starty = (24 - HEIGHT) /2;

WINDOW *game_win;

//RETORNA O MAXIMO DE DOIS INTEIROS - FUNCAO DO MOODLE
int max(int a, int b){
  return (a>b) ? a:b;
}

void trataCC(int s){

  unlink(cp);
  endwin();
  printf("\n");
  exit(EXIT_SUCCESS);
}

void print_game(WINDOW *game_win, int highlightx, int highlighty){
		int i, j;
		int x=1, y=1;

		box(game_win, 0, 0);

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

//TERMINAÇÃO EM CASO DE ERRO - FUNCAO MOODLE
void sayThisAndExit(char *p){
  perror(p);
  exit(EXIT_FAILURE);
}

int main(void) {

  system("clear"); // LIMPA O ECRA


  int fd, res, fd_resp, fd_cp;
  int comando;
  int i, j;
  PEDIDO p;
  RESPOSTA_MAPA mapa;
  REFRESH_POS pos;
  fd_set fontes;

  // CONFIRMAR SE EXISTE SERVIDOR
  if (access("dados.bin", F_OK)!=0) {
      printf("[ERRO] Nao existe servidor!\n");
      exit(1);
  }

  p.pid = getpid(); // FIFO CLIENTE "p.nome" -> "p.pid"
  p.login = 0;
  mapa.pid = p.pid;

  sprintf(cp, "%d", p.pid); // CRIA A CP DO CLIENTE
  pos.pid = p.pid; // PARA IDENTTIFICAR O CLIENTES NO REFRESCAMENTO DO MAPA

  mkfifo(cp, 0600); // mkfifo, open, unlink... CRIA O PIPE
  signal(SIGINT, trataCC);

  fd = open("dados.bin", O_WRONLY);

  //FAZ LOGIN
  while(p.login == 0){

       printf("\nNome: ");
       scanf("%s", p.nome);

       printf("\nPassword: ");
       scanf("%s", p.passwd);

       write(fd, &p, sizeof(PEDIDO)); // manda login

       fd_resp = open(cp, O_RDONLY);
       read(fd_resp, &p, sizeof(PEDIDO)); // recebe login 0 ou 1
       close(fd_resp);

  }
  close(fd);

  pos.x = p.posX;
  pos.y = p.posY;

  fd = open(cp,O_WRONLY);
  write(fd, &mapa, sizeof(RESPOSTA_MAPA)); // manda mapa
  close(fd);

  //RECEBE MAPA
  fd_resp = open(cp, O_RDONLY);
  read(fd_resp, &mapa, sizeof(RESPOSTA_MAPA)); // recebe mapa preenchido
  close(fd_resp);

  //COPIA MATRIZ PARA LOCAL
  for(i=0;i<22;i++){
    for(j=0;j<19;j++){
        mat[i][j]=mapa.mat[i][j];
     }
  }

  initscr(); // initializes the terminal in curses mode
  raw(); // funcao imediatamente depois da initscr
  cbreak(); // funcao imediatamente depois da initscr
  noecho(); //These functions control the echoing of
            //characters typed by the user to the terminal.
            //noecho() switches off echoing
  clear();

  game_win = newwin(HEIGHT, WIDTH, starty, startx); // CRIA JANELA DE JOGO
  keypad(game_win, TRUE);

  // IMPRIME JOGO
  wrefresh(game_win);
  print_game(game_win, starty, startx);

  fd_cp = open(cp, O_RDWR);
  if(fd == -1) sayThisAndExit("Erro no open pipe mapa.bin");

  /************* INICIO DO JOGO ************/

  while(1){

    FD_ZERO(&fontes);         // LIMPA FONTES
    FD_SET(0, &fontes);       // TECLADO...
    FD_SET(fd_cp, &fontes);   // FIFO...

    tempo.tv_sec  = 0;     // TIMEOUT SEGUNDOS
    tempo.tv_usec = 100000;     // TIMEOUT MICRO SEGUNDOS

    res = select(fd_cp+1, &fontes, NULL, NULL, &tempo);

    // TIMEOUT
    if (res==0) {
       fflush(stdout);
       continue;
    }

    //ERRO NO SELECT
    if (res==-1){
      perror("Erro no select");
      close(fd);
      unlink(cp);
      return EXIT_FAILURE;
    }

    // TECLADO
    if (FD_ISSET(0, &fontes)){

      fd = open("mapa.bin", O_WRONLY);

      comando = wgetch(game_win);

      switch(comando){
        case KEY_UP:
          pos.x--;
          write(fd, &pos, sizeof(REFRESH_POS));
          break;

        case KEY_DOWN:
          pos.x++;
          write(fd, &pos, sizeof(REFRESH_POS));
          break;

        case KEY_LEFT:
          pos.y--;
          write(fd, &pos, sizeof(REFRESH_POS));
          break;

        case KEY_RIGHT:
          pos.y++;
          write(fd, &pos, sizeof(REFRESH_POS));
          break;

        default:
          write(fd, &pos, sizeof(REFRESH_POS));
          break;
      }

      close(fd);
    }

    // ESCUTA FIFO CLIENTE
    if (FD_ISSET(fd_cp, &fontes)){

      res = read(fd_cp, &mapa, sizeof(RESPOSTA_MAPA)); // recebe mapa preenchido

      if(res == sizeof(RESPOSTA_MAPA)){

        /*
          if((pos.x != mapa.pacmanX) || (pos.y != mapa.pacmanY)){
          pos.x = mapa.pacmanX;
          pos.y = mapa.pacmanY;
        }
        */

        //COPIA MATRIZ PARA LOCAL
        for(i=0;i<22;i++){
          for(j=0;j<19;j++){
            mat[i][j]=mapa.mat[i][j];
          }
        }

        // IMPRIME MAPA
        wrefresh(game_win);
        print_game(game_win,mapa.pacmanX, mapa.pacmanY);

        clear();
        noecho();
        cbreak();

      }

      /*res = read(fd_cp, &p, sizeof(PEDIDO));

      if(res == sizeof(PEDIDO) && p.login == 0){
        unlink(cp);
        endwin();
        printf("bye!\n");
        close(fd_cp);
        break;
      }*/

    }

  }

   clrtoeol();
   wrefresh(game_win);
   endwin();

   return 0;
}
