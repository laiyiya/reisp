#ifndef __Baasicdef_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstdint>
#include "math.h"
#include <omp.h>

#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp> 

#define PI 3.14159
#define CLIP(x, min, max) ((x)<(min))?(min):((x)>(max)?(max):(x))
#define MIN2(x, y) (((x)<(y))?(x):(y))
#define MAX2(x, y) (((x)>(y))?(x):(y))
enum RAWTYPE
{
	BGGR = 0, GBRG, GRBG, RGGB
};



typedef struct tagGlobalControl
{
	int nFrameNum;
	int nCFAPattern;
	int nAWBGain[4];
	int nCameraGain;
	int nCameraExposure;
	int nBLC;
	int nWP;
	int nBit;
	int nISPGain;
	int nDigiGain;
	int nEQGain;
	int nLENCQ;
	int nfaceNum;
	float nCCM[3][3];
	tagGlobalControl()
	{
		nFrameNum = 1;
		nCFAPattern = 0;
		nfaceNum = 0;
		nAWBGain[0] = nAWBGain[1] = nAWBGain[2] = nAWBGain[3] = 256;
		nCameraGain = 16;
		nISPGain = 128;
		nDigiGain = 128;
		nCameraExposure = 1;
		nBLC = 16;
		nBit = 16;
		nEQGain = 16;
		nLENCQ = 64;
		nCCM[0][0] = 1; nCCM[0][1] = 0; nCCM[0][2] = 0;
		nCCM[1][0] = 0; nCCM[1][1] = 1; nCCM[1][2] = 0;
		nCCM[2][0] = 0; nCCM[2][1] = 0; nCCM[2][2] = 1;
	}
}TGlobalControl;



#endif // !__Baasicdef_H_

