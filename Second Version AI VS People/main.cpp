/** \brief
 *
    //可双人五子棋，目前还未实现AI部分
    //由于没有详细设计IO处理，所以输入格式需严格按照 “数字，数字” 格式，
      否则程序会终止
      编译环境 GNU GCC (have g++ [-std=c++11])
 *
 */


#include <iostream>
#include "mcts.h"
#include <random>
#include <algorithm>
#include <queue>
#include <cmath>



using namespace std;

int main()
{
    /*
    struct cmp
    {
        bool operator()(const pair<int,int> &a,const pair<int,int> &b)
        {
            if(a.first == b.first)
            {
                return a.second < b.second;
            }
            return a.first < b.first;
        }
    };
    map<pair<int,int>,int,cmp> test;
    test[pair<int,int>(1,1)] = 9;
    test[pair<int,int>(1,2)] = 8;
    test[pair<int,int>(1,-1)] = 8;
    test[pair<int,int>(1,3)] = 8;
    test[pair<int,int>(3,3)] = 8;
    test[pair<int,int>(3,2)] = 8;
    test[pair<int,int>(-1,2)] = 8;
    //auto  a =  test[pair<int,int>(-1,2)];
    cout<<test[pair<int,int>(8,2)]<<",5"<<endl;
    map<pair<int,int>,int,cmp>::iterator it;
    it = test.find(pair<int,int>(-1,3));
    if(it != test.end())
        cout<<it->first.first<<", "<<it->first.second<<endl;
    it = test.find(pair<int,int>(1,3));
    if(it != test.end())
        cout<<it->first.first<<", "<<it->first.second<<endl;
    it = test.find(pair<int,int>(3,3));
    if(it != test.end())
        cout<<it->first.first<<", "<<it->first.second<<endl;

    for(auto x:test)
        cout<<x.first.first<<", "<<x.first.second<<endl;
    test.clear();
    for(auto x:test)
        cout<<x.first.first<<", "<<x.first.second<<endl;
    cout<<"aa'";
*/


    char ch;
    for(int i = 0;i<5;i++)  //连开五局
    {
        Game game(4,4,3);   //4*4棋盘,连三子获胜
        game.Start();
        cout<<"start again, y/n?";
        cin>>ch;
        if(ch == 'n') break;
    }


    return 0;
}
