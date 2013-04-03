#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>

#define infinity 32700

void printMaze(short int matrix[100][100], int mummy1, int mummy2, int explorer, int portal1, int portal2);
int mummyMove(int currPos, int speed, char explorerMovement, int explorer, int portal1, int portal2, short int matrix[100][100]);
int bfs(short int matrix[100][100], int explorer, int mummy1, int mummy2, int portal1, int portal2, int bfsQueue[100]);
int extractPath(short int costMatrix[100][100], short int pred[100][100], int explorer, int mummy1, short int path[100]);
char explorerMove(short int path[100], int explorer, int mummy1, int mummy2, int portal1, int portal2);

FILE *fp;
int main () {
	short int matrix[100][100]; // This is the graph's adjacency matrix
	short int costMatrix[100][100]; //This is the D matrix (Floyd's)
	short int pred[100][100]; //This is the pred matrix (Floyd's)
	short int path[100]; //This stores the shortest path between two points (using Floyd's)
	int bfsQueue[100]; //This is for breadth-first search
	int i, j, k, numOfMummies = 0, portal1, portal2, mummy1, mummy2, explorer, mumSpeed = 0, vertexCount = 0, cost = 0;
	char movement; // This determines if the explorer moved left(L), right(R), up(U) or down(D)
	

	fp = fopen("Movements.txt", "w");
	srand((unsigned)time(0)); //random seed
	
	//RANDOM MAZE
	//This generates the random maze	
	for(i=0; i<=99; i++){
		for(j=0; j<=99; j++){
			matrix[i][j] = 0;
			if(j < 99) matrix[j][j+1] = rand() %2;
			if(j <= 89) matrix[j][j+10] = rand() %2;
			if(j % 10 == 9) matrix[j][j+1] = 0;
		}	
	}
	
	//Randomly generate portal 1 & 2
	portal1 = rand() % 100;
	//This is to make them not equal
	do{
		portal2 = rand() % 100; 
	} while(portal1 == portal2);

	//make the matrix symmetric
	for(i=0; i<100; i++){
		for(j=0; j<100; j++){
			if(matrix[i][j] == 1) matrix[j][i] = 1;	
		}	
	}

	printf("Random Maze: TP1 & TP2 - Teleportation portals\n");
	printMaze(matrix, -1, -1, -1, portal1, portal2);

	
	//RANDOMLY GENERATE MUMMY/MUMMIES, Explorer

	//Ask how many mummies
	while( numOfMummies != 2 && numOfMummies != 1){
		printf("How many mummies will there be? (1 or 2) > ");
		scanf("%d", &numOfMummies);
		if( numOfMummies != 2 && numOfMummies != 1 ) printf("Please pick 1 or 2 mummies.\n");
	}
	
	//ranodmly generate mummy
	do{
		mummy1 = rand()%100;	
	}while(mummy1 == portal1 || mummy1 == portal2);
	
	//randomly generate explorer
	do{
		explorer = rand()%100;	
	}while(explorer == portal1 || explorer == portal2 || explorer == mummy1);
	
	fprintf(fp, "Explorer location: row %d, column %d\n", explorer/10 + 1, explorer%10 +1);
	fprintf(fp, "Mummy 1 location: row %d, column %d\n", mummy1/10 + 1, mummy1%10 +1);


	if(numOfMummies ==1){
		mummy2 = -1;
		printf("You chose to have 1 mummy in the maze.\n");
		fprintf(fp, "You chose to have 1 mummy in the maze.\n");
		printf("Your Maze: %c - explorer; X - mummy\n", 1);
		printMaze(matrix, mummy1, mummy2, explorer, portal1, portal2);
	}
	else{
		printf("You chose to have 2 mummies in the maze.\n");
		fprintf(fp, "You chose to have 2 mummies in the maze.\n");
		printf("Your Maze: %c - explorer; X - mummies\n", 1);
		do{
		mummy2 = rand()%100;	
		}while(mummy2 == portal1 || mummy2 == portal2 || mummy2 == mummy1 || mummy2 == explorer);
		
		fprintf(fp, "Mummy2 location: row %d, column %d\n", mummy2/10 + 1, mummy2%10 +1);
		printMaze(matrix, mummy1, mummy2, explorer, portal1, portal2);
	}
	
	fprintf(fp, "Portal 1 location: row %d, column %d\n", portal1/10 + 1, portal1%10 +1);
	fprintf(fp,"Portal 2 location: row %d, column %d\n", portal2/10 + 1, portal2%10 +1);
	
	//make the two portals connected to each other
	matrix[portal1][portal2] = 1;
   matrix[portal2][portal1] = 1;
   
   //BREADTH FIRST SEARCH
   vertexCount =  bfs(matrix, explorer, mummy1, mummy2, portal1, portal2, bfsQueue);
   for(i=0; i<vertexCount; i++){
	 if(bfsQueue[i] == mummy1 || bfsQueue[i] == mummy2) break;  
	}
   if(i == vertexCount){ // if the mummy is not traversed, the solution does not exist.
	    printf("\nA solution does not exist.");
	    fprintf(fp, "A solution does not exist.");
	    fclose(fp);
	    exit(1);
	}
	
	//if solution exists...
	
	//ASK THE SPEED OF THE MUMMIES
	while( mumSpeed != 2 && mumSpeed != 1){
		printf("What is the speed of the mummy/mummies? (1 or 2) > ");
		scanf("%d", &mumSpeed);	
		if(mumSpeed != 2 && mumSpeed != 1) printf("Please enter a valid speed.");
	}
	printf("The mummy's speed is set to %d.\n", mumSpeed);
   fprintf(fp, "The mummy's speed is set to %d.\n", mumSpeed);


	//FLOYD'S ALGO FOR APSP
	for(i=0; i<100; i++){
		for(j=0; j<100; j++){
			if(matrix[i][j] == 1)	costMatrix[i][j] = 1;
			else if(i == j) costMatrix[i][j] = 0;
			else costMatrix[i][j] = infinity;
		}	
	}
	costMatrix[portal1][portal2] = 0;
	costMatrix[portal2][portal1] = 0;
	
	for(i=0; i<100; i++){
		for(j=0; j<100; j++){
			if(i == j || costMatrix[i][j] == infinity)	pred[i][j] = 0;
			else pred[i][j] = i;
		}	
	}
	
	for(k=0; k<100; k++){
		for(i=0; i<100; i++){
			for(j=0; j<100; j++){
				cost = costMatrix[i][k] + costMatrix[k][j];
				if(cost < costMatrix[i][j]) {
					costMatrix[i][j] = cost;
					pred[i][j] = pred[k][j];
					}
			}	
		}	
	}
	//END OF FLOYD'S
	
	while(1){
	if(numOfMummies == 2){
		if(costMatrix[explorer][mummy1] > costMatrix[explorer][mummy2]) // if mummy1 is farther, choose the path to mummy2
		cost = extractPath(matrix, pred, explorer, mummy2, path);

		else { // if mummy2 is farther, choose the path to mummy1
		cost = extractPath(matrix, pred, explorer, mummy1, path);
		}
	}
	else cost = extractPath(matrix, pred, explorer, mummy1, path); // if there is only 1 mummy
	
	movement = explorerMove(path, explorer, mummy1, mummy2, portal1, portal2); // explorer moves depending on the path (Floyd's)
	
	if(explorer == mummy1 || explorer == mummy2){ // If the mummy and explorer are already on the same cell, exit
		printf("Explorer: COLLISION\n");
		fprintf(fp, "Explorer: COLLISION\n");
		printf("\nPlease refer to MummyMaze.txt for the list of movements (no visualization).\n");
		fclose(fp);
		exit(1);
	}
	
	if(movement == 'L') { // move left
		explorer = explorer - 1;
		printf("\nThe explorer moved to left.\n");
		fprintf(fp, "\nThe explorer moved to left.\n");
	}
	else if(movement == 'R'){ //move right
		 explorer = explorer + 1;
		printf("\nThe explorer moved to right.\n");
		fprintf(fp, "\nThe explorer moved to right.\n");
	}
	else if(movement == 'U') { // move up
		explorer = explorer - 10;
		printf("\nThe explorer moved up.\n");
		fprintf(fp, "\nThe explorer moved up.\n");
	}
	else if(movement == 'D') { // move down
			explorer = explorer + 10;
			printf("\nThe explorer moved down.\n");
			fprintf(fp, "\nThe explorer moved down.\n");
	}

	
	if(explorer == portal1){ // if explorer lands on portal1, transport to portal2
			explorer = portal2;
			printf("Explorer stepped on portal 1. It is transported to portal 2.\n");	
			
	}
	else if(explorer == portal2){// if explorer lands on portal2, transport to portal1
			explorer = portal1;
			printf("Explorer stepped on portal 2. It is transported to portal 1.\n");
	}
	

	
	printf("Mummy 1:");
	fprintf(fp, "Mummy 1:");
	//mummy moves depending on explorer's move
	mummy1 = mummyMove(mummy1, mumSpeed, movement, explorer, portal1, portal2, matrix);
	
	if(mummy1 == -99) {// if collision already occurs
		printf(" COLLISION\n");
		fprintf(fp, " COLLISION\n");
		printMaze(matrix, mummy1, mummy2, explorer, portal1, portal2);
		printf("\nPlease refer to MummyMaze.txt for the list of movements (no visualization).\n"); fclose(fp);
		exit(1);
	}
	if(numOfMummies == 2){
		printf("Mummy 2:");
		fprintf(fp, "Mummy 2:");
		mummy2 = mummyMove(mummy2, mumSpeed, movement, explorer, portal1, portal2, matrix);
		
		if(mummy2 == -99) {
			printf(" COLLISION\n");
			fprintf(fp, " COLLISION\n");
			printMaze(matrix, mummy1, mummy2, explorer, portal1, portal2);
			printf("\nPlease refer to MummyMaze.txt for the list of movements (no visualization).\n"); fclose(fp);
			exit(1);
		}
	}
	printMaze(matrix, mummy1, mummy2, explorer, portal1, portal2);
	fprintf(fp, "\n");
	}
	


		
	

return 0;	
}

void printMaze(short int matrix[100][100], int mummy1, int mummy2, int explorer, int portal1, int portal2){
	int i, j;
	printf(" ---- ---- ---- ---- ---- ---- ---- ---- ---- ----\n");
	for(i=0; i<100; i=i+10){
		
		printf("| ");
		for(j=0; j<10; j++){
			if((i)+ j == mummy1 || (i)+ j == mummy2){ // if there is a mummy on that cell, print X
				if(matrix[i+j][i+j+1] == 0)printf(" X |");
				else printf(" X  ");
			}
			else if((i)+ j == explorer){ // if the explorer is on that cell, print ASCII character 1
				if(matrix[i+j][i+j+1] == 0)printf(" %c |", 1);
				else printf(" %c  ", 1);
			}
			else if((i)+ j == portal1){ // if there is a teleportation portal on that cell, print TP1
				if(matrix[i+j][i+j+1] == 0)printf("TP1|");
				else printf("TP1 ");
			}
			else if(i+j == portal2){ // if there is a teleportation portal on that cell, print TP2
				if(matrix[i+j][i+j+1] == 0)printf("TP2|");
				else printf("TP2 ");
			}
			else{
				if(matrix[i+j][i+j+1] == 0)printf("   |"); // if the current cell and the cell to its right is not connected, print a wall between them
				else printf("    "); // otherwise just print a space
				
			}
			printf(" ");
		}
		printf("\n");
		
		if(i < 90){
		printf("|");
			for(j=0; j<10; j++){
				if(matrix[i+j][i+j+10] == 0)printf("----"); // if the current cell and the cell below it is not connected, print a wall below the cell
				else printf("    "); // otherwise just print a space
				if(j<9)printf(" ");
			
			}
			printf("|\n");
		}
		else printf(" ---- ---- ---- ---- ---- ---- ---- ---- ---- ----\n");
		
	}
}

int mummyMove(int currPos, int speed, char explorerMovement, int explorer, int portal1, int portal2, short int matrix[100][100]){
	int newPos = 999;
	
	while(speed > 0){
      if(currPos == explorer){
	   	return -99;   
	   }
                                                                                               		
		if(explorerMovement == 'L'){
			if(currPos % 10 != 9 && matrix[currPos][currPos+1] == 1) {newPos = currPos + 1; printf(" Mummy moved to the right\n"); fprintf(fp, " Mummy moved to the right\n"); speed--;} // if not yet on the rightmost edge, and if there is no wall blocking, move right
			if(matrix[currPos][currPos+1] == 0 || (currPos%10 == 9 && currPos != 0)) {printf(" Mummy wasn't able to move right\n"); fprintf(fp, " Mummy wasn't able to move right\n"); speed--;}
		}	
		else if(explorerMovement == 'R'){
			if(currPos % 10 != 0 && matrix[currPos-1][currPos] == 1){ newPos = currPos - 1; printf(" Mummy moved to the left\n"); fprintf(fp, " Mummy moved to the left\n");speed--; }// if not yet on the leftmost edge, and if there is no wall blocking, move left
			if(matrix[currPos-1][currPos] == 0 || currPos % 10 == 0) {printf(" Mummy wasn't able to move left\n");fprintf(fp, " Mummy wasn't able to move left\n");speed--;}
		}
		else if(explorerMovement == 'U'){
			if(currPos < 90 && matrix[currPos][currPos+10] == 1) { newPos = currPos + 10; printf(" Mummy moved down\n");fprintf(fp, " Mummy moved down\n");speed--;} // if not yet on the bottom edge, and if there is no wall blocking, move down
			if(matrix[currPos][currPos+10] == 0 || currPos >= 90){ printf(" Mummy wasn't able to move down\n");	fprintf(fp, " Mummy wasn't able to move down\n");speed--;}		
		}
		else if(explorerMovement == 'D'){
			if(currPos > 9 && matrix[currPos-10][currPos] == 1) { newPos = currPos - 10; printf(" Mummy moved up\n");fprintf(fp, " Mummy moved up\n");speed--;}// if not yet on the uppermost edge, and if there is no wall blocking, move up
			if(matrix[currPos-10][currPos] == 0 || currPos <= 9) {printf(" Mummy wasn't able to move up\n");fprintf(fp, " Mummy wasn't able to move up\n");speed--;}
		}
		if(newPos != 999) currPos = newPos;
		
		if(currPos == portal1){
			newPos = portal2;
			currPos = newPos;
			printf("Mummy stepped on portal 1. It is transported to portal 2.\n");	
			
		}
		else if(currPos == portal2){
			newPos = portal1;
			currPos = newPos;
			printf("Mummy stepped on portal 2. It is transported to portal 1.\n");
		}
		
		if(currPos == explorer){
	   	return -99;   
	   }
		
		
		
	}
	

	
	return currPos;

	
}

int bfs(short int matrix[100][100], int explorer, int mummy1, int mummy2, int portal1, int portal2, int bfsQueue[100]){
		short int discovered[100];
		int current;
		int i = 0, front = 0, j;
		
		for(j=0; j<100; j++) discovered[j] = 0;
		for(j=0; j<100; j++) bfsQueue[j] = '\0';
		
		bfsQueue[i] = explorer;
		i++;
		current = explorer;
		discovered[current] = 1;
	
		while(bfsQueue[front]!= '\0'){
			//printf("%d ", current);
			if(current > 9){
				if(matrix[current-10][current] == 1 && discovered[current-10] != 1){
					 bfsQueue[i] = current - 10;
					 discovered[current-10] = 1;
					 i++;
				}
			}
			if(current > 0){
				if(matrix[current-1][current] == 1 && discovered[current-1] != 1) {
						bfsQueue[i] = current - 1;
						discovered[current-1] = 1;
						i++;
				}
			}
			if(current < 99){
				if(matrix[current][current+1] == 1 && discovered[current+1] != 1){
						bfsQueue[i] = current + 1;
						discovered[current+1] = 1;
						i++;
				}	
			}
			if(current < 90){
				if(matrix[current][current+10] ==1 && discovered[current+10] != 1){
					bfsQueue[i] = current + 10;
					discovered[current+10] = 1;
					i++;	
				}	
			}
			if(current == portal1 || current == portal2){
				if(current == portal1 && discovered[portal2] != 1){
					bfsQueue[i] = portal2;	
					discovered[portal2] = 1;
					i++;
				}
				else if(current == portal2 && discovered[portal1] != 1){
					bfsQueue[i] = portal1;
					discovered[portal1] = 1;
					i++;
				}
			}
			front++;
			if(bfsQueue[front] != '\0')
				current = bfsQueue[front];
		}
		return i;
	
}

char explorerMove(short int path[100], int explorer, int mummy1, int mummy2, int portal1, int portal2){

/*	if(path[1] == portal1){
		path[1] = portal2;	
	}
	else if(path[1] == portal2){
		path[1] = portal1;
	}*/
	
	if(path[1] == explorer + 1)return 'R';
	else if(path[1] == explorer + 10) return 'D';
	else if(path[1] == explorer - 10) return 'U';
	else if(path[1] == explorer -1)return 'L';
	else if(path[1] == explorer){return 'X';}
	else return 'X';
}

int extractPath(short int costMatrix[100][100], short int pred[100][100], int explorer, int mummy, short int path[100]){
	int i, j, k, len;
	
	i = mummy;
	j = explorer;
	len = 0;
	path[len] = j;
	k = j;
	while(k != i){
		if(pred[i][k] == 0) {printf("No path found."); break;}
		else {
			k = pred[i][k];
			len++;
			path[len] = k;	
		}	
	}
	return len;
	
}




	