#include "CvvImage.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CV_INLINE RECT NormalizeRect( RECT r );
CV_INLINE RECT NormalizeRect( RECT r )
{
    int t;

    if( r.left > r.right )
    {
        t = r.left;
        r.left = r.right;
        r.right = t;
    }

    if( r.top > r.bottom )
    {
        t = r.top;
        r.top = r.bottom;
        r.bottom = t;
    }

    return r;
}

CV_INLINE cv::Rect RectToCvRect( RECT sr );
CV_INLINE cv::Rect RectToCvRect( RECT sr )
{
    sr = NormalizeRect( sr );
    return cv::Rect( sr.left, sr.top, sr.right - sr.left, sr.bottom - sr.top );
}

CV_INLINE RECT CvRectToRect( cv::Rect sr );
CV_INLINE RECT CvRectToRect( cv::Rect sr )
{
    RECT dr;
    dr.left = sr.x;
    dr.top = sr.y;
    dr.right = sr.x + sr.width;
    dr.bottom = sr.y + sr.height;

    return dr;
}

void  FillBitmapInfoEx( BITMAPINFO* bmi, int width, int height, int bpp, int origin )
{
    assert( bmi && width >= 0 && height >= 0 && (bpp == 8 || bpp == 24 || bpp == 32));

    BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);

    memset( bmih, 0, sizeof(*bmih));
    bmih->biSize = sizeof(BITMAPINFOHEADER);
    bmih->biWidth = width;
    bmih->biHeight = origin ? abs(height) : -abs(height);
    bmih->biPlanes = 1;
    bmih->biBitCount = (unsigned short)bpp;
    bmih->biCompression = BI_RGB;

    if( bpp == 8 )
    {
        RGBQUAD* palette = bmi->bmiColors;
        int i;
        for( i = 0; i < 256; i++ )
        {
            palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
            palette[i].rgbReserved = 0;
        }
    }
}

CvvImage::CvvImage()
{
    m_img = 0;
}

void CvvImage::Destroy()
{

}

CvvImage::~CvvImage()
{
    Destroy();
}

bool  CvvImage::Load( const char* filename)
{
	m_img = cv::imread(filename);
	if (m_img.empty())
	{
		return false;
	}

	if (m_img.cols % 4 != 0)
	{
		int w = (m_img.cols / 4) * 4;
		cv::resize(m_img, m_img, cv::Size(w, m_img.rows));
	}

    return true;
}


bool  CvvImage::LoadRect( const char* filename, cv::Rect r )
{
    if( r.width < 0 || r.height < 0 ) return false;

    cv::Mat img = cv::imread(filename);
    if(img.empty())
        return false;

    if( r.width == 0 || r.height == 0 )
    {
        r.width = img.cols;
        r.height = img.rows;
        r.x = r.y = 0;
    }

    if( r.x > img.cols || r.y > img.rows ||
        r.x + r.width < 0 || r.y + r.height < 0 )
    {
        return false;
    }

    /* truncate r to source image */
    if( r.x < 0 )
    {
        r.width += r.x;
        r.x = 0;
    }
    if( r.y < 0 )
    {
        r.height += r.y;
        r.y = 0;
    }

    if( r.x + r.width > img.cols )
        r.width = img.cols - r.x;

    if( r.y + r.height > img.rows )
        r.height = img.rows - r.y;

	img(r).copyTo(m_img);

    return true;
}


bool  CvvImage::Save( const char* filename )
{
    if(m_img.empty())
        return false;
    cv::imwrite(filename, m_img);
    return true;
}


void  CvvImage::Show( const char* window )
{
    if(!m_img.empty())
        cv::imshow(window, m_img);
}

void  CvvImage::Show( HDC dc, int x, int y, int w, int h, int from_x, int from_y )
{
    if(!m_img.empty() && m_img.depth() == CV_8UC1)
    {
        uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];
        BITMAPINFO* bmi = (BITMAPINFO*)buffer;
        int bmp_w = m_img.cols, bmp_h = m_img.rows;

        FillBitmapInfoEx( bmi, bmp_w, bmp_h, Bpp(), 0);

        from_x = MIN( MAX( from_x, 0 ), bmp_w - 1 );
        from_y = MIN( MAX( from_y, 0 ), bmp_h - 1 );

        int sw = MAX( MIN( bmp_w - from_x, w ), 0 );
        int sh = MAX( MIN( bmp_h - from_y, h ), 0 );

        SetDIBitsToDevice(
              dc, x, y, sw, sh, from_x, from_y, from_y, sh,
              m_img.data + from_y * m_img.step,
              bmi, DIB_RGB_COLORS );
    }
}


void  CvvImage::DrawToHDC(HDC hDCDst, RECT* pDstRect)
{
    if( pDstRect && !m_img.empty())
    {
        uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];
        BITMAPINFO* bmi = (BITMAPINFO*)buffer;
        int bmp_w = m_img.cols, bmp_h = m_img.rows;

		
		cv::Size sz;
		cv::Point pt;
		m_img.locateROI(sz, pt);
		cv::Rect roi = cv::Rect(pt.x, pt.y, sz.width, sz.height);
        cv::Rect dst = RectToCvRect( *pDstRect );

        if( roi.width == dst.width && roi.height == dst.height )
        {
            Show( hDCDst, dst.x, dst.y, dst.width, dst.height, roi.x, roi.y );
            return;
        }
    
        if( roi.width > dst.width )
        {
            SetStretchBltMode(
                   hDCDst,           // handle to device context
                   HALFTONE );
        }
        else
        {
            SetStretchBltMode(
                   hDCDst,           // handle to device context
                   COLORONCOLOR );
        }
	
        FillBitmapInfoEx( bmi, bmp_w, bmp_h, Bpp(), 0);

		float fx = (float)dst.width / roi.width;
		float fy = (float)dst.height / roi.height;

		float f = MIN(fx, fy);
		int offsetx = (dst.width - f * roi.width) / 2;
		int offsety = (dst.height - f * roi.height) / 2;

        ::StretchDIBits(
            hDCDst,
            dst.x + offsetx,
			dst.y + offsety,
			//dst.width - offsetx * 2,
			f * roi.width,
			//dst.height - offsety * 2,
			f * roi.height,
            roi.x, roi.y, roi.width, roi.height,
            m_img.data, bmi, DIB_RGB_COLORS, SRCCOPY );
    }
}