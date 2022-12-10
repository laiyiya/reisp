#include "HDRPlus_ColorCorrect.h"


bool CHDRPlus_ColorCorrect::Forward(cv::Mat& OutRGBData, TGlobalControl* pControl)
{
	m_nMax = pControl->nWP;
	m_nMin = pControl->nBLC;

	int nWidth = OutRGBData.cols;
	int nHeight = OutRGBData.rows;
	short ccm[3][3];
	//��pControl->nCCM[i][j]�Ŵ�4096�����������
	for (int i = 0;i < 3;i++)
	{
		for (int j = 0;j < 3;j++)
		{
			ccm[i][j] = pControl->nCCM[i][j] * 4096;
		}
	}
#pragma omp parallel for
	for (int y = 0;y < nHeight;y++)
	{
		unsigned short* pRGBLine = (unsigned short*)(OutRGBData.data + y * OutRGBData.step[0]);
		for (int x = 0;x < nWidth;x++)
		{
			//��Ϊ֮ǰ�Ŵ���4096��������ʹ��int����
			int R = (int)(pRGBLine[0] * ccm[0][0] + pRGBLine[1] * ccm[0][1] + pRGBLine[2] * ccm[0][2]);
			int G = (int)(pRGBLine[0] * ccm[1][0] + pRGBLine[1] * ccm[1][1] + pRGBLine[2] * ccm[1][2]);
			int B = (int)(pRGBLine[0] * ccm[2][0] + pRGBLine[1] * ccm[2][1] + pRGBLine[2] * ccm[2][2]);
			//��С4096��
			R >>= 12;
			G >>= 12;
			B >>= 12;
			//��ֹԽ��
			pRGBLine[0] = CLIP(R, m_nMin, m_nMax);
			pRGBLine[1] = CLIP(G, m_nMin, m_nMax);
			pRGBLine[2] = CLIP(B, m_nMin, m_nMax);
			//ָ�����
			pRGBLine += 3;
		}
	}

}