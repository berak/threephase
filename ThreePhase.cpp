#include "ThreePhase.h"
#include <opencv2/core/core.hpp>

template < class T > 
T min(T a, T b)
{
	return a<b?a:b;
}
template < class T > 
T max(T a, T b)
{
	return a>b?a:b;
}



#define WRAP_V_SIZE 511 // i1 - i3, -255 to 255, |511|
#define WRAP_U_SIZE 1021 // 2 * i2 - i1 - i3, -510 to 510, |1021|
#define WRAP_V_OFFSET 255 // min value is -255
#define WRAP_U_OFFSET 510 // min value is -510
unsigned char phaseGammaLut[WRAP_V_SIZE][WRAP_U_SIZE][2];



ThreePhase::ThreePhase(void)
{
	float sqrt3 = sqrtf(3.);
	float ipi = float(128. / CV_PI);
	for(int vo = 0; vo < WRAP_V_SIZE; vo++) 
	{
		for(int uo = 0; uo < WRAP_U_SIZE; uo++) 
		{
			float v = float(vo - WRAP_V_OFFSET);
			float u = float(uo - WRAP_U_OFFSET);
			float phase = atan2f(sqrt3 * v, u) * ipi;
			float modulation = sqrtf(3 * v * v + u * u);
			phaseGammaLut[vo][uo][0] = 128 + (unsigned char) phase;
			phaseGammaLut[vo][uo][1] = modulation > 255 ? 255 : (unsigned char) modulation;
		}
	}
}

ThreePhase::~ThreePhase(void)
{
}


void ThreePhase::setup(std::string sdir)
{
	phases[0] = cv::imread(sdir+"phase1.png",0); // gray!
	phases[1] = cv::imread(sdir+"phase2.png",0);
	phases[2] = cv::imread(sdir+"phase3.png",0);

	int inputWidth  = phases[0].cols;
	int inputHeight = phases[0].rows;
	phase0 = cv::Mat::zeros(inputHeight,inputWidth,CV_32F);
	mask = cv::Mat::zeros(inputHeight,inputWidth,CV_8U);
	process = cv::Mat::zeros(inputHeight,inputWidth,CV_8U);
}


void ThreePhase::phaseUnwrap() 
{
	int inputWidth  = phases[0].cols;
	int inputHeight = phases[0].rows;
	int startX = inputWidth / 2;
	int startY = inputHeight / 2;

	toProcess.clear();
	toProcess.push_back(cv::Point(startX, startY));
	process.at<uchar>(startY,startX) = false;

	while (!toProcess.empty()) 
	{
		cv::Point xy = toProcess.back();
		toProcess.pop_back();
		int x = xy.x;
		int y = xy.y;
		float r = phase0.at<float>(y,x);

		if (y > 0)
			phaseUnwrap(r, x, y-1);
		if (y < inputHeight-2)
			phaseUnwrap(r, x, y+1);
		if (x > 0)
			phaseUnwrap(r, x-1, y);
		if (x < inputWidth-2)
			phaseUnwrap(r, x+1, y);
	}
}

void ThreePhase::phaseUnwrap(float basePhase, int x, int y) 
{
	int idx = x+y*process.cols;
	if(process.data[idx])
	{
		float & p0 = ((float*)phase0.data)[idx];
		float diff = p0 - (basePhase - (int) basePhase);
		if (diff > .5f)
			diff--;
		if (diff < -.5f)
			diff++;
		p0 = basePhase + diff;
		process.data[idx] = false;
		toProcess.push_back(cv::Point(x, y));
	}
}




void ThreePhase::phaseWrap(float noiseThreshold)
{
	int inputWidth = phases[0].cols;
	int inputHeight = phases[0].rows;
	for (int y = 0; y < inputHeight; y++) 
	{
		for (int x = 0; x < inputWidth; x++) 
		{     
			int i = x + y * inputWidth;  

			int color1 = phases[0].data[i];
			int color2 = phases[1].data[i];
			int color3 = phases[2].data[i];

			int v = color1 - color3;
			int u = 2 * color2 - color1 - color3;

			unsigned char* cur = phaseGammaLut[v + WRAP_V_OFFSET][u + WRAP_U_OFFSET];
			mask.data[i] = ( cur[1] < noiseThreshold * 64 );

			process.data[i] = ! mask.data[i];
			//if ( process.data[i] ) 
			{
				// Song Zhang  "Recent progresses on real-time 3D shape measurement..."
				//float p2 = atan2(sqrt3 * (phase1 - phase3), 2.0f * phase2 - phase1 - phase3) / TWO_PI;
				float p2 = (float(cur[0])-128.0f) / 255.0f;
				((float*)phase0.data)[i] = p2;
			}
		}
	}
}


cv::Mat ThreePhase::computeDepth (float zscale,float zskew) 
{
	cv::Mat depth(phase0.rows,phase0.cols,phase0.type());
	float* ptrUnwrappedPhase = (float *)(phase0.data);

	for(int i = 0; i<phase0.rows; i++) 
	{
		float planephase = float(i - phase0.rows/2);
		if ( zskew )
			planephase /= zskew;
		planephase += 0.5f;
		for(int j=0; j<phase0.cols; j++) 
		{
			int ii = i*phase0.cols+j;
			((float*)(depth.data))[ii] = mask.data[ii] ? .0f : ((ptrUnwrappedPhase[ii] - planephase) * zscale);
		}
	}
	return depth;
}

uchar ThreePhase::mix(int r, int c)
{
	return uchar(int(phases[0].at<uchar>(r,c) + phases[1].at<uchar>(r,c) + phases[2].at<uchar>(r,c)) / 3);
}
