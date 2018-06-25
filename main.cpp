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

struct Position
{
    Position(int row, int col){
        this->row = row;
        this->col = col;
    }
    bool operator==(const Position& b){
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



set<Position> pieceSet;

struct ModelShape{
    // int shape[10];
    string shape;
    int len;
    int value;
};

ModelShape modelTable[20];
int modelCnt = 0;

void initModel(int nth, string model, int value){
    modelTable[nth].value = value;
    modelTable[nth].len = model.length();
    modelTable[nth].shape = model;
}

int direction[4][2] = {{0, 1}, {1, 0}, {1, -1}, {1, 1}};

char chessboard[20][20];
int chessNumOnDrct[20][20][4];

int predictRow, predictCol;

bool compare(ModelShape a, ModelShape b){
    return a.value > b.value;
}

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

void initTable(){
    initModel(0, "AAAAA", 900000);
    initModel(1, "?AAAA?", 300000);
    initModel(2, "AAAA?", 2500);
    initModel(3, "?AAAA", 2500);
    initModel(4, "AAA?A", 3000);
    initModel(5, "A?AAA", 3000);
    initModel(6, "AA?AA", 2600);
    initModel(7, "??AAA??", 3000);
    initModel(8, "AAA??", 500);
    initModel(9, "??AAA", 500);
    initModel(10, "?A?AA?", 800);
    initModel(11, "?AA?A?", 800);
    initModel(12, "A?A?A", 550);
    initModel(13, "???AA???", 650);
    initModel(14, "AA???", 150);
    initModel(15, "???AA", 150);
    initModel(16, "??A?A??", 250);
    initModel(17, "?A??A?", 200);
    sort(modelTable, modelTable + 18, compare);
    modelCnt = 17;
}

set<Position>& getPossibleSet(set<Position>& possibleSet, int row, int col){
    for (int i = row - OFFSET >= 1 ? row - OFFSET : 1; i <= (row + OFFSET <= 15 ? row + OFFSET : 15); i++){
        for(int j = col - OFFSET >= 1 ? col - OFFSET : 1; j <= (col + OFFSET <= 15 ? col + OFFSET : 15); j++){
            if(!chessboard[i][j]){
                possibleSet.insert(Position(i, j));
            }
        }
    }

    return possibleSet;
}

void setPiece(int row, int col, int player, set<Position>& possibleSet){
    chessboard[row][col] = player; 
    getPossibleSet(possibleSet, row, col);
    possibleSet.erase(Position(row, col));
}

void delPiece(int row, int col){
    chessboard[row][col] = 0;
}

int getScore(string shape){
    for (int i = 0; i <= modelCnt; i++){
        if(shape.find(modelTable[i].shape) != string::npos){
            return modelTable[i].value;
        }
    }

    return 0;
}

bool isGameOver(){
    memset(chessNumOnDrct, 0, sizeof(chessNumOnDrct));
    for (int row = 1; row <= 15; row++){
        for (int col = 1; col <= 15; col++){
            if(!chessboard[row][col]){
                continue;
            }
            int player = chessboard[row][col];
            int enemy = AI + MAN - player;
            for (int drct = 0; drct < 4; drct++){
                if(chessNumOnDrct[row][col][drct] == -1){
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
                        shape.append("?");
                    }
                    else if(chessboard[i][j] != 0){
                        shape.append("A");
                        chessNumOnDrct[i][j][drct] = -1;
                    }
                    i += direction[drct][0];
                    j += direction[drct][1];
                }

                if(getScore(shape) == 900000){
                    return true;
                }
            }
        }
    }

    return false;
}

int evaluator(){
    int totalScore = 0;
    memset(chessNumOnDrct, 0, sizeof(chessNumOnDrct));
    for (int row = 1; row <= 15; row++){
        for (int col = 1; col <= 15; col++){
            if(!chessboard[row][col]){
                continue;
            }
            int player = chessboard[row][col];
            int enemy = AI + MAN - player;
            for (int drct = 0; drct < 4; drct++){
                if(chessNumOnDrct[row][col][drct] == -1){
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
                        shape.append("?");
                    }
                    else if(chessboard[i][j] != 0){
                        shape.append("A");
                        chessNumOnDrct[i][j][drct] = -1;
                    }
                    i += direction[drct][0];
                    j += direction[drct][1];
                }

                if(chessboard[row][col] == AI){
                    totalScore += getScore(shape);
                }
                else{
                    totalScore -= getScore(shape);
                }
            }
        }
    }

    return totalScore;
}

int alphaBeta(int depth, int alpha, int beta, int player, set<Position>lastPossibleSet){
    if(depth == 0 || isGameOver()){
        return evaluator();
    }

    if(player == AI){
        int v = MINVALUE;
        //Every child is every point in lastPossibleSet
        for (set<Position>::iterator it = lastPossibleSet.begin(); it != lastPossibleSet.end(); it++){
            set<Position> possibleSet = lastPossibleSet;
            setPiece(it->row, it->col, AI, possibleSet);
            v = max(v, alphaBeta(depth - 1, alpha, beta, MAN, possibleSet));
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
        for (set<Position>::iterator it = lastPossibleSet.begin(); it != lastPossibleSet.end(); it++){
            set<Position> possibleSet = lastPossibleSet;
            setPiece(it->row, it->col, MAN, possibleSet);
            v = min(v, alphaBeta(depth - 1, alpha, beta, AI, possibleSet));
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
    initTable();

    set<Position> possibleSet;
    setPiece(8, 8, AI, possibleSet);
    print();
    while(1){
        int row, col;
        cin >> row >> col;
        setPiece(row, col, MAN, possibleSet);
        print();
        alphaBeta(DEPTH, MINVALUE, MAXVALUE, AI, possibleSet);
        setPiece(predictRow, predictCol, AI, possibleSet);
        print();
        printf("AI: row=%d, col=%d\n", predictRow, predictCol);
    }
    

    getchar();
    return 0;
}