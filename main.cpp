#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <string.h>
#include <set>
#include <algorithm>

using namespace std;

#define AI 1
#define MAN 2
#define MAXVALUE 999999
#define MINVALUE -999999
#define DEPTH 4
#define OFFSET 1
#define RATIO 1

int nodeCnt = 0;
int evaCnt = 0;

//four directions
int direction[4][2] = {{0, 1}, {1, 0}, {1, -1}, {1, 1}};

//struct of (row, col)
struct Position
{
    Position(int row, int col){
        this->row = row;
        this->col = col;
    }
    bool operator==(const Position& b)const{
        return this->row == b.row && this->col == b.col;
    }
    bool operator<(const Position& b)const{
        if(this->row == b.row){
            return this->col < b.col;
        }
        else{
            return this->row < b.row;
        }
    }
    int row, col;
};

//type of chess and its value
struct ModelShape{
    string shape;
    int value;
};

//the table saved all types of chess
ModelShape modelTable[20];
int modelCnt = 0;

//the chessboard
char chessboard[20][20];

//the answer AI gives
int predictRow, predictCol;

//to mark if the position was calculated
int chessNumOnDrct[20][20][4];

//print the chessboard
void print(){
    printf("   ");
    for(int i = 1; i <= 15; i++){
        printf("%-4d", i);
    }
    cout << "\n-----------------------------------------------------------------\n";
    for(int i = 1; i <= 15; i++){
        printf("%2d|", i);
        for(int j = 1; j <= 15; j++){
            if(chessboard[i][j] == AI){
                cout << "+   ";
            }
            else if(chessboard[i][j] == MAN){
                cout << "-   ";
            }
            else{
                cout << "    ";
            }
        }
        cout << " |" << "\n\n";
    }
    cout << "-----------------------------------------------------------------\n";
}

//to sort types of chess by their values
bool compare(ModelShape a, ModelShape b){
    return a.value > b.value;
}

//initiate the model
void initModel(int nth, string model, int value){
    modelTable[nth].value = value;
    modelTable[nth].shape = model;
}

//initiate the table of models
void initTable(){
    initModel(modelCnt++, "ooooo", 900000);
    initModel(modelCnt++, "+oooo+", 300000);
    initModel(modelCnt++, "oooo+", 2500);
    initModel(modelCnt++, "+oooo", 2500);
    initModel(modelCnt++, "ooo+o", 3000);
    initModel(modelCnt++, "o+ooo", 3000);
    initModel(modelCnt++, "oo+oo", 2600);
    initModel(modelCnt++, "++ooo++", 3000);
    initModel(modelCnt++, "ooo++", 500);
    initModel(modelCnt++, "++ooo", 500);
    initModel(modelCnt++, "+o+oo+", 800);
    initModel(modelCnt++, "+oo+o+", 800);
    initModel(modelCnt++, "o++oo", 600);
    initModel(modelCnt++, "oo++o", 600);
    initModel(modelCnt++, "o+o+o", 550);
    initModel(modelCnt++, "+++oo+++", 650);
    initModel(modelCnt++, "oo+++", 150);
    initModel(modelCnt++, "+++oo", 150);
    initModel(modelCnt++, "++o+o++", 250);
    initModel(modelCnt++, "+o++o+", 200);
    sort(modelTable, modelTable + modelCnt, compare);
}

int getScore(string shape){
    for (int i = 0; i < modelCnt; i++){
        if(shape.find(modelTable[i].shape) != string::npos){
            return modelTable[i].value;
        }
    }

    return 0;
}

bool inBoundary(int row, int col){
    return row >= 1 && row <= 15 && col >= 1 && col <= 15;
}

vector<Position> pieceOnBoard;

int evaluator(){
    evaCnt++;
    int totalScore = 0;
    memset(chessNumOnDrct, 0, sizeof(chessNumOnDrct));

    for(vector<Position>::iterator iter = pieceOnBoard.begin(); iter != pieceOnBoard.end(); iter++){
        int row = iter->row;
        int col = iter->col;

        int player = chessboard[row][col];
        int enemy = AI + MAN - player;

        for (int drct = 0; drct < 4; drct++){
            if(chessNumOnDrct[row][col][drct] == -1){
                continue;
            }

            string shape;
            int i = row, j = col;
            while(inBoundary(i, j) && chessboard[i][j] != enemy){
                i -= direction[drct][0];
                j -= direction[drct][1];
            }
            i += direction[drct][0];
            j += direction[drct][1];
            while(inBoundary(i, j) && chessboard[i][j] != enemy){
                if(chessboard[i][j] == 0){
                    shape.append("+");
                }
                else if(chessboard[i][j] != 0){
                    shape.append("o");
                    chessNumOnDrct[i][j][drct] = -1;
                }
                i += direction[drct][0];
                j += direction[drct][1];
            }

            if(chessboard[row][col] == AI){
                totalScore += getScore(shape) * RATIO;
            }
            else{
                totalScore -= getScore(shape);
            }
        }
    }

    return totalScore;
}

bool isOver(){
    memset(chessNumOnDrct, 0, sizeof(chessNumOnDrct));

    for(vector<Position>::iterator iter = pieceOnBoard.begin(); iter != pieceOnBoard.end(); iter++){
        int row = iter->row;
        int col = iter->col;

        int player = chessboard[row][col];
        int enemy = AI + MAN - player;

        for (int drct = 0; drct < 4; drct++){
            if(chessNumOnDrct[row][col][drct] == -1){
                continue;
            }

            int count = 1; //itself is the one

            string shape;
            int i = row, j = col;
            while(inBoundary(i, j) && chessboard[i][j] == player){
                count++;
                i -= direction[drct][0];
                j -= direction[drct][1];
            }
            count--;

            i = row;
            j = col;
            while(inBoundary(i, j) && chessboard[i][j] == player){
                count++;
                chessNumOnDrct[i][j][drct] = -1;
                i += direction[drct][0];
                j += direction[drct][1];
            }
            count--;

            if(count >= 5){
                return true;
            }
        }
    }

    return false;
}

void setPiece(int row, int col, int player){
    chessboard[row][col] = player;
    pieceOnBoard.push_back(Position(row, col));
}

void delPiece(int row, int col){
    chessboard[row][col] = 0;
    pieceOnBoard.pop_back();
}

void getNewPos(vector<Position>& q2, set<Position>& s2, vector<Position>& q1, int row, int col){
    int cnt = 8;
    int u1 = row - 1, u2 = row - 2, d1 = row + 1, d2 = row + 2, l1 = col - 1, l2 = col - 2, r1 = col + 1, r2 = col + 2;
    // int row_Arr[24] = {u1,u1,u1,row,row,d1,d1,d1, u2,u2,u2,u2,u2,u1,u1,row,row,d1,d1,d2,d2,d2,d2,d2};
    // int col_Arr[24] = {l1,col,r1,l1,r1,l1,col,r1, l2,l1,col,r1,r2,l2,r2,l2,r2,l2,r2,l2,l1,col,r1,r2};

    int row_Arr[24] = {u1, u1, u1, row, row, d1, d1, d1};
    int col_Arr[24] = {l1, col, r1, l1, r1, l1, col, r1};
    for (int i = 0; i < cnt; i++)
    {
        if(!chessboard[row_Arr[i]][col_Arr[i]] && inBoundary(row_Arr[i], col_Arr[i])){
            q2.push_back(Position(row_Arr[i], col_Arr[i]));
            s2.insert(Position(row_Arr[i], col_Arr[i]));
        }
    }

    for(vector<Position>::iterator it1 = q1.begin(); it1 != q1.end(); it1++){
        if(s2.count(*it1) || (it1->row == row && it1->col == col)){
            continue;
        }
        q2.push_back(*it1);
    }


}

int alphaBeta(int depth, int alpha, int beta, int player, vector<Position> q1){
    nodeCnt++;
    if (depth == 0 || isOver())
    {
        return evaluator();
    }

    //convert q1 to s1
    set<Position> s1;
    for(vector<Position>::iterator iter = q1.begin(); iter != q1.end(); iter++){
        s1.insert(*iter);
    }

    if(player == AI){
        int v = MINVALUE;
        //Every child is every point in q1
        for (vector<Position>::iterator it = q1.begin(); it != q1.end(); it++){
            vector<Position> q2;
            set<Position> s2;
            getNewPos(q2, s2, q1, it->row, it->col);
            setPiece(it->row, it->col, AI);
            v = max(v, alphaBeta(depth - 1, alpha, beta, MAN, q2));
            delPiece(it->row, it->col);
            if(alpha < v){
                if(depth == DEPTH){
                    predictRow = it->row;
                    predictCol = it->col;
                }
                alpha = v;
            }
            if(alpha >= beta){
                break;
            }
        }
        return v;
    }
    else{
        int v = MAXVALUE;
        //Every child is every point in lastPossibleSet
        for (vector<Position>::iterator it = q1.begin(); it != q1.end(); it++){
            vector<Position> q2;
            set<Position> s2;
            getNewPos(q2, s2, q1, it->row, it->col);
            setPiece(it->row, it->col, MAN);
            v = min(v, alphaBeta(depth - 1, alpha, beta, AI, q2));
            delPiece(it->row, it->col);
            beta = min(beta, v);
            if(alpha >= beta){
                break;
            }
        }
        return v;
    }
}








int main(){
    //init
    initTable();

    setPiece(8, 8, AI);
    vector<Position> q1, t;
    set<Position> s1;
    getNewPos(q1, s1, t, 8, 8);
    print();
    while(1){
        int row, col;
        cin >> row >> col;

        setPiece(row, col, MAN);

        vector<Position> q2;
        set<Position> s2;
        getNewPos(q2, s2, q1, row, col);

        print();

        if(isOver()){
            cout << "You win!\n";
            getchar();
            getchar();
            break;
        }

        int value = alphaBeta(DEPTH, MINVALUE, MAXVALUE, AI, q2);

        setPiece(predictRow, predictCol, AI);

        vector<Position> q3;
        set<Position> s3;
        getNewPos(q3, s3, q2, predictRow, predictCol);

        q1 = q3;

        print();

        if(isOver()){
            cout << "AI win!\n";
            getchar();
            getchar();
            break;
        }

        printf("AI: row=%d, col=%d\n", predictRow, predictCol);
        printf("Value=%d, %d nodes, %d evaluations\n", value, nodeCnt, evaCnt);
    }

    return 0;
}
