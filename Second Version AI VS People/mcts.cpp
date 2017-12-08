#include "mcts.h"
#include <algorithm>
#include <random>
#include <queue>
#include <ctime>
#include <cstdlib>
#include <cmath>

/* Board类方法实现 */

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


/* Human类方法实现 */

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

/* Game类方法实现 */

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
    if(choice == 2) Player[1] = new class AI(2,1000,n_in_rows,1.96);    //设置AI的参数
    shuffle(Player,Player+2, e);        //随机决定两个玩家的顺序
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

        board.Update(Player[index],Player[index]->GetAction(board));    //相应玩家着子，并更新棋盘

        if(Player[index]->GetStatus() == AI)    //用于玩家确认AI信息
        {
            cout<<"Enter y for continue\n";
            char ch;
            cin>>ch;
        }

        cout<<board<<endl;

        result res = Winner();  //用于判断游戏赢家
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
    result res = GameEnd(); //用于判断游戏是否结束
    if(res.first) return result(true,res.second);   //游戏结束
    else if(!board.GetAvailablesNum())  //平局状态
    {
        cout<<"Game end. Tie\n";
        return result(true,EMPTY);
    }
    else return result(false,EMPTY);    //游戏未结束
}

result Game::GameEnd()
{
    list<int> moved = board.GetMoved();
    if(moved.size() < ((unsigned int)(n_in_rows * 2) - 1))    //当总下子数少于胜利达成的最少子数时，不进行胜利条件判断
        return result(false,EMPTY);
    location temp;
    STATUS who;

    /* 胜利条件判断 */
    for(auto m : moved)
    {
        temp = board.MoveToLocation(m);
        who = board.GetStatus()[m];
        int count;

        if(temp.second >= 0 && temp.second<board.Size().second - n_in_rows + 1) //水平判断
        {
            count = 0;
            for(int i = m;i<m+n_in_rows;i++)
            {
                if(board.GetStatus()[i] != who) break;
                count++;
            }
            if(count == n_in_rows) return result(true,who);
        }


        if(temp.first >= 0 && temp.first<board.Size().first - n_in_rows + 1)    //垂直判断
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
           && temp.first >= 0 && temp.first<board.Size().first - n_in_rows + 1) //135度判断
        {
            count = 0;
            for(int i = m;i<m + n_in_rows * (board.Size().second + 1);i+=(board.Size().second+1))
            {
                if(board.GetStatus()[i] != who) break;
                count++;
            }
            if(count == n_in_rows) return result(true,who);
        }

        if(temp.second >= 0 && temp.second<board.Size().second - n_in_rows + 1  //45度判断
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


/* AI类方法实现 */

AI::AI(int calculation_time_,int max_actions_,int n_in_rows_,double confident_)
    : Human(STATUS::AI),calculation_time(calculation_time_), max_actions(max_actions_), n_in_rows(n_in_rows_)
      , confident(confident_), max_depth(1),simulated_p1(P1)
{
}

int AI::GetAction(Board &b)
{
    cout<<"AI Get Action\n";
    //如果棋盘上就剩最后一个位置，就直接选该位置为着子点
    if(b.GetAvailablesNum() == 1) return b.availables.front();

    //AI每一次着子时，都要重新构建一颗MCT树,将当前棋局作为初始状态
    plays.clear();
    wins.clear();
    int simulations = 0;
    time_t start;
    start = time(nullptr);
    while(time(nullptr) - start < calculation_time)
    {
        Board board_copy(b);    //拷贝一份当前棋局，作为state0
        RunSimulation(board_copy);  //基于state0(作为根节点)开始进行模拟，构建MCT树(每次模拟四个步骤：选点，扩张，模拟对弈，统计更新数据)
        simulations++;  //统计总共仿真模拟的次数
    }
    cout<<"Total simulations = "<<simulations<<endl;

    int Move;
    Move = SelectOneMove(b);    //基于构建的MCT树，选择一个着子点

    cout<<"Maximum depth searched: "<<max_depth<<endl;  //输出该MCT树的最大深度

    location loc = b.MoveToLocation(Move);
    cout<<"AI move: "<<loc.first<<","<<loc.second<<endl;    //输出AI的选择

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
    player = fixed_turn[index];     //在模拟中,AI永远是先手,因为要基于AI构建MCT树

    STATUS winner = EMPTY;
    list<status> visited_status;  //每个玩家所访问到的状态
    visited_status.clear();
    bool expand = true;
    double log_total;
    double value;
    int Move;

    for(int t = 1;t<max_actions+1;t++)  //开始一局模拟对弈，每局最多下max_action个回合
    {
        /* 为保证UCT值的存在，首先要保证每个状态都至少被探索过一次 */
        if(AllMovesHaveInfo(b.availables,player->GetStatus()))  //每个玩家,每个点都有统计信息后(笛卡尔积)，开始有目的下
        {
            log_total = log(SumPlays(b.availables,player->GetStatus()));
            MaxUCB(value,Move,log_total,player->GetStatus(),b.availables);
        }
        else    //否则随机选择（这里可以使用启发式算法来更有效的选择着子点）
            Move = choice(b.availables);

        b.Update(player,Move);

        /* 树生长部分 */
        if(expand &&    //每次模拟最多扩展一次，每次扩展只增加一个着法,一是为了提高算法速度，二是想每次获得更多的数据，提高算法的精度
           (plays.find(status(player->GetStatus(),Move)) == plays.end()))
        {
            expand = false;
            plays[status(player->GetStatus(),Move)] = 0;
            wins[status(player->GetStatus(),Move)] = 0;
            /* 因为在一局模拟对弈中，对应方向始终是沿树向下搜索的，所以每下一个子，树深度就加一 */
            if(t>max_depth) max_depth = t;  //目前遍历的最大深度
        }

        /*用来记录本次模拟的搜索路径 */
        visited_status.push_back(status(player->GetStatus(),Move));


        result res = Winner(b);
        winner = res.second;
        if(res.first) break; //平局或胜负关系确定，此轮结束

        //换手
        index = turn.front();
        turn.pop();
        turn.push(index);
        player = fixed_turn[index];
    }

    //反向传播:每模拟完一局，对树中已生成的节点统计相关信息
    for(auto x : visited_status)    //遍历本次搜索路径
    {
        if(plays.find(status(x.first,x.second)) == plays.end()) //虽然该节点在搜索路径中，
            continue;                                           //但目前该树却还没有生成该节点
        plays[status(x.first,x.second)]++;  //所有在搜索路径中且该树已经生成的点，访问次数加一
        if(x.first == winner) wins[status(x.first,x.second)]++; //如果搜索路径导致胜利，则胜利方的节点胜利次数加一
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

        if(temp.second >= 0 && temp.second<board.Size().second - n_in_rows + 1) //水平判断
        {
            count = 0;
            for(int i = m;i<m+n_in_rows;i++)
            {
                if(board.GetStatus()[i] != who) break;
                count++;
            }
            if(count == n_in_rows) return result(true,who);
        }


        if(temp.first >= 0 && temp.first<board.Size().first - n_in_rows + 1)   //垂直判断
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
           && temp.first >= 0 && temp.first<board.Size().first - n_in_rows + 1) //135度判断
        {
            count = 0;
            for(int i = m;i<m + n_in_rows * (board.Size().second + 1);i+=(board.Size().second+1))
            {
                if(board.GetStatus()[i] != who) break;
                count++;
            }
            if(count == n_in_rows) return result(true,who);
        }

        if(temp.second >= 0 && temp.second<board.Size().second - n_in_rows + 1 //45度判断
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
    if(res.first) return result(true,res.second);   //胜负关系确定
    else if(!board.GetAvailablesNum())  //平局
    {
        return result(true,EMPTY);
    }
    else return result(false,EMPTY);    //胜负关系还未确定
}

/* 基于MCT树的信息，选择最优点，可以选择不同的criteria,这里选择了胜率指标 */
int AI::SelectOneMove(const Board &b)
{
    int best_choice = -1;
    double percent_wins = -10.0;
    double temp_ratio;
    double member,denominator;
    for(auto Move : b.availables)
    {
        if(wins.find(status(STATUS::AI,Move)) != wins.end())
            member = (double)wins[status(STATUS::AI,Move)];
        else member = 0.0;

        if(plays.find(status(STATUS::AI,Move)) != plays.end())
            denominator = (double)plays[status(STATUS::AI,Move)];
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
