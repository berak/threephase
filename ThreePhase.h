#pragma once

#include <opencv2/opencv.hpp>

class ThreePhase
{
	std::vector<cv::Point> toProcess;

	void phaseUnwrap(float basePhase, int x, int y);

public:

	cv::Mat process;
	cv::Mat phases[3];
	cv::Mat phase0;
	cv::Mat mask;


	ThreePhase(void);
	~ThreePhase(void);

	void setup(std::string sdir);

	void phaseWrap(float noiseThreshold);
	void phaseUnwrap();

	// depth image, mainly for visualization
	cv::Mat ThreePhase::computeDepth(float zscale,float zskew);

	// retrieve color from the phase images
	uchar mix(int r, int c);
};

