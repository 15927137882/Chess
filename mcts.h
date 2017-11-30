#ifndef MCTS_H_INCLUDED
#define MCTS_H_INCLUDED

#include <iostream>
#include <vector>
#include <map>
#include <list>

using namespace std;

enum STATUS {EMPTY,P1,P2};

typedef pair<int,int> location;
typedef pair<bool,STATUS> result;


class Board;


class Human
{
private:
    STATUS ID;
public:
    Human(STATUS ID_ = STATUS::EMPTY) : ID(ID_) {}
    int GetAction(Board &b);    //棋手着子
    STATUS GetStatus() const { return ID; }
    void SetID(STATUS ID_) { ID = ID_; }
};

class Board
{
private:
    int width,height;   //棋盘行数与列数
    int n_in_rows;      //多少连子获胜
    vector<STATUS> status;  //棋盘每个点的状态
    list<int> availables;   //剩余可以下子的地方
    list<int> total;    //所有点的一维索引
public:
    Board(int width_ = 8,int height_ = 8,int n_in_rows_ = 5);
    void Initial();
    const vector<STATUS> & GetStatus() {return status;} //获得棋盘状态
    unsigned int GetAvailablesNum()  {return availables.size();}    //获得可着子位置的数量
    list<int> GetMoved();   //获得可以着子的地方
    pair<int,int> Size() {return pair<int,int>(height,width);}
    location MoveToLocation(int Move);      //线性地址转化为二维地址
    int LocationToMove(location loc);      //二维地址转化为线性地址
    void Update(Human s,int Move);      //更新相应棋手的着子
    friend ostream & operator<<(ostream &os,Board &b);
    friend int Human::GetAction(Board &b);
};

class Game
{
private:
    Board board;
    Human Player[2];
    int n_in_rows;
    double time;
    int max_actions;
public:
    Game(int width_ = 8,int height_ = 8,int n_in_rows_ = 5,double time_ = 5,int max_actions_ = 1000);
    void InitialPlayer();
    void Start();
    result GameEnd();
    result Winner();
};



#endif // MCTS_H_INCLUDED
