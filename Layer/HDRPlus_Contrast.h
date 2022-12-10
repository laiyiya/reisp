#ifndef __HDRPlus_Contrast_H_
#define __HDRPlus_Contrast_H_

#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp> 
#include "../Mat/WeightConfig.h"


class CHDRPlus_Contrast : public CSingleConfigTitleFILE
{
protected:
	virtual void InitConfigParamList()
	{
		m_nConfigParamList.ConfigParamListAddVariable("bDumpFileEnable", &m_bDumpFileEnable, 0, 1);
		m_bDumpFileEnable = 1;
		m_nConfigParamList.ConfigParamListAddVariable("nBlacklevel", &m_nBlacklevel, 0, 65535);
		m_nBlacklevel = 2000;
		m_nConfigParamList.ConfigParamListAddVariable("ContrastStrength", &m_ContrastStrength, 0, 65535, 16);
		m_ContrastStrength = 80 * 16;
	}
	virtual void CreateConfigTitleName()
	{
		strcpy(m_pConfigTitleName, "CHDRPlus_Contrast");
	}
public:
	int m_bDumpFileEnable;
	int m_nMin;
	int m_nMax;
	int m_nBlacklevel;
	int m_ContrastStrength;
	CHDRPlus_Contrast()
	{
		Initialize();
	}
	void Forward(cv::Mat& pRGBImage, TGlobalControl* pControl);
};




#endif // !__HDRPlus_Contrast_H_
