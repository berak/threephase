#pragma once

#include <opencv2/opencv.hpp>

class ThreePhase
{
	cv::Mat process;
	std::vector<cv::Point> toProcess;

	void phaseUnwrap(float basePhase, int x, int y);

public:

	cv::Mat phases[3];
	cv::Mat phase0;
	cv::Mat mask;


	ThreePhase(void);
	~ThreePhase(void);

	void setup(std::string sdir);

	void phaseWrap(float noiseThreshold);
	void phaseUnwrap();

	cv::Mat ThreePhase::computeDepth(float zscale,float zskew);
	uchar mix(int r, int c);
};

