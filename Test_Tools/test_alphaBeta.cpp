#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <string.h>
#include <set>
#include <algorithm>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <time.h>

using namespace std;

#define AI 1
#define MAN 2
#define MAXVALUE 999999
#define MINVALUE -999999
#define DEPTH 4
#define OFFSET 1
#define L 0
#define R 1

struct BinTree{
    BinTree* children[2];
    int v;
};

BinTree btArr[35];
int ans;
int pruneNode[10];
int pruneCnt = 0;

bool isGameOver(BinTree& bt){
    if(bt.children[0] == NULL && bt.children[1] == NULL){
        return true;
    }
    return false;
}

int evaluator(int player, BinTree& bt){
    return bt.v;
}

int alphaBeta(int depth, int alpha, int beta, int player, BinTree& bt){
    if(depth == 0 || isGameOver(bt)){
        return evaluator(player, bt);
    }

    if(player == AI){
        int v = MINVALUE;
        for (int i = 0; i < 2; i++){
            v = max(v, alphaBeta(depth - 1, alpha, beta, MAN, *bt.children[i]));
            if(alpha < v && depth == DEPTH){
                ans = i;
                alpha = v;
            }
            if(alpha >= beta){
                pruneNode[pruneCnt++] = depth;
                break;
            }
        }
        return v;
    }
    else{
        int v = MAXVALUE;
        for (int i = 0; i < 2; i++){
            v = min(v, alphaBeta(depth - 1, alpha, beta, AI, *bt.children[i]));
            beta = min(beta, v);
            if(alpha >= beta){
                pruneNode[pruneCnt++] = depth;
                break;
            }
        }
        return v;
    }
}

void initBinTree(){
    int nodes[16] = {44, 29, 86, 35, 72, 45, 86, 88, 61, 68, 33, 59, 74, 79, 21, 94};
    for(int ceng = 0; ceng < 4; ceng++){
        int j = pow(2, ceng+1);
        for(int i = pow(2, ceng); i < pow(2, ceng+1);i++){
            btArr[i].children[0] = &btArr[j++];
            btArr[i].children[1] = &btArr[j++];
        }
    }

    for(int i = 16; i < 32; i++){
        btArr[i].v = nodes[i-16];
        btArr[i].children[0] = btArr[i].children[1] = 0;
    }
}

int main(){
    // srand((unsigned)time(NULL));
    // for(int i = 0; i < 32; i++){
    //     cout << rand()%100 << ", ";
    // }

    initBinTree();
    int v = alphaBeta(DEPTH, MINVALUE, MAXVALUE, AI, btArr[1]);
    cout << "Best Value: " << v << "\nPredict step: " << ans << "\nPrune count: " << pruneCnt << "\n\n\nPrune depth:\n";
    for(int i = 0; i < pruneCnt; i++){
        cout << pruneNode[i] << endl;
    }

    getchar();
    return 0;
}