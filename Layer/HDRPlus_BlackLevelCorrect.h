#ifndef __HDRPlus_BlackLevelCorrection_H_
#define __HDRPlus_BlackLevelCorrection_H_


#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp> 
#include "../Mat/WeightConfig.h"

class CHDRPlus_BalckLevelCorrect :public CSingleConfigTitleFILE
{
private:

protected:
	virtual void InitConfigParamList()
	{
		m_nConfigParamList.ConfigParamListAddVariable("bDumpFileEnable", &m_bDumpFileEnable, 0, 1);
		m_bDumpFileEnable = 1;
	}

	virtual void CreateConfigTitleName()
	{
		strcpy(m_pConfigTitleName, "CHDRPlus_BalckLevelCorrect");
	}

public:
	int m_bDumpFileEnable;

	CHDRPlus_BalckLevelCorrect()
	{
		Initialize();
	}

	bool Forward(cv::Mat& pInRawImage, TGlobalControl* pControl);
};



#endif
