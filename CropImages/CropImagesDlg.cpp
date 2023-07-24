
// CropImagesDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "CropImages.h"
#include "CropImagesDlg.h"
#include "afxdialogex.h"
#include "CvvImage.h"
#include "opencv2/freetype.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCropImagesDlg 对话框



CCropImagesDlg::CCropImagesDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CROPIMAGES_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCropImagesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LEFT_TOP_X, m_edit_left_top_x);
	DDX_Control(pDX, IDC_EDIT_LEFT_TOP_Y, m_edit_left_top_y);
	DDX_Control(pDX, IDC_EDIT_RIGHT_BOTTOM_X, m_edit_right_bottom_x);
	DDX_Control(pDX, IDC_EDIT_RIGHT_BOTTOM_Y, m_edit_right_bottom_y);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_static_image);
}

BEGIN_MESSAGE_MAP(CCropImagesDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CCropImagesDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CCropImagesDlg::OnBnClickedButtonOpen)
END_MESSAGE_MAP()


// CCropImagesDlg 消息处理程序

BOOL CCropImagesDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCropImagesDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCropImagesDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCropImagesDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCropImagesDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
	cv::RNG rng(time(NULL));
	cv::Mat im = cv::imread(m_strImagePath);
	if (im.cols % 4 != 0)
	{
		int w = (im.cols / 4) * 4;
		cv::resize(im, im, cv::Size(w, im.rows));
	}

	if (im.empty())
	{
		AfxMessageBox("请先打开图片~!");
		return;
	}

	cv::Point lt;
	CString str;
	m_edit_left_top_x.GetWindowTextA(str);
	lt.x = atoi(((std::string)str.GetBuffer()).c_str());
	if (str.IsEmpty() || lt.x < 0 || lt.x > im.cols)
	{
		AfxMessageBox("左上角X坐标不正确~!");
		return;
	}

	m_edit_left_top_y.GetWindowTextA(str);
	lt.y = atoi(((std::string)str.GetBuffer()).c_str());
	if (str.IsEmpty() || lt.y < 0 || lt.y > im.cols)
	{
		AfxMessageBox("左上角Y坐标不正确~!");
		return;
	}

	m_edit_right_bottom_x.GetWindowTextA(str);
	cv::Point rb;
	rb.x = atoi(((std::string)str.GetBuffer()).c_str());
	if (str.IsEmpty() || rb.x < 0 || rb.y > im.rows)
	{
		AfxMessageBox("右下角坐标X不正确~!");
		return;
	}

	m_edit_right_bottom_y.GetWindowTextA(str);
	rb.y = atoi(((std::string)str.GetBuffer()).c_str());
	if (str.IsEmpty() || rb.y < 0 || rb.y > im.rows)
	{
		AfxMessageBox("右下角坐标Y不正确~!");
		return;
	}

	if (lt.x >= rb.x)
	{
		AfxMessageBox("左上角X坐标>=右下角坐标X~!");
		return;
	}

	if (lt.y >= rb.y)
	{
		AfxMessageBox("左上角Y坐标>=右下角坐标Y~!");
		return;
	}

	cv::imwrite("snap.jpg", im(cv::Rect(lt.x, lt.y, rb.x - lt.x + 1, rb.y - lt.y + 1)));
	cv::rectangle(im, lt, rb, cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 2);
	im.copyTo(*m_image.GetImage());
	DrawPic2HDC(m_image, IDC_STATIC_IMAGE);
}


void CCropImagesDlg::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strFile = _T("");

	CFileDialog    dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Describe Files (*.jpg)|*.jpg|All Files (*.*)|*.*||"), NULL);

	if (dlgFile.DoModal())
	{
		strFile = dlgFile.GetPathName();
		m_strImagePath = strFile.GetBuffer();
		DrawPic2HDC(strFile.GetBuffer(), IDC_STATIC_IMAGE);
	}
}


void CCropImagesDlg::DrawPic2HDC(std::string strFile, UINT ID)
{
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();

	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	GetDlgItem(ID)->Invalidate(TRUE);
	GetDlgItem(ID)->UpdateWindow();
	
	m_image.Load(strFile.c_str());
	
	//cv::Ptr<cv::freetype::FreeType2> freeType;
//	freeType = cv::freetype::createFreeType2();
	//freeType->loadFontData("simsun.ttc", 0);	// 加载字体文件，宋体

	// 在图片中显示带中文的文字
	//freeType->putText(*cimg.GetImage(), "测试 hello world!", cv::Point(10, 30), 20, cv::Scalar(255, 0, 0), 1, 8, true);

	m_image.DrawToHDC(hDC, &rect);
	ReleaseDC(pDC);
}

void CCropImagesDlg::DrawPic2HDC(CvvImage img, UINT ID)
{
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();

	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);

	//m_image.Load(strFile.c_str());

	//cv::Ptr<cv::freetype::FreeType2> freeType;
//	freeType = cv::freetype::createFreeType2();
	//freeType->loadFontData("simsun.ttc", 0);	// 加载字体文件，宋体

	// 在图片中显示带中文的文字
	//freeType->putText(*cimg.GetImage(), "测试 hello world!", cv::Point(10, 30), 20, cv::Scalar(255, 0, 0), 1, 8, true);

	m_image.DrawToHDC(hDC, &rect);
	ReleaseDC(pDC);
}
