/** \brief
 *
    //可双人五子棋,AI采用 Monte Carlo Search Tree 实现
    //由于没有详细设计IO处理，所以输入格式需严格按照 “数字，数字” 格式，
      否则程序会终止
      编译环境 GNU GCC (have g++ [-std=c++11])
 *
 */


#include <iostream>
#include "mcts.h"



using namespace std;

int main()
{
    char ch;
    for(int i = 0;i<5;i++)  //连开五局
    {
        Game game(5,5,4);   //创建游戏对象,行数，列数，多少子连胜
        game.Start();   //启动游戏
        cout<<"start again, y/n?";
        cin>>ch;
        if(ch == 'n') break;
    }


    return 0;
}
