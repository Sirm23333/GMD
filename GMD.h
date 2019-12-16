#ifndef GMD_H
#define GMD_H
#define N 7             //码长
#define K 3            //信息位
#define DH 4            //最小距离
#define TN 8           //总码字数
#define R (double)K / N //码率
#define T 1             //量化器门限
#define TRY_COUNT 4     //最大试探次数
#define SNR_START 0.5 //测试起始信噪比
#define SNR_END 12 //测试截止信噪比
#define SNR_STEP 0.5 //测试信噪比步长
#define TOTAL_TRIAL 500000//每个新增比下，测试发送码元的个数

//生成矩阵
int G[K][N] = {
    {1, 0, 0, 1, 1, 1, 0},
    {0, 1, 0, 0, 1, 1, 1},
    {0, 0, 1, 1, 1, 0, 1}};

// 所有信息位比特
int info[TN][K] = {
    {0, 0, 0},
    {0, 0, 1},
    {0, 1, 0},
    {0, 1, 1},
    {1, 0, 0},
    {1, 0, 1},
    {1, 1, 0},
    {1, 1, 1}};

/**
*  得到某个信息比特编码后的码字
*  info：       发送的信息比特
*  coded：  返回数组
*/
void getCoded(int info[K], int coded[N]);

/**
 *  得到所有编码后的码字
 * */
void getAllCodeds(int info[TN][K], int allCodeds[TN][N]);

/** 
*   输出len长度的数组
*   code：    要打印的数组
*   len：       数组长度
**/
void showCode(int code[], int len);

/**
*   得到coded调制后的传输映射（理想状态下）0-->1  1-->-1
*   code：            发送方编码后的码字
*   codeMap：   返回数组
**/
void getCodedMap(int coded[N], int codedMap[N]);

/**
 *  得到所有码字的调制后映射
 * */
void getAllCodedMaps(int allCodeds[TN][N], int allCodedMaps[TN][N]);

/**
 * 模拟在指定信噪比的高斯白噪声下，解调器解调码字
 * codedMap：   传输映射
 *  snr：                 信噪比
 *  codedMapWhithNoise：返回数组
 * */
void getCodedMapWithNoise(int codedMap[N], double snr, double codedMapWithNoise[N]);

/**
 * 将接收到的带有噪声的码字送入似然比估值器，输出各码元的似然函数比
 * codedMapWithNoise：带有噪声的码字序列
 *  snr：                         信噪比
 *  codeMapLLH：    返回数组
 * */
void getCodedLLH(double codedMapWithNoise[N], double snr, double codeMapLLH[N]);

/**
 *  将码元对数似然比送入量化器，输出量化结果
 * codeMapLLH：       对数似然比序列
 * codeMapQuan：    返回数组
 * */
void getCodedQuan(double codedMapLLH[N], double codedMapQuan[N]);

/**
 *     求量化结果的绝对值
 * */
void getCodedQuanAbs(double codedMapQuan[N], double codedMapQuanAbs[N]);
/**
 *  冒泡排序，并将原始坐标位置存到unbe数组中
 * */
void bubbleSort(double codedMapQuanAbs[N], int unbe[N], int len);

/**
 *  纠删除错误译码
 *  译码结果为allcodedMap[row_s]
 * */
void errorErasureDecording(int tryCoded[N], int allCodedMap[TN][N], int*row);

/**
 *  gmd译码
 *  输入codedMapQuan[n]为量化器输出的序列
 *  输入allcodedMaps[TN][N]为所有调制后码字映射
 *  输出输入allcodedMaps[row]为译码结果
 * */
int gmd( double codedMapQuan[N] , int allcodedMaps[TN][N] , int *row);

/**
* 产生服从标准正太分布随机数
**/
double Gaussian();
#endif