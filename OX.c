//ox.c

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#define SENKO 0
#define KOUKO 1
#define KARA 0
#define MARU 1
#define BATU 2
#define XMAX 3
#define YMAX 3
#define TURNMAX 9
#define DRAW 0
#define WIN 100
#define LOSE (-100)

#define SEACHDEPTH 2
#define BESTMAX 32

char strtmp[32];
int jun;
int cnt_i, cnt_j;
int turn;
int tmpdepth;//AI
int tmpcnt;//AI
int xkouho[BESTMAX], ykouho[BESTMAX];//AI
int tmpban[XMAX][YMAX];//check
int doitsu;

void output(void);
void input(void);
int situcheck(int);
void AI(void);//AI
void subAI(void);//AI
void weightcheck(void);//AI
void weightlook(void);//AI
int doitsucheck(int);//AI


struct ban{
	int pos[XMAX][YMAX];
	int depth;
	int situ;
	int set[2];
	int weight;
	struct ban *next;
	struct ban *back;
} root;
struct ban *now;
struct ban *prv;
struct ban *test;

int main() {

	srand(time(NULL));
	for(cnt_i = 0;cnt_i < XMAX;cnt_i++) {
		for(cnt_j = 0;cnt_j < YMAX;cnt_j++) {
			root.pos[cnt_i][cnt_j] = KARA;
		}
	}

	printf("\n***OX GAME***\n\n\n");
	
	printf("Take first turn?(y/n)---");
	scanf("%c", strtmp);
	if(strtmp[0] == 'Y' || strtmp[0] == 'y') {
		jun = SENKO;
	} else {
		jun = KOUKO;
	}
	
	output();

    for(turn = 0; turn < TURNMAX; turn++) {
		printf("Now status:%d\n", situcheck(0));
		if(jun == SENKO) {
			printf("\nYour turn\n");
			input();
			output();
			jun = KOUKO;
		} else {
			printf("\nEnemy's turn\n");
			AI();
			output();
			jun = SENKO;
		}
		
		if(situcheck(0) == WIN){
			printf("\nVictory!\n");
			break;
		}
		if(situcheck(0) == LOSE){
			printf("\nDefeat!\n");
			break;
		}
		
	}
	
	if(situcheck(0) == DRAW){
		printf("Draw\n");
	} 
    printf("\n***GAME OVER***\n\n\n");

	return 0;
}


void output(void) {

	char *tmpp[XMAX][YMAX];
	for(cnt_i = 0;cnt_i < XMAX;cnt_i++) {
		for(cnt_j = 0;cnt_j < YMAX;cnt_j++) {
			if(root.pos[cnt_i][cnt_j] == MARU) {
				tmpp[cnt_i][cnt_j] = "o";
			} else if(root.pos[cnt_i][cnt_j] == BATU) {
				tmpp[cnt_i][cnt_j] = "x";
			} else {
				tmpp[cnt_i][cnt_j] = " ";
			}	
		}
	}
	printf("   0 1 2      \n");
	printf(" 0 %s|%s|%s     \n", tmpp[0][0], tmpp[1][0], tmpp[2][0]);
	printf("   -+-+-     \n");
	printf(" 1 %s|%s|%s     \n", tmpp[0][1], tmpp[1][1], tmpp[2][1]);
	printf("   -+-+-     \n");
	printf(" 2 %s|%s|%s     \n", tmpp[0][2], tmpp[1][2], tmpp[2][2]);
	
}


void input(void) {

	int tmpx, tmpy;
	while(-1){
		printf("Input pos(xy)---");
		scanf("%1d%1d", &tmpx, &tmpy);
		if(tmpx == 4 && tmpy == 4) {
			printf("Exit...\n");
			exit(-1);
		}
		if(root.pos[tmpx][tmpy] == KARA && tmpx >= 0 && tmpx < XMAX && tmpy >= 0 && tmpy < YMAX) {
			break;
		}
	}
	root.pos[tmpx][tmpy] = MARU;
	printf("\n");
}

void AI(void) {
	int count = 0;
	int tmpcnt;
	int tmp;
	
	//Set struct
	root.depth = 0;
	root.situ = situcheck(0);
	root.set[0] = 0;
	root.set[1] = 0;
	root.weight = 100;
	root.next = NULL;
	root.back = NULL;
	prv = &root;
		
	//Create future data
	subAI();
	
	//Add weight to future data 
	weightcheck();
	
	//Visuarize weight
	weightlook();
	
	//Search good option from data
	now = root.next;
	
	xkouho[0] = now->set[0];
	ykouho[0] = now->set[1];
	now = root.next;
	while(now->next != NULL){//Create list of good option
		if(root.weight == (now->weight) && now->depth == 1) {
			xkouho[count] = now->set[0];
			ykouho[count] = now->set[1];
			count++;
			if(BESTMAX <= count) {
				exit(-1);
			}
		}
		now = now->next;
	}
	if(count != 1) {
		printf("AI thinking...\n");
	}
	
	//Select choice
	root.pos[xkouho[0]][ykouho[0]] = BATU;
	
	
	
	//Old AI, Random hand
	/*for(cnt_i = 0;cnt_i < XMAX;cnt_i++) {
		for(cnt_j = 0;cnt_j < YMAX;cnt_j++) {
			if(root.pos[cnt_i][cnt_j] == KARA) {
				root.pos[cnt_i][cnt_j] = BATU;
				return;
			}
		}
	}*/
}

//Create future data
void subAI(void) {

	int cnt1, cnt2;
	int cntx, cnty;
	struct ban *tmpnow;
	
	
	tmpdepth = (prv->depth) + 1;
	
	for(cntx = 0;cntx < XMAX;cntx++) {
		for(cnty = 0;cnty < YMAX;cnty++) {
			if(prv->pos[cntx][cnty] == KARA) {
				
				now = malloc(sizeof(struct ban));
				for(cnt1 = 0;cnt1 < XMAX;cnt1++) {
					for(cnt2 = 0;cnt2 < YMAX;cnt2++) {
						now->pos[cnt1][cnt2] = prv->pos[cnt1][cnt2];
					}
				}
				if(tmpdepth%2 == 1) {
					now->pos[cntx][cnty] = BATU;
				} else {
					now->pos[cntx][cnty] = MARU;
				}
				now->depth = tmpdepth;
				
				if(prv->depth >= now->depth){//1
					now->pos[prv->set[0]][prv->set[1]] = KARA;
				}
				if(prv->depth > now->depth) {//2
					tmpnow = prv;
					while(tmpnow->depth != now->depth){
						tmpnow = tmpnow->back;
					}
					now->pos[tmpnow->set[0]][tmpnow->set[1]] = KARA;
				}
				
				//If same board, Continue loop
				test = &root;
				doitsu = 0;
				while(-1) {
					if(test->next == NULL) {
						break;
					}
					
					for(cnt1 = 0;cnt1 < XMAX;cnt1++) {
						for(cnt2 = 0;cnt2 < YMAX;cnt2++) {
							tmpban[cnt1][cnt2] = test->pos[cnt1][cnt2];
						}
					}
					if(doitsucheck(1)) {
						doitsu = 1;
						break;
					}
					
					test = test->next;
				}
				if(doitsu == 1){
					continue;
				}
				
				now->situ = situcheck(1);
				now->set[0] = cntx;
				now->set[1] = cnty;
				now->weight = situcheck(1);
				now->back = prv;
				prv->next = now;
				now->next = NULL;
				
				//printf("\nnow=%d,prv=%d\n", now, prv);
				/*printf("%2d %2d %2d\n", now->pos[0][0], now->pos[1][0], now->pos[2][0]);
				printf("%2d %2d %2d\n", now->pos[0][1], now->pos[1][1], now->pos[2][1]);
				printf("%2d %2d %2d\n", now->pos[0][2], now->pos[1][2], now->pos[2][2]);
				printf("depth=%d,situ=%d\n", now->depth, now->situ);
				printf("setx=%d,sety=%d\n", now->set[0], now->set[1]);*/
				//printf("prv->next=%d,now->back=%d\n", prv->next, now->back);
				
				prv = now;
				
				//Search next layer
				if(tmpdepth < SEACHDEPTH) {
					if(abs(now->situ) != 100) {//If not find winner, Continue search
						subAI();
					}
				}
				
				//Return from under layer
				now = malloc(sizeof(struct ban));
				for(cnt1 = 0;cnt1 < XMAX;cnt1++) {
					for(cnt2 = 0;cnt2 < YMAX;cnt2++) {
						now->pos[cnt1][cnt2] = prv->pos[cnt1][cnt2];
					}
				}
				if(tmpdepth%2 == 1) {
					now->pos[cntx][cnty] = BATU;
				} else {
					now->pos[cntx][cnty] = MARU;
				}
				now->depth = tmpdepth;
				
				if(prv->depth >= now->depth){//1
					now->pos[prv->set[0]][prv->set[1]] = KARA;
				}
				if(prv->depth > now->depth) {//2
					tmpnow = prv;
					while(tmpnow->depth != now->depth){
						tmpnow = tmpnow->back;
					}
					now->pos[tmpnow->set[0]][tmpnow->set[1]] = KARA;
				}
				
				now->situ = situcheck(1);
				now->set[0] = cntx;
				now->set[1] = cnty;
				now->weight = 1000;
				now->back = prv;
				prv->next = now;
				now->next = NULL;
				
				//printf("\nnow=%d,prv=%d\n", now, prv);
				/*
				printf("%2d %2d %2d\n", now->pos[0][0], now->pos[1][0], now->pos[2][0]);
				printf("%2d %2d %2d\n", now->pos[0][1], now->pos[1][1], now->pos[2][1]);
				printf("%2d %2d %2d\n", now->pos[0][2], now->pos[1][2], now->pos[2][2]);
				printf("depth=%d,situ=%d\n", now->depth, now->situ);
				printf("setx=%d,sety=%d\n", now->set[0], now->set[1]);*/
				//printf("prv->next=%d,now->back=%d\n", prv->next, now->back);
				
				prv = now;
				
			}
		}
	}
	tmpdepth = (prv->depth) - 1;
}

void weightcheck(void){
	
	int nowdepth;
	int value;
	
	
	test = prv;
	//Add depth each layer. If depth is odd, Take min value, If depth is even, Take max value
	for(nowdepth = SEACHDEPTH; nowdepth >= 0; nowdepth--){
		//printf("Searching... depth:%d\n", nowdepth);	
		test = prv;
		if(nowdepth%2==1){value=100;}else{value=(-100);}
		while(test != NULL){
			if(test->weight != 1000 && nowdepth == test->depth){
				if(nowdepth%2 == 1 && test->weight < value){
					value = test->weight;
				}
				if(nowdepth%2 == 0 && test->weight > value){
					value = test->weight;
				}
			}
			
			if(test->weight != 1000 && nowdepth - 1 == test->depth ){
				test->weight = value;
				//printf("Depth:%d, update:%d\n", test->depth, test->weight);
				if(nowdepth%2==1){value=100;}else{value=(-100);}
			}
			test = test->back;
		}
	}
}
void weightlook(void){
	test = &root;
	
	printf("root\n");
	while(test->next != NULL){
		if(test->weight != 1000){
			for(tmpcnt = test->depth;tmpcnt > 0;tmpcnt--){
				printf("----");
			}
			printf("%4d\n", test->weight);
		}
	test = test->next;
	}
}

int situcheck(int mode) {//0:Check now board　1:Check future board
	
	//100:Player victory, n:Now score, -100:AI victory,
	int marucheck = 0, batucheck = 0;
	int cnt1, cnt2, shohai = 0;
	
	//Mode chage
	if(mode == 0) {
		for(cnt1 = 0;cnt1 < XMAX;cnt1++) {
			for(cnt2 = 0;cnt2 < YMAX;cnt2++) {
				tmpban[cnt1][cnt2] = root.pos[cnt1][cnt2];
			}
		}
	}
	if(mode == 1) {
		for(cnt1 = 0;cnt1 < XMAX;cnt1++) {
			for(cnt2 = 0;cnt2 < YMAX;cnt2++) {
				tmpban[cnt1][cnt2] = now->pos[cnt1][cnt2];
			}
		}
	}
	
	//Check victory
	for(cnt_i = 0;cnt_i < XMAX;cnt_i++) {
		if(tmpban[cnt_i][0] == tmpban[cnt_i][1] && tmpban[cnt_i][1] == tmpban[cnt_i][2] && tmpban[cnt_i][1] != 0) {
			//printf("%d,%d,%d\n", tmpban[cnt_i][0], tmpban[cnt_i][1], tmpban[cnt_i][2]);
			shohai = tmpban[cnt_i][1];
		}
	}
	for(cnt_j = 0;cnt_j < YMAX;cnt_j++) {
		if(tmpban[0][cnt_j] == tmpban[1][cnt_j] && tmpban[1][cnt_j] == tmpban[2][cnt_j] && tmpban[1][cnt_j] != 0) {
			shohai = tmpban[1][cnt_j];
		}
	}
	if(tmpban[0][0] == tmpban[1][1] && tmpban[1][1] == tmpban[2][2] && tmpban[1][1] != 0) {
		shohai = tmpban[1][1];
	}
	if(tmpban[2][0] == tmpban[1][1] && tmpban[1][1] == tmpban[0][2] && tmpban[1][1] != 0) {
		shohai = tmpban[1][1];
	}

	//Check score
	for(cnt1 = 0;cnt1 < XMAX;cnt1++) {
		if(tmpban[cnt1][0] != MARU && tmpban[cnt1][1] != MARU && tmpban[cnt1][2] != MARU) {
			batucheck++;
		}
		if(tmpban[cnt1][0] != BATU && tmpban[cnt1][1] != BATU && tmpban[cnt1][2] != BATU) {
			marucheck++;
		}
	}
	for(cnt2 = 0;cnt2 < YMAX;cnt2++) {
		if(tmpban[0][cnt2] != MARU && tmpban[1][cnt2] != MARU && tmpban[2][cnt2] != MARU) {
			batucheck++;
		}
		if(tmpban[0][cnt2] != BATU && tmpban[1][cnt2] != BATU && tmpban[2][cnt2] != BATU) {
			marucheck++;
		}
	}
	if(tmpban[0][0] != MARU && tmpban[1][1] != MARU && tmpban[2][2] != MARU) {
		batucheck++;
	}
	if(tmpban[0][0] != BATU && tmpban[1][1] != BATU && tmpban[2][2] != BATU) {
		marucheck++;
	}
	if(tmpban[2][0] != MARU && tmpban[1][1] != MARU && tmpban[0][2] != MARU) {
		batucheck++;
	}
	if(tmpban[2][0] != BATU && tmpban[1][1] != BATU && tmpban[0][2] != BATU) {
		marucheck++;
	}
	//printf("%d - %d = %d\n", marucheck, batucheck, marucheck - batucheck);
	if(shohai == MARU) {
		return 100;
	}
	if(shohai == BATU) {
		return -100;
	}
	
	return marucheck - batucheck;
}
//Check same board
int doitsucheck(int mode) {//0:Check now board　1:Check future board

	int check;//0:NotSame,1:Same,2:90,3:180,4:270,5:RLSymmetry,6:UDSymmetry,7:DiagonalSymmetry1,8:DiagonalSymmetry2
	int cnt1, cnt2;
	int position[XMAX][YMAX];
	int test, ichiji;
	
	
	//Mode change
	if(mode == 0) {
		for(cnt1 = 0;cnt1 < XMAX;cnt1++) {
			for(cnt2 = 0;cnt2 < YMAX;cnt2++) {
				position[cnt1][cnt2] = root.pos[cnt1][cnt2];
			}
		}
	}
	if(mode == 1) {
		for(cnt1 = 0;cnt1 < XMAX;cnt1++) {
			for(cnt2 = 0;cnt2 < YMAX;cnt2++) {
				position[cnt1][cnt2] = now->pos[cnt1][cnt2];
			}
		}
	}
	
	//Same check
	test = 0;
	for(cnt1 = 0;cnt1 < XMAX;cnt1++) {
		for(cnt2 = 0;cnt2 < YMAX;cnt2++) {
			if(tmpban[cnt1][cnt2] == position[cnt1][cnt2]) {
				test++;
			}
		}
	}
	if(test == 9) {
		return 1;
	}
	
	//Rotate check
	for(check = 2;check < 5; check++) {
		test = 0;
		//90 degree rotate
		ichiji = tmpban[0][0];
		tmpban[0][0] = tmpban[0][2];
		tmpban[0][2] = tmpban[2][2];
		tmpban[2][2] = tmpban[2][0];
		tmpban[2][0] = ichiji;
		ichiji = tmpban[0][1];
		tmpban[0][1] = tmpban[1][2];
		tmpban[1][2] = tmpban[2][1];
		tmpban[2][1] = tmpban[1][0];
		tmpban[1][0] = ichiji;
		
		for(cnt1 = 0;cnt1 < XMAX;cnt1++) {
			for(cnt2 = 0;cnt2 < YMAX;cnt2++) {
				if(tmpban[cnt1][cnt2] == position[cnt1][cnt2]) {
					test++;
				}
			}
		}
		if(test == 9) {
			return check;
		}
		
	}
	//90 degree rotate, return to origin
	ichiji = tmpban[0][0];
	tmpban[0][0] = tmpban[0][2];
	tmpban[0][2] = tmpban[2][2];
	tmpban[2][2] = tmpban[2][0];
	tmpban[2][0] = ichiji;
	ichiji = tmpban[0][1];
	tmpban[0][1] = tmpban[1][2];
	tmpban[1][2] = tmpban[2][1];
	tmpban[2][1] = tmpban[1][0];
	tmpban[1][0] = ichiji;
	
	//Right Left Symmetry
	ichiji = tmpban[0][0];
	tmpban[0][0] = tmpban[2][0];
	tmpban[2][0] = ichiji;
	ichiji = tmpban[0][1];
	tmpban[0][1] = tmpban[2][1];
	tmpban[2][1] = ichiji;
	ichiji = tmpban[0][2];
	tmpban[0][2] = tmpban[2][2];
	tmpban[2][2] = ichiji;
	
	test = 0;
	for(cnt1 = 0;cnt1 < XMAX;cnt1++) {
		for(cnt2 = 0;cnt2 < YMAX;cnt2++) {
			if(tmpban[cnt1][cnt2] == position[cnt1][cnt2]) {
				test++;
			}
		}
	}
	if(test == 9) {
		return 5;
	}
	//Right Left Symmetry, return to origin
	ichiji = tmpban[0][0];
	tmpban[0][0] = tmpban[2][0];
	tmpban[2][0] = ichiji;
	ichiji = tmpban[0][1];
	tmpban[0][1] = tmpban[2][1];
	tmpban[2][1] = ichiji;
	ichiji = tmpban[0][2];
	tmpban[0][2] = tmpban[2][2];
	tmpban[2][2] = ichiji;
	
	//Up Down Symmetry
	ichiji = tmpban[0][0];
	tmpban[0][0] = tmpban[0][2];
	tmpban[0][2] = ichiji;
	ichiji = tmpban[1][0];
	tmpban[1][0] = tmpban[1][2];
	tmpban[1][2] = ichiji;
	ichiji = tmpban[2][0];
	tmpban[2][0] = tmpban[2][2];
	tmpban[2][2] = ichiji;
	test = 0;
	for(cnt1 = 0;cnt1 < XMAX;cnt1++) {
		for(cnt2 = 0;cnt2 < YMAX;cnt2++) {
			if(tmpban[cnt1][cnt2] == position[cnt1][cnt2]) {
				test++;
			}
		}
	}
	if(test == 9) {
		return 6;
	}
	//Up Down Symmetry, return to origin
	ichiji = tmpban[0][0];
	tmpban[0][0] = tmpban[0][2];
	tmpban[0][2] = ichiji;
	ichiji = tmpban[1][0];
	tmpban[1][0] = tmpban[1][2];
	tmpban[1][2] = ichiji;
	ichiji = tmpban[2][0];
	tmpban[2][0] = tmpban[2][2];
	tmpban[2][2] = ichiji;
	
	//Diagonal Symmetry1
	ichiji = tmpban[0][0];
	tmpban[0][0] = tmpban[2][2];
	tmpban[2][2] = ichiji;
	ichiji = tmpban[1][0];
	tmpban[1][0] = tmpban[2][1];
	tmpban[2][1] = ichiji;
	ichiji = tmpban[0][1];
	tmpban[0][1] = tmpban[1][2];
	tmpban[1][2] = ichiji;
	test = 0;
	for(cnt1 = 0;cnt1 < XMAX;cnt1++) {
		for(cnt2 = 0;cnt2 < YMAX;cnt2++) {
			if(tmpban[cnt1][cnt2] == position[cnt1][cnt2]) {
				test++;
			}
		}
	}
	if(test == 9) {
		return 7;
	}
	//Diagonal Symmetry1, return to origin
	ichiji = tmpban[0][0];
	tmpban[0][0] = tmpban[2][2];
	tmpban[2][2] = ichiji;
	ichiji = tmpban[1][0];
	tmpban[1][0] = tmpban[2][1];
	tmpban[2][1] = ichiji;
	ichiji = tmpban[0][1];
	tmpban[0][1] = tmpban[1][2];
	tmpban[1][2] = ichiji;
	
	//Diagonal Symmetry2
	ichiji = tmpban[2][0];
	tmpban[2][0] = tmpban[0][2];
	tmpban[0][2] = ichiji;
	ichiji = tmpban[1][0];
	tmpban[1][0] = tmpban[0][1];
	tmpban[0][1] = ichiji;
	ichiji = tmpban[2][1];
	tmpban[2][1] = tmpban[1][2];
	tmpban[1][2] = ichiji;
	test = 0;
	for(cnt1 = 0;cnt1 < XMAX;cnt1++) {
		for(cnt2 = 0;cnt2 < YMAX;cnt2++) {
			if(tmpban[cnt1][cnt2] == position[cnt1][cnt2]) {
				test++;
			}
		}
	}
	if(test == 9) {
		return 8;
	}
	//Diagonal Symmetry2, return to origin
	ichiji = tmpban[2][0];
	tmpban[2][0] = tmpban[0][2];
	tmpban[0][2] = ichiji;
	ichiji = tmpban[1][0];
	tmpban[1][0] = tmpban[0][1];
	tmpban[0][1] = ichiji;
	ichiji = tmpban[2][1];
	tmpban[2][1] = tmpban[1][2];
	tmpban[1][2] = ichiji;
	
	return 0;
}