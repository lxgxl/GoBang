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

#define GLOBAL 0
#define PORTION 1
#define LOREATT 2
#define LOREDEFDEF 3
#define LOREDEFATT 4

int nodeCnt = 0;
int evaCnt = 0;
int loreCnt = 0;

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

//special model for loration
struct LoreModel{
    string shape;
    int pieceNum;
    vector<int> effectivePos;
};

LoreModel AttTab[50], DefDTab[50], DefATab[50];
int AttCnt, DefDCnt, DefACnt;

//the table saved all types of chess
ModelShape modelTable[20];
int modelCnt = 0;

//the chessboard
char chessboard[20][20];

//the answer AI gives
int predictRow, predictCol;

//to mark if the position was calculated
int chessNumOnDrct[20][20][4];

bool portionCompare(Position, Position);
int heuristicScore(int, int);

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

bool portionCompare(Position a, Position b){
    return heuristicScore(a.row, a.col) > heuristicScore(b.row, b.col);
}

//initiate the model
void initModel(int nth, string model, int value){
    modelTable[nth].value = value;
    modelTable[nth].shape = model;
}

void initLoreModel(LoreModel model[], int nth, string shape, int pieceNum, vector<int> effectivePos){
    model[nth].shape = shape;
    model[nth].pieceNum = pieceNum;
    model[nth].effectivePos = effectivePos;
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

    int pieceNum;
    // Attacker search from ATTACKER
    //4
    pieceNum = 4;
    initLoreModel(AttTab, AttCnt++, "oooo+", pieceNum, {4});
    initLoreModel(AttTab, AttCnt++, "ooo+o", pieceNum, {3});
    initLoreModel(AttTab, AttCnt++, "oo+oo", pieceNum, {2});
    initLoreModel(AttTab, AttCnt++, "o+ooo", pieceNum, {1});
    initLoreModel(AttTab, AttCnt++, "+oooo", pieceNum, {0});
    //3
    pieceNum = 3;
    initLoreModel(AttTab, AttCnt++, "+ooo+", pieceNum, {-1, 0, 4, 5});
    initLoreModel(AttTab, AttCnt++, "+oo+o", pieceNum, {0, 3, 5});
    initLoreModel(AttTab, AttCnt++, "o+oo+", pieceNum, {-1, 1, 4});
    initLoreModel(AttTab, AttCnt++, "+o+oo", pieceNum, {0, 2, 5});
    initLoreModel(AttTab, AttCnt++, "oo+o+", pieceNum, {-1, 2, 4});
    initLoreModel(AttTab, AttCnt++, "++ooo", pieceNum, {0, 1});
    initLoreModel(AttTab, AttCnt++, "ooo++", pieceNum, {3, 4});
    initLoreModel(AttTab, AttCnt++, "o+o+o", pieceNum, {1, 3});
    initLoreModel(AttTab, AttCnt++, "o++oo", pieceNum, {1, 2});
    initLoreModel(AttTab, AttCnt++, "oo++o", pieceNum, {2, 3});
    //2
    pieceNum = 2;
    initLoreModel(AttTab, AttCnt++, "++oo++", pieceNum, {0, 1, 4, 5});
    initLoreModel(AttTab, AttCnt++, "+o+o++", pieceNum, {0, 2, 4});
    initLoreModel(AttTab, AttCnt++, "++o+o+", pieceNum, {1, 3, 5});
    initLoreModel(AttTab, AttCnt++, "+o++o+", pieceNum, {2, 3});

    //Defender defend, search from ATTACKER. Attacker can use it to defend!
    //4
    pieceNum = 4;
    initLoreModel(DefDTab, DefDCnt++, "oooo+", pieceNum, {4});
    initLoreModel(DefDTab, DefDCnt++, "ooo+o", pieceNum, {3});
    initLoreModel(DefDTab, DefDCnt++, "oo+oo", pieceNum, {2});
    initLoreModel(DefDTab, DefDCnt++, "o+ooo", pieceNum, {1});
    initLoreModel(DefDTab, DefDCnt++, "+oooo", pieceNum, {0});
    //3
    pieceNum = 3;
    initLoreModel(DefDTab, DefDCnt++, "+ooo++", pieceNum, {0, 4});
    initLoreModel(DefDTab, DefDCnt++, "++ooo+", pieceNum, {1, 5});
    initLoreModel(DefDTab, DefDCnt++, "+oo+o+", pieceNum, {0, 3, 5});
    initLoreModel(DefDTab, DefDCnt++, "+o+oo+", pieceNum, {0, 2, 5});
}

bool inBoundary(int row, int col){
    return row >= 1 && row <= 15 && col >= 1 && col <= 15;
}

int getScore(string shape){
    for (int i = 0; i < modelCnt; i++){
        if(shape.find(modelTable[i].shape) != string::npos){
            return modelTable[i].value;
        }
    }

    return 0;
}

void getShape(bool setChessOnDrct, string& shape, int row, int col, int drct,int who, int* startRow = NULL, int* startCol = NULL, int* endRow = NULL, int *endCol = NULL){
    if(chessNumOnDrct[row][col][drct] == -1){
        return;
    }

    int i = row, j = col;
    while(inBoundary(i, j) && (chessboard[i][j] == who || chessboard[i][j] == 0)){
        i -= direction[drct][0];
        j -= direction[drct][1];
    }
    i += direction[drct][0];
    j += direction[drct][1];

    if(startRow && startCol)
        *startRow = i, *startCol = j;

    while(inBoundary(i, j) && (chessboard[i][j] == who || chessboard[i][j] == 0)){
        if(chessboard[i][j] == 0){
            shape.append("+");
        }
        else if(chessboard[i][j] != 0){
            shape.append("o");
            if(setChessOnDrct){
                chessNumOnDrct[i][j][drct] = -1;
            }
        }
        i += direction[drct][0];
        j += direction[drct][1];
    }

    if(endRow && endCol){
        *endRow = i - direction[drct][0];
        *endCol = j - direction[drct][1];
    }
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
            getShape(1, shape, row, col, drct, player);

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

int getHalfShape(string& shape, int row, int col, int rdrct, int cdrct){
    int i = row, j = col;
    int me = 0, t = 3;

    while(inBoundary(i, j)){
        if(chessboard[i][j] != 0 && me == 0){
            me = chessboard[i][j];
            t -= me;
        }
        else if(chessboard[i][j] == t){
            break;
        }
        i -= rdrct;
        j -= cdrct;
    }

    if(me == 0){
        return 0;
    }

    i += rdrct;
    j += cdrct;
    chessboard[row][col] = me;
    while(inBoundary(i, j)){
        if(chessboard[i][j] == t){
            break;
        }
        else if(chessboard[i][j] == me){
            shape.append("o");
        }
        else if(chessboard[i][j] == 0){
            shape.append("+");
        }
        i += rdrct;
        j += cdrct;
    }
    chessboard[row][col] = 0;

    return me;
}

int heuristicScore(int row, int col){
    int totalScore = 0;
    for (int drct = 1; drct < 4; drct++){
        string shape_a, shape_b;
        int a = getHalfShape(shape_a, row, col, -direction[drct][0], -direction[drct][1]);
        int b = getHalfShape(shape_b, row, col, direction[drct][0], direction[drct][1]);

        if(a == 0 && b == 0){
            totalScore += 0;
        }
        else if(a == b){
            totalScore += getScore(shape_a);
        }
        else if(a == 0 && b){
            totalScore += getScore(shape_b);
        }
        else if(a && b == 0){
            totalScore += getScore(shape_a);
        }
        else{
            totalScore += getScore(shape_a) + getScore(shape_b);
        }
    }

    return totalScore;
}

bool getLorePos(string& shape, int startRow, int startCol, int drct, set<Position> posSave[], LoreModel posTab[], int tabCnt){

    for(int i = 0; i < tabCnt; i++){
        int subStrOffset = shape.find(posTab[i].shape);

        if(subStrOffset == string::npos){
            continue;
        }

        int pieceNum = posTab[i].pieceNum;
        int offsetRow = startRow + subStrOffset * direction[drct][0];
        int offsetCol = startCol + subStrOffset * direction[drct][1];

        for(vector<int>::iterator it = posTab[i].effectivePos.begin(); it != posTab[i].effectivePos.end(); it++){
            Position p(offsetRow + *it * direction[drct][0], offsetCol + *it * direction[drct][1]);

            if(!inBoundary(p.row, p.col)){
                continue;
            }
            if(*it < 0 || *it >= posTab[i].shape.length()){
                if(chessboard[p.row][p.col] != 0){
                    continue;
                }
            }

            posSave[pieceNum].insert(p);
        }

        return true;
    }

    return false;
}

void unionSet(set<Position>& inS1, set<Position>& dst){
    for(set<Position>::iterator it1 = inS1.begin(); it1 != inS1.end(); it1++){
        if(dst.count(*it1)){
            continue;
        }
        dst.insert(*it1);
    }
}

void getDoubleLorePos(set<Position> enemyPos[], string& shape, int startRow, int startCol, int drct, int who){
    for(int i = 5; i <= 14; i++){
        int subStrOffset = shape.find(AttTab[i].shape);

        if(subStrOffset == string::npos){
            continue;
        }

        int pieceNum = AttTab[i].pieceNum;
        int offsetRow = startRow + subStrOffset * direction[drct][0];
        int offsetCol = startCol + subStrOffset * direction[drct][1];

        for(vector<int>::iterator it = AttTab[i].effectivePos.begin(); it != AttTab[i].effectivePos.end(); it++){
            Position p(offsetRow + *it * direction[drct][0], offsetCol + *it * direction[drct][1]);

            if(!inBoundary(p.row, p.col) || enemyPos[pieceNum].count(p)){
                continue;
            }
            if(*it < 0 || *it >= AttTab[i].shape.length()){
                if(chessboard[p.row][p.col] != 0){
                    continue;
                }
            }

            //find the second lore model
            for(int drct2 = 0; drct2 < 4; drct2++){
                bool hasShape2 = false;
                string shape2;
                if(drct2 == drct){
                    continue;
                }

                getShape(0, shape2, p.row, p.col, drct2, who);
                for(int j = 5; j <= 14; j++){
                    if(shape2.find(AttTab[j].shape) != string::npos){
                        pieceNum = max(pieceNum, AttTab[j].pieceNum);
                        hasShape2 = true;
                        break;
                    }
                }
                if(hasShape2){
                    enemyPos[pieceNum].insert(p);
                    break;
                }
            }
        }
    }
}

int loreAB(int depth, int maxDepth, int player, int attacker, int defender, bool update = false){
    loreCnt++;
    if(isOver()){
        //if this depth is attcker, it says that the last player who is defender get five
        if(player == attacker){
            return -1;
        }
        else{
            return 1;
        }
    }
    else if(depth == 0){
        return -1;
    }

    set<Position> myPos[6], enemyPos[6], totalPos, lastPos;
    if(player == attacker){
        memset(chessNumOnDrct, 0, sizeof(chessNumOnDrct));

        for(vector<Position>::iterator iter = pieceOnBoard.begin(); iter != pieceOnBoard.end(); iter++){
            int row = iter->row;
            int col = iter->col;
            
            for (int drct = 0; drct < 4; drct++){
                string shape;
                int startRow, startCol;
                getShape(1, shape, row, col, drct, chessboard[row][col], &startRow, &startCol);

                if(chessboard[row][col] == attacker){
                    getLorePos(shape, startRow, startCol, drct, myPos, AttTab, AttCnt);
                }
                else{
                    getLorePos(shape, startRow, startCol, drct, enemyPos, DefDTab, DefDCnt);
                }
            }
        }

        for(int i = 4; i >= 2; i--){
            unionSet(myPos[i], totalPos);
            if(!enemyPos[i].empty()){          
                if(i == 4 && totalPos.empty()){    //It defend only four to five
                    totalPos = enemyPos[i];    
                }
                break;                  //If defender has this level piece, attacker does not need to find lower level pieces.
            }
        }
        for(set<Position>::iterator iter = totalPos.begin(); iter != totalPos.end(); iter++){
            setPiece(iter->row, iter->col, attacker);
            int v = loreAB(depth - 1, maxDepth, defender, attacker, defender);
            delPiece(iter->row, iter->col);
            if(v == 1){
                if(update && depth == maxDepth){
                    predictRow = iter->row;
                    predictCol = iter->col;
                }
                return 1;
            }
        }
        return -1;
    }
    else{
        memset(chessNumOnDrct, 0, sizeof(chessNumOnDrct));
        set<Position> myPos[6], enemyPos[6], totalPos;

        for(vector<Position>::iterator iter = pieceOnBoard.begin(); iter != pieceOnBoard.end(); iter++){
            int row = iter->row;
            int col = iter->col;
            
            for (int drct = 0; drct < 4; drct++){
                string shape;
                int startRow, startCol;
                getShape(1, shape, row, col, drct, chessboard[row][col], &startRow, &startCol);

                if(chessboard[row][col] == defender){       //defender attack
                    getLorePos(shape, startRow, startCol, drct, myPos, AttTab, AttCnt);
                }
                else{         //defender defend
                    bool geted = getLorePos(shape, startRow, startCol, drct, enemyPos, DefDTab, DefDCnt);
                    if(!geted){   //not geted obvious threat
                        getDoubleLorePos(enemyPos, shape, startRow, startCol, drct, attacker);
                    }
                }
            }
        }

        for(int i = 4; i >= 2; i--){
            unionSet(myPos[i], totalPos);
            if(!enemyPos[i].empty()){
                unionSet(enemyPos[i], totalPos);    //If attacker can not get points which has high level than defender, attacker should defend defender.
                break;
            }
        }
        bool flag = 0;
        for(set<Position>::iterator iter = totalPos.begin(); iter != totalPos.end(); iter++){
            if(flag == 0){
                flag = 1;
            }
            setPiece(iter->row, iter->col, defender);
            int v = loreAB(depth - 1, maxDepth, attacker, attacker, defender);
            delPiece(iter->row, iter->col);
            if(v == -1){
                return -1;
            }
        }
        if(flag){
            return 1;
        }
    }
}

int alphaBeta(int depth, int alpha, int beta, int player, vector<Position> q1){
    nodeCnt++;
    
    if(depth == 0 || isOver()){
        return evaluator();
    }

    if(depth == DEPTH){
        if(loreAB(9, 9, player, player, 3-player, 1) == 1){
            if(player == AI){
                return MAXVALUE;
            }
            else{
                return MINVALUE;
            }
        }
    }
    else if(depth == 1){
        if(loreAB(3, 3, player, player, 3-player, 0) == 1){
            if(player == AI){
                return MAXVALUE;
            }
            else{
                return MINVALUE;
            }
        }
    }
    

    //Heuristic search to prune more
    sort(q1.begin(), q1.end(), portionCompare);

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


bool isLegal(int row, int col){
    if(inBoundary(row, col) && !chessboard[row][col]){
        return true;
    }
    return false;
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
        do{
            cin >> row >> col;
            printf("The position is invalid!\n");
        }while(!isLegal(row, col));

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

        int v = alphaBeta(DEPTH, MINVALUE, MAXVALUE, AI, q2);
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
        printf("Value=%d, %d nodes count, %d evaluation, %d loration\n", v, nodeCnt, evaCnt, loreCnt);
        nodeCnt = 0;
        evaCnt = 0;
        loreCnt = 0;
        // if(loreRow != predictRow || loreCol != predictCol){
        //     printf("Error!\n");
        // }
    }

    return 0;
}