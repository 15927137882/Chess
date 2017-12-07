#ifndef MCTS_H_INCLUDED
#define MCTS_H_INCLUDED

#include <iostream>
#include <vector>
#include <map>
#include <list>

using namespace std;

enum STATUS {EMPTY,P1,P2,AI};   //���岻ͬ���״̬

typedef pair<int,int> location;     //λ��(x,y)
typedef pair<bool,STATUS> result;   //������(ʤ����ϵ,Ӯ��)


class Board;


class Human
{
protected:
    STATUS ID;
public:
    Human(STATUS ID_ = STATUS::EMPTY) : ID(ID_) {}
    virtual int GetAction(Board &b);    //�������ӣ��鷽��
    STATUS GetStatus() const { return ID; } //�������ID
    void SetID(STATUS ID_) { ID = ID_; }    //�������ID
    virtual ~Human() {} //����������
};

class AI : public Human     //AI�࣬���м̳�Human��
{
public:
    using status = pair<STATUS,int>;
    using win_times = int;
    using visit_times = int;
private:
    double calculation_time;    //����ʱ������
    int max_actions;        //ÿһ��ģ�������Ĵ���
    int n_in_rows;      //��ʤ����
    double confident;   //UCT�ĳ���
    int max_depth;      //ÿ�ι�����������������
    struct cmp      //map�������ȽϺ����������Ȱ���first�������У���first��ͬ,��second��������
    {
        bool operator()(const pair<STATUS,int> &a,const pair<STATUS,int> &b)
        {
            if(a.first == b.first) return a.second < b.second;
            return a.first < b.first;
        }
    };
    map<status,visit_times,cmp> plays;    //��¼ÿ��״̬(<���,����>)��ģ�����
    map<status,win_times,cmp> wins;     //��¼ÿ��״̬(<���,����>)��ʤ������
    Human simulated_p1;     //����һ��ģ����Ҷ��������ڷ���ʱ����AIģ�����

    bool AllMovesHaveInfo(const list<int> &availables,STATUS who);  //�ж��Ƿ����е��ƶ�����ͳ����Ϣ
    double SumPlays(const list<int> &availables,STATUS who);    //�������нڵ㱻���ʵĴ���֮��
    void MaxUCB(double &value,int &Move,double &log_total,STATUS who,const list<int> &availables);  //�������λ��������UCBֵ
    int choice(const list<int> &a); //�ӿ���λ�������ѡ��һ��λ��
    result GameEnd(const Board &board);   //�ж���Ϸ�Ƿ����
    result Winner(const Board &board);    //�ж���Ϸʤ����
public:
    AI(int calculation_time_,int max_actions_,int n_in_rows_,double confident_);    //AI���캯��
    AI() = default;
    int GetAction(Board &b);    //AI���Ӻ���
    void RunSimulation(Board &b);   //���ģ�ⲩ�ĺ���
    int SelectOneMove(const Board &b);  //ѡ��һ�����µ�
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
    Board(const Board &b);  //���ƹ��캯����������ȸ���
    void Initial();     //���� || ��ʼ������
    const vector<STATUS> & GetStatus() const {return status;} //�������״̬
    unsigned int GetAvailablesNum() const {return availables.size();}    //��ÿ�����λ�õ�����
    list<int> GetMoved() const;   //����Ѿ����ӵĵط�
    pair<int,int> Size() const {return pair<int,int>(height,width);}  //������̴�С
    location MoveToLocation(int Move) const;      //���Ե�ַת��Ϊ��ά��ַ
    int LocationToMove(location loc) const;      //��ά��ַת��Ϊ���Ե�ַ
    void Update(Human *s,int Move);      //������Ӧ���ֵ�����

    /* ��Ԫ���� */
    friend ostream & operator<<(ostream &os,Board &b);  //��ʾ���̵�ǰ״̬
    friend int Human::GetAction(Board &b);
    friend int AI::GetAction(Board &b);
    friend void AI::RunSimulation(Board &b);
    friend int AI::SelectOneMove(const Board &b);
};


class Game //��Ϸ�࣬ʵ��������Ϸ�Ĺ���
{
private:
    Board board;    //һ������
    Human *Player[3];   //2�����
    int n_in_rows;  //��Ϸ��ʤ����
    double time;    //AI����ʱ������
    int max_actions;    //AIÿ��ģ�ⲩ��������
public:
    Game(int width_ = 8,int height_ = 8,int n_in_rows_ = 5,double time_ = 5,int max_actions_ = 1000);
    void InitialPlayer();   //��ҳ�ʼ��
    void Start();       //��ʼ������Ϸ
    result GameEnd();   //�ж���Ϸ�Ƿ����
    result Winner();    //�ж���Ϸʤ����
    ~Game();            //���������������ͷŶ��ڴ�����(Players)
};



#endif // MCTS_H_INCLUDED
