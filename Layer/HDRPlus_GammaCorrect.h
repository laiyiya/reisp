#ifndef __HDRPlus_GammaCorrect_H_
#define __HDRPlus_GammaCorrect_H_

#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp> 
#include "../Mat/WeightConfig.h"

class CHDRPlus_GammaCorrect :public CSingleConfigTitleFILE
{
protected:
	virtual void InitConfigParamList()
	{
		m_nConfigParamList.ConfigParamListAddVariable("bDumpFileEnable", &m_bDumpFileEnable, 0, 1);
		m_bDumpFileEnable = 1;
	}
	
	virtual void CreateConfigTitleName()
	{
		strcpy(m_pConfigTitleName, "CHDRPlus_GammaCorrect");
	}


public:

	int m_bDumpFileEnable;
	int m_nMin;
	int m_nMax;
	CHDRPlus_GammaCorrect()
	{
		Initialize();
	}

	void Forward(cv::Mat& pRGBImage, TGlobalControl* pControl);

};





#endif // !__HDRPlus_GammaCorrect_H_
