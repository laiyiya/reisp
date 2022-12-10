#ifndef __HDRPlus_Sharpen_H_
#define __HDRPlus_Sharpen_H_

#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp> 
#include "../Mat/WeightConfig.h"


class CHDRPlus_Sharpen : public CSingleConfigTitleFILE
{
protected:
	virtual void InitConfigParamList()
	{
		m_nConfigParamList.ConfigParamListAddVariable("bDumpFileEnable", &m_bDumpFileEnable, 0, 1);
		m_bDumpFileEnable = 1;
		m_nConfigParamList.ConfigParamListAddVariable("nSharpenStrength", &m_nSharpenStrength, 0, 65535, 16);
		m_nSharpenStrength = 32 * 2;
	}
	virtual void CreateConfigTitleName()
	{
		strcpy(m_pConfigTitleName, "CHDRPlus_Sharpen");
	}
public:
	int m_bDumpFileEnable;
	int m_nSharpenStrength;
	CHDRPlus_Sharpen()
	{
		Initialize();
	}
		void Forward(cv::Mat& pRGBImage);
};


#endif // !__HDRPlus_Sharpen_H_

