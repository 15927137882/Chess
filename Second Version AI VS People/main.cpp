/** \brief
 *
    //��˫��������,AI���� Monte Carlo Search Tree ʵ��
    //����û����ϸ���IO�������������ʽ���ϸ��� �����֣����֡� ��ʽ��
      ����������ֹ
      ���뻷�� GNU GCC (have g++ [-std=c++11])
 *
 */


#include <iostream>
#include "mcts.h"



using namespace std;

int main()
{
    char ch;
    for(int i = 0;i<5;i++)  //�������
    {
        Game game(6,6,4);   //������Ϸ����,��������������������ʤ
        game.Start();   //������Ϸ
        cout<<"start again, y/n?";
        cin>>ch;
        if(ch == 'n') break;
    }


    return 0;
}
