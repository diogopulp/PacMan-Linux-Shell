#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "util.h"

int main(){

	int res;

	res=menu();

		if(res==1){

			clear();
			//res=login("ana","54321");
			//if(res==1)
				jogo();
				clrtoeol();
				refresh();
				endwin();
			//


		}else if(res==2){
			system("clear");
			printf("OLLA!!!");
			sleep(1);
			clrtoeol();
			refresh();
			endwin();
			return 0;
		}




		return 0;
}
