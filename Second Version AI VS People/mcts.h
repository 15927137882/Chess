#ifndef MCTS_H_INCLUDED
#define MCTS_H_INCLUDED

#include <iostream>
#include <vector>
#include <map>
#include <list>

using namespace std;

enum STATUS {EMPTY,P1,P2,AI};

typedef pair<int,int> location;     //位置(x,y)
typedef pair<bool,STATUS> result;   //下棋结果(胜负关系,赢家)


class Board;


class Human
{
protected:
    STATUS ID;
public:
    Human(STATUS ID_ = STATUS::EMPTY) : ID(ID_) {}
    virtual int GetAction(Board &b);    //棋手着子
    STATUS GetStatus() const { return ID; }
    void SetID(STATUS ID_) { ID = ID_; }
    virtual ~Human() {}
};

class AI : public Human
{
private:
    double calculation_time;
    int max_actions;
    int n_in_rows;
    double confident;
    int max_depth;
    struct cmp      //map容器键比较函数符，首先按照first升序排列，若first相同,则按second升序排列
    {
        bool operator()(const pair<STATUS,int> &a,const pair<STATUS,int> &b)
        {
            if(a.first == b.first) return a.second < b.second;
            return a.first < b.first;
        }
    };
    map<pair<STATUS,int>,int,cmp> plays;
    map<pair<STATUS,int>,int,cmp> wins;
    Human simulated_p1;
    bool AllMovesHaveInfo(const list<int> &availables,STATUS who);
    double SumPlays(const list<int> &availables,STATUS who);
    void MaxUCB(double &value,int &Move,double &log_total,STATUS who,const list<int> &availables);
    int choice(const list<int> &a);
    result GameEnd(const Board &board);   //判断游戏是否结束
    result Winner(const Board &board);    //判断游戏胜利者
public:
    AI(int calculation_time_,int max_actions_,int n_in_rows_,double confident_);
    AI() = default;
    int GetAction(Board &b);
    void RunSimulation(Board &b);
    int SelectOneMove(const Board &b);
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
    Board(const Board &b);
    void Initial();     //重置 || 初始化棋盘
    const vector<STATUS> & GetStatus() const {return status;} //获得棋盘状态
    unsigned int GetAvailablesNum() const {return availables.size();}    //获得可着子位置的数量
    list<int> GetMoved() const;   //获得可以着子的地方
    pair<int,int> Size() const {return pair<int,int>(height,width);}  //获得棋盘大小
    location MoveToLocation(int Move) const;      //线性地址转化为二维地址
    int LocationToMove(location loc) const;      //二维地址转化为线性地址
    void Update(Human *s,int Move);      //更新相应棋手的着子
    friend ostream & operator<<(ostream &os,Board &b);  //显示棋盘当前状态
    friend int Human::GetAction(Board &b);
    friend int AI::GetAction(Board &b);
    friend void AI::RunSimulation(Board &b);
    friend int AI::SelectOneMove(const Board &b);
};

class Game
{
private:
    Board board;
    Human *Player[3];
    int n_in_rows;
    double time;
    int max_actions;
public:
    Game(int width_ = 8,int height_ = 8,int n_in_rows_ = 5,double time_ = 5,int max_actions_ = 1000);
    void InitialPlayer();   //玩家初始化
    void Start();       //开始运行游戏
    result GameEnd();   //判断游戏是否结束
    result Winner();    //判断游戏胜利者
    ~Game();
};



#endif // MCTS_H_INCLUDED
