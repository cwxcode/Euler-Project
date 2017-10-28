// SweepDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Euler.h"
#include "SweepDlg.h"
#include ".\sweepdlg.h"


// CSweepDlg dialog

IMPLEMENT_DYNAMIC(CSweepDlg, CDialog)
CSweepDlg::CSweepDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSweepDlg::IDD, pParent)
	, m_vx(0)
	, m_vy(0)
	, m_vz(0)
	, m_dLen(0)
{
}

CSweepDlg::~CSweepDlg()
{
}

void CSweepDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_X, m_vx);
	DDX_Text(pDX, IDC_EDIT_Y, m_vy);
	DDX_Text(pDX, IDC_EDIT_Z, m_vz);
	DDX_Text(pDX, IDC_EDIT_LEN, m_dLen);
}


BEGIN_MESSAGE_MAP(CSweepDlg, CDialog)
END_MESSAGE_MAP()


// CSweepDlg message handlers