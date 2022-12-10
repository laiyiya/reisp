#include "HDRPlus_BlackLevelCorrect.h"

bool CHDRPlus_BalckLevelCorrect::Forward(cv::Mat& pInRawImage, TGlobalControl* pControl)
{
	int nWidth = pInRawImage.cols;
	int nHeight = pInRawImage.rows;

	float factor = 65535.f / (float)(pControl->nWP - pControl->nBLC);
	unsigned short table[65536];
	for (int k = 0;k < 65536;k++)
	{
		int tmp = k * factor;
		table[k] = CLIP(tmp, 0, 65535);
	}
#pragma omp parallel for
	for (int y = 0;y < nHeight;y++)
	{
		unsigned short* pRawLine = (unsigned short*)(pInRawImage.data + y * pInRawImage.step[0]);
		int x = 0;
		for (;x < nWidth;x++)
		{
			int tmp = pRawLine[x] - pControl->nBLC;
			if (tmp < 0)
			{
				tmp = 0;
			}
			pRawLine[x] = table[tmp];
		}
	}
	pControl->nWP = 65535;
	pControl->nBLC = 0;
}