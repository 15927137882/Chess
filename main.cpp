/** \brief
 *
    //��˫�������壬Ŀǰ��δʵ��AI����
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
        Game game(4,4,3);   //4*4����,�����ӻ�ʤ
        game.Start();
        cout<<"start again, y/n?";
        cin>>ch;
        if(ch == 'n') break;
    }

    return 0;
}
