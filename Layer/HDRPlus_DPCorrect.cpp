#include "HDRPlus_DPCorrect.h"


unsigned short* CHDRPlus_DPCorrection::GetImageLine(cv::Mat image, int y)
{
	return (unsigned short*)(image.data + y * image.step[0]);
}

/*
��ȡ������Max��min���������ֵY����max+thre��Y= max Y+thre<min Y=min ����Yֵ��Ϊȥ��������ֵ
����ֵ�����������ҪС�ڵ������Ĵ��ڵ�����С��
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
	Max = MAX2(MAX2(nMax[0], nMax[4]), MAX2(nBlock[0][2], nBlock[2][2]));  // �����ĵ���Χ8��������ֵ
	Min = MIN2(MIN2(nMin[0], nMin[4]), MIN2(nBlock[0][2], nBlock[2][2]));
	if (m_bWhitePointCEnable == 1 && Y > Max + nWhitePointCThre)//nWhitePointCThreԽ���������ȥ�׵���
	{
		Y = Max;
	}
	if (m_bBlackPointCEnable == 1 && Y + nBlackPointCThre < Min)//nBlackPointCThreԽ�����������ȥ�ڵ���
	{
		Y = Min;
	}
	return Y;
}

void CHDRPlus_DPCorrection::ProcessLine(unsigned short* pInLines[], unsigned short* pOutLine, int nWidth)
{
	//��������int a[10] = {0,1,2,3,4,5,6,7,8,9}
	//int* p =a,  *p++��*��p++����*���ȼ�С��++����ʾ��ȡ*pֵȻ��ʹp��1


	unsigned short* pIn[3];
	unsigned int nBlock[3][5];
	unsigned int nSum[5];
	unsigned int nMax[5];
	unsigned int nMin[5];
	for (int i = 0; i < 3; i++) // ��ʼ����Ե�Ĳ��֣� pInLines��5�У�width ��
	{
		pIn[i] = pInLines[i * 2];
		nBlock[i][0] = pIn[i][2];    //����ͼ���Ե��䣬��ǰ�����2��
		nBlock[i][1] = pIn[i][3];    //����ͼ���Ե���
		nBlock[i][2] = *(pIn[i]++);  //ע��pIn[i]ָ�ĵ�ַ
		nBlock[i][3] = *(pIn[i]++);

	}
	for (int i = 0; i < 4; i++)   // ��nBlock3��5��,��ÿ�е����ֵ����Сֵ�����һ����5�����ֵ����Сֵ���˴������4��
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
		for (int i = 0; i < 3; i++)       //��ȡ��4�е����ݣ����ҳ���4�е����ֵ����Сֵ
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
		*(pOutLine++) = (unsigned short)ProcessBlock(nBlock, nMax, nMin);   //��3*5�Ĵ����ڽ��кڵ�ƽУ��
		for (int i = 0; i < 3; i++)         //��3*5�����������һ�У����ڵ�4��������forѭ���ʼ��ȡ���Ӷ�ʵ�ֹ���
		{
			for (int j = 0; j < 4; j++)
			{
				nBlock[i][j] = nBlock[i][j + 1];
			}
		}
		for (int j = 0; j < 4; j++)       //ͬ������ÿ�е����ֵ����Сֵ�����ڵ�4�е�������forѭ���ʼ��ȡ���Ӷ�ʵ�ֹ���
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
		//ͨ������loopΪ˽�����ԣ���loop==0��y==0�����жϣ����϶��߳��ڷֿ��ͼ�����ʱ������ֱ�Ե����
		if (loop == 0)
		{
			pInLines[0] = pBuffer + nWidth * 5 * nThreadId;                       //nThreadId�߳�id�����Լ���Ϊ0���ȽϺ����
			pInLines[1] = pBuffer + nWidth * 5 * nThreadId + nWidth * 1;
			pInLines[2] = pBuffer + nWidth * 5 * nThreadId + nWidth * 2;
			pInLines[3] = pBuffer + nWidth * 5 * nThreadId + nWidth * 3;
			pInLines[4] = pBuffer + nWidth * 5 * nThreadId + nWidth * 4;
			if (y == 0)                                      //��0Ȧʱ��Ҫ����Ե����ȡǰ4������
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
		if (y < nHeight - 2)                                //��ȡ��4������
		{
			memcpy(pInLines[4], GetImageLine(pInRawImage, y + 2), nWidth * sizeof(unsigned short));
		}
		else
		{
			pInLines[4] = pInLines[0];
		}
		//����ͼ��5�е����ݣ����кڵ�ƽУ��
		ProcessLine(pInLines, GetImageLine(pOutRawImage, y), nWidth);

		if (y < nHeight - 2)
		{
			unsigned short* pTemp = pInLines[0];                 //5�����ݴ��ڽ��й����������ȵ���ǰ4�У���4������һ��forѭ��ʱ��ȡ�������177�У�
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