#include <iostream>
#include <nmmintrin.h>
#include<math.h>
#include<windows.h>
#include<random>
#include<ctime>
#include<time.h>
using namespace std;


const int N = 10;
float matrix[N][N];
LARGE_INTEGER frequency;

float get_random_float()
{
    static std::mt19937 gen(std::time(nullptr));
    std::uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen);
}

//初始化矩阵
void m_reset()
{
    cout<<"开始初始化矩阵"<<endl;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            matrix[i][j] = 0;
        }
        matrix[i][i] = 1.0;
        for (int j = i + 1; j < N; j++)
        {
            matrix[i][j] = rand();
        }
    }
    for (int k = 0; k < N; k++)
    {
        for (int i = k + 1; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                matrix[i][j] += matrix[k][j];
            }
        }
    }
}
//打印矩阵
void print()
{
    cout<<"开始打印矩阵"<<endl;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

int main()
{
    m_reset();

    cout<<"开始：串行算法"<<endl;

    long long head, tail, freq;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    QueryPerformanceCounter((LARGE_INTEGER *)&head);

    for(int i=0;i<N;i++)
    {
        for(int j=i+1;j<N;j++)
        {
            matrix[i][j]/=matrix[i][i];
        }
        matrix[i][i] = 1;
        for(int ii=i+1; ii<N; ii++)
        {
            for(int jj = i+1; jj<N; jj++)
            {
                matrix[ii][jj]-=matrix[ii][i]*matrix[i][jj];
            }
            matrix[ii][i] = 0;

        }
    }


    QueryPerformanceCounter((LARGE_INTEGER *)&tail);


    cout<<"串行算法结束"<<endl;
    cout<<"N="<<N<<"，耗时："<<(tail-head)*1000.0/freq<<"ms"<<endl;


    return 0;
}

