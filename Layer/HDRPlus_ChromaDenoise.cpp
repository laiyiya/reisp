#include "HDRPlus_ChromaDenoise.h"



bool CHDRPlus_ChromaDenoise::DesaturateNoise(cv::Mat& pUVImage)
{
	int nWidth = pUVImage.cols;
	int nHeight = pUVImage.rows;
	cv::Mat pBlurUVImage1;
	cv::GaussianBlur(pUVImage, pBlurUVImage1, cv::Size(15, 15), 0);
	cv::GaussianBlur(pBlurUVImage1, pBlurUVImage1, cv::Size(15, 15), 0);

	float ratiothre = (float)m_nRatioThre / (float)16;//Factor;// 1.4f;//default=1.4
	float threshold = m_nThreshold;// 25000.f;// 25000.f;//default=25000.f
#pragma omp parallel for
	for (int y = 0; y < nHeight; y++)
	{
		unsigned short* pOutYUVline = (unsigned short*)(pUVImage.data + y * pUVImage.step[0]);
		unsigned short* pBlurYUVline = (unsigned short*)(pBlurUVImage1.data + y * pBlurUVImage1.step[0]);
		for (int x = 0; x < nWidth; x++)
		{
			float blur = pBlurYUVline[0] - 32768;
			float Input = pOutYUVline[0] - 32768;
			float Ratio = blur / Input;
			if (abs(Ratio) < ratiothre && abs(Input) < threshold && blur < threshold)
			{
				pOutYUVline[0] = 0.7f * pBlurYUVline[0] + 0.3f * pOutYUVline[0];
			}
			pOutYUVline++;
			pBlurYUVline++;
		}
	};
	return true;

}
void CHDRPlus_ChromaDenoise::IncreaseSaturation(cv::Mat& pUVImage, float len)
{
	int nWidth = pUVImage.cols;
	int nHeight = pUVImage.rows;

#pragma omp parallel for
	for (int y = 0; y < nHeight; y++)
	{
		unsigned short* pYUVline = (unsigned short*)(pUVImage.data + y * pUVImage.step[0]);
		for (int x = 0; x < nWidth; x++)
		{
			int tmp = pYUVline[0] - 32768;
			tmp = len * tmp;
			pYUVline[0] = CLIP(tmp, -32768, 32767);
			pYUVline[0] += 32768;
			pYUVline++;
		}
	}
}


bool CHDRPlus_ChromaDenoise::Forward(cv::Mat& pRGBImage, TGlobalControl* pControl)
{
	m_nMin = pControl->nBLC;
	m_nMax = pControl->nWP;
	int nGain = pControl->nCameraGain;//nGain x128
	float Amount;
	if (m_bAutoAmount)
	{
		if (m_nGainOption == 0)
		{
			nGain = pControl->nCameraGain;//iso gain
		}
		else if (m_nGainOption == 1)
		{
			nGain = (pControl->nCameraGain * pControl->nDigiGain + 64) >> 7;//iso+digigain
		}
		else
		{
			nGain = pControl->nEQGain;//lenshading之后的
		}
		//线性计算Amount值
		if (nGain < m_nGainList[0])
		{
			Amount = m_nAmountGainList[0];
		}
		else if (nGain < m_nGainList[1])
		{
			int dG = (nGain - m_nGainList[0]);
			Amount = m_nAmountGainList[0];
			Amount += (m_nAmountGainList[1] - m_nAmountGainList[0]) * dG / (m_nGainList[1] - m_nGainList[0]);
		}
		else if (nGain < m_nGainList[2])
		{
			int dG = (nGain - m_nGainList[1]);
			Amount = m_nAmountGainList[1];
			Amount += (m_nAmountGainList[2] - m_nAmountGainList[1]) * dG / (m_nGainList[2] - m_nGainList[1]);
		}
		else
		{
			Amount = m_nAmountGainList[2];
		}
	}
	else
	{
		Amount = m_nManualAmount;
	}
	Amount = Amount / 16.0;
	m_nYnoiseBilateralThre = m_nYnoiseBilateralThre * Amount;
	printf("%d %f\n", nGain, Amount);

	cv::Mat YImage, UImage, VImage, YUVImage;
	cv::cvtColor(pRGBImage, YUVImage, cv::COLOR_BGR2YUV);
	std::vector<cv::Mat> channels(3);
	cv::split(YUVImage, channels);
	YImage = channels[0];
	//printf("%0x %0x\n", YImage.datastart, channels[0].datastart);
	UImage = channels[1];
	VImage = channels[2];
	cv::Mat result;
	if (m_nDenoiseTimes > 0)
	{
		//经过convertTo方法后地址会发生改变，即使是convertTo自身也一样
		YImage.convertTo(YImage, CV_32FC1);
		//printf("%0x\n", YImage.datastart);
		cv::bilateralFilter(YImage, result, 5, m_nYnoiseBilateralThre, m_nYnoiseBilateralThre);
		result.convertTo(YImage, CV_16UC1);
		//printf("%0x\n", YImage.datastart);

	}
	for (int pass = 0;pass < m_nDenoiseTimes;pass++)
	{
		DesaturateNoise(UImage);
		DesaturateNoise(VImage);
	}
	if (m_nDenoiseTimes > 2)
	{
		float len = (float)m_nAddSaturation / (float)16;
		IncreaseSaturation(UImage, len);
		IncreaseSaturation(VImage, len);
	}
	//由于地址发生改变需要重新指向新的YImage地址
	channels[0] = YImage;
	//channels[1] = UImage;
	//channels[2] = VImage;
	cv::merge(channels, YUVImage);
	cv::cvtColor(YUVImage, pRGBImage, cv::COLOR_YUV2BGR);
	return true;
}