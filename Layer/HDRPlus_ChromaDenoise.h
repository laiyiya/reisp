#ifndef __HDRPlus_ChromaDenoise_H_
#define __HDRPlus_ChromaDenoise_H_

#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp> 
#include "..//Mat/WeightConfig.h"

class CHDRPlus_ChromaDenoise :public CSingleConfigTitleFILE
{
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
		m_nConfigParamList.ConfigParamListAddArray("nGainList", m_nGainList, 3, 16, 8192, 16);
		m_nGainList[0] = 32;
		m_nGainList[1] = 64;
		m_nGainList[2] = 128;
		m_nConfigParamList.ConfigParamListAddArray("nAmountGainList", m_nAmountGainList, 3, 0, 4096, 16);
		m_nAmountGainList[0] = 16;
		m_nAmountGainList[1] = 32;
		m_nAmountGainList[2] = 48;
		m_nConfigParamList.ConfigParamListAddVariable("nYnoiseBilateralThre", &m_nYnoiseBilateralThre, 0, 65535);
		m_nYnoiseBilateralThre = 512;
		m_nConfigParamList.ConfigParamListAddVariable("nDenoiseTimes", &m_nDenoiseTimes, 0, 65535);
		m_nDenoiseTimes = 3;
		m_nConfigParamList.ConfigParamListAddVariable("nRatioThre", &m_nRatioThre, 0, 65535, 16);
		m_nRatioThre = 22 * 1.4;
		m_nConfigParamList.ConfigParamListAddVariable("nThreshold", &m_nThreshold, 0, 65535);
		m_nThreshold = 25000;
		m_nConfigParamList.ConfigParamListAddVariable("nAddSaturation", &m_nAddSaturation, 0, 65535, 16);
		m_nAddSaturation = 17 * 1.1;
	}
	virtual void CreateConfigTitleName()
	{
		strcpy(m_pConfigTitleName, "CHDRPlus_ChromaDenoise");
	}


public:

	int m_bDumpFileEnable;                   //是否保存降彩噪后的图片
	int m_nMin;                              //像素的最小值
	int m_nMax;                              //像素的最大值
	int m_nManualAmount;                     //手动设置amount参数
	int m_bAutoAmount;                       //是否自动设置amount参数
	int m_nGainOption;                       //gain类型选择
	int m_nGainList[3];                      //根据gain与m_nGainList相对大小，设置amount参数
	int m_nAmountGainList[3];                //结合m_nAmountGainList，设置amount参数
	int m_nYnoiseBilateralThre;              //Y通道双边滤波阈值
	int m_nDenoiseTimes;                     //降噪次数
	int m_nRatioThre;                        //U、V通道高斯滤波后与原始融合相关比例阈值   
	int m_nThreshold;                        //U、V通道高斯滤波后与原始融合相关阈值  
	int m_nAddSaturation;                    //降噪后提升饱和度相关比例

	CHDRPlus_ChromaDenoise()
	{
		Initialize();
	}
	bool DesaturateNoise(cv::Mat& pUVImage);
	void IncreaseSaturation(cv::Mat& pUVImage, float len);
	bool Forward(cv::Mat& pRGBImage, TGlobalControl* pControl);

};



#endif // !__HDRPlus_ChromaDenoise_H_

