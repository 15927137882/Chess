#include "mcts.h"
#include <algorithm>
#include <random>
#include <queue>
#include <ctime>
#include <cstdlib>
#include <cmath>

/* Board�෽��ʵ�� */

Board::Board(int width_,int height_ ,int n_in_rows_) : width(width_), height(height_), n_in_rows(n_in_rows_),status(width*height)
{
}

Board::Board(const Board &b) : status(b.width*b.height), availables(b.availables.begin(),b.availables.end())
                               ,total(b.total.begin(),b.total.end())
{
    width = b.width;
    height = b.height;
    n_in_rows = b.n_in_rows;
    for(int i = 0;i<width*height;i++) status[i] = b.status[i];
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

location Board::MoveToLocation(int Move) const
{
    location temp;
    temp.first = Move / width;
    temp.second = Move % width;
    return temp;
}

int Board::LocationToMove(location loc) const
{
    return loc.first * height + loc.second;
}

void Board::Update(Human *s,int Move)
{
    status[Move] = s->GetStatus();
    availables.remove(Move);
}

list<int> Board::GetMoved() const
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
            case STATUS::AI    :
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
                case STATUS::AI    :
                case STATUS::P2    : os<<"O";
                                     break;
            }
        }
        os<<"\n\n\n";
    }

    return os;
}


/* Human�෽��ʵ�� */

void input_judge(istream &is)
{
    if(!is)
    {
        cerr<<"Bad Input!\n";
        exit(0);
    }
}

int Human::GetAction(Board &b)
{
    location loc;
    int mov;
    if(ID == P1) cout<<"P1:\n";
    if(ID == P2) cout<<"P2:\n";
    cout<<"Your move: ";
    cin>>loc.first;

    input_judge(cin);

    cin.get();
    cin>>loc.second;

    input_judge(cin);

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
        input_judge(cin);
        cin.get();
        cin>>loc.second;
        input_judge(cin);
        mov = b.LocationToMove(loc);
    }
    return mov;
}

/* Game�෽��ʵ�� */

Game::Game(int width_,int height_,int n_in_rows_,double time_,int max_actions_)
        : board(width_,height_,n_in_rows_) , n_in_rows(n_in_rows_) , time(time_),max_actions(max_actions_)
{
    Player[0] = nullptr;
    Player[1] = nullptr;
    Player[2] = nullptr;
}

Game::~Game()
{
    delete Player[0];
    delete Player[1];
    delete Player[2];
}

void Game::InitialPlayer()
{
    static default_random_engine e;
    Player[0] = new Human(P1);
    cout<<"P1 Vs P2 or P1 Vs AI?\n"
        <<"1 for first choice,2 for second choice\n";
    int choice = 0;
    cin>>choice;
    if(choice == 1) Player[1] = new Human(P2);
    if(choice == 2) Player[1] = new class AI(2,1000,n_in_rows,1.96);    //����AI�Ĳ���
    shuffle(Player,Player+2, e);        //�������������ҵ�˳��
}


void Game::Start()
{
    board.Initial();
    InitialPlayer();
    cout<<board<<endl;
    queue<int> turn;
    turn.push(0);
    turn.push(1);
    int index;
    while(1)
    {
        index = turn.front();
        turn.pop();
        turn.push(index);
        if(Player[index]->GetStatus() == AI) cout<<"AI's choice:\n";

        board.Update(Player[index],Player[index]->GetAction(board));    //��Ӧ������ӣ�����������

        if(Player[index]->GetStatus() == AI)    //�������ȷ��AI��Ϣ
        {
            cout<<"Enter y for continue\n";
            char ch;
            cin>>ch;
        }

        cout<<board<<endl;

        result res = Winner();  //�����ж���ϷӮ��
        if(res.first)
        {
            if(res.second != EMPTY) cout<<"Winner is "
                <<(res.second == P1 ? "Player1" : (res.second == P2 ? "Player2" : "AI"))<<endl;
            break;
        }
    }
}

result Game::Winner()
{
    result res = GameEnd(); //�����ж���Ϸ�Ƿ����
    if(res.first) return result(true,res.second);   //��Ϸ����
    else if(!board.GetAvailablesNum())  //ƽ��״̬
    {
        cout<<"Game end. Tie\n";
        return result(true,EMPTY);
    }
    else return result(false,EMPTY);    //��Ϸδ����
}

result Game::GameEnd()
{
    list<int> moved = board.GetMoved();
    if(moved.size() < ((unsigned int)(n_in_rows * 2) - 1))    //��������������ʤ����ɵ���������ʱ��������ʤ�������ж�
        return result(false,EMPTY);
    location temp;
    STATUS who;

    /* ʤ�������ж� */
    for(auto m : moved)
    {
        temp = board.MoveToLocation(m);
        who = board.GetStatus()[m];
        int count;

        if(temp.second >= 0 && temp.second<board.Size().second - n_in_rows + 1) //ˮƽ�ж�
        {
            count = 0;
            for(int i = m;i<m+n_in_rows;i++)
            {
                if(board.GetStatus()[i] != who) break;
                count++;
            }
            if(count == n_in_rows) return result(true,who);
        }


        if(temp.first >= 0 && temp.first<board.Size().first - n_in_rows + 1)    //��ֱ�ж�
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
           && temp.first >= 0 && temp.first<board.Size().first - n_in_rows + 1) //135���ж�
        {
            count = 0;
            for(int i = m;i<m + n_in_rows * (board.Size().second + 1);i+=(board.Size().second+1))
            {
                if(board.GetStatus()[i] != who) break;
                count++;
            }
            if(count == n_in_rows) return result(true,who);
        }

        if(temp.second >= 0 && temp.second<board.Size().second - n_in_rows + 1  //45���ж�
           && temp.first >= board.Size().first - n_in_rows + 1 && temp.first<board.Size().first)
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


/* AI�෽��ʵ�� */

AI::AI(int calculation_time_,int max_actions_,int n_in_rows_,double confident_)
    : Human(STATUS::AI),calculation_time(calculation_time_), max_actions(max_actions_), n_in_rows(n_in_rows_)
      , confident(confident_), max_depth(1),simulated_p1(P1)
{
}

int AI::GetAction(Board &b)
{
    cout<<"AI Get Action\n";
    //��������Ͼ�ʣ���һ��λ�ã���ֱ��ѡ��λ��Ϊ���ӵ�
    if(b.GetAvailablesNum() == 1) return b.availables.front();
    //AIÿһ������ʱ����Ҫ���¹���һ��MCT��,����ǰ�����Ϊ��ʼ״̬
    plays.clear();
    wins.clear();
    int simulations = 0;
    time_t start;
    start = time(nullptr);
    while(time(nullptr) - start < calculation_time)
    {
        Board board_copy(b);    //����һ�ݵ�ǰ��֣���Ϊstate0
        RunSimulation(board_copy);  //����state0(��Ϊ���ڵ�)��ʼ����ģ�⣬����MCT��(ÿ��ģ���ĸ����裺ѡ�㣬���ţ�ģ����ģ�ͳ�Ƹ�������)
        simulations++;  //ͳ���ܹ�����ģ��Ĵ���
    }
    cout<<"Total simulations = "<<simulations<<endl;

    int Move;
    Move = SelectOneMove(b);    //���ڹ�����MCT����ѡ��һ�����ӵ�

    cout<<"Maximum depth searched: "<<max_depth<<endl;  //�����MCT����������

    location loc = b.MoveToLocation(Move);
    cout<<"AI move: "<<loc.first<<","<<loc.second<<endl;    //���AI��ѡ��

    return Move;
}


void AI::RunSimulation(Board &b)
{
    Human *fixed_turn[3];
    Human *player;
    fixed_turn[0] = this;
    fixed_turn[1] = &simulated_p1;

    queue<int> turn;
    turn.push(0);
    turn.push(1);
    int index;
    index = turn.front();
    turn.pop();
    turn.push(index);
    player = fixed_turn[index];     //��ģ����,AI��Զ������,��ΪҪ����AI����MCT��

    STATUS winner = EMPTY;
    list<pair<STATUS,int>> visited_status;  //ÿ����������ʵ���״̬
    visited_status.clear();
    bool expand = true;
    double log_total;
    double value;
    int Move;
    for(int t = 1;t<max_actions+1;t++)  //��ʼģ����ģ�ÿ�������max_action���غ�
    {
        if(AllMovesHaveInfo(b.availables,player->GetStatus()))  //ÿ�����,ÿ���㶼��ͳ����Ϣ��(�ѿ�����)����ʼ��Ŀ����
        {
            log_total = log(SumPlays(b.availables,player->GetStatus()));
            MaxUCB(value,Move,log_total,player->GetStatus(),b.availables);
        }
        else    //���ѡ��
            Move = choice(b.availables);

        b.Update(player,Move);

        if(expand &&    //ÿ��ģ�������չһ�Σ�ÿ����չֻ����һ���ŷ�,һ��Ϊ������㷨�ٶȣ�������ÿ�λ�ø�������ݣ�����㷨�ľ���
           (plays.find(pair<STATUS,int>(player->GetStatus(),Move)) == plays.end()))
        {
            expand = false;
            plays[pair<STATUS,int>(player->GetStatus(),Move)] = 0;
            wins[pair<STATUS,int>(player->GetStatus(),Move)] = 0;
            if(t>max_depth) max_depth = t;  //Ŀǰ������������
        }

        visited_status.push_back(pair<STATUS,int>(player->GetStatus(),Move));

        result res = Winner(b);
        winner = res.second;
        if(res.first) break; //ƽ�ֻ�ʤ����ϵȷ�������ֽ���

        //����
        index = turn.front();
        turn.pop();
        turn.push(index);
        player = fixed_turn[index];
    }

    //���򴫲�:ÿģ����һ�֣�ͳ�������Ϣ
    for(auto x : visited_status)
    {
        if(plays.find(pair<STATUS,int>(x.first,x.second)) == plays.end())
            continue;
        plays[pair<STATUS,int>(x.first,x.second)]++;
        if(x.first == winner) wins[pair<STATUS,int>(x.first,x.second)]++;
    }
}

bool AI::AllMovesHaveInfo(const list<int> &availables,STATUS who)
{
    map<pair<STATUS,int>,int,cmp>::iterator it;
    if(who == P1)
    {
        for(auto Move : availables)
        {
            it = plays.find(pair<STATUS,int>(P1,Move));
            if(it != plays.end() && it->second != 0) continue;
            return false;
        }
    }
    else if(who == STATUS::AI)
    {
        for(auto Move : availables)
        {
            it = plays.find(pair<STATUS,int>(STATUS::AI,Move));
            if(it != plays.end() && it->second != 0) continue;
            return false;
        }
    }
    else return false;

    return true;
}

double AI::SumPlays(const list<int> &availables,STATUS who)
{
    map<pair<STATUS,int>,int,cmp>::iterator it;
    double total = 0;
    for(auto Move : availables)
    {
        it = plays.find(status(who,Move));
        total += it->second;
    }
    return total;
}

void AI::MaxUCB(double &value,int &Move,double &log_total,STATUS who,const list<int> &availables)
{
    double temp_value;
    int temp_Move;
    value = -10;

    for(auto m : availables)
    {
        temp_Move = m;
        temp_value = (double(wins[status(who,m)]) / double(plays[status(who,m)]))
                        + sqrt(confident * log_total / double(plays[status(who,m)]));
        if(temp_value > value)
        {
            value = temp_value;
            Move = temp_Move;
        }
    }
}

int AI::choice(const list<int> &a)
{
    static default_random_engine e;
    vector<int> b(a.begin(),a.end());
    random_shuffle(b.begin(),b.end());
    return b[0];
}

result AI::GameEnd(const Board &board)
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

        if(temp.second >= 0 && temp.second<board.Size().second - n_in_rows + 1) //ˮƽ�ж�
        {
            count = 0;
            for(int i = m;i<m+n_in_rows;i++)
            {
                if(board.GetStatus()[i] != who) break;
                count++;
            }
            if(count == n_in_rows) return result(true,who);
        }


        if(temp.first >= 0 && temp.first<board.Size().first - n_in_rows + 1)   //��ֱ�ж�
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
           && temp.first >= 0 && temp.first<board.Size().first - n_in_rows + 1) //135���ж�
        {
            count = 0;
            for(int i = m;i<m + n_in_rows * (board.Size().second + 1);i+=(board.Size().second+1))
            {
                if(board.GetStatus()[i] != who) break;
                count++;
            }
            if(count == n_in_rows) return result(true,who);
        }

        if(temp.second >= 0 && temp.second<board.Size().second - n_in_rows + 1 //45���ж�
           && temp.first >= board.Size().first - n_in_rows + 1 && temp.first<board.Size().first)
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

result AI::Winner(const Board &board)
{
    result res = GameEnd(board);
    if(res.first) return result(true,res.second);   //ʤ����ϵȷ��
    else if(!board.GetAvailablesNum())  //ƽ��
    {
        return result(true,EMPTY);
    }
    else return result(false,EMPTY);    //ʤ����ϵ��δȷ��
}

int AI::SelectOneMove(const Board &b)
{
    int best_choice = -1;
    double percent_wins = -10.0;
    double temp_ratio;
    double member,denominator;
    for(auto Move : b.availables)
    {
        if(wins.find(pair<STATUS,int>(STATUS::AI,Move)) != wins.end())
            member = (double)wins[pair<STATUS,int>(STATUS::AI,Move)];
        else member = 0.0;

        if(plays.find(pair<STATUS,int>(STATUS::AI,Move)) != plays.end())
            denominator = (double)plays[pair<STATUS,int>(STATUS::AI,Move)];
        else denominator = 1.0;

        temp_ratio = member / denominator;
        if(temp_ratio > percent_wins)
        {
            percent_wins = temp_ratio;
            best_choice = Move;
        }
    }
    return best_choice;
}
