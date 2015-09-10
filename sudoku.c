#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int solver(void);
int placeRow(int row, int num);
int placeCol(int col, int num);
int placeBox(int box, int num);
int placeNum(void);
int eliminateCombos(int rbc);
int scanRow(int row, int num);
int scanCol(int col, int num);
int scanBox(int box, int num);
int check(int space, int num);
int board[9][9], notBoard[9][9];

//does file input and guesses 'n checks
int main(void){
	int i, j, m, n, result, num, skip=0, guess=0, complete=0, guessList[81]={0};
	FILE* puzzle;

	printf("About to overwrite 'enter_puzzle.txt'\n");
	system("PAUSE");
	puzzle=fopen("enter_puzzle.txt","w");
	for(i=0;i<9;i++){
		for(j=0;j<9;j++){
			fprintf(puzzle,"0 ");
			if(j%3==2)
				fprintf(puzzle," ");
		}
		fprintf(puzzle,"\n");
		if(i%3==2)
			fprintf(puzzle,"\n");
	}
	fclose(puzzle);

	printf("\nLocate the file 'enter_puzzle.txt' and enter your puzzle.\n");
	printf("Empty spaces should be left as 0. When done, click 'Save'.\n\n");
	system("PAUSE");
	puzzle=fopen("enter_puzzle.txt","r");
	if(puzzle!=NULL){
		for(i=0;i<9;i++){
			for(j=0;j<9;j++)
				fscanf(puzzle,"%i",&board[i][j]);
		}
		fclose(puzzle);
	}
	else{
		printf("\nError opening file.\n");
		system("PAUSE");
		return;
	}
	for(i=0;i<9;i++){
		for(j=0;j<9;j++){
			board[i][j]=pow(2.0,board[i][j]);
			notBoard[i][j]=1;
		}
	}
	
	result=solver();
	if(result==1)
		printf("\nThe puzzle is complete! Open 'solution.txt'.\n\n");
	if(result==-1)
		printf("\nThis puzzle is impossible.\n\n");
	else{//guess and check
		while(!complete){
			if(!skip)
				num=1;
			while(num<=9){
				if(!skip)
					i=0;
				while(i<9){
					if(!skip)
						j=0;
					while(j<9){
						skip=0;
						if(check(board[i][j],0) && !check(notBoard[i][j],num) && !scanRow(i,num) && !scanCol(j,num) && !scanBox((i/3)*3+j/3,num)){
							board[i][j]=pow(2.0,num);
							result=solver();
							if(result==-1){
								puzzle=fopen("enter_puzzle.txt","r");							
								for(m=0;m<9;m++){
									for(n=0;n<9;n++){
										fscanf(puzzle,"%i",&board[m][n]);
										board[m][n]=pow(2.0,board[m][n]);
										notBoard[m][n]=1;
									}
								}
								fclose(puzzle);
								solver();
								for(guess=0;guessList[guess];guess++){
									board[(guessList[guess]/10)%10][guessList[guess]%10]=pow(2.0,(guessList[guess]/100)%10);
									solver();
								}
							}
							else if(result==1){
								complete=1;
								num=10;
								i=9;
								j=9;
							}
							else{
								guessList[guess]=num*100+i*10+j;
								printf("Guess %i: Place %i in row %i, column %i\n",guess,(guessList[guess]/100)%10,(guessList[guess]/10)%10+1,guessList[guess]%10+1);
								guess++;
								num=1;
								i=0;
								j=-1;
							}
						}
						j++;
					}
					i++;
				}
				num++;
			}
			if(guess && !complete){
				guess--;
				printf("Undo guess %i\n",guess);
				num=(guessList[guess]/100)%10;
				i=(guessList[guess]/10)%10;
				j=guessList[guess]%10+1;
				skip=1;
				guessList[guess]=0;
				puzzle=fopen("enter_puzzle.txt","r");							
				for(m=0;m<9;m++){
					for(n=0;n<9;n++){
						fscanf(puzzle,"%i",&board[m][n]);
						board[m][n]=pow(2.0,board[m][n]);
						notBoard[m][n]=1;
					}
				}
				fclose(puzzle);
				solver();
				for(guess=0;guessList[guess];guess++){
					board[(guessList[guess]/10)%10][guessList[guess]%10]=pow(2.0,(guessList[guess]/100)%10);
					solver();
				}
			}
			else
				break;
		}
		if(complete){
			printf("\nThe puzzle is complete! Open 'solution.txt'.\n");
			printf("Note: there may or may not be multiple solutions.\n\n");
		}
		else
			printf("\nThis puzzle is impossible. Please disregard 'solution.txt'.\n\n");
	}

	system("PAUSE");
	return 0;
}

//solves the puzzle and prints output in solution.txt. returns 1 if complete. -1 if impossible. 0 if incomplete.
int solver(void){
	FILE* solution;
	int i, j, num, test=1, temp, count=0;
	while(test){
		for(num=1,test=0;num<=9;num++){
			for(i=0;i<9;i++){
					temp=placeRow(i,num);
					if(temp==1)
						test=1;
					else if(temp==-1)
						return -1;
					temp=placeCol(i,num);
					if(temp==1)
						test=1;
					else if(temp==-1)
						return -1;
					temp=placeBox(i,num);
					if(temp==1)
						test=1;
					else if(temp==-1)
						return -1;
					temp=eliminateCombos(i);
					if(temp)
						test=1;
			}
		}

		temp=placeNum();
		if(temp==1)
			test=1;
		else if(temp==-1)
			return -1;
	}
	solution=fopen("solution.txt","w");
	for(i=0;i<9;i++){
		for(j=0;j<9;j++){
			num=log((double)board[i][j])/log(2.0);
			fprintf(solution,"%i ",num);
			if(j%3==2)
				fprintf(solution," ");
			count+=check(board[i][j],0);
		}
		fprintf(solution,"\n");
		if(i%3==2)
			fprintf(solution,"\n");
	}
	fclose(solution);
	if(count)
		return 0;
	return 1;
}

//returns 1 if did something successfully. -1 if impossible. 0 if nothing happened
int placeRow(int row, int num){
	int i, j, test=0, empties=0;
	//find
	for(i=0;i<9;i++){
		if(check(board[row][i],num) && !check(board[row][i],0))
			return 0;
		if(check(board[row][i],0) && !check(notBoard[row][i],num) && !scanCol(i,num) && !scanBox((row/3)*3+i/3,num))
			empties+=pow(2.0,i);
	}
	//place
	if(!empties)
		return -1;
	for(i=0;i<9;i++){
		if(empties==pow(2.0,i)){
			board[row][i]=pow(2.0,num);
			return 1;
		}
	}
	//eliminate remaining spaces in that box.
	if(empties<8){
		for(i=(row/3)*3;i<(row/3+1)*3;i++){
			for(j=0;j<3;j++){
				if(i!=row && !check(notBoard[i][j],num)){
					notBoard[i][j]+=pow(2.0,num);
					test=1;
				}
			}
		}
		if(test)
			return 1;
	}
	if(empties<64 && !(empties%8)){
		for(i=(row/3)*3;i<(row/3+1)*3;i++){
			for(j=3;j<6;j++){
				if(i!=row && !check(notBoard[i][j],num)){
					notBoard[i][j]+=pow(2.0,num);
					test=1;
				}
			}
		}
		if(test)
			return 1;
	}
	if(!(empties%64)){
		for(i=(row/3)*3;i<(row/3+1)*3;i++){
			for(j=6;j<9;j++){
				if(i!=row && !check(notBoard[i][j],num)){
					notBoard[i][j]+=pow(2.0,num);
					test=1;
				}
			}
		}
		if(test)
			return 1;
	}
	return 0;
}
int placeCol(int col, int num){
	int i, j, test=0, empties=0;
	//find
	for(i=0;i<9;i++){
		if(check(board[i][col],num) && !check(board[i][col],0))
			return 0;
		if(check(board[i][col],0) && !check(notBoard[i][col],num) && !scanRow(i,num) && !scanBox((i/3)*3+col/3,num))
			empties+=pow(2.0,i);
	}
	//place
	if(!empties)
		return -1;
	for(i=0;i<9;i++){
		if(empties==pow(2.0,i)){
			board[i][col]=pow(2.0,num);
			return 1;
		}
	}
	//eliminate remaining spaces in that box.
	if(empties<8){
		for(i=0;i<3;i++){
			for(j=(col/3)*3;j<(col/3+1)*3;j++){
				if(j!=col && !check(notBoard[i][j],num)){
					notBoard[i][j]+=pow(2.0,num);
					test=1;
				}
			}
		}
		if(test)
			return 1;
	}
	if(empties<64 && !(empties%8)){
		for(i=3;i<6;i++){
			for(j=(col/3)*3;j<(col/3+1)*3;j++){
				if(j!=col && !check(notBoard[i][j],num)){
					notBoard[i][j]+=pow(2.0,num);
					test=1;
				}
			}
		}
		if(test)
			return 1;
	}
	if(!(empties%64)){
		for(i=6;i<9;i++){
			for(j=(col/3)*3;j<(col/3+1)*3;j++){
				if(j!=col && !check(notBoard[i][j],num)){
					notBoard[i][j]+=pow(2.0,num);
					test=1;
				}
			}
		}
		if(test)
			return 1;
	}
	return 0;
}
int placeBox(int box, int num){
	int i, j, test=0, empties=0;
	//find
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			if(check(board[(box/3)*3+i][(box%3)*3+j],num) && !check(board[(box/3)*3+i][(box%3)*3+j],0))
				return 0;
			if(check(board[(box/3)*3+i][(box%3)*3+j],0) && !check(notBoard[(box/3)*3+i][(box%3)*3+j],num) && !scanRow((box/3)*3+i,num) && !scanCol((box%3)*3+j,num))
				empties+=pow(2.0,3*i+j);
		}
	}
	//place
	if(!empties)
		return -1;
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			if(empties==pow(2.0,3*i+j)){
				board[(box/3)*3+i][(box%3)*3+j]=pow(2.0,num);
				return 1;
			}
		}
	}
	//eliminate remaining spaces in that row
	for(i=0,j=-1;i<9;i++){
		if(check(empties,i)){
			if(j==-1 || j==i/3)
				j=i/3;
			else{
				j=-1;
				break;
			}
		}
	}
	if(j!=-1){
		for(i=0;i<9;i++){
			if(i/3!=box%3 && !check(notBoard[(box/3)*3+j][i],num)){
				notBoard[(box/3)*3+j][i]+=pow(2.0,num);
				test=1;
			}
		}
		if(test)
			return 1;
	}
	//eliminate remaining spaces in that column
	for(i=0,j=-1;i<9;i++){
		if(check(empties,i)){
			if(j==-1 || j==i%3)
				j=i%3;
			else{
				j=-1;
				break;
			}
		}
	}
	if(j!=-1){
		for(i=0;i<9;i++){
			if(i/3!=box/3 && !check(notBoard[i][(box%3)*3+j],num)){
				notBoard[i][(box%3)*3+j]+=pow(2.0,num);
				test=1;
			}
		}
		if(test)
			return 1;
	}
	return 0;	
}
int placeNum(void){
	int i, j, num, result=0;
	for(i=0;i<9;i++){
		for(j=0;j<9;j++){
			for(num=1;num<=9;num++){
				if(check(board[i][j],0) && !check(board[i][j],num) && !check(notBoard[i][j],num) && !scanRow(i,num) && !scanCol(j,num) && !scanBox((i/3)*3+j/3,num)){
					board[i][j]+=pow(2.0,num);
					result=1;
				}
				else if(check(board[i][j],0) && check(board[i][j],num) && (check(notBoard[i][j],num) || scanRow(i,num) || scanCol(j,num) || scanBox((i/3)*3+j/3,num))){
					board[i][j]-=pow(2.0,num);
					result=1;
				}
			}
			for(num=1;num<=9;num++){
				if(board[i][j]==1)
					return -1;
				else if(board[i][j]==pow(2.0,num)+1){
					board[i][j]--;
					result=1;
				}
			}
		}
	}
	return result;
}
int eliminateCombos(int rbc){
	int i, j, k, counta, countb, result=0;
	//eliminate in the row rcb
	for(i=0;i<9;i++){
		counta=0;
		countb=0;
		for(j=i;j<9;j++){
			if(board[rbc][i]==board[rbc][j])
				counta++;
		}
		if(counta>=2){
			for(j=1;j<=9;j++){
				if(check(board[rbc][i],j))
					countb++;
			}
			if(counta==countb){
				for(j=0;j<9;j++){
					for(k=1;k<=9;k++){
						if(check(board[rbc][i],k) && board[rbc][j]!=board[rbc][i] && !check(notBoard[rbc][j],k)){
							notBoard[rbc][j]+=pow(2.0,k);
							result=1;
						}
					}
				}
			}
		}
	}
	//eliminate in the column rbc
	for(i=0;i<9;i++){
		counta=0;
		countb=0;
		for(j=i;j<9;j++){
			if(board[i][rbc]==board[j][rbc])
				counta++;
		}
		if(counta>=2){
			for(j=1;j<=9;j++){
				if(check(board[i][rbc],j))
					countb++;
			}
			if(counta==countb){
				for(j=0;j<9;j++){
					for(k=1;k<=9;k++){
						if(check(board[i][rbc],k) && board[j][rbc]!=board[i][rbc] && !check(notBoard[j][rbc],k)){
							notBoard[j][rbc]+=pow(2.0,k);
							result=1;
						}
					}
				}
			}
		}
	}
	//eliminate in the box rbc
	for(i=0;i<9;i++){
		counta=0;
		countb=0;
		for(j=i;j<9;j++){
			if(board[(rbc/3)*3+i/3][(rbc%3)*3+i%3]==board[(rbc/3)*3+j/3][(rbc%3)*3+j%3])
				counta++;
		}
		if(counta>=2){
			for(j=1;j<=9;j++){
				if(check(board[(rbc/3)*3+i/3][(rbc%3)*3+i%3],j))
					countb++;
			}
			if(counta==countb){
				for(j=0;j<9;j++){
					for(k=1;k<=9;k++){
						if(check(board[(rbc/3)*3+i/3][(rbc%3)*3+i%3],k) && board[(rbc/3)*3+j/3][(rbc%3)*3+j%3]!=board[(rbc/3)*3+i/3][(rbc%3)*3+i%3] && !check(notBoard[(rbc/3)*3+j/3][(rbc%3)*3+j%3],k)){
							notBoard[(rbc/3)*3+j/3][(rbc%3)*3+j%3]+=pow(2.0,k);
							result=1;
						}
					}
				}
			}
		}
	}
	return result;
}

//returns 1 if found num. else 0
int scanRow(int row, int num){
	int i;
	for(i=0;i<9;i++){
		if(check(board[row][i],num) && !check(board[row][i],0))
			return 1;
	}
	return 0;
}
int scanCol(int col, int num){
	int i;
	for(i=0;i<9;i++){
		if(check(board[i][col],num) && !check(board[i][col],0))
			return 1;
	}
	return 0;
}
int scanBox(int box, int num){
	int i, j;
	for(i=(box/3)*3;i<(box/3+1)*3;i++){
		for(j=(box%3)*3;j<(box%3+1)*3;j++){
			if(check(board[i][j],num) && !check(board[i][j],0))
				return 1;
		}
	}
	return 0;
}

//returns 1 if num works in space. else 0
int check(int space, int num){
	return (space/(int)pow(2.0,num))%2;
}
