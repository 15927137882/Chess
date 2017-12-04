#ifndef MCTS_H_INCLUDED
#define MCTS_H_INCLUDED

#include <iostream>
#include <vector>
#include <map>
#include <list>

using namespace std;

enum STATUS {EMPTY,P1,P2,AI};

typedef pair<int,int> location;     //λ��(x,y)
typedef pair<bool,STATUS> result;   //������(ʤ����ϵ,Ӯ��)


class Board;


class Human
{
protected:
    STATUS ID;
public:
    Human(STATUS ID_ = STATUS::EMPTY) : ID(ID_) {}
    virtual int GetAction(Board &b);    //��������
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
    struct cmp      //map�������ȽϺ����������Ȱ���first�������У���first��ͬ,��second��������
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
    result GameEnd(const Board &board);   //�ж���Ϸ�Ƿ����
    result Winner(const Board &board);    //�ж���Ϸʤ����
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
    int width,height;   //��������������
    int n_in_rows;      //�������ӻ�ʤ
    vector<STATUS> status;  //����ÿ�����״̬
    list<int> availables;   //ʣ��������ӵĵط�
    list<int> total;    //���е��һά����
public:
    Board(int width_ = 8,int height_ = 8,int n_in_rows_ = 5);
    Board(const Board &b);
    void Initial();     //���� || ��ʼ������
    const vector<STATUS> & GetStatus() const {return status;} //�������״̬
    unsigned int GetAvailablesNum() const {return availables.size();}    //��ÿ�����λ�õ�����
    list<int> GetMoved() const;   //��ÿ������ӵĵط�
    pair<int,int> Size() const {return pair<int,int>(height,width);}  //������̴�С
    location MoveToLocation(int Move) const;      //���Ե�ַת��Ϊ��ά��ַ
    int LocationToMove(location loc) const;      //��ά��ַת��Ϊ���Ե�ַ
    void Update(Human *s,int Move);      //������Ӧ���ֵ�����
    friend ostream & operator<<(ostream &os,Board &b);  //��ʾ���̵�ǰ״̬
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
    void InitialPlayer();   //��ҳ�ʼ��
    void Start();       //��ʼ������Ϸ
    result GameEnd();   //�ж���Ϸ�Ƿ����
    result Winner();    //�ж���Ϸʤ����
    ~Game();
};



#endif // MCTS_H_INCLUDED
