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
    int GetAction(Board &b);    //��������
    STATUS GetStatus() const { return ID; }
    void SetID(STATUS ID_) { ID = ID_; }
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
    void Initial();
    const vector<STATUS> & GetStatus() {return status;} //�������״̬
    unsigned int GetAvailablesNum()  {return availables.size();}    //��ÿ�����λ�õ�����
    list<int> GetMoved();   //��ÿ������ӵĵط�
    pair<int,int> Size() {return pair<int,int>(height,width);}
    location MoveToLocation(int Move);      //���Ե�ַת��Ϊ��ά��ַ
    int LocationToMove(location loc);      //��ά��ַת��Ϊ���Ե�ַ
    void Update(Human s,int Move);      //������Ӧ���ֵ�����
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
