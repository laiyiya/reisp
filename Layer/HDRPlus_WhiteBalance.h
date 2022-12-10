#ifndef __HDRPlus_WhiteBalance_H_
#define __HDRPlus_WhiteBalance_H_

#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp> 
#include "../Mat/WeightConfig.h"




class CHDRPlus_WhiteBalance : public CSingleConfigTitleFILE
{
protected:
	virtual void InitConfigParamList()
	{
		m_nConfigParamList.ConfigParamListAddVariable("bDumpFileEnable", &m_bDumpFileEnable, 0, 1);
		m_bDumpFileEnable = 1;

		m_nConfigParamList.ConfigParamListAddVariable("bAutoAWBEnable", &m_bAutoAWBEnable, 0, 1);
		m_bAutoAWBEnable = 1;
		m_nConfigParamList.ConfigParamListAddVariable("nMapThre", &m_nMapThre, 0, 65536);
		m_nMapThre = 1;
		m_nConfigParamList.ConfigParamListAddVariable("nBlockSize", &m_nBlockSize, 0, 65536);
		m_nBlockSize = 16;


	}
	virtual void CreateConfigTitleName()
	{
		strcpy(m_pConfigTitleName, "CHDRPlus_WhiteBalance");
	}
public:
	int m_bDumpFileEnable;
	int m_bAutoAWBEnable;
	int m_nMapThre;//128
	int m_nBlockSize;

	int m_nMin = 0;
	int m_nMax = 65535;


	cv::Mat m_nAWBMap;
	cv::Mat m_nROIAllWPareaImage;
	cv::Mat m_nROIUsefulWPareaImage;


	CHDRPlus_WhiteBalance()
	{
		Initialize();
	}
	void Forward(cv::Mat &pInRawImage, TGlobalControl* pControl);

	//void Forward2(cv::Mat& pInRawImage, TGlobalControl* pControl);
	//bool EstimateCTandAWB(cv::Mat &pImage, TGlobalControl* pControl, cv::Mat& Map, int& nCT, float fAWBGain[]);
	//void threesigma(cv::Mat& pRGImage, cv::Mat& pBGImage, int num, float* std, float* mean);
	//bool LoadMapFromFile(char* pFileName);

};



#endif // !__HDRPlus_WhiteBalance_H_
