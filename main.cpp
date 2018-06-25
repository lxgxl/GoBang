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
    initModel(modelCnt++, "ooooo", 100000);
    initModel(modelCnt++, "+oooo+", 30000);
    initModel(modelCnt++, "++ooo++", 10000);
    initModel(modelCnt++, "+ooo++", 6500);
    initModel(modelCnt++, "++ooo+", 6500);
    initModel(modelCnt++, "oooo+", 6000);
    initModel(modelCnt++, "+oooo", 6000);
    initModel(modelCnt++, "+++oo+++", 1000);
    initModel(modelCnt++, "+++oo++", 500);
    initModel(modelCnt++, "++oo+++", 500);
    initModel(modelCnt++, "++oo++", 200);
    initModel(modelCnt++, "oo", 100);
    initModel(modelCnt++, "++++o++++", 50);
    sort(modelTable, modelTable + modelCnt + 1, compare);
}

int getScore(string shape){
    for (int i = 0; i <= modelCnt; i++){
        if(shape.find(modelTable[i].shape) != string::npos){
            return modelTable[i].value;
        }
    }

    return 0;
}

vector<Position> pieceOnBoard;

int evaluator(){
    int totalScore = 0;
    memset(chessNumOnDrct, 0, sizeof(chessNumOnDrct));

    for(vector<Position>::iterator iter = pieceOnBoard.begin(); iter != pieceOnBoard.end(); iter++){
        int row = iter->row;
        int col = iter->col;

        int player = chessboard[row][col];
        int enemy = AI + MAN - player;

        for (int drct = 0; drct < 4; drct++){
            if(chessNumOnDrct[row][col][drct] == -2){
                continue;
            }

            string shape;
            int i = row, j = col;
            while(i >= 1 && j >= 1 && chessboard[i][j] != enemy){
                i -= direction[drct][0];
                j -= direction[drct][1];
            }
            i += direction[drct][0];
            j += direction[drct][1];
            while(i <= 15 && j <= 15 && chessboard[i][j] != enemy){
                if(chessboard[i][j] == 0){
                    shape.append("+");
                }
                else if(chessboard[i][j] != 0){
                    shape.append("o");
                    chessNumOnDrct[i][j][drct] = -2;
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
            if(chessNumOnDrct[row][col][drct] == -2){
                continue;
            }

            int count = 1; //itself is the one

            string shape;
            int i = row, j = col;
            while(i >= 1 && j >= 1 && chessboard[i][j] == player){
                count++;
                i -= direction[drct][0];
                j -= direction[drct][1];
            }
            count--;

            i = row;
            j = col;
            while(i <= 15 && j <= 15 && chessboard[i][j] == player){
                count++;
                chessNumOnDrct[i][j][drct] = -2;
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

void getNewPos(vector<Position>& q2, int row, int col, set<Position>& s2){
    int u1 = row-1, u2 = row-2, d1 = row+1, d2 = row+2, l1 = col-1, l2 = col-2, r1 = col+1, r2 = col+2;
    int row_Arr[24] = {u1,u1,u1,row,row,d1,d1,d1, u2,u2,u2,u2,u2,u1,u1,row,row,d1,d1,d2,d2,d2,d2,d2};
    int col_Arr[24] = {l1,col,r1,l1,r1,l1,col,r1, l2,l1,col,r1,r2,l2,r2,l2,r2,l2,r2,l2,l1,col,r1,r2};

    for(int i = 0; i < 20; i++){
        if(!chessboard[row_Arr[i]][col_Arr[i]]){
            q2.push_back(Position(row_Arr[i], col_Arr[i]));
            s2.insert(Position(row_Arr[i], col_Arr[i]));
        }
    }
}

int alphaBeta(int depth, int alpha, int beta, int player, vector<Position> q1){
    if(isOver()){
        return 100000;
    }
    if(depth == 0){
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
            getNewPos(q2, it->row, it->col, s2);
            for(vector<Position>::iterator it1 = q1.begin(); it1 != q1.end(); it1++){
                if(s2.count(*it1) || *it1 == *it){
                    continue;
                }
                q2.push_back(*it1);
            }
            setPiece(it->row, it->col, AI);
            v = max(v, alphaBeta(depth - 1, alpha, beta, MAN, q2));
            delPiece(it->row, it->col);
            if(alpha < v){
                predictRow = it->row;
                predictCol = it->col;
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
            getNewPos(q2, it->row, it->col, s2);
            for(vector<Position>::iterator it1 = q1.begin(); it1 != q1.end(); it1++){
                if(s2.count(*it1) || *it1 == *it){
                    continue;
                }
                q2.push_back(*it1);
            }
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
    vector<Position> q1;
    set<Position> s1;
    getNewPos(q1, 8, 8, s1);
    print();
    while(1){
        int row, col;
        cin >> row >> col;
        setPiece(row, col, MAN);
        vector<Position> q2;
        set<Position> s2;
        getNewPos(q2, row, col, s2);
        for(vector<Position>::iterator it1 = q1.begin(); it1 != q1.end(); it1++){
            if(s2.count(*it1) || *it1 == Position(row, col)){
                continue;
            }
            q2.push_back(*it1);
        }
        print();
        alphaBeta(DEPTH, MINVALUE, MAXVALUE, AI, q2);
        setPiece(predictRow, predictCol, AI);
        vector<Position> q3;
        set<Position> s3;
        getNewPos(q3, predictRow, predictCol, s3);
        for(vector<Position>::iterator it1 = q2.begin(); it1 != q2.end(); it1++){
            if(s3.count(*it1) || *it1 == Position(predictRow, predictCol)){
                continue;
            }
            q3.push_back(*it1);
        }
        q1 = q3;
        print();
        printf("AI: row=%d, col=%d\n", predictRow, predictCol);
    }

    return 0;
}