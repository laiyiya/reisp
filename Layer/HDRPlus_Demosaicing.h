#ifndef __HDRPlus_Demosaicing_H_
#define __HDRPlus_Demosaicing_H_

#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp> 
#include "..//Mat/WeightConfig.h"



// using namespace cv;
class CHDRPlus_Demosaicing :public CSingleConfigTitleFILE
{
protected:
	virtual void InitConfigParamList()
	{
		m_nConfigParamList.ConfigParamListAddVariable("bDumpFileEnable", &m_bDumpFileEnable, 0, 1, 1);
		m_bDumpFileEnable = 1;
	}
	virtual void CreateConfigTitleName()
	{
		strcmp(m_pConfigTitleName, "CHDRPlus_Demosaicing");
	}
public:
	int m_bDumpFileEnable;
	int m_nMin = 0;
	int m_nMax = 65535;

	CHDRPlus_Demosaicing()
	{
		Initialize();
	}



	bool Forward(cv::Mat& source, cv::Mat& dst);

};
#endif