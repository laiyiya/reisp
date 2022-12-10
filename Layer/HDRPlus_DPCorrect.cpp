#include "HDRPlus_DPCorrect.h"


unsigned short* CHDRPlus_DPCorrection::GetImageLine(cv::Mat image, int y)
{
	return (unsigned short*)(image.data + y * image.step[0]);
}

/*
获取块里面Max和min，如果中心值Y大于max+thre则Y= max Y+thre<min Y=min 返回Y值作为去除坏点后的值
像素值在这个块里面要小于等于最大的大于等于最小的
*/
int CHDRPlus_DPCorrection::ProcessBlock(unsigned int nBlock[][5], unsigned int nMax[5], unsigned int nMin[5])
{
	unsigned int Y, Max, Min;
	bool bDPC = false;
	Y = nBlock[1][2];
	int nWhitePointCThre = m_nWhitePointCThre;
	int nBlackPointCThre = m_nBlackPointCThre;
	if (Y >= m_nBLC)
	{
		nWhitePointCThre += ((Y - m_nBLC) * m_nWhitePointLRatioT) >> 8;
		nBlackPointCThre += ((Y - m_nBLC) * m_nBlackPointLRatioT) >> 8;
	}
	Max = MAX2(MAX2(nMax[0], nMax[4]), MAX2(nBlock[0][2], nBlock[2][2]));  // 求中心点周围8个点的最大值
	Min = MIN2(MIN2(nMin[0], nMin[4]), MIN2(nBlock[0][2], nBlock[2][2]));
	if (m_bWhitePointCEnable == 1 && Y > Max + nWhitePointCThre)//nWhitePointCThre越大则进来少去白点少
	{
		Y = Max;
	}
	if (m_bBlackPointCEnable == 1 && Y + nBlackPointCThre < Min)//nBlackPointCThre越大则进来的少去黑点少
	{
		Y = Min;
	}
	return Y;
}

void CHDRPlus_DPCorrection::ProcessLine(unsigned short* pInLines[], unsigned short* pOutLine, int nWidth)
{
	//例如数组int a[10] = {0,1,2,3,4,5,6,7,8,9}
	//int* p =a,  *p++或*（p++）【*优先级小于++】表示先取*p值然后使p加1


	unsigned short* pIn[3];
	unsigned int nBlock[3][5];
	unsigned int nSum[5];
	unsigned int nMax[5];
	unsigned int nMin[5];
	for (int i = 0; i < 3; i++) // 初始化边缘的部分， pInLines是5行，width 列
	{
		pIn[i] = pInLines[i * 2];
		nBlock[i][0] = pIn[i][2];    //类似图像边缘填充，在前面填充2列
		nBlock[i][1] = pIn[i][3];    //类似图像边缘填充
		nBlock[i][2] = *(pIn[i]++);  //注意pIn[i]指的地址
		nBlock[i][3] = *(pIn[i]++);

	}
	for (int i = 0; i < 4; i++)   // 对nBlock3行5列,求每列的最大值和最小值，因此一共有5个最大值和最小值，此处先求出4个
	{
		nSum[i] = nBlock[0][i] + nBlock[1][i] + nBlock[2][i];
		if (m_bWhitePointCEnable == 1)
		{
			nMax[i] = MAX2(nBlock[0][i], MAX2(nBlock[1][i], nBlock[2][i]));
		}
		if (m_bBlackPointCEnable == 1)
		{
			nMin[i] = MIN2(nBlock[0][i], MIN2(nBlock[1][i], nBlock[2][i]));
		}
	}
	int x = 0;
	for (; x < nWidth - 2; x++)
	{
		for (int i = 0; i < 3; i++)       //获取第4列的数据，并找出第4列的最大值和最小值
		{
			nBlock[i][4] = *(pIn[i]++);
			nSum[4] = nBlock[0][4] + nBlock[1][4] + nBlock[2][4];
			if (m_bWhitePointCEnable == 1)
			{
				nMax[4] = MAX2(nBlock[0][4], MAX2(nBlock[1][4], nBlock[2][4]));
			}
			if (m_bBlackPointCEnable == 1)
			{
				nMin[4] = MIN2(nBlock[0][4], MIN2(nBlock[1][4], nBlock[2][4]));
			}
		}
		*(pOutLine++) = (unsigned short)ProcessBlock(nBlock, nMax, nMin);   //在3*5的窗口内进行黑电平校正
		for (int i = 0; i < 3; i++)         //将3*5窗口整体后移一列，对于第4列数据在for循环最开始获取，从而实现滚动
		{
			for (int j = 0; j < 4; j++)
			{
				nBlock[i][j] = nBlock[i][j + 1];
			}
		}
		for (int j = 0; j < 4; j++)       //同理，更新每列的最大值和最小值，对于第4列的数据在for循环最开始获取，从而实现滚动
		{
			nSum[j] = nSum[j + 1];
			if (m_bWhitePointCEnable == 1)
			{
				nMax[j] = nMax[j + 1];
			}
			if (m_bBlackPointCEnable == 1)
			{
				nMin[j] = nMin[j + 1];
			}
		}
	}
	for (; x < nWidth; x++)
	{
		for (int i = 0; i < 3; i++)
		{
			nBlock[i][4] = nBlock[i][0];
			nSum[4] = nSum[0];
			if (m_bWhitePointCEnable == 1)
			{
				nMax[4] = nMax[0];
			}
			if (m_bBlackPointCEnable == 1)
			{
				nMin[4] = nMin[0];
			}
		}
		*(pOutLine++) = (unsigned short)ProcessBlock(nBlock, nMax, nMin);
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				nBlock[i][j] = nBlock[i][j + 1];
			}
		}
		for (int j = 0; j < 4; j++)
		{
			nSum[j] = nSum[j + 1];
			if (m_bWhitePointCEnable == 1)
			{
				nMax[j] = nMax[j + 1];
			}
			if (m_bBlackPointCEnable == 1)
			{
				nMin[j] = nMin[j + 1];
			}
		}
	}

}


bool CHDRPlus_DPCorrection::Forward(cv::Mat& pInRawImage, cv::Mat& pOutRawImage, TGlobalControl* pControl)
{
	int nWidth = pInRawImage.cols;
	int nHeight = pInRawImage.rows;

	unsigned short* pInLines[5];
	unsigned short* pBuffer = NULL;

	//cv::Mat OutRGBImage8(cv::Size(nWidth, nHeight), CV_16UC1);
	int nProcs = omp_get_num_procs();
	pBuffer = new unsigned short[nWidth * 5 * nProcs];
	if (pBuffer == NULL) return false;
	int loop = 0;

#pragma omp parallel for num_threads(nProcs) firstprivate(loop) private(pInLines)
	for (int y = 0; y < nHeight;y++)
	{
		int nThreadId = omp_get_thread_num();
		//通过设置loop为私有属性，与loop==0，y==0两个判断，保障多线程在分块对图像操作时不会出现边缘问题
		if (loop == 0)
		{
			pInLines[0] = pBuffer + nWidth * 5 * nThreadId;                       //nThreadId线程id，可以假设为0，比较好理解
			pInLines[1] = pBuffer + nWidth * 5 * nThreadId + nWidth * 1;
			pInLines[2] = pBuffer + nWidth * 5 * nThreadId + nWidth * 2;
			pInLines[3] = pBuffer + nWidth * 5 * nThreadId + nWidth * 3;
			pInLines[4] = pBuffer + nWidth * 5 * nThreadId + nWidth * 4;
			if (y == 0)                                      //第0圈时需要填充边缘，获取前4行数据
			{
				memcpy(pInLines[0], GetImageLine(pInRawImage, 2), nWidth * sizeof(unsigned short));
				memcpy(pInLines[1], GetImageLine(pInRawImage, 3), nWidth * sizeof(unsigned short));
				memcpy(pInLines[2], GetImageLine(pInRawImage, 0), nWidth * sizeof(unsigned short));
				memcpy(pInLines[3], GetImageLine(pInRawImage, 1), nWidth * sizeof(unsigned short));
			}
			else
			{
				memcpy(pInLines[0], GetImageLine(pInRawImage, y - 2), nWidth * sizeof(unsigned short));
				memcpy(pInLines[1], GetImageLine(pInRawImage, y - 1), nWidth * sizeof(unsigned short));
				memcpy(pInLines[2], GetImageLine(pInRawImage, y + 0), nWidth * sizeof(unsigned short));
				memcpy(pInLines[3], GetImageLine(pInRawImage, y + 1), nWidth * sizeof(unsigned short));
			}
			loop++;
		}
		if (y < nHeight - 2)                                //获取第4行数据
		{
			memcpy(pInLines[4], GetImageLine(pInRawImage, y + 2), nWidth * sizeof(unsigned short));
		}
		else
		{
			pInLines[4] = pInLines[0];
		}
		//处理图像5行的数据，进行黑电平校正
		ProcessLine(pInLines, GetImageLine(pOutRawImage, y), nWidth);

		if (y < nHeight - 2)
		{
			unsigned short* pTemp = pInLines[0];                 //5行数据窗口进行滚动迭代，先迭代前4行，第4行在下一次for循环时获取（代码第177行）
			for (int i = 0; i < 4; i++)
			{
				pInLines[i] = pInLines[i + 1];
			}
			pInLines[4] = pTemp;
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				pInLines[i] = pInLines[i + 1];
			}
		}

	}

	delete[] pBuffer;
	return true;

}