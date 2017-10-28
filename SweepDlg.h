#pragma once


// CSweepDlg dialog

class CSweepDlg : public CDialog
{
	DECLARE_DYNAMIC(CSweepDlg)

public:
	CSweepDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSweepDlg();

// Dialog Data
	enum { IDD = IDD_SWEEP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	GLdouble m_vx;
	GLdouble m_vy;
	GLdouble m_vz;
	GLdouble m_dLen;
};
