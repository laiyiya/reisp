#ifndef __HDRPlus_ColorCorrect_H_
#define __HDRPlus_ColorCorrect_H_

#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp> 
#include "..//Mat/WeightConfig.h"


class CHDRPlus_ColorCorrect :public CSingleConfigTitleFILE
{
private:


protected:

	virtual void CreateConfigTitleName()
	{
		strcpy(m_pConfigTitleName, "CHDRPlus_ColorCorrect");
	}

	virtual void InitConfigParamList()
	{
		m_nConfigParamList.ConfigParamListAddVariable("bDumpFileEnable", &m_bDumpFileEnable, 0, 1);
		m_bDumpFileEnable = 1;
	}

public:
	int m_bDumpFileEnable;
	int m_nMin;
	int m_nMax;

	CHDRPlus_ColorCorrect()
	{
		Initialize();
	}

	bool Forward(cv::Mat & OutRGBData, TGlobalControl* pControl);


};



#endif // !__HDRPlus_ColorCorrect_H_
