#pragma once
#include <wtypes.h>
#include "opencv2/opencv.hpp"

/* CvvImage class definition */
class CvvImage
{
public:
    CvvImage();
    ~CvvImage();


    /* Load image from specified file */
    bool  Load( const char* filename);

    /* Load rectangle from the file */
    bool  LoadRect( const char* filename, cv::Rect r );

#if defined WIN32 || defined _WIN32
    bool  LoadRect( const char* filename, RECT r )
    {
        return LoadRect( filename, cv::Rect( r.left, r.top, r.right - r.left, r.bottom - r.top ));
    }
#endif

    /* Save entire image to specified file. */
    bool  Save( const char* filename );

    cv::Mat* GetImage() { return &m_img; };
    void  Destroy(void);

    /* width and height of ROI */
    int Width() { return m_img.cols; };
    int Height() { return m_img.rows;};
    int Bpp() {
		int depth = 8;
		switch (m_img.depth())
		{
		case CV_8UC1:
			depth = 8;
		default:
			break;
		}
		return (depth & 255)*m_img.channels();
	};

    /* draw to highgui window */
    void  Show( const char* window );

#if defined WIN32 || defined _WIN32
    /* draw part of image to the specified DC */
    void  Show( HDC dc, int x, int y, int width, int height,
                        int from_x = 0, int from_y = 0 );
    /* draw the current image ROI to the specified rectangle of the destination DC */
    void  DrawToHDC( HDC hDCDst, RECT* pDstRect, BOOL bKeepRatio=TRUE);
#endif

protected:

	cv::Mat m_img;
};