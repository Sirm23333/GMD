#include<stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "GMD.h"
int main()
{
        srand((unsigned)time(NULL));
        int allCodeds[TN][N];    //所有码字
        int allCodedMaps[TN][N]; //所有码字调制结果
        getAllCodeds(info, allCodeds);
        getAllCodedMaps(allCodeds, allCodedMaps);
        printf("所有可用码字：\n");
        for (int i = 0; i < TN; i++)
        {
                showCode(allCodeds[i], N);
        }
        printf("- - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
        int trial = 0;               //发送码字序号
        int failure = 0;             //译码失败码字数
        int error = 0;  //译码错误码字数
        int error_bits = 0;          //错误比特数
        int coded[N] ;               // 发送的码字
        int codedMap[N];             //调制后的码字
        double codedMapWithNoise[N]; ///模拟加入白噪声的序列
        double codedLLH[N];          //似然对数序列
        double codedMapQuan[N];      //量化后的序列
        int row;                     //译码后的码字序号

        //每个信噪比下模拟total_trial个码字的译码
        for (double snr = SNR_START; snr <= SNR_END; snr += SNR_STEP)
        {       
                trial = 0;
                failure = 0;
                error = 0;
                error_bits = 0;
                int error_bits_tmp = 0;
                while (trial < TOTAL_TRIAL )
                {
                        memcpy(coded, allCodeds[rand() % TN], N * sizeof(int)); //随机发送一个码字
                        getCodedMap(coded, codedMap);                           //该码字调制映射结果
                        getCodedMapWithNoise(codedMap, snr, codedMapWithNoise); //模拟加入白噪声
                        getCodedLLH(codedMapWithNoise, snr, codedLLH);          //对数似然序列
                        getCodedQuan(codedLLH, codedMapQuan);                   //量化结果
                        if (!gmd(codedMapQuan, allCodedMaps, &row))             //译码失败
                        {
                                ++failure;
                        }
                        else  //译码成功
                        {
                                error_bits_tmp = 0;
                                for (int i = 0; i < N; i++)
                                        if (allCodeds[row][i] != coded[i])
                                                ++error_bits_tmp;
                                if (error_bits_tmp > 0) //译码错误
                                {
                                        error_bits += error_bits_tmp;
                                        ++error;
                                }
                        }
                        ++trial;
                }
                printf("信噪比:%f\n",snr);
                 printf("发送码字数:%d\n",trial);
                printf("译码失败数:%d\t译码错误次数:%d\t错误总比特数:%d\n",failure,error,error_bits);
                printf("译码失败率:%.2f%\t译码成功错误率:%.2f%\t错误比特占比:%.2f%\n",(double)failure / trial * 100,(double)error / trial * 100,(double)error_bits / (trial *N)* 100) ;
                printf("- - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
        }
        system("pause");
        return 0;
}

void getCoded(int info[K], int coded[N])
{
        for (int n = 0; n < N; n++)
        {
                int tmp = 0;
                for (int k = 0; k < K; k++)
                {
                        tmp += info[k] * G[k][n];
                        tmp %= 2;
                }
                coded[n] = tmp;
        }
}
void getAllCodeds(int info[TN][K], int allCodeds[TN][N])
{
        for (int i = 0; i < TN; i++)
        {
                int tmpCoded[N];
                getCoded(info[i], tmpCoded);
                memcpy(allCodeds[i], tmpCoded, N * sizeof(int));
        }
}
void showCode(int code[], int len)
{
        for (int i = 0; i < len; i++)
        {
                printf("%d ",code[i]);
        }
        printf("\n");
}


void getCodedMap(int coded[N], int codedMap[N])
{
        for (int i = 0; i < N; i++)
        {
                codedMap[i] = 1 - 2 * coded[i];
        }
}
void getAllCodedMaps(int allcodeds[TN][N], int allCodedMaps[TN][N])
{
        for (int i = 0; i < TN; i++)
        {
                int tmpCodedMap[N];
                getCodedMap(allcodeds[i], tmpCodedMap);
                memcpy(allCodedMaps[i], tmpCodedMap, N * sizeof(int));
        }
}

void getCodedMapWithNoise(int codedMap[N], double snr, double codedMapWithNoise[N])
{
        double noise_var = sqrt(1.0 / (2.0 * R * pow(10, (snr / 10.0)))); //标准差
        double noise ;                   //噪声
        for (int i = 0; i < N; i++)
        {
                noise = noise_var * (double)Gaussian(); 
                codedMapWithNoise[i] = codedMap[i] + noise;
        }
}

void getCodedLLH(double codedMapWithNoise[N], double snr, double codeMapLLH[N])
{
        double noise_var = sqrt(1.0 / (2.0 * R * pow(10, (snr / 10.0)))); //标准差
        for (int i = 0; i < N; i++)
        {
                codeMapLLH[i] = (2.0 * codedMapWithNoise[i]) / (noise_var * noise_var);
        }
}

void getCodedQuan(double codedMapLLH[N], double codedMapQuan[N])
{
        for (int i = 0; i < N; i++)
        {
                if (codedMapLLH[i] >= T)
                        codedMapQuan[i] = 1.0;
                else if (codedMapLLH[i] > -T && codedMapLLH[i] < T)
                        codedMapQuan[i] = codedMapLLH[i] / T;
                else
                        codedMapQuan[i] = -1.0;
        }
}
void getCodedQuanAbs(double codedMapQuan[N], double codedMapQuanAbs[N])
{
        for (int i = 0; i < N; i++)
        {
                codedMapQuanAbs[i] = fabs(codedMapQuan[i]);
        }
}

void bubbleSort(double codedMapQuanAbs[N], int unbe[N], int len)
{
        int i, j;
        double temp;
        int temp1;
        for (i = 0; i < len - 1; i++)
        {
                for (j = 0; j < len - 1 - i; j++)
                {
                        if (codedMapQuanAbs[j] > codedMapQuanAbs[j + 1])
                        {
                                temp = codedMapQuanAbs[j];
                                codedMapQuanAbs[j] = codedMapQuanAbs[j + 1];
                                codedMapQuanAbs[j + 1] = temp;
                                //下标
                                temp1 = unbe[j];
                                unbe[j] = unbe[j + 1];
                                unbe[j + 1] = temp1;
                        }
                }
        }
}

void errorErasureDecording(int tryCoded[N], int allCodedMap[TN][N], int *row)
{
        int distence = 0;      //广义距离
        int mid_disten = 1000; //最小距离
        //纠错纠删译码
        for (int k = 0; k < TN; k++)
        {
                distence = 0;
                for (int j = 0; j < N; j++)
                {
                        distence += fabs(tryCoded[j] - allCodedMap[k][j]);
                }
                if (distence < mid_disten)
                {
                        mid_disten = distence;
                        *row = k;
                }
        }
}

int gmd(double codedMapQuan[N], int allcodedMaps[TN][N], int *row)
{
        //取量化结果的绝对值
        double codedMapQuanAbs[N];
        getCodedQuanAbs(codedMapQuan, codedMapQuanAbs);
        //对绝对值进行排序，得到一个下标数组
        int unbeve[N];
        for (int i = 0; i < N; i++)
                unbeve[i] = i;
        bubbleSort(codedMapQuanAbs, unbeve, N);
        int flag = 0;
        for (int i = 0; i < TRY_COUNT && !flag; i++)
        {
                // 1、在量化器输出的序列codedMapQuan中，删去i个最不可信的码元（绝对值小的i个码元）
                //        并生成一个由-1,1,0组成的试探序列，量化值大于0的1，小于0为-1，删除位为0
                int tryCoded[N]; //试探序列
                for (int j = 0; j < N; j++)
                {
                        if (codedMapQuan[j] >= 0)
                                tryCoded[j] = 1;
                        else
                                tryCoded[j] = -1;
                }
                for (int j = 0; j < i; j++)
                {
                        tryCoded[unbeve[j]] = 0;
                }

                //2、把试探序列tryCoded 送入纠错纠删译码器中，得到一个已译码字
                errorErasureDecording(tryCoded, allcodedMaps, row);
                //3、检验是否译码成功
                double check = 0.0;
                for (int k = 0; k < N; k++)
                {
                        check += codedMapQuan[k] * allcodedMaps[*row][k];
                }
                if (check > (double)(N - DH))
                {
                        flag = 1;
                        break;
                }
        }
        return flag;
}



double Gaussian()
{
	static double var1, var2, fac;
	static int phase = 0;
	static double rand1, rand2;
	double random_num, var_sum;
	if (phase == 0)
	{
		do
		{
			rand1 = (double)rand() / (RAND_MAX);
			rand2 = (double)rand() / (RAND_MAX);
			var1 = 1 - 2 * rand1;
			var2 = 1 - 2 * rand2;
			var_sum = var1 * var1 + var2 * var2;
		} while (var_sum >= 1 || var_sum <= 0);
		
		fac = sqrt(-2.0*log(var_sum) / var_sum);
		random_num = var1 * fac;
	}
	else
	{
		random_num = var2 * fac;
	}
	phase = 1 - phase;
	return random_num;            
}