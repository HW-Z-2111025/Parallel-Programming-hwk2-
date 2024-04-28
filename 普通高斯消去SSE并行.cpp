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


    cout<<"开始：SIMD"<<endl;

    long long head, tail, freq;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    QueryPerformanceCounter((LARGE_INTEGER *)&head);

    __m128 vt, va, vaik, vakj, vaij, vx;

    for (int k = 0; k < N; k++)
    {
        vt = _mm_set1_ps(matrix[k][k]);

        for (int j = k + 1; j < N; j += 4)
        {
            if (j + 4 > N)
            {
                for (; j < N; j++)
                {
                    matrix[k][j] = matrix[k][j] / matrix[k][k];
                }
                break;
            }
            va = _mm_loadu_ps(&matrix[k][j]);
            va = _mm_div_ps(va, vt);
            _mm_storeu_ps(&matrix[k][j], va);
        }
        matrix[k][k] = 1.0;

        for (int i = k + 1; i < N; i++)
        {
            vaik = _mm_set1_ps(matrix[i][k]);
            for (int j = k + 1; j < N; j += 4)
            {
                if (j + 4 > N)
                {
                    for (; j < N; j++)
                    {
                        matrix[i][j] = matrix[i][j] - matrix[k][j] * matrix[i][k];
                    }
                    break;
                }
                vakj = _mm_loadu_ps(&matrix[k][j]);
                vaij = _mm_loadu_ps(&matrix[i][j]);
                vx = _mm_mul_ps(vakj, vaik);
                vaij = _mm_sub_ps(vaij, vx);
                _mm_storeu_ps(&matrix[i][j], vaij);
            }
            matrix[i][k] = 0;
        }
    }

    QueryPerformanceCounter((LARGE_INTEGER *)&tail);

    cout<<"SIMD结束"<<endl;
    cout<<"N="<<N<<"，耗时："<<(tail-head)*1000.0/freq<<"ms"<<endl;

    return 0;
}

