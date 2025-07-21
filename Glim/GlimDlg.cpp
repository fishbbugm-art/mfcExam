
// GlimDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Glim.h"
#include "GlimDlg.h"
#include "afxdialogex.h"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#include <random>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CGlimDlg 대화 상자



CGlimDlg::CGlimDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GLIM_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGlimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_COORDS, m_staticCoords);
	DDX_Control(pDX, IDC_EDIT_RADIUS, m_editRadius);
	DDX_Control(pDX, IDC_EDIT_THICKNESS, m_editThickness);
}

BEGIN_MESSAGE_MAP(CGlimDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON_RANDOM_MOVE, &CGlimDlg::OnBnClickedButtonRandomMove)
	ON_MESSAGE(WM_USER + 1, &CGlimDlg::OnRandomMoveUpdate)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CGlimDlg::OnBnClickedButtonReset)
END_MESSAGE_MAP()


// CGlimDlg 메시지 처리기

BOOL CGlimDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	m_editRadius.SetWindowTextW(_T("10"));
	m_editThickness.SetWindowTextW(_T("2"));

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CGlimDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CGlimDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);

		return;
	}

	CPaintDC dc(this);

	Gdiplus::Graphics graphics(dc.GetSafeHdc());

	// 클릭한 좌표마다 빨간 원을 그림
	for (const auto& pt : m_clickPoints)
	{
		DrawCircleOnDC(&dc, pt.x, pt.y, m_pointRadius, RGB(255, 0, 0), 1);
	}

	if (m_clickPoints.size() == 3)
	{
		PointF center;
		float radius;

		if (CalcCircleFrom3Points(m_clickPoints[0], m_clickPoints[1], m_clickPoints[2], center, radius))
		{
			DrawCircleOnDC(&dc, (int)center.X, (int)center.Y, (int)radius, RGB(0, 0, 255), m_circleThickness);
		}
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CGlimDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGlimDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
	m_threadRunning = false;
	if (m_randomMoveThread.joinable())
	{
		m_randomMoveThread.join();
	}
}

void CGlimDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	ReadUserSettings();

	if (m_clickPoints.size() == 3)
	{
		// 클릭한 점 중 근처에 있는 점 찾기 (반지름 내)
		for (int i = 0; i < 3; i++)
		{
			if (abs(point.x - m_clickPoints[i].x) <= m_pointRadius && abs(point.y - m_clickPoints[i].y) <= m_pointRadius)
			{
				m_isDragging = true;
				m_draggingIndex = i;
				SetCapture();  // 마우스 캡처
				break;
			}
		}
	}
	else if (m_clickPoints.size() < 3)
	{
		m_clickPoints.push_back(point);
		UpdateCoordsDisplay();
		Invalidate();  // 화면 다시 그리기
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}

bool CGlimDlg::CalcCircleFrom3Points(CPoint p1, CPoint p2, CPoint p3, PointF& center, float& radius)
{
	// 세 점을 이용해 외접원의 중심과 반지름 계산 (해석기하학 공식)
	float x1 = p1.x, y1 = p1.y;
	float x2 = p2.x, y2 = p2.y;
	float x3 = p3.x, y3 = p3.y;

	float a = x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2;
	if (a == 0) return false; // 세 점이 일직선일 경우

	float b = (x1 * x1 + y1 * y1) * (y3 - y2)
		+ (x2 * x2 + y2 * y2) * (y1 - y3)
		+ (x3 * x3 + y3 * y3) * (y2 - y1);

	float c = (x1 * x1 + y1 * y1) * (x2 - x3)
		+ (x2 * x2 + y2 * y2) * (x3 - x1)
		+ (x3 * x3 + y3 * y3) * (x1 - x2);

	float d = (x1 * x1 + y1 * y1) * (x3 * y2 - x2 * y3)
		+ (x2 * x2 + y2 * y2) * (x1 * y3 - x3 * y1)
		+ (x3 * x3 + y3 * y3) * (x2 * y1 - x1 * y2);

	center.X = -b / (2 * a);
	center.Y = -c / (2 * a);
	radius = sqrtf((center.X - x1) * (center.X - x1) + (center.Y - y1) * (center.Y - y1));

	return true;
}

void CGlimDlg::UpdateCoordsDisplay()
{
	CString str;
	str.Format(_T("좌표: "));
	for (size_t i = 0; i < m_clickPoints.size(); i++)
	{
		CString temp;
		temp.Format(_T("(%d, %d) "), m_clickPoints[i].x, m_clickPoints[i].y);
		str += temp;
	}
	m_staticCoords.SetWindowTextW(str);
}
void CGlimDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (m_isDragging)
	{
		m_isDragging = false;
		m_draggingIndex = -1;
		ReleaseCapture();  // 마우스 캡처 해제
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CGlimDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (m_isDragging && m_draggingIndex != -1)
	{
		m_clickPoints[m_draggingIndex] = point;
		UpdateCoordsDisplay();
		Invalidate();  // 실시간 다시 그리기
	}

	CDialogEx::OnMouseMove(nFlags, point);
}

void CGlimDlg::OnBnClickedButtonRandomMove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_threadRunning)
	{
		// 이미 실행 중이면 무시 또는 메시지 박스 출력
		AfxMessageBox(_T("랜덤 이동이 이미 실행 중입니다."));
		return;
	}

	m_threadRunning = true;
	m_randomMoveThread = std::thread(&CGlimDlg::RandomMoveLoop, this);
	m_randomMoveThread.detach(); // 백그라운드 실행, 종료 대기하지 않음
}

void CGlimDlg::RandomMoveLoop()
{
	std::random_device rd;
	std::mt19937 gen(rd());

	// 클라이언트 영역 크기 얻기
	CRect rect;
	GetClientRect(&rect);

	// 이동 가능한 좌표 범위 (원 크기 고려)
	int minX = m_pointRadius;
	int maxX = rect.Width() - m_pointRadius;
	int minY = m_pointRadius;
	int maxY = rect.Height() - m_pointRadius;

	std::uniform_int_distribution<> distX(minX, maxX);
	std::uniform_int_distribution<> distY(minY, maxY);

	for (int i = 0; i < 10 && m_threadRunning; i++)
	{
		// 점 위치 모두 랜덤 이동
		for (auto& pt : m_clickPoints)
		{
			pt.x = distX(gen);
			pt.y = distY(gen);
		}

		// UI 갱신은 메인 쓰레드에서 실행해야 하므로 PostMessage 사용
		PostMessage(WM_USER + 1);

		std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 2Hz = 500ms 간격
	}

	m_threadRunning = false;  // 종료 표시
}

LRESULT CGlimDlg::OnRandomMoveUpdate(WPARAM wParam, LPARAM lParam)
{
	UpdateCoordsDisplay();
	Invalidate(); // 다시 그리기
	return 0;
}

void CGlimDlg::ReadUserSettings()
{
	CString strRadius, strThickness;
	m_editRadius.GetWindowTextW(strRadius);
	m_editThickness.GetWindowTextW(strThickness);

	int radius = _ttoi(strRadius);
	int thickness = _ttoi(strThickness);

	// 유효성 검사
	if (radius > 0) m_pointRadius = radius;
	if (thickness > 0) m_circleThickness = thickness;
}
void CGlimDlg::OnBnClickedButtonReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_clickPoints.clear();
	m_isDragging = false;
	m_draggingIndex = -1;
	UpdateCoordsDisplay();
	Invalidate();  // 화면 갱신
}

void CGlimDlg::DrawCircleOnDC(CDC* pDC, int centerX, int centerY, int radius, COLORREF color, int thickness)
{
	for (int t = 0; t < thickness; t++)
	{
		int r = radius - t;
		if (r <= 0) break;

		int x = 0;
		int y = r;
		int d = 3 - 2 * r;

		auto draw8Points = [&](int cx, int cy, int x, int y)
			{
				pDC->SetPixel(cx + x, cy + y, color);
				pDC->SetPixel(cx - x, cy + y, color);
				pDC->SetPixel(cx + x, cy - y, color);
				pDC->SetPixel(cx - x, cy - y, color);
				pDC->SetPixel(cx + y, cy + x, color);
				pDC->SetPixel(cx - y, cy + x, color);
				pDC->SetPixel(cx + y, cy - x, color);
				pDC->SetPixel(cx - y, cy - x, color);
			};

		while (x <= y)
		{
			draw8Points(centerX, centerY, x, y);
			if (d < 0)
			{
				d += 4 * x + 6;
			}
			else
			{
				d += 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	}
}
