
// CropImagesDlg.h: 头文件
//

#pragma once
#include<iostream>
#include "CvvImage.h"
// CCropImagesDlg 对话框
class CCropImagesDlg : public CDialogEx
{
// 构造
public:
	CCropImagesDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CROPIMAGES_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonOpen();
public:
	void DrawPic2HDC(std::string strFile, UINT ID);
	void DrawPic2HDC(CvvImage img, UINT ID);
	CEdit m_edit_left_top_x;
	CEdit m_edit_left_top_y;
	CEdit m_edit_right_bottom_x;
	CEdit m_edit_right_bottom_y;
	std::string m_strImagePath;
	CvvImage m_image;
	CStatic m_static_image;
};
