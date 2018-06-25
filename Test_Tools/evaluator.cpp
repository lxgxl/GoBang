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
    initModel(modelCnt++, "ooooo", 900000);
    initModel(modelCnt++, "+oooo+", 50000);
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







void test1(){
    for (int i = 4; i <= 7; i++){
        setPiece(i + 3, i, AI);
    }
    for (int i = 7; i <= 10; i++){
        setPiece(15 - i, i, MAN);
    }
    setPiece(9, 7, MAN);
    setPiece(8, 8, AI);
    setPiece(9, 8, AI);
    cout << evaluator() << endl;
    cout << isOver() << endl;

    cout << "\n\n\n\n\n";

    setPiece(4, 11, MAN);
    cout << evaluator() << endl;
}

void test2(){
    for (int i = 11; i >= 8; i--){
        setPiece(i, 10, AI);
    }
    setPiece(12, 10, MAN);
    setPiece(9, 5, AI);
    setPiece(10, 6, AI);
    for (int i = 6; i <= 9; i++){
        setPiece(9, i, MAN);
    }
    setPiece(8, 6, MAN);
    setPiece(8, 7, MAN);
    setPiece(8, 8, AI);
    setPiece(8, 9, AI);
    setPiece(7, 7, AI);
    setPiece(7, 8, AI);
    setPiece(7, 10, MAN);
    setPiece(6, 6, MAN);
    setPiece(6, 7, MAN);
    cout << evaluator() << endl;

    cout << "\n\n\n\n\n";

    setPiece(8, 4, AI);
    setPiece(7, 6, MAN);
    setPiece(5, 6, AI);
    setPiece(6, 5, MAN);
    cout << evaluator() << endl;

    cout << "\n\n\n\n\n";

    // setPiece(7, 6, AI);
    // setPiece(7, 5, MAN);
    // setPiece(10, 8, AI);
    // setPiece(6, 4, MAN);
    // cout << evaluator() << endl;

    cout << "\n\n\n\n\n";
}

int main(){
    initTable();
    test2();

    getchar();
    return 0;
}

/*
   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15
-----------------------------------------------------------------
 1|                                                             |

 2|                                                             |

 3|                                                             |

 4|                                                             |

 5|                                    -                        |

 6|                                -                            |

 7|            +               -                                |

 8|                +       -   +                                |

 9|                    +   -   +                                |

10|                        +                                    |

11|                                                             |

12|                                                             |

13|                                                             |

14|                                                             |

15|                                                             |

-----------------------------------------------------------------
*/

/*
   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15
-----------------------------------------------------------------
 1|                                                             |

 2|                                                             |

 3|                                                             |

 4|                                                             |

 5|                                                             |

 6|                    -   -                                    |

 7|                        +   +       -                        |

 8|                    -   -   +   +   +                        |

 9|                +   -   -   -   -   +                        |

10|                    +               +                        |

11|                                    +                        |

12|                                    -                        |

13|                                                             |

14|                                                             |

15|                                                             |

-----------------------------------------------------------------
*/