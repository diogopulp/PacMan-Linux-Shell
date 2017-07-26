#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>   // INCLUIR BIBLIOTECA SINAIS
#include <sys/time.h>
#include <sys/select.h>
#include <ncurses.h>
#include <pthread.h>

#include "util.h"

#define WIDTH 57
#define HEIGHT 24

//PEDIDO
typedef struct {

  int login;
  char nome[30], passwd[30];
  int  pid, posX, posY;
  char player[3]; // glu / pac

} PEDIDO;

//RESPOSTA MAPA
typedef struct{

    int pacmanX, pacmanY;
    int pid;
    char mat[22][19];

} RESPOSTA_MAPA;

//RFRESH POS
typedef struct{

  int pid;
	int x;
	int y;

}REFRESH_POS;

int startx = 0;
int starty = 0;

int tamx = 22, tamy = 19;
char mat[22][19];

int fd, res, fd_resp, fd_cp;
char cp[30];
RESPOSTA_MAPA mapa;
REFRESH_POS pos;

int cliente[] = {-1,-1,-1,-1,-1}; // ARRAY cliente[] (int)

void trataCC(int s){

  //NAO CONSEGUE TERMINAR O AMBIENTE GRÁIFICO

  unlink("dados.bin");
  unlink("mapa.bin");
  printf("\n");
  exit(EXIT_SUCCESS);
}

int kick(char user[30], char *array[30]){

  int i;

    for(i=0;i<5;i++){
      if(strcmp(user,array[i])==0) return i;
    }

  return -1;
}

int login(char user[30], char passwd[30]){

	FILE* f;
	char campo1[10], campo2[10];


	f = fopen("users.txt","rt");

	if (f == NULL) {
		 printf("Erro na abertura do ficheiro!\n");
		 exit(1);
	}

	while(!feof(f)){

  	fscanf(f,"%s %s", campo1, campo2);

    	if((strcmp(campo1,user)==0) && (strcmp(campo2,passwd)==0)){
        printf("\n> " );
    		printf("O utilizador %s logou-se com sucesso!",user);
        printf("\n> " );

        fclose(f);
  			return 1;
      }

  	}
  printf("\n> " );
  printf("Username ou Password Incorretos!");
  printf("\n> " );

  fclose(f);
	return 0;

}

void *glutao(void *cord){

 int i, choise, x, y;
 char mascara = 'n';
 char cache = ' ';

 REFRESH_POS copia;

 x=copia.x;
 y=copia.y;

 mat[x][y] = mascara;

 while(1){

	choise=rand()%4;
	usleep(100000);

	switch(choise){

		case 0: //UP
			if(mat[x-1][y] == '*')break;
			if (mat[x-1][y] == 'o') {
          cache = 'o';
          mat[x][y] = '.';
  				mat[x-1][y] = mascara;
  				x--;
  				break;
      }
      if(cache == 'o'){
        mat[x][y] = cache;
        mat[x-1][y] = mascara;
        x--;
        cache = '.';
        break;
      }else{
  				mat[x][y] = '.';
  				mat[x-1][y] = mascara;
  				x--;
  				break;
      }

		case 1: //DOWN
			if(mat[x+1][y] == '*')break;
      if (mat[x+1][y] == 'o') {
          cache = 'o';
          mat[x][y] = '.';
          mat[x+1][y] = mascara;
          x++;
          cache = '.';
          break;
      }
      if(cache == 'o'){
        mat[x][y] = cache;
        mat[x+1][y] = mascara;
        x++;
        cache = '.';
        break;
      }else{
          mat[x][y] = '.';
          mat[x+1][y] = mascara;
          x++;
          break;
      }

		case 2: //LEFT
			if(mat[x][y-1] == '*')break;
      if (mat[x][y-1] == 'o') {
          cache = 'o';
          mat[x][y] = '.';
          mat[x][y-1] = mascara;
          y--;
          break;
      }
      if(cache == 'o'){
        mat[x][y] = cache;
        mat[x][y-1] = mascara;
        y--;
        cache = '.';
        break;
      }else{
          mat[x][y] = '.';
          mat[x][y-1] = mascara;
          y--;
          break;
      }

		case 3: //RIGHT
			if(mat[x][y+1] == '*')break;
      if (mat[x][y+1] == 'o') {
          cache = 'o';
          mat[x][y] = '.';
          mat[x][y+1] = mascara;
          y++;
          break;
      }
      if(cache == 'o'){
        mat[x][y] = cache;
        mat[x][y+1] = mascara;
        y++;
        cache = '.';
        break;
      }else{
          mat[x][y] = '.';
          mat[x][y+1] = mascara;
          y++;
          break;
      }

		default:
			break;

	}

  memcpy(&copia, cord, sizeof(REFRESH_POS));

 }

 pthread_exit(NULL);

}

//RETORNA O MAXIMO DE DOIS INTEIROS - FUNCAO DO MOODLE
int max(int a, int b){
  return (a>b) ? a:b;
}

//TERMINAÇÃO EM CASO DE ERRO - FUNCAO MOODLE
void sayThisAndExit(char *p){
  perror(p);
  exit(EXIT_FAILURE);
}

int addUser(char user[30], char passwd[30]){

  FILE *fr, *fw;
	char campo1[10], campo2[10];

	fr = fopen("users.txt","rt");

	if (fr == NULL) {
		 printf("Erro na abertura do ficheiro!\n");
		 exit(1);
	}

	while(!feof(fr)){

  	fscanf(fr,"%s %s", campo1, campo2);

    	if(strcmp(campo1,user)==0){
        printf("\n> " );
    		printf("O username %s já existe!\n Por favor escolha outro username.",user);
        printf("\n> " );

        fclose(fr);
  			return 0;
      }

  	}

    fw = fopen("users.txt","a");
    if (fw == NULL) {
  		 printf("Erro na abertura do ficheiro!\n");
  		 exit(1);
  	}

  fprintf(fw,"%s %s\n",user,passwd);

  printf("\n> " );
  printf("O utilizador %s foi adicionado com sucesso!", user);
  printf("\n> " );

  fclose(fw);
  return 1;

}

int main(void) {

    system("clear"); // LIMPA O ECRA

   int i, j;
   char *cliente_usr[30] = {"livre","livre","livre","livre","livre"};
   int comando;
   int x,y,num = 10;
   int pacmanX = 16, pacmanY = 9; // PACMAN - POSICAO INICIAL

   char new_user[30], new_passwd[30];

   char cmd[20];
   //char cp[20];   // STRING cp

   struct timeval tempo;

   PEDIDO p;
   //REFRESH_POS pos;
   fd_set fontes;

   signal(SIGINT, trataCC);

   mkfifo("dados.bin", 0600);
   mkfifo("mapa.bin", 0600);

   fd = open("dados.bin", O_RDWR);
   if(fd == -1) sayThisAndExit("Erro no open pipe dados.bin");

   fd_cp = open("mapa.bin", O_RDWR);
   if(fd == -1) sayThisAndExit("Erro no open pipe mapa.bin");

   // PREPARACAO DO JOGO

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

   // POSICAO INICIAL DOS GLUTOES
   REFRESH_POS cord;
   cord.x = 10;
   cord.y = 9;

   pthread_t glutao1, glutao2, glutao3, glutao4, glutao5;

   pthread_create(&glutao1, NULL, glutao, &cord); // GLUTAO 1
   pthread_create(&glutao2, NULL, glutao, &cord); // GLUTAO 2
   pthread_create(&glutao3, NULL, glutao, &cord); // GLUTAO 2
   pthread_create(&glutao4, NULL, glutao, &cord); // GLUTAO 2
   pthread_create(&glutao5, NULL, glutao, &cord); // GLUTAO 2

   mat[pacmanX][pacmanY]='c';

   // AGUARDA QUE CLIENTES SE LIGUEM

   printf("\n> " );

   do {

      FD_ZERO(&fontes);
      FD_SET(0, &fontes);    // TECLADO...
      FD_SET(fd, &fontes);   // FIFO...
      FD_SET(fd_cp, &fontes);

      tempo.tv_sec  = 0;     // TIMEOUT SEGUNDOS
      tempo.tv_usec = 100000;     // TIMEOUT MICRO SEGUNDOS

      res = select(max(fd,fd_cp)+1, &fontes, NULL, NULL, &tempo);
      //printf("%d",res);

      // ATUALIZA CLIENTES
      for(i=0;i<5;i++){
        if(cliente[i]!=-1){
          sprintf(cp,"%d",cliente[i]);
          fd_resp = open(cp,O_WRONLY);
          write(fd_resp, &mapa, sizeof(RESPOSTA_MAPA));
          close(fd_resp);
        }
      }

      // TIMEOUT...
      if (res==0) {
         fflush(stdout);
         continue;
      }

      //ERRO NO SELECT
      if (res==-1){
        perror("Erro no select");
        close(fd);
        unlink("mapa.bin");
        unlink("dados.bin");
        return EXIT_FAILURE;
      }

      // COMANDOS DO SERVIDOR
      if (FD_ISSET(0,&fontes)) {

        scanf("%s", cmd);

        if (strcmp(cmd,"add")==0){
          printf("Username: ");
          scanf("%s",new_user);

          printf("\nPassword: ");
          scanf("%s",new_passwd);

          addUser(new_user,new_passwd);

        }

        if (strcmp(cmd, "shutdown")==0){
           break;
         }else{
           printf("\n> " );
         }

        if (strcmp(cmd, "users")==0){
           printf("\nUser\t|\tPid \n");
           printf("%s\t|\t%d\n",cliente_usr[0],cliente[0]);
           printf("%s\t|\t%d\n",cliente_usr[1],cliente[1]);
           printf("%s\t|\t%d\n",cliente_usr[2],cliente[2]);
           printf("%s\t|\t%d\n",cliente_usr[3],cliente[3]);
           printf("%s\t|\t%d\n",cliente_usr[4],cliente[4]);
           printf("\n> " );
         }
           else{
           printf("\n> " );
         }

        if (strcmp(cmd, "kick")==0){
          printf("\nusername: ");
          scanf("%s",new_user);

          i=kick(new_user,cliente_usr);

          if(i!=-1){
            sprintf(cp,"%d",cliente[i]);

              printf("1-%d\n",p.pid);
            fd_cp = open(cp, O_RDONLY);
            res = read(fd_cp,&p,sizeof(PEDIDO));
            close(fd_cp);

            printf("chega aqui %d\n",cliente[i]);
            printf("%d\n",res);

            if (res == sizeof(PEDIDO)) {
              printf("%d\n",p.pid);
              p.login = 0;

              fd_cp = open(cp,O_WRONLY);
              write(fd_cp,&p, sizeof(PEDIDO));
              close(fd_cp);

              cliente[i] = -1;
              cliente_usr[i] = "livre";
              printf("%s desligado\n",new_user);
            }

          }

          if(i==-1)
            printf("%s nao existe\n",new_user);


        }

        continue;
      }

      // dados.bin
      if (FD_ISSET(fd, &fontes)) {   // FIFO: read(fd, ...);

        res = read(fd, &p, sizeof(PEDIDO));

        //FAZ LOGIN
        if (res == sizeof(PEDIDO)) {

          if(login(p.nome,p.passwd)==1){

             // ADICIONAR "pid" A LISTA CLI. (1º PEDIDO)

             int existe, pos_livre;
             existe = 0;
             pos_livre = -1;

             // POS LIVRE
             for (i=0; i<5; i++) {
                if (cliente[i]==p.pid)
                   existe = 1;
                if (pos_livre==-1 && cliente[i]==-1)
                   pos_livre = i;

             }

            // ADICIONA USER À LISTA
             if (existe==0  && pos_livre!=-1){
                cliente[pos_livre] = p.pid;
                cliente_usr[pos_livre] = p.nome;
              }

              p.login = 1;
              p.posX = pacmanX;
              p.posY = pacmanY;


            // MUDA LOGIN PARA 1 E MANDA AS CORDENADAS

            sprintf(cp, "%d", p.pid);

            fd_resp = open(cp, O_WRONLY);
            res = write(fd_resp, &p, sizeof(PEDIDO));
            close(fd_resp);

            fd_resp = open(cp, O_RDONLY);
            read(fd_resp, &mapa, sizeof(RESPOSTA_MAPA));
            close(fd_resp);

            // COPIA MATRIZ PARA SER ENVIADA
            for(i=0;i<tamx;i++){
              for(j=0;j<tamy;j++){
                mapa.mat[i][j]=mat[i][j];
              }
            }

            fd_resp = open(cp, O_WRONLY);
            write(fd_resp, &mapa, sizeof(RESPOSTA_MAPA));
            close(fd_resp);


            }else{

                p.login = 0;
                sprintf(cp, "%d", p.pid);
                fd_resp = open(cp, O_WRONLY);
                res = write(fd_resp, &p, sizeof(PEDIDO));
                close(fd_resp);


            }
          }

      }

      // mapa.bin
      if (FD_ISSET(fd_cp, &fontes)){

            res = read(fd_cp, &pos, sizeof(REFRESH_POS));

            if(res == sizeof(REFRESH_POS)){

            sprintf(cp,"%d",pos.pid);
            printf("%s\n",cp);

            if(mapa.mat[pos.x][pos.y]!='*'){

              mapa.mat[pacmanX][pacmanY]=' ';

              pacmanX = pos.x;
              pacmanY = pos.y;

              mapa.mat[pacmanX][pacmanY] = 'c';
              mapa.pacmanX = pacmanX;
              mapa.pacmanY = pacmanY;

              printf("x:%d y:%d\n",mapa.pacmanX, mapa.pacmanY);


            }else if(mapa.mat[pos.x][pos.y]=='*'){



            printf("x:%d y:%d\n",mapa.pacmanX, mapa.pacmanY);

            }

          }

      }

    }while(1);



   close(fd);
   close(fd_cp);
   unlink("dados.bin");
   unlink("mapa.bin");

   exit(0);
}
