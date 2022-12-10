#ifndef __HDRPlus_Forward_H_
#define __HDRPlus_Forward_H_
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp> 
#include "..//Mat/WeightConfig.h"
#include "HDRPlus_Demosaicing.h"
#include "HDRPlus_WhiteBalance.h"
#include "HDRPlus_DPCorrect.h"
#include "HDRPlus_BlackLevelCorrect.h"
#include "HDRPlus_ColorCorrect.h"
#include "HDRPlus_Tonemapping.h"
#include "HDRPlus_GammaCorrect.h"
#include "HDRPlus_ChromaDenoise.h"
#include "HDRPlus_Contrast.h"
#include "HDRPlus_Sharpen.h"

class CHDRPlus_Forward :public CMultiConfigFILE
{
private:
	CHDRPlus_BalckLevelCorrect m_HDRPlus_BalckLevelCorrect;
	CHDRPlus_DPCorrection m_HDRPlus_DPCorrection;
	CHDRPlus_WhiteBalance m_HDRPlusWhiteBalance;
	CHDRPlus_Demosaicing m_HDRPlus_Demosaicing;
	CHDRPlus_ColorCorrect m_HDRPlus_ColorCorrect;
	CHDRPlus_Tonemapping m_HDRPlus_Tonemapping;
	CHDRPlus_GammaCorrect m_HDRPlus_GammaCorrect;
	CHDRPlus_ChromaDenoise m_HDRPlus_ChromaDenoise;
	CHDRPlus_Contrast m_HDRPlus_Contrast;
	CHDRPlus_Sharpen m_HDRPlus_Sharpen;

	//需要按顺序添加
	virtual void CreateConfigTitleNameList()
	{
		AddConfigTitle(&m_HDRPlus_DPCorrection);
		AddConfigTitle(&m_HDRPlus_BalckLevelCorrect);
		AddConfigTitle(&m_HDRPlusWhiteBalance);
		AddConfigTitle(&m_HDRPlus_Demosaicing);
		AddConfigTitle(&m_HDRPlus_ColorCorrect);
		AddConfigTitle(&m_HDRPlus_Tonemapping);
		AddConfigTitle(&m_HDRPlus_GammaCorrect);
		AddConfigTitle(&m_HDRPlus_ChromaDenoise);
		AddConfigTitle(&m_HDRPlus_Contrast);
		AddConfigTitle(&m_HDRPlus_Sharpen);
	}
	virtual void CreateConfigTitleName()
	{
		strcpy(m_pConfigTitleName, "CHDRPlus_Forward");
	}
	virtual void InitConfigParamList()
	{
		m_nConfigParamList.ConfigParamListAddVariable("nDPCorrectionEnable", &m_nDPCorrectionEnable, 0, 1);
		//m_nDPCorrectionEnable = 1;
		m_nConfigParamList.ConfigParamListAddVariable("nBlackLevelCorrectEnable", &m_nBlackLevelCorrectEnable, 0, 1);
		//m_nBlackLevelCorrectEnable = 1;
		m_nConfigParamList.ConfigParamListAddVariable("nWhiteBalanceEnable", &m_nWhiteBalanceEnable, 0, 1);
		//m_nWhiteBalanceEnable = 1;
		m_nConfigParamList.ConfigParamListAddVariable("nDemosaicingEnable", &m_nDemosaicingEnable, 0, 1);
		//m_nDemosaicingEnable = 1;
		m_nConfigParamList.ConfigParamListAddVariable("nColorCorrectEnable", &m_nColorCorrectEnable, 0, 1);
		//m_nColorCorrectEnable = 1;
		m_nConfigParamList.ConfigParamListAddVariable("nTonemappingEnable", &m_nTonemappingEnable, 0, 1);
		//m_nTonemappingEnable = 1;
		m_nConfigParamList.ConfigParamListAddVariable("nGammaCorrectEnable", &m_nGammaCorrectEnable, 0, 1);
		//m_nGammaCorrectEnable = 1;
		m_nConfigParamList.ConfigParamListAddVariable("nChromaDenoiseEnable", &m_nChromaDenoiseEnable, 0, 1);
		//m_nChromaDenoiseEnable = 1;
		m_nConfigParamList.ConfigParamListAddVariable("nContrastEnable", &m_nContrastEnable, 0, 1);
		//m_nContrastEnable = 1;
		m_nConfigParamList.ConfigParamListAddVariable("nSharpenEnable", &m_nSharpenEnable, 0, 1);
		//m_nSharpenEnable = 1;
	}

public:
	int m_bDumpFileEnable = 1;
	int m_nBlockMatchFusionEnable = 1;
	int m_nBlackLevelCorrectEnable = 1;
	int m_nDPCorrectionEnable = 1;
	int m_nWhiteBalanceEnable = 1;
	int m_nDemosaicingEnable = 1;
	int m_nChromaDenoiseEnable = 1;
	int m_nColorCorrectEnable = 1;
	int m_nTonemappingEnable = 1;
	int m_nGammaCorrectEnable = 1;
	int m_nContrastEnable = 1;
	int m_nSharpenEnable = 1;

	//bool LoadAWBMapFile(char* pFileName)
	//{
	//	return (m_HDRPlusWhiteBalance.LoadMapFromFile(pFileName));
	//}


	CHDRPlus_Forward()
	{
		Initialize();
	}

	void Forward(cv::Mat& RawData, cv::Mat& OutRGBData, TGlobalControl* pControl);

};


#endif 