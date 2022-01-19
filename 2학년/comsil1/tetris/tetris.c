#include "tetris.h"

static struct sigaction act, oact;
int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));
	createRankList();
	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_RECO : recommendedPlay(); break;
		case MENU_EXIT: exit=1; break;
		case MENU_RECO_FAST: recommendedPlay_Fast(); break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;
	for(i=0;i<BLOCK_NUM;i++)
		nextBlock[i]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	modified_recommend(root);
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);

	move(9,WIDTH+10);
	DrawBox(10,WIDTH+10,4,8);
	
	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(19,WIDTH+10);
	printw("SCORE");
	DrawBox(20,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}

void PrintScore(int score){
	move(21,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
	for( i = 0; i < 4; i++ ){
		move(11+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[2]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}
void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}
void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	getch();
	refresh();
	newRank(score);
	writeRankFile();
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	printw("5. fast recommended play\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i,j,flag=1;
	for(i=0;i<BLOCK_HEIGHT && flag ;i++){
		for(j=0;j<BLOCK_WIDTH && flag;j++){
			if(block[currentBlock][blockRotate][i][j]==1){
				if(i+blockY<0||j+blockX<0||i+blockY>HEIGHT-1||j+blockX>=WIDTH)
					flag=0;
				else if(f[i+blockY][j+blockX]==1)
					flag=0;
			}
		}
	}
	return flag;
}
int CheckToMoveForRec(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i,j;
	for(i=0;i<BLOCK_HEIGHT;i++){
		for(j=0;j<BLOCK_WIDTH ;j++){
			if(block[currentBlock][blockRotate][i][j]==1 && f[blockY+i][blockX+j]==1)
				return 0;
			else if(block[currentBlock][blockRotate][i][j]==1&&blockY+i>=HEIGHT) return 0;
			else if(block[currentBlock][blockRotate][i][j]==1&& (j+blockX<0||j+blockX>=WIDTH))
					return 0;
			
		}
	}
	return 1;
}
void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int prev_x,prev_y,prev_rotate;
	int i,j;

	switch(command){
		case KEY_UP:{
						prev_rotate = (blockRotate+3)%4;
						prev_x = blockX;
						prev_y = blockY;
						break;
					}
		case KEY_DOWN: {
						   prev_x = blockX;
						   prev_y = blockY-1;
						   break;
					   }
		case KEY_LEFT:{
						   prev_x = blockX+1;
						   prev_y = blockY;
						   break;
		}
		case KEY_RIGHT:{
						   prev_x = blockX-1;
						   prev_y = blockY;
						   break;
		}
		default : break;
	}
	for(i=0;i<BLOCK_HEIGHT;i++){
		for(j=0;j<BLOCK_WIDTH;j++){
			if(command==KEY_UP){
				if(block[currentBlock][prev_rotate][i][j]==1 && i+prev_y>=0){
					move(i+prev_y+1,j+prev_x+1);
					printw("%c",'.');
					if(field[i+shadow][j+prev_x]!=1){
						move(i+shadow+1,j+prev_x+1);
						printw("%c",'.');
					}
				}
			}
			else if(block[currentBlock][blockRotate][i][j]==1 && i+prev_y>=0){
				move(i+prev_y+1,j+prev_x+1);
				printw("%c",'.');
			}
			if(block[currentBlock][blockRotate][i][j]==1&&command !=KEY_DOWN && command != KEY_UP && i+shadow<HEIGHT){
				if(field[i+shadow][j+prev_x]!=1){
					move(i+shadow+1,j+prev_x+1);
					printw("%c",'.');
				}
			}
			if(block[currentBlock][recommendR][i][j]==1&& i+recommendY<HEIGHT){
				if(field[i+recommendY][j+recommendX]!=1){
					move(i+recommendY+1,j+recommendX+1);
					printw("%c",'.');
				}
			}
		}
	}
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	move(20,20);	

}

void BlockDown(int sig){
	// user code
	move(20,20);
	timed_out=0;
	if(CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)) {
		blockY++;
		DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);
		return;
	}	
	if(blockY==-1)
		gameOver = 1;
	score += AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
	score += DeleteLine(field);
	int i;
	for(i=0;i<BLOCK_NUM-1;i++)
		nextBlock[i] = nextBlock[i+1];
	nextBlock[i] = rand()%7;
	modified_recommend(root);
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	DrawNextBlock(&nextBlock[0]);
	PrintScore(score);
	DrawField();
	
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	
	int i,j,touched=0;
	for(i=0;i<BLOCK_HEIGHT;i++){
		for(j=0;j<BLOCK_WIDTH;j++){
			if(block[currentBlock][blockRotate][i][j]==1){
				f[i+blockY][j+blockX]=1;
				if(f[i+blockY+1][j+blockX]==1 || i+blockY == HEIGHT-1)touched++;
			}
		}
	}
	return touched*10;

}

int DeleteLine(char f[HEIGHT][WIDTH]){
	int num=0,check,i,j,k,l;
	for(i=0;i<HEIGHT;i++){
		check=1;
		for(j=0;j<WIDTH;j++){
			if(f[i][j]==0) check=0;
		}
		if(check==1){
			for(j=0;j<WIDTH;j++)
				f[i][j]=0;
			num++;
			for(k=i;k>0;k--)
				for(l=0;l<WIDTH;l++)
					f[k][l] = f[k-1][l];

		}
	}
	return num*num*100;
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
	int i,j;
	shadow = y;
	while(CheckToMove(field,blockID,blockRotate,shadow,x)){
			shadow++;
	}
	shadow--;
	for(i=0;i<BLOCK_HEIGHT;i++){
		for(j=0;j<BLOCK_WIDTH;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+shadow+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",'/');
				attroff(A_REVERSE);
			}
		}
	}
	move(HEIGHT,WIDTH+10);

}
void DrawBlockWithFeatures(int y,int x,int blockID,int blockRotate){
	DrawRecommend(recommendY,recommendX,blockID,recommendR,'R');
	DrawShadow(y,x,blockID,blockRotate);
	DrawBlock(y,x,nextBlock[0],blockRotate,' ');
}
void createRankList(){
	// user code
	FILE* fp = fopen("rank.txt","r");
	int i;
		
	if(fscanf(fp,"%d",&rank_num)!=EOF){
	NodePointer new,walk,prev;
	for(i=0;i<rank_num;i++){
		new = (NodePointer)malloc(sizeof(Node));
		fscanf(fp,"%s %d",new->name,&new->score);
		new->link = NULL;
		prev = head;
		if(head==NULL){
			head = new;
		}else{
			for(walk=head;(new->score < walk->score);walk=walk->link){
				prev = walk;
				if(walk->link==NULL){
					walk=walk->link;
					break;
				}	
			}
			if(walk!=head){
				prev->link = new;          //리스트의 중간 삽입이나 마지막 삽입일 경우
				if(walk!=NULL)			
					new->link = walk;
			}
			else{
				new->link = head;				// 리스트의 처음일 경우
				head = new;
			}
		}
	}
	}
	fclose(fp);
}

void rank(){
	// user cod
	clear();
	move(0,0);
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");
	int x=0,y=0;
	char find[NAMELEN];
	int find_rank;
	int command;
	command = wgetch(stdscr);
	switch(command){
		
		case '1': {
			echo();
			move(3,0);
			printw("X: ");
			refresh();
			scanw("%d",&x);
			printw("Y: ");
			refresh();
			scanw("%d",&y);
			Rank_Print(x,y);
			break;
		}
		case '2':{
			echo();
			move(3,0);
			printw("input the name: ");
			scanw("%s",find);
			refresh();
			Find_Name(find);
			break;
		}
		case '3':{
			echo();
			move(3,0);
			printw("input the rank: ");
			scanw("%d",&find_rank);
			refresh();
			Find_Rank(find_rank);
			break;
		}
		default: break;

	}
	getch();
}
void Find_Rank(int target){
	NodePointer walk,prev=head;
	int cnt=1;
	if(target<1||target>rank_num){
		printw("\nsearch failure: the rank not in the list\n");
		return;
	}
	for(walk=head;cnt!=target;walk=walk->link,cnt++)
		prev = walk;
	prev->link = walk->link;
	free(walk);
	rank_num--;
	printw("\nresult: the rank deleted\n");
	writeRankFile();
}
void Find_Name(char* target){
	NodePointer walk;
	int flag,find=false;
	printw("     name      |	score\n");
	printw("----------------------------------\n");
	for(walk = head; walk!=NULL;walk=walk->link){
		flag = strcmp(target,walk->name);
		if(flag==0){
			printw("%-15s| %d\n",walk->name,walk->score);
			find = true;
		}
	}
	if(!find)
		printw("\nsearch failure: no rank in the list\n");

}
void Rank_Print(int x,int y){
	int cnt=0;
	int first,last;
	printw("     name      |	score\n");
	printw("----------------------------------\n");
	NodePointer walk;
	if((x>y && x!=0 && y!=0)|| x < 0 || y < 0 )
		printw("\nsearch failure: no rank in the list\n");
	else{
		first = (x==0)? 0:x-1;
		last = (y==0)? rank_num-1:y-1;
		for(walk = head; walk!=NULL;walk=walk->link,cnt++){
			if(cnt >= first && cnt <= last)
				printw("%-15s| %d\n",walk->name,walk->score);
		}
	}
}
void writeRankFile(){
	
	FILE* fp = fopen("rank.txt","w");
	NodePointer walk;
	fprintf(fp,"%d\n",rank_num);
	for(walk = head; walk !=NULL;walk = walk->link)
		fprintf(fp,"%s %d\n",walk->name,walk->score);
	
	fclose(fp);

}

void newRank(int score){
	clear();
	char temp[NAMELEN];
	NodePointer new = (NodePointer)malloc(sizeof(Node));
	NodePointer walk,prev;
	move(0,0);
	new->score = score;
	new->link = NULL;
	rank_num++;
	echo();
	printw("your name: ");

	wgetnstr(stdscr,temp,15);
	strcpy(new->name,temp);
	prev = head;
	if(head!=NULL){
		for(walk=head;(new->score < walk->score);walk=walk->link){
			prev = walk;	
			if(walk->link==NULL){
				walk=walk->link;	
				break;
			}
		}	
		if(walk!=head){
			prev->link = new;
			if(walk!=NULL)
				new->link = walk;	
		}
		else{
			new->link = head;
			head = new;
		}
	}
	else head = new;
	
}
void DrawRecommend(int y, int x, int blockID,int blockRotate,char tile){
	// user code
	int i,j;
	for(i=0;i<BLOCK_HEIGHT;i++){
		for(j=0;j<BLOCK_WIDTH;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}
	}
	move(HEIGHT,WIDTH+10);		
		
}
int modified_recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int i,j;
	if(root==NULL){
	re_start = clock();
		root = (RecNode*)malloc(sizeof(RecNode));
		root->level = 0; root->accumulatedScore=0;
		root->curBlockID = nextBlock[0];
		for(i=0;i<HEIGHT;i++)
			for(j=0;j<WIDTH;j++)
					root->recField[i][j] = field[i][j];
	}
	int rotate_num;
	int case_num;
	int y ;
	int s,k;
	if(root->curBlockID==0 || root->curBlockID == 5 || root->curBlockID == 6){
		rotate_num=2;
		case_num = 17;
	}
	else if(root->curBlockID==4){
		rotate_num = 1;
		case_num = 9;
	}
	else{
		rotate_num = 4;
		case_num = 34;
	}

	int	cnt=0;
	int min = 90000;
	size += sizeof(RecNode)*case_num;
	for(i=0;i<case_num;i++){
		root->child[i] = (RecNode*)malloc(sizeof(RecNode));
		root->child[i]->curBlockID = nextBlock[root->level];
		root->child[i]->accumulatedScore =0;
		(root->child[i])->level = (root->level)+1;									//child의 레벨 증가 . 즉, 다음 루트노드의 레벨 증가시킴
		for( k=0;k<HEIGHT;k++){
			for( s=0;s<WIDTH;s++)
				root->child[i]->recField[k][s] = root->recField[k][s];
		}
	}
	for(i=0;i<rotate_num;i++){
		for(j=start_idx[root->curBlockID][i];j<=last_idx[root->curBlockID][i];j++){
			y = -1;	
			while(CheckToMoveForRec(root->child[cnt]->recField,root->child[cnt]->curBlockID,i,y,j)){
				y++;
			}
			y--;
			if(y<=-1) root->child[cnt]->accumulatedScore =-1;
			else{
			(root->child[cnt])->accumulatedScore += AddBlockToField(root->child[cnt]->recField,root->child[cnt]->curBlockID,i,y,j);
			(root->child[cnt])->accumulatedScore += DeleteLine(root->child[cnt]->recField);
			//DeleteLine해서 accumulatedScore에 저장함
			min = min>root->child[cnt]->accumulatedScore?root->child[cnt]->accumulatedScore:min;
			if(root->child[cnt]->level<MAX_LEVEL) 
				if(root->child[cnt]->accumulatedScore > min && cnt!=0) root->child[cnt]->accumulatedScore += modified_recommend(root->child[cnt]);
			}
			if( max <= root->child[cnt]->accumulatedScore){
				max  = root->child[cnt]->accumulatedScore;
				if(root->level==0){
					recommendR = i;	
					recommendY = y; 
					recommendX = j;
					good = cnt;
				}
			}
			cnt++;
		}
	}
	if(root->level!=0){
		for(i=0;i<case_num;i++){
			free(root->child[i]);
		}
		return max;
	}
	j=0;
	for(i=0;i<case_num;i++){
		free(root->child[i]);
	}
	if(root->level==0){
		free(root);
		root= NULL;
		re_end = clock();
		re_time += ((double)(re_end-re_start))/CLOCKS_PER_SEC;
	}
}

void BlockDownForRec(int sig){
	move(20,20);
	timed_out=0;	
	int i;
		if(CheckToMove(field,nextBlock[0],recommendR,0,recommendX)==0){
			gameOver=1;
		}
	score += AddBlockToField(field,nextBlock[0],recommendR,recommendY,recommendX);
	score += DeleteLine(field);
	for(i=0;i<BLOCK_NUM-1;i++)
		nextBlock[i] = nextBlock[i+1];

	nextBlock[i] = rand()%7;
	modified_recommend(root);
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	DrawNextBlock(&nextBlock[0]);
	PrintScore(score);
	DrawField();
}
void recommendedPlay(){

	int command;
	clear();
	start_time = time(NULL);
	act.sa_handler = BlockDownForRec;
	sigaction(SIGALRM,&act,&oact);

	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(command==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();
			return;
		}else{
			DrawRecommend(recommendY,recommendX,nextBlock[0],recommendR,'R');
		}
	}while(!gameOver);
	end_time=time(NULL);
	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	move(HEIGHT/2+1,WIDTH/2-4);
	printw("Time_t : %d ",end_time-start_time);
	move(HEIGHT/2+2,WIDTH/2-4);
	printw("score(t) : %d",score);
	move(HEIGHT/2+3,WIDTH/2-4);
	printw("space(t) : %lld",size);
	move(HEIGHT/2+4,WIDTH/2-4);
	printw("time(t) : %.2lf",re_time);
	

	getch();
	refresh();
	newRank(score);
	writeRankFile();
	
}
void recommendedPlay_Fast(){

	int command;
	start_time = time(NULL);
	clear();
	act.sa_handler = BlockDownForRec;
	sigaction(SIGALRM,&act,&oact);

	InitTetris();
	do{
		if(timed_out==0){
			ualarm(1000,0);
			timed_out=1;
		}

		command = GetCommand();
		if(command==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();
			return;
		}else{
			//recommend(root);
			DrawRecommend(recommendY,recommendX,nextBlock[0],recommendR,'R');
		}
	}while(!gameOver);
	end_time = time(NULL);
	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,6,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	move(HEIGHT/2+1,WIDTH/2-4);
	printw("Time_t : %d ",end_time-start_time);
	move(HEIGHT/2+2,WIDTH/2-4);
	printw("score(t) : %d",score);
	move(HEIGHT/2+3,WIDTH/2-4);
	printw("space(t) : %lld",size);
	move(HEIGHT/2+4,WIDTH/2-4);
	printw("time(t) : %.2lf",re_time);
	getch();
	refresh();
	newRank(score);
	writeRankFile();	
}
