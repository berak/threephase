#include "glapp.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
//using namespace cv;

#include "ThreePhase.h"
ThreePhase f3;


int step=4;
int psize=2;
int mscale = 200;
int zrange = 45;
int zscale = 151;
int zskew = 179;
int znoise = 7200;
int zblur = 16;
float rotY = 0.0f;
float posX = -1.0f;
float posY = 0.0f;
float posZ = -8.0f;
int inputWidth;
int inputHeight;

void cbnoise(int v, void *)
{
	int64 t0 = cv::getTickCount();
	float thresh = float(v)/10000.0f; 
	f3.phaseWrap(thresh);
	int64 t1 = cv::getTickCount();
    f3.phaseUnwrap();
	int64 t2 = cv::getTickCount();
	if ( zblur )
		cv::blur(f3.mask,f3.mask,cv::Size(zblur,zblur));
	int64 t3 = cv::getTickCount();
	double freq= cv::getTickFrequency();
	cerr << (t1-t0)/freq << "\t"<< (t2-t1)/freq << "\t"<< (t3-t2)/freq << "\t"<< (t3-t0)/freq << endl;
}

//
// filter outliers in z dir
//
float meanz()
{
	int scale=zscale-mscale;
	int skew=zskew-mscale;
	if ( skew == 0)
		skew = 1;
	float m = 0;
	float n = 0;
	for ( int r=0; r<inputHeight; r+=step )
	{
		float planephase = 0.5f + float(r - (inputHeight / 2)) / skew;
		for ( int c=0; c<inputWidth; c+=step )
		{
			if ( ! f3.mask.at<uchar>(r,c) )
			{
				float z = (f3.phase0.at<float>(r,c) - planephase) * scale;
				m += z;
				n += 1.0f;
			}
		}
	}
	return (m) / n;
}

void drawpoints()
{
	int scale=zscale-mscale;
	int skew=zskew-mscale;
	if ( skew == 0)
		skew = 1;
	float mz = meanz();

	glPointSize(float(psize));
	glBegin(GL_POINTS);
	for ( int r=0; r<inputHeight; r+=step )
	{
		float planephase =  0.5 + float(r - (inputHeight / 2)) / skew;
		float z0 = 0;
		float range = zrange;
		for ( int c=0; c<inputWidth; c+=step )
		{
			if ( ! f3.mask.at<uchar>(r,c) )
			{
				float z = (f3.phase0.at<float>(r,c) - planephase) * scale;
				if ( abs(z-mz)<range )
				{
					uchar col = (f3.phases[0].at<uchar>(r,c)+f3.phases[1].at<uchar>(r,c)+f3.phases[2].at<uchar>(r,c)) / 3;
					glColor3ub(col,col,col);
					glVertex3f( 0.01f*c, -0.01f*r, 0.01f*z );
					z0 = z;
				}
			}
		}
	}
	glEnd();
}


//
// colored vertices quads, no real texture yet
// outliers suck!
//
void drawmesh()
{
	int scale=zscale-mscale;
	int skew=zskew-mscale;
	if ( skew == 0)
		skew = 1;
	int range = zrange;
	float wn=6,hn=6;
	float mz = meanz();
	for ( int r=0; r<inputHeight-step; r+=step )
	{
		for ( int c=0; c<inputWidth-step; c+=step )
		{
			if ( f3.mask.at<uchar>(r,c) )
				continue;
			int px = c;
			int py = r;
			float planephase = 0.5f + float(py - (inputHeight / 2)) / skew;
			float pz0 = (f3.phase0.at<float>(py,px) - planephase) * scale;
			float pz = pz0;
			if ( abs(pz-mz)>range )
				continue;

			glBegin(GL_QUADS);
			uchar col = f3.mix(py,px);
			glColor3ub(col,col,col);
			glVertex3f(0.01f*px, -0.01f*py, 0.01f*pz);
			px = c;
			py = r+step;
			planephase = 0.5f + float(py - (inputHeight / 2)) / skew;
			pz0 = (f3.phase0.at<float>(py,px) - planephase) * scale;
			if ( abs(mz-pz0)<range )
				pz = pz0;
			col = f3.mix(py,px);
			glColor3ub(col,col,col);
			glVertex3f(0.01f*px, -0.01f*py, 0.01f*pz);
			px = c+step;
			py = r+step;
			planephase = 0.5f + float(py - (inputHeight / 2)) / skew;
			pz0 = (f3.phase0.at<float>(py,px) - planephase) * scale;
			if ( abs(mz-pz0)<range )
				pz = pz0;
			col = f3.mix(py,px);
			glColor3ub(col,col,col);
			glVertex3f(0.01f*px, -0.01f*py, 0.01f*pz);
			px = c+step;
			py = r;
			planephase = 0.5f + float(py - (inputHeight / 2)) / skew;
			pz0 = (f3.phase0.at<float>(py,px) - planephase) * scale;
			if ( abs(mz-pz0)<range )
				pz = pz0;
			col = f3.mix(py,px);
			glColor3ub(col,col,col);
			glVertex3f(0.01f*px, -0.01f*py, 0.01f*pz);

			glEnd();
		}
	}
}

void appInit()
{
	glClearColor(0.0f,0.3f,0.0f,1.0f);
	glEnable(GL_DEPTH_TEST);
	reshape(400,400);

	f3.setup("data/m/");

	cbnoise(znoise,0);

	cv::namedWindow("uwrap",0);
	cv::createTrackbar("range","uwrap",&zrange,1000);
	cv::createTrackbar("blur","uwrap",&zblur,64);
	cv::createTrackbar("noise","uwrap",&znoise,10000,cbnoise,0);
	cv::createTrackbar("scale","uwrap",&zscale,2*mscale);
	cv::createTrackbar("skew","uwrap",&zskew,2*mscale);
	cv::createTrackbar("psize","uwrap",&psize,10);
	cv::namedWindow("mask",0);
	cv::namedWindow("phase",0);
}



void appIdle(void) 
{
	inputHeight = f3.mask.rows;
	inputWidth = f3.mask.cols;

	cv::Mat uv;
	f3.phase0.convertTo(uv,CV_8U,80,127);
	cv::imshow("uwrap",uv);
	cv::Mat ms;
	f3.mask.convertTo(ms,CV_8U,200);
	cv::imshow("mask",ms);
	cv::Mat dm;
	f3.computeDepth(float(zscale-mscale),float(zskew-mscale)).convertTo(dm,CV_8UC1);
	cv::Mat dmj;
	cv::applyColorMap(dm, dmj, cv::COLORMAP_JET);	
	cv::imshow("phase",dmj);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(posX,posY,posZ);
	glRotatef(rotY,0,1,0);
	if ( psize>0 ) 
	{
		drawpoints();
	}
	else 
	{
		drawmesh();
	}						

	int k = cv::waitKey(5);
	if (k== 27) exit(0);
	if (k=='a') rotY += 8.0f;
	if (k=='s') rotY -= 8.0f;
	if (k=='y') posX -= 0.2f;
	if (k=='x') posX += 0.2f;
	if (k=='q') posZ -= 0.2f;
	if (k=='w') posZ += 0.2f;
	if (k=='e') posY -= 0.2f;
	if (k=='r') posY += 0.2f;
	if (k=='1' && step>1) step -= 1;
	if (k=='2') step += 1;
}

