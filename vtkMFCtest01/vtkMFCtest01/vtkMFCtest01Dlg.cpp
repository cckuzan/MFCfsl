
// vtkMFCtest01Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "vtkMFCtest01.h"
#include "vtkMFCtest01Dlg.h"
#include "afxdialogex.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkImageMapToColors.h"

#include "vtkCallbackCommand.h"

#define NUM 1002
#define INUM 2000

#define NIILEN 182 * 218 * 182
#define HundK (100 * 1024)


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)

	
//	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CvtkMFCtest01Dlg 对话框




CvtkMFCtest01Dlg::CvtkMFCtest01Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CvtkMFCtest01Dlg::IDD, pParent)
	, m_SliderValue(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	this->pvtkMFCWindow = NULL;
	this->pvtkDataSetMapper = NULL;
	this->pvtkDataSetReader = NULL;
	this->pvtkRenderer = vtkRenderer::New();

	this->pvtkDataSetMapper = vtkDataSetMapper::New();
	this->pvtkActor = vtkActor::New();
	this->pvtkActor2D = vtkActor2D::New();
	this->pvtkTextMapper = vtkTextMapper::New();


	this->pvtkImageData = vtkImageData::New();
	this->popacityTransferFunction = vtkPiecewiseFunction::New();

	this->pvtkColorTransferFunction = vtkColorTransferFunction::New();
	this->pvtkVolumeProperty = vtkVolumeProperty::New();
	this->pCompositeFunction = vtkVolumeRayCastCompositeFunction::New();
	this->pVolumeMapper = vtkVolumeRayCastMapper::New();
	this->volume = vtkVolume::New();
	this->ren = vtkRenderer::New();
	this->iren = vtkRenderWindowInteractor::New();
	this->renwin = vtkRenderWindow::New();
	this->pvtkreslice = vtkImageReslice::New();
//	this->pviewer = vtkImageViewer::New();

	this->pvtkMFCWindow1 = NULL;
	this->slicex = 0;
	this->ren1 = vtkRenderer::New();
	this->renwin1 = vtkRenderWindow::New();
	this->iren1 = vtkRenderWindowInteractor::New();

	this->clientx = 0;
	this->clienty = 0;
//	this->ptable = vtkWindowLevelLookupTable::New();
//	this->pcolor = vtkImageMapToColors::New();
//	this->pactor = vtkImageActor::New();
	this->prender = vtkRenderer::New();
	this->prenderwindow = vtkRenderWindow::New();
	this->pactor2d = vtkActor2D::New();
	this->pmapper = vtkImageMapper::New();
	this->pfilter = vtkImageDataGeometryFilter::New();
//	this->iren2 = vtkRenderWindowInteractor::New();
//	this->style = vtkInteractorStyleImage::New();

	this->Cpath = "";
	this->ptBorder = CPoint(0,0);
	this->picx = CPoint(0,0);
}

void CvtkMFCtest01Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_Slider);
	DDX_Text(pDX, IDC_EDIT1, m_SliderValue);
}

BEGIN_MESSAGE_MAP(CvtkMFCtest01Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(ID_LOADFILE, &CvtkMFCtest01Dlg::OnClickedLoadfile)
	ON_BN_CLICKED(IDOK, &CvtkMFCtest01Dlg::OnBnClickedOk)
	ON_BN_CLICKED(ID_RESETSCENE, &CvtkMFCtest01Dlg::OnClickedResetscene)
//	ON_WM_HSCROLL()
	ON_NOTIFY(NM_CUSTOMDRAW,IDC_SLIDER1,CvtkMFCtest01Dlg::OnNMCustomdrawSlider1)
//	ON_STN_CLICKED(IDC_PIC_X, &CvtkMFCtest01Dlg::OnStnClickedPicX)
//ON_WM_MOUSEMOVE()
//ON_WM_TIMER()
//ON_WM_LBUTTONDOWN()
//ON_WM_LBUTTONUP()
//ON_WM_LBUTTONUP()
ON_BN_CLICKED(IDCANCEL, &CvtkMFCtest01Dlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CvtkMFCtest01Dlg 消息处理程序

static void handle_double_click(vtkObject *obj,unsigned long,void *,void *)
{
	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::SafeDownCast(obj);
	if(iren && iren->GetRepeatCount())
	{
		AfxMessageBox(_T("Double Click!"));
	}
}

void CvtkMFCtest01Dlg::ExecutePipeline()
{
	if(pvtkDataSetReader)
	{
		
		char nii_x[2],nii_y[2],nii_z[2];
	    unsigned char niix,niiy,niiz;
		unsigned char nii_dimension = 0;
		char niifile_1[HundK];
		this->ifile.open(this->Cpath,ios::binary);
		this->ifile.seekg(40,ios::beg);
		this->ifile >> nii_dimension;
		this->ifile.seekg(1,ios::cur);
		this->ifile.read(nii_x,2);
		this->ifile.read(nii_y,2);
		this->ifile.read(nii_z,2);
	    niix = (unsigned char)nii_x[0];
	    niiy = (unsigned char)nii_y[0];
	    niiz = (unsigned char)nii_z[0];

		this->ifile.seekg(0x160,ios::beg);

		unsigned int FileLenth = niix * niiy * niiz * 2;
		

		if(this->pvtkImageData != NULL)
		{
			this->pvtkImageData->Delete();
			this->pvtkImageData = vtkImageData::New();
			this->pvtkImageData->SetDimensions(niix,niiy,niiz);
			this->pvtkImageData->SetScalarType(VTK_UNSIGNED_CHAR);
			this->pvtkImageData->SetSpacing(1.0,1.0,1.0);
			this->pvtkImageData->SetOrigin(0.0,0.0,0.0);
			this->pvtkImageData->SetScalarTypeToUnsignedShort();
			this->pvtkImageData->SetNumberOfScalarComponents(1);
			this->pvtkImageData->AllocateScalars();
		}

		unsigned short *ptr=(unsigned short *)this->pvtkImageData->GetScalarPointer(); 

//		unsigned short *ptr1 = ptr;
		unsigned int i = FileLenth / HundK;
		unsigned int j = FileLenth % HundK;
		for(int k = 0; k < i;k++)
		{
			this->ifile.read(niifile_1,HundK);
			for(int t = 0;t < HundK; t = t + 2)
			{
				*(ptr++) = niifile_1[t];
			}
		}

		for(int t = 0;t < j; t = t + 2)
		{
			*(ptr++) = niifile_1[t];
		}
		this->slicex = niiz / 2;
		double center[3] = {niix/2,niiy/2,niiz/2};
		 static double axiaX[3] = {1,0,0};
		 static double axiaY[3] = {0,1,0};
		 static double axiaZ[3] = {0,0,1};

		this->pvtkreslice->SetInput((vtkImageData *)this->pvtkImageData);
		this->pvtkreslice->SetOutputDimensionality(2);
		this->pvtkreslice->SetResliceAxesDirectionCosines(axiaX,axiaY,axiaZ);
		this->pvtkreslice->SetResliceAxesOrigin(center);
		this->pvtkreslice->SetInterpolationModeToLinear();
//		this->pviewer->SetInput(this->pvtkreslice->GetOutput());

		



		this->pVolumeMapper->RemoveAllInputs();
		this->pVolumeMapper->Delete();
		this->pVolumeMapper = vtkVolumeRayCastMapper::New();
		
		this->pVolumeMapper->SetVolumeRayCastFunction(this->pCompositeFunction);

		this->pVolumeMapper->SetInput((vtkImageData *)this->pvtkImageData);
		this->ren->RemoveVolume(this->volume);
		this->volume->Delete();
		this->volume = vtkVolume::New();
		this->volume->SetMapper(this->pVolumeMapper);
		this->volume->SetProperty(this->pvtkVolumeProperty);
		this->ren->AddVolume(this->volume);
		

//		this->pviewer->SetupInteractor(this->iren1);  // cc


//		this->pviewer->Render();
//		this->iren->Initialize(); 
		this->pvtkMFCWindow1->GetRenderWindow()->AddRenderer(this->prender);
		this->pvtkMFCWindow1->RedrawWindow();
//		UpdateData(TRUE);
		this->pvtkMFCWindow->GetRenderWindow()->AddRenderer(this->ren);
		this->ifile.close();

	}
	else
	{
		this->pvtkTextMapper->SetInput("Hello World!");
		this->pvtkTextMapper->GetTextProperty()->SetFontSize(24);
		this->pvtkActor2D->SetMapper(this->pvtkTextMapper);

		this->pvtkRenderer->SetBackground(0.0,0.0,0.4);
		this->pvtkRenderer->AddActor(this->pvtkActor2D);
	}
	this->pvtkRenderer->ResetCamera();
}

BOOL CvtkMFCtest01Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


    this->popacityTransferFunction->AddPoint(32,0.0);
	this->popacityTransferFunction->AddPoint(224,1.0);

	this->pvtkColorTransferFunction->AddRGBPoint(0.0, 0.0,0.0,0.0);
	this->pvtkColorTransferFunction->AddRGBPoint(64.0, 1.0,0.0,0.0);
	this->pvtkColorTransferFunction->AddRGBPoint(128.0, 0.0,0.0,1.0);
	this->pvtkColorTransferFunction->AddRGBPoint(192.0, 0.0,1.0,0.0);
	this->pvtkColorTransferFunction->AddRGBPoint(255.0, 0.0,0.2,0.0);

	//this->pvtkColorTransferFunction->AddRGBPoint(16, 0.1,0.1,0.1);
	//this->pvtkColorTransferFunction->AddRGBPoint(32, 0.2,0.2,0.2);
	//this->pvtkColorTransferFunction->AddRGBPoint(64, 0.3,0.3,0.3);
	//this->pvtkColorTransferFunction->AddRGBPoint(96, 0.4,0.4,0.4);
	//this->pvtkColorTransferFunction->AddRGBPoint(128, 0.5,0.5,0.5);
	//this->pvtkColorTransferFunction->AddRGBPoint(160, 0.6,0.6,0.6);
	//this->pvtkColorTransferFunction->AddRGBPoint(192, 0.7,0.7,0.7);
	//this->pvtkColorTransferFunction->AddRGBPoint(220, 0.8,0.8,0.8);
	//this->pvtkColorTransferFunction->AddRGBPoint(240, 1.0,1.0,1.0);
	//this->pvtkColorTransferFunction->AddRGBPoint(255, 1.0,1.0,1.0);


	this->pvtkVolumeProperty->SetColor(pvtkColorTransferFunction);
	this->pvtkVolumeProperty->SetScalarOpacity(popacityTransferFunction);
	this->pvtkVolumeProperty->SetInterpolationTypeToLinear();
	this->pvtkVolumeProperty->SetDiffuse(0.7);
	this->pvtkVolumeProperty->SetAmbient(0.01);
	this->pvtkVolumeProperty->SetSpecular(0.5);
	this->pvtkVolumeProperty->SetSpecularPower(70.0);




	

	this->renwin->AddRenderer(this->ren);
	this->renwin->SetParentId(this->GetDlgItem(IDC_MAIN_WND));
	this->iren->SetRenderWindow(this->renwin);
	this->pVolumeMapper->SetVolumeRayCastFunction(this->pCompositeFunction);

//	this->pviewer->SetColorLevel(10000.0);
//	this->pviewer->
//	this->ptable->SetWindow(311);
//	this->ptable->SetLevel(155);

//	this->pcolor->SetLookupTable(this->ptable);
//	this->pcolor->SetInputConnection(this->pvtkreslice->GetOutputPort());

//	this->pfilter->SetInputConnection(this->pcolor->GetOutputPort());
//	this->pfilter->set
	this->pmapper->SetInput(this->pvtkreslice->GetOutput());
	this->pmapper->SetColorWindow(311);
	this->pmapper->SetColorLevel(155);
//	this->pmapper->SetInputConnection(this->pfilter->GetOutputPort());
//	this->pmapper->Update();
	this->pactor2d->SetMapper(this->pmapper);
	this->pactor2d->SetPosition(100,20);
//	this->pactor->SetInput(this->pcolor->GetOutput());

	this->prender->AddActor(this->pactor2d);

	this->prenderwindow->AddRenderer(this->prender);
	this->prenderwindow->SetParentId(this->GetDlgItem(IDC_PIC_X));
//	this->prenderwindow->SetSize(300,300);
//	this->iren2->SetInteractorStyle(style);
//	this->prenderwindow->SetInteractor(this->iren2);
//	this->iren2->SetRenderWindow(this->prenderwindow);
//	this->prenderwindow->HideCursor();



//	this->pviewer->GetRenderWindow()->SetSize(1000,1000);
//	this->pviewer->SetInputConnection(this->pvtkreslice);

//	this->pviewer->SetColorWindow(311);
//	this->pviewer->SetColorLevel(11);
//	this->pviewer->SetPosition(600,800);

//	this->pviewer->set
//	this->pviewer->SetParentId(this->GetDlgItem(IDC_PIC_X));
	this->pvtkMFCWindow1 = new vtkMFCWindow(this->GetDlgItem(IDC_PIC_X));


	this->pvtkMFCWindow = new vtkMFCWindow(this->GetDlgItem(IDC_MAIN_WND));
	vtkCallbackCommand *callback = vtkCallbackCommand::New();
	callback->SetCallback(handle_double_click);
	this->pvtkMFCWindow->GetInteractor()->AddObserver(vtkCommand::LeftButtonPressEvent,callback,1.0);
	callback->Delete();

	CRect cRectVTK;
	this->pvtkMFCWindow->GetClientRect(&cRectVTK);

	CRect cRectClient;
	GetClientRect(&cRectClient);
	this->clientx = cRectClient.right;
	this->clienty = cRectClient.bottom;

	this->ptBorder.x = cRectClient.Width() - cRectVTK.Width();
	this->ptBorder.y = cRectClient.Height() - cRectVTK.Height();

	CRect cPicx;
	this->pvtkMFCWindow1->GetClientRect(&cPicx);

	this->picx.x = cRectClient.Width() - cPicx.Width();
	this->picx.y = cRectClient.Height() - cPicx.Height();

	m_Slider.SetRangeMin(1, false);
	m_Slider.SetRangeMax(182, false);
	m_Slider.SetTicFreq(1);
//	m_SliderValue = "1";
	UpdateData(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CvtkMFCtest01Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CvtkMFCtest01Dlg::OnPaint()
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
HCURSOR CvtkMFCtest01Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CvtkMFCtest01Dlg::OnDestroy()
{
	if(this->pvtkDataSetReader)  this->pvtkDataSetReader->Delete();
	if(this->pvtkRenderer)  this->pvtkRenderer->Delete();

	if(this->pvtkDataSetMapper)  this->pvtkDataSetMapper->Delete();
	if(this->pvtkActor)   this->pvtkActor->Delete();
	if(this->pvtkActor2D)  this->pvtkActor2D->Delete();

	if(this->pvtkTextMapper)  this->pvtkTextMapper->Delete();

	if(this->pvtkMFCWindow)
	{
		delete this->pvtkMFCWindow;
	}

	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}


void CvtkMFCtest01Dlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if(::IsWindow(this->GetSafeHwnd()))
	{
		int tmpcx,tmpcy;
		int chcx,chcy;
		tmpcx = cx;
		tmpcy = cy;
		CRect cRectVTK;
		this->GetDlgItem(IDC_MAIN_WND)->GetWindowRect(&cRectVTK);
		ScreenToClient(cRectVTK);

//		static CRect cRectClient;
//		GetClientRect(&cRectClient);
//		this->pvtkMFCWindow->GetClientRect(&cRectVTK);
		CRect cRectPicx;
		this->GetDlgItem(IDC_PIC_X)->GetWindowRect(&cRectPicx);
		chcx = cx - this->clientx;
		chcy = cy - this->clienty;
		this->clientx = cx;
		this->clienty = cy;
		if(this->pvtkMFCWindow)
		{
//			tmpcx -= ptBorder.x;
//			tmpcy -= ptBorder.y;
			this->GetDlgItem(IDC_MAIN_WND)->SetWindowPos(NULL,cRectVTK.left,cRectVTK.top,cRectVTK.Width() + chcx / 2,cRectVTK.Height() + chcy /2,
				SWP_NOACTIVATE | SWP_NOZORDER );
			this->pvtkMFCWindow->SetWindowPos(NULL,0,0,cRectVTK.Width() + chcx / 2,cRectVTK.Height() + chcy /2,
				SWP_NOACTIVATE | SWP_NOZORDER );
		}
//		tmpcx = cx;
//		tmpcy = cy;
//		tmpcx = tmpcx + 
		if(this->pvtkMFCWindow1)
		{
			cx -= this->picx.x;
			cy -= this->picx.y;
			this->GetDlgItem(IDC_PIC_X)->SetWindowPos(NULL,cRectVTK.left + cRectVTK.Width() + chcx / 2,cRectVTK.top,
				cRectPicx.Width() + (chcx - chcx / 2) ,cRectPicx.Height() + (chcy - chcy / 2),
				SWP_NOACTIVATE | SWP_NOZORDER );
			this->pvtkMFCWindow1->SetWindowPos(NULL,0,0,cRectPicx.Width() + (chcx - chcx / 2),cRectPicx.Height() + (chcy - chcy / 2),
				SWP_NOACTIVATE | SWP_NOZORDER );
		}

	}
}


void CvtkMFCtest01Dlg::OnClickedLoadfile()
{
	// TODO: 在此添加控件通知处理程序代码
	static char BASED_CODE szFilter[] = "VTK Files(*.nii)|*.nii|All Files(*.*)|*.*||";
	CFileDialog cFileDialog(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);

	if(cFileDialog.DoModal() == IDOK)
	{
		this->pvtkMFCWindow->GetRenderWindow()->RemoveRenderer(this->ren);
		this->pvtkRenderer->RemoveActor(this->pvtkActor);
		this->pvtkRenderer->RemoveActor(this->pvtkActor2D);

		if(!this->pvtkDataSetReader)
			this->pvtkDataSetReader = vtkDataSetReader::New();
		this->pvtkDataSetReader->SetFileName(cFileDialog.GetPathName());

		this->Cpath = cFileDialog.GetPathName();
		ExecutePipeline();

		if(this->pvtkMFCWindow)
			this->pvtkMFCWindow->RedrawWindow();
	}
}


void CvtkMFCtest01Dlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
//	this->pvtkMFCWindow->GetRenderWindow()->RemoveRenderer(this->ren);
	CDialogEx::OnOK();
}


void CvtkMFCtest01Dlg::OnClickedResetscene()
{
	// TODO: 在此添加控件通知处理程序代码
	this->pvtkRenderer->RemoveActor(this->pvtkActor);
	this->pvtkRenderer->RemoveActor(this->pvtkActor2D);

//	this->pVolumeMapper->RemoveAllInputs();

	if(this->pvtkDataSetReader)
	{
		this->pvtkDataSetReader->Delete();
		this->pvtkDataSetReader = NULL;
	}

	ExecutePipeline();

	if(this->pvtkMFCWindow)
		this->pvtkMFCWindow->RedrawWindow();
}


//void CvtkMFCtest01Dlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	if(nSBCode == SB_THUMBPOSITION) {
//			m_SliderValue.Format(_T("%d%%"), nPos);//m_SliderValue.Format
//		
//			
//	
//		this->pviewer->Render();
//		this->pviewer->GetCommand(vtkCommand::LeftButtonPressEvent);
//		this->pviewer->InvokeEvent(vtkCommand::LeftButtonPressEvent,NULL);
//		PostMessage(WM_COMMAND,MAKEWPARAM(IDC_SLIDER1, STN_CLICKED),NULL);
//		GetDlgItem(IDC_SLIDER1)->Invalidate();
//		KillTimer(1);
//	UpdateData(false);
//	}
//
//	else {
//	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
//	}
//	KillTimer(1);
//	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
//}


//void CvtkMFCtest01Dlg::OnStnClickedPicX()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	AfxMessageBox("test");
//}
//static UINT_PTR timer1 = 0;
void CvtkMFCtest01Dlg::OnNMCustomdrawSlider1(NMHDR* pNMHDR,LRESULT* pResult)
{
//	int nPos = m_Slider.GetPos();
	int nPos = m_Slider.GetPos();
	m_SliderValue.Format(_T("%d%%"), nPos);
	UpdateData(false);
	this->slicex = nPos;
		double center[3] = {91,108,this->slicex};
		 static double axiaX[3] = {1,0,0};
		 static double axiaY[3] = {0,1,0};
		 static double axiaZ[3] = {0,0,1};
		 this->pvtkreslice->SetResliceAxesDirectionCosines(axiaX,axiaY,axiaZ);
		this->pvtkreslice->SetResliceAxesOrigin(center);
		this->pvtkMFCWindow1->RedrawWindow();



//	m_SliderValue.Format(_T("%d%%"), nPos);
}

//void CAboutDlg::OnMouseMove(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	int nPos = m_Slider.GetPos();
//	m_SliderValue.Format(_T("%d%%"), nPos);
//	CDialogEx::OnMouseMove(nFlags, point);
//}


//void CvtkMFCtest01Dlg::OnMouseMove(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	int nPos = m_Slider.GetPos();
//	m_SliderValue.Format(_T("%d%%"), nPos);
//	UpdateData(false);
//	CDialogEx::OnMouseMove(nFlags, point);
//}



//void CvtkMFCtest01Dlg::OnTimer(UINT_PTR nIDEvent)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	int nPos = m_Slider.GetPos();
//	m_SliderValue.Format(_T("%d%%"), nPos);
//	UpdateData(false);
//	this->slicex = nPos;
//		double center[3] = {91,108,this->slicex};
//		 static double axiaX[3] = {1,0,0};
//		 static double axiaY[3] = {0,1,0};
//		 static double axiaZ[3] = {0,0,1};
//		 this->pvtkreslice->SetResliceAxesDirectionCosines(axiaX,axiaY,axiaZ);
//		this->pvtkreslice->SetResliceAxesOrigin(center);
//		this->pvtkMFCWindow1->RedrawWindow();	
//	CDialogEx::OnTimer(nIDEvent);
//}


//void CvtkMFCtest01Dlg::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	SetTimer(1,50,NULL);
//	CDialogEx::OnLButtonDown(nFlags, point);
//}


//void CvtkMFCtest01Dlg::OnLButtonUp(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	KillTimer(1);
//	CDialogEx::OnLButtonUp(nFlags, point);
//}


//void CvtkMFCtest01Dlg::OnLButtonUp(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	if(	timer1 == 1)
//	{
//		KillTimer(1);
//		timer1 = 0;
//	}
//	CDialogEx::OnLButtonUp(nFlags, point);
//}


void CvtkMFCtest01Dlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
//	CDialogEx::OnCancel();

}
