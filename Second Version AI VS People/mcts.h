#ifndef MCTS_H_INCLUDED
#define MCTS_H_INCLUDED

#include <iostream>
#include <vector>
#include <map>
#include <list>

using namespace std;

enum STATUS {EMPTY,P1,P2,AI};   //定义不同玩家状态

typedef pair<int,int> location;     //位置(x,y)
typedef pair<bool,STATUS> result;   //下棋结果(胜负关系,赢家)


class Board;


class Human
{
protected:
    STATUS ID;
public:
    Human(STATUS ID_ = STATUS::EMPTY) : ID(ID_) {}
    virtual int GetAction(Board &b);    //棋手着子，虚方法
    STATUS GetStatus() const { return ID; } //返回玩家ID
    void SetID(STATUS ID_) { ID = ID_; }    //设置玩家ID
    virtual ~Human() {} //虚析构函数
};

class AI : public Human     //AI类，公有继承Human类
{
public:
    using status = pair<STATUS,int>;
    using win_times = int;
    using visit_times = int;
private:
    double calculation_time;    //计算时间限制
    int max_actions;        //每一局模拟最大对弈次数
    int n_in_rows;      //连胜条件
    double confident;   //UCT的常数
    int max_depth;      //每次构建的搜索树最大深度
    struct cmp      //map容器键比较函数符，首先按照first升序排列，若first相同,则按second升序排列
    {
        bool operator()(const pair<STATUS,int> &a,const pair<STATUS,int> &b)
        {
            if(a.first == b.first) return a.second < b.second;
            return a.first < b.first;
        }
    };
    map<status,visit_times,cmp> plays;    //记录每个状态(<玩家,落子>)的模拟次数
    map<status,win_times,cmp> wins;     //记录每个状态(<玩家,落子>)的胜利次数
    Human simulated_p1;     //内置一个模拟玩家对象，用于在仿真时，和AI模拟对弈

    bool AllMovesHaveInfo(const list<int> &availables,STATUS who);  //判段是否所有的移动都有统计信息
    double SumPlays(const list<int> &availables,STATUS who);    //计算所有节点被访问的次数之和
    void MaxUCB(double &value,int &Move,double &log_total,STATUS who,const list<int> &availables);  //计算可下位置中最大的UCB值
    int choice(const list<int> &a); //从可下位置中随机选择一个位置
    result GameEnd(const Board &board);   //判断游戏是否结束
    result Winner(const Board &board);    //判断游戏胜利者
public:
    AI(int calculation_time_,int max_actions_,int n_in_rows_,double confident_);    //AI构造函数
    AI() = default;
    int GetAction(Board &b);    //AI着子函数
    void RunSimulation(Board &b);   //随机模拟博弈函数
    int SelectOneMove(const Board &b);  //选择一个可下点
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
    Board(const Board &b);  //复制构造函数，用于深度复制
    void Initial();     //重置 || 初始化棋盘
    const vector<STATUS> & GetStatus() const {return status;} //获得棋盘状态
    unsigned int GetAvailablesNum() const {return availables.size();}    //获得可着子位置的数量
    list<int> GetMoved() const;   //获得已经着子的地方
    pair<int,int> Size() const {return pair<int,int>(height,width);}  //获得棋盘大小
    location MoveToLocation(int Move) const;      //线性地址转化为二维地址
    int LocationToMove(location loc) const;      //二维地址转化为线性地址
    void Update(Human *s,int Move);      //更新相应棋手的着子

    /* 友元函数 */
    friend ostream & operator<<(ostream &os,Board &b);  //显示棋盘当前状态
    friend int Human::GetAction(Board &b);
    friend int AI::GetAction(Board &b);
    friend void AI::RunSimulation(Board &b);
    friend int AI::SelectOneMove(const Board &b);
};


class Game //游戏类，实现整个游戏的管理
{
private:
    Board board;    //一个棋盘
    Human *Player[3];   //2个玩家
    int n_in_rows;  //游戏获胜条件
    double time;    //AI计算时间限制
    int max_actions;    //AI每局模拟博弈最大次数
public:
    Game(int width_ = 8,int height_ = 8,int n_in_rows_ = 5,double time_ = 5,int max_actions_ = 1000);
    void InitialPlayer();   //玩家初始化
    void Start();       //开始运行游戏
    result GameEnd();   //判断游戏是否结束
    result Winner();    //判断游戏胜利者
    ~Game();            //析构函数，用于释放堆内存数据(Players)
};



#endif // MCTS_H_INCLUDED
