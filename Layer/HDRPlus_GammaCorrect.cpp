#include "HDRPlus_GammaCorrect.h"

//该函数对图像做gamma变换，pRGBImage是指向rgb图像的指针，pControl是一些全局变量的指针
void CHDRPlus_GammaCorrect::Forward(cv::Mat& pRGBImage, TGlobalControl* pControl)
{
	int nWidth = pRGBImage.cols;
	int nHeight = pRGBImage.rows;
	m_nMax = pControl->nWP;
	m_nMin = pControl->nBLC;

	int cutoff = 200;
	float gamma_toe = 12.92;
	float gamma_pow = 0.416667;         // 1 / 2.4
	float gamma_fac = 680.552897;       // 1.055 * UINT16_MAX ^ (1 - gamma_pow);
	float gamma_con = -3604.425;        // -0.055 * UINT16_MAX
	unsigned short* GammaTable = new unsigned short[m_nMax + 1];         //创建一个数组，存储gamma表
	for (int k = 0;k < m_nMax + 1;k++)
	{
		long int tmp;
		if (k < cutoff)
		{
			tmp = gamma_toe * k;
		}
		else
		{
			tmp = gamma_fac * pow(k, gamma_pow) + gamma_con;
		}
		GammaTable[k] = CLIP(tmp, m_nMin, m_nMax);
	}

	//int qq = pRGBImage.step[0];
	//int ww = pRGBImage.step[1];

#pragma omp parallel for               //openmp
	for (int y = 0; y < nHeight; y++)
	{
		unsigned short* pRGBLine = (unsigned short*)(pRGBImage.data + y * pRGBImage.step[0]);
		for (int x = 0; x < nWidth; x++)
		{
			pRGBLine[0] = GammaTable[pRGBLine[0]];                 //查表做gamma变换
			pRGBLine[1] = GammaTable[pRGBLine[1]];
			pRGBLine[2] = GammaTable[pRGBLine[2]];
			pRGBLine += 3;
		}
	}
	delete[]GammaTable;


}