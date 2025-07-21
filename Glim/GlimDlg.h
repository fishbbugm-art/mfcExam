
// GlimDlg.h: 헤더 파일
//

#pragma once
#include <vector>
#include <gdiplus.h>
#include <thread>
#include <atomic>

using namespace Gdiplus;

// CGlimDlg 대화 상자
class CGlimDlg : public CDialogEx
{
// 생성입니다.
public:
	CGlimDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GLIM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	ULONG_PTR m_gdiplusToken;
	afx_msg void OnDestroy();
	std::vector<CPoint> m_clickPoints;
	int m_pointRadius = 20;		 // 클릭 원 반지름 (임시 기본값)
	int m_circleThickness = 2;   // 정원 두께 (임시 기본값)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	bool CalcCircleFrom3Points(CPoint p1, CPoint p2, CPoint p3, PointF& center, float& radius);
	CStatic m_staticCoords;
	void UpdateCoordsDisplay();
	bool m_isDragging = false;   // 드래그 상태 여부
	int m_draggingIndex = -1;    // 드래그 중인 점 인덱스
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonRandomMove();
	std::thread m_randomMoveThread;      // 랜덤 이동 쓰레드
	std::atomic<bool> m_threadRunning;   // 쓰레드 실행 상태 체크
	afx_msg LRESULT OnRandomMoveUpdate(WPARAM wParam, LPARAM lParam);
	void RandomMoveLoop();
	CEdit m_editRadius;
	CEdit m_editThickness;
	afx_msg void ReadUserSettings();
	afx_msg void OnBnClickedButtonReset();
	void DrawCircleOnDC(CDC* pDC, int centerX, int centerY, int radius, COLORREF color, int thickness = 1);
};
