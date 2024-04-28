#include<stdio.h>
#include<stdlib.h>
#include <arm_neon.h>
#include<iostream>
#include<time.h>

using namespace std;



const int N = 10;
float m[N][N];

void m_reset()
{
    for(int i=0; i<N; i++)
    {
        for(int j=0;j<N;j++)
        {
            m[i][j]=0;
        }
        m[i][i]=1.0;
        for(int j=i+1; j<N; j++)
        {
            m[i][j]=rand();
        }
    }
    for(int k=0; k<N; k++)
    {
        for(int i=k+1; i<N; i++)
        {
            for(int j=0; j<N; j++)
            {
                m[i][j]+=m[k][j];
            }
        }
    }
}

int main()
{
    m_reset();

    struct timespec sts,ets;
    timespec_get(&sts, TIME_UTC);

    for (int k = 0; k < N ; k++)
    {
        float t = 1.0/m[k][k];
        float32x4_t vt = vmovq_n_f32(t);

        for (int j = k + 1; j < N; j += 4)
        {
            if (j + 4 > N)
            {
                for (; j < N; j++)
                {
                    m[k][j] = m[k][j] / m[k][k];
                }
                break;
            }
            float32x4_t va = vld1q_f32(&m[k][j]);
            va = vmulq_f32(va, vt);
            vst1q_f32(&m[k][j], va);
        }
        m[k][k] = 1.0;

        for (int i = k + 1; i < N; i++)
        {
            float32x4_t vaik = vmovq_n_f32(m[i][k]);
            for (int j = k + 1; j < N; j += 4)
            {
                if (j + 4 > N)
                {
                    for (; j < N; j++)
                    {
                        m[i][j] = m[i][j] - m[k][j] * m[i][k];
                    }
                    break;
                }
                float32x4_t vakj = vld1q_f32(&m[k][j]);
                float32x4_t vaij = vld1q_f32(&m[i][j]);
                float32x4_t vx = vmulq_f32(vakj, vaik);
                vaij = vsubq_f32(vaij, vx);
                vst1q_f32(&m[i][j], vaij);
            }
            m[i][k] = 0;
        }
    }
    timespec_get(&ets, TIME_UTC);
    time_t dsec = ets.tv_sec - sts.tv_sec;
    long dnsec = ets.tv_nsec - sts.tv_nsec;
    if(dnsec<0)
    {
        dsec--;
        dnsec+=1000000000ll;
    }
    printf ("%ld.%09lds\n",dsec,dnsec);
    return 0;
}
