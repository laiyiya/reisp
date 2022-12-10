#ifndef __HDRPlus_Tonemapping_H_
#define __HDRPlus_Tonemapping_H_

#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp> 
#include "../Mat/WeightConfig.h"


class CHDRPlus_Tonemapping : public CSingleConfigTitleFILE
{
private:

protected:
	virtual void InitConfigParamList()
	{
		m_nConfigParamList.ConfigParamListAddVariable("bDumpFileEnable", &m_bDumpFileEnable, 0, 1);
		m_bDumpFileEnable = 1;
		m_nConfigParamList.ConfigParamListAddVariable("nManualAmount", &m_nManualAmount, 1, 100000, 16);
		m_nManualAmount = 16;
		m_nConfigParamList.ConfigParamListAddVariable("bAutoAmount", &m_bAutoAmount, 0, 1);
		m_bAutoAmount = 1;
		m_nConfigParamList.ConfigParamListAddVariable("nGainOption", &m_nGainOption, 0, 2);
		m_nGainOption = 1;
		m_nConfigParamList.ConfigParamListAddArray("nGainList", m_nGainList, 3, 16, 8192, 128);
		m_nGainList[0] = 512;
		m_nGainList[1] = 1024;
		m_nGainList[2] = 2048;
		m_nConfigParamList.ConfigParamListAddArray("nAmountGainList", m_nAmountGainList, 3, 0, 4096, 16);
		m_nAmountGainList[0] = 16;
		m_nAmountGainList[1] = 32;
		m_nAmountGainList[2] = 48;

		m_nConfigParamList.ConfigParamListAddVariable("nTargetY", &m_nTargetY, 0, 65535);
		m_nTargetY = 1500;
		m_nConfigParamList.ConfigParamListAddVariable("bAutoDigiGainEnable", &m_bAutoDigiGainEnable, 0, 1);
		m_bAutoDigiGainEnable = 1;
		m_nConfigParamList.ConfigParamListAddVariable("bKeepHighLightEnable", &m_bKeepHighLightEnable, 0, 1);
		m_bKeepHighLightEnable = 1;
		m_nConfigParamList.ConfigParamListAddVariable("nMaxDigiGain", &m_nMaxDigiGain, 0, 65535, 128);
		m_nMaxDigiGain = 128;
		m_nConfigParamList.ConfigParamListAddVariable("nMaxTotalGain", &m_nMaxTotalGain, 1, 65536, 128);
		m_nMaxTotalGain = 18;
		m_nConfigParamList.ConfigParamListAddMatrixRow("nBlockWeightMap", m_nBlockWeightMap[0], 0, 4, 0, 16);
		m_nConfigParamList.ConfigParamListAddMatrixRow("nBlockWeightMap", m_nBlockWeightMap[1], 1, 4, 0, 16);
		m_nConfigParamList.ConfigParamListAddMatrixRow("nBlockWeightMap", m_nBlockWeightMap[2], 2, 4, 0, 16);
		m_nConfigParamList.ConfigParamListAddMatrixRow("nBlockWeightMap", m_nBlockWeightMap[3], 3, 4, 0, 16);
		//for (int i = 0; i < 4; i++)
		//{
		//	for (int j = 0; j < 4; j++)
		//	{
		//		m_nBlockWeightMap[i][j] = 1;
		//	}
		//}
		m_nConfigParamList.ConfigParamListAddArray("nHighLevelPtsPercent", m_nHighLevelPtsPercent, 2, 0, 65536);
		m_nHighLevelPtsPercent[0] = 16;
		m_nHighLevelPtsPercent[1] = 64;


		m_nConfigParamList.ConfigParamListAddVariable("nVirtualExposureNum", &m_nVirtualExposureNum, 0, 65535);
		m_nVirtualExposureNum = 3;
		m_nConfigParamList.ConfigParamListAddVariable("nDynamicCompression", &m_nDynamicCompression, 0, 65535, 128);
		m_nDynamicCompression = 742;

	}

	virtual void CreateConfigTitleName()
	{
		strcpy(m_pConfigTitleName, "CHDRPlus_Tonemapping");
	}

public:
	int m_bDumpFileEnable;
	int m_nMin;
	int m_nMax;

	int m_nManualAmount;
	int m_bAutoAmount;
	int m_nGainOption;
	int m_nGainList[3];
	int m_nAmountGainList[3];
	int m_nTargetY;

	int m_bAutoDigiGainEnable;
	int m_bKeepHighLightEnable;

	int m_nMaxDigiGain;
	int m_nMaxTotalGain;

	int m_nBlockWeightMap[4][4] = {
		{1,2,2,1},
		{2,4,4,2},
		{2,4,4,2},
		{1,2,2,1}
	};
	int m_nHighLevelPtsPercent[2];
	int m_nVirtualExposureNum;
	int m_nDynamicCompression;
	int m_nSmoothYThreP[13];
	int m_nSmoothYThreM[13];

	CHDRPlus_Tonemapping()
	{
		Initialize();
	}

	bool Forward(cv::Mat& pInRGBData, TGlobalControl* pControl);

	bool EstimateDigiGain(cv::Mat &pInImage, TGlobalControl* pControl);
	bool Brighten(cv::Mat& pInDarkImage, float gain, cv::Mat& pOutBrightImage);
	bool GrayGammaCorrect(cv::Mat& pInGrayImage, cv::Mat& pOutGammaImage);
	bool GammaInverse(cv::Mat& pInGrayImage, cv::Mat& pOutInverseImage);
	bool CombineDarkAndBrightImage(cv::Mat& pDarkGammaImage, cv::Mat& pBrightGammaImage, cv::Mat& pOutCombineImage);

	bool BuildWeight(cv::Mat& pDarkGammaImage, cv::Mat& pBrightGammaImage, cv::Mat& DarkWeightImage, cv::Mat& BrightWeightImage, int ScaleBit);
	bool GaussPyramidImage(cv::Mat& InputImage, cv::Mat pOutPyarmImage[], cv::Mat pOutEdgeImage[], int& nPyramidLevel, bool SaveEdge);
	void GammaCombinRGB(cv::Mat& pRGBImage, cv::Mat& pGrayImage, cv::Mat& pDarkImage);

};


#endif // !__HDRPlus_Tonemapping_H_
