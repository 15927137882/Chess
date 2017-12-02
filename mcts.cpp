#include "mcts.h"
#include <algorithm>
#include <random>
#include <cstdlib>

Board::Board(int width_,int height_ ,int n_in_rows_) : width(width_), height(height_), n_in_rows(n_in_rows_),status(width*height)
{
}

void Board::Initial()
{
    if(!availables.size()) availables.clear();
    if(!total.size()) total.clear();
    for(int i = 0;i<width*height;i++)
    {
        status[i] = EMPTY;
        availables.push_back(i);
        total.push_back(i);
    }
}

location Board::MoveToLocation(int Move)
{
    location temp;
    temp.first = Move / width;
    temp.second = Move % width;
    return temp;
}

int Board::LocationToMove(location loc)
{
    return loc.first * height + loc.second;
}

void Board::Update(Human s,int Move)
{
    status[Move] = s.GetStatus();
    availables.remove(Move);
}

list<int> Board::GetMoved()
{
    list<int> moved;
    set_difference(total.begin(),total.end(),availables.begin(),availables.end()
                     ,insert_iterator<list<int>>(moved,moved.begin()));
    return moved;
}

ostream & operator<<(ostream &os,Board &b)
{
    system("cls");
    for(int i = 0;i<b.width;i++)
        os<<"\t"<<i;
    os<<endl<<endl<<endl;
    for(int i = 0;i<b.height;i++)
    {
        os<<"  "<<i<<"     ";
        switch(b.status[i*b.height])
        {
            case STATUS::EMPTY : os<<"_";
                                 break;
            case STATUS::P1    : os<<"X";
                                 break;
            case STATUS::P2    : os<<"O";
                                 break;
        }

        for(int j = 1;j<b.width;j++)
        {
            os<<"\t";
            switch(b.status[i*b.height + j])
            {
                case STATUS::EMPTY : os<<"_";
                                     break;
                case STATUS::P1    : os<<"X";
                                     break;
                case STATUS::P2    : os<<"O";
                                     break;
            }
        }
        os<<"\n\n\n";
    }

    return os;
}



int Human::GetAction(Board &b)
{
    location loc;
    int mov;
    if(ID == P1) cout<<"P1:\n";
    if(ID == P2) cout<<"P2:\n";
    cout<<"Your move: ";
    cin>>loc.first;
    if(!cin)
    {
        cerr<<"Bad Input!\n";
        exit(0);
    }
    cin.get();
    cin>>loc.second;
    if(!cin)
    {
        cerr<<"Bad Input!\n";
        exit(0);
    }
    mov = b.LocationToMove(loc);
    while(loc.first >= b.height || loc.first < 0
       || loc.second >= b.width || loc.second < 0
       || mov >= b.width*b.height
       || find(b.availables.begin(),b.availables.end(),mov) == b.availables.end())
    {
        cout<<"Invalid Input.\n";
        if(ID == P1) cout<<"P1:\n";
        if(ID == P2) cout<<"P2:\n";
        cout<<"Your move: ";
        cin.clear();
        cin>>loc.first;
        if(!cin)
        {
        cerr<<"Bad Input!\n";
        exit(0);
        }
        cin.get();
        cin>>loc.second;
        if(!cin)
        {
        cerr<<"Bad Input!\n";
        exit(0);
        }
        mov = b.LocationToMove(loc);
    }
    return mov;
}


Game::Game(int width_,int height_,int n_in_rows_,double time_,int max_actions_)
        : board(width_,height_,n_in_rows_) , n_in_rows(n_in_rows_) , time(time_),max_actions(max_actions_)
{
}

void Game::InitialPlayer()
{
    static default_random_engine e;
    static uniform_int_distribution<unsigned> u(1,2);
    if(u(e) == 1)
    {
        Player[0].SetID(STATUS::P1);
        Player[1].SetID(STATUS::P2);
    }
    else
    {
        Player[0].SetID(STATUS::P2);
        Player[1].SetID(STATUS::P1);
    }
}

void Game::Start()
{
    board.Initial();
    InitialPlayer();
    cout<<board<<endl;
    int index;
    int turn = 0;
    while(1)
    {
        index = (turn++) % 2;
        if(turn == 16) turn = 0;
        board.Update(Player[index],Player[index].GetAction(board));
        cout<<board<<endl;
        result res = Winner();
        if(res.first)
        {
            if(res.second != EMPTY) cout<<"Winner is "<<(res.second == P1 ? "Player1" : "Player2")<<endl;
            break;
        }
    }

}

result Game::GameEnd()
{
    list<int> moved = board.GetMoved();
    if(moved.size() < (unsigned int)(n_in_rows + 2))
        return result(false,EMPTY);
    location temp;
    STATUS who;
    for(auto m : moved)
    {
        temp = board.MoveToLocation(m);
        who = board.GetStatus()[m];
        int count;

        if(temp.second >= 0 && temp.second<board.Size().second - n_in_rows + 1)
        {
            count = 0;
            for(int i = m;i<m+n_in_rows;i++)
            {
                if(board.GetStatus()[i] != who) break;
                count++;
            }
            if(count == n_in_rows) return result(true,who);
        }


        if(temp.first >= 0 && temp.first<board.Size().first - n_in_rows + 1)
        {
            count = 0;
            for(int i = m;i<m + n_in_rows * board.Size().second;i+=board.Size().second)
            {
                if(board.GetStatus()[i] != who) break;
                count++;
            }
            if(count == n_in_rows) return result(true,who);
        }

        if(temp.second >= 0 && temp.second<board.Size().second - n_in_rows + 1
           && temp.first >= 0 && temp.first<board.Size().first - n_in_rows + 1)
        {
            count = 0;
            for(int i = m;i<m + n_in_rows * (board.Size().second + 1);i+=(board.Size().second+1))
            {
                if(board.GetStatus()[i] != who) break;
                count++;
            }
            if(count == n_in_rows) return result(true,who);
        }

        if(temp.second >= 0 && temp.second<board.Size().second - n_in_rows + 1
           && temp.first >= board.Size().first - n_in_rows + 2 && temp.first<board.Size().first)
        {
            count = 0;
            for(int i = m;i>m - n_in_rows * (board.Size().second - 1);i-=(board.Size().second-1))
            {
                if(board.GetStatus()[i] != who) break;
                count++;
            }
            if(count == n_in_rows) return result(true,who);
        }
    }
    return result(false,EMPTY);
}

result Game::Winner()
{
    result res = GameEnd();
    if(res.first) return result(true,res.second);
    else if(!board.GetAvailablesNum())
    {
        cout<<"Game end. Tie\n";
        return result(true,EMPTY);
    }
    else return result(false,EMPTY);
}
