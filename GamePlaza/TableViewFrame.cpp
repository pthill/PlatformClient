#include "Stdafx.h"
#include "Resource.h"
#include "GamePlaza.h"
#include "GlobalUnits.h"
#include "TableViewFrame.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////////////

//���и�ʽ
#define	NT_LEFT						0x01								//�����
#define	NT_RIGHT					0x02								//�Ҷ���

//�����ʽ
#define	NT_TOP						0x10								//�϶���
#define	NT_BOTTOM					0x20								//�¶���

//��������
#define WHELL_TIMES					2									//���ֱ���
#define SCROLL_POS_LINE				10									//��������

//��ʱ������
#define IDI_TICK_TIMER				1									//���涨ʱ
#define IDI_UPDATE_TIME				100									//����ʱ��
#define IDI_WAIT_ANIMATION			101									//�ȴ�����

//ʱ�䶨��
#define TIME_UPDATE_TIME			1000								//���¼��
#define TIME_WAIT_ANIMATION			120									//����ʱ��

//λ��ģʽ
#define MATCH_PM_ABSOLUTE			0									//����λ��
#define MATCH_PM_RELATIVE			1									//���λ�� 

//////////////////////////////////////////////////////////////////////////////////
//��̬����
CTableResource * CTableResource::m_pTableResource=NULL;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CTableViewFrame, CWnd)

	//ϵͳ��Ϣ
	ON_WM_SIZE()	
	ON_WM_PAINT()	
	ON_WM_TIMER()
	ON_WM_VSCROLL()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()

	//�Զ���Ϣ
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_PLATFORM_EVENT, OnMessagePlatformEvent)

	ON_WM_NCDESTROY()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CMatchViewFrame, CFGuiWnd)

	//ϵͳ��Ϣ
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_NCDESTROY()
	
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CTableResource::CTableResource()
{
	//���Ա���
	m_bShowUser=true;
	m_bShowChair=true;
	m_bShowTable=true;
	m_bShowFocus=true;
	m_bShowEnter=false;
	m_bCustomUser=false;
	m_bShowTableID=true;
	m_bRenderImage=true;
	m_bShowCellScore=false;

	//��Ŀ����
	m_wTableItemCount=0;
	m_wChairItemCount=0;

	//������Ŀ
	m_wUserGenreCount=0;
	m_wTableGenreCount=0;
	m_wChairGenreCount=0;

	//��������
	m_nAnimationFrame=0;
	m_ptAnimation.SetPoint(0,0);

	//��ɫ����
	m_crName=RGB(0,0,0);
	m_crMember=RGB(0,0,0);
	m_crMaster=RGB(0,0,0);

	//��������
	m_SizeChair.SetSize(0,0);
	m_SizeTable.SetSize(0,0);
	m_SizeEnter.SetSize(0,0);

	//λ�ñ���
	m_ptLock.SetPoint(0,0);
	m_ptEnter.SetPoint(0,0);
	m_ptTableID.SetPoint(0,0);

	//λ�ñ���
	for (WORD i=0;i<MAX_CHAIR;i++)
	{
		m_ptReadyArray[i].SetPoint(0,0);
		m_rcNameArray[i].SetRect(0,0,0,0);
		m_rcChairArray[i].SetRect(0,0,0,0);
	}

	//��������
	ZeroMemory(m_nDrawStyle,sizeof(m_nDrawStyle));
	ZeroMemory(m_szDirectory,sizeof(m_szDirectory));

	//������Դ
	m_ImageReady.LoadImage(AfxGetInstanceHandle(),TEXT("TABLE_READY"));
	m_ImageLocker.LoadImage(AfxGetInstanceHandle(),TEXT("TABLE_LOCKER"));
	m_ImageNumberBK.LoadImage(AfxGetInstanceHandle(),TEXT("TABLE_NUMBER_BK"));
	m_ImageNumberNB.LoadImage(AfxGetInstanceHandle(),TEXT("TABLE_NUMBER_NB"));

	//���ñ���
	m_SizeNumberBK.SetSize(m_ImageNumberBK.GetWidth(),m_ImageNumberBK.GetHeight());
	m_SizeNumberNB.SetSize(m_ImageNumberNB.GetWidth()/10L,m_ImageNumberNB.GetHeight());

	if(m_pTableResource==NULL) m_pTableResource=this;

	return;
}

//��������
CTableResource::~CTableResource()
{
	//�ͷ���Դ
	DestroyResource();

	//���ñ���
	if(m_pTableResource==this) m_pTableResource=NULL;

	return;
}

//�ͷ���Դ
VOID CTableResource::DestroyResource()
{
	//��������
	m_SizeChair.SetSize(0,0);
	m_SizeTable.SetSize(0,0);

	//��Ŀ����
	m_wTableItemCount=0;
	m_wChairItemCount=0;

	//����֡��
	m_nAnimationFrame=0;
	m_ptAnimation.SetPoint(0,0);

	//�������
	ZeroMemory(m_nDrawStyle,sizeof(m_nDrawStyle));

	//λ������
	ZeroMemory(&m_ptLock,sizeof(m_ptLock));
	ZeroMemory(m_rcNameArray,sizeof(m_rcNameArray));
	ZeroMemory(m_rcChairArray,sizeof(m_rcChairArray));
	ZeroMemory(m_ptReadyArray,sizeof(m_ptReadyArray));

	//��ɫ����
	m_crName=RGB(0,0,0);
	m_crMember=RGB(0,0,0);
	m_crMaster=RGB(0,0,0);

	//�ͷ�λͼ
	m_ImageChair.Destroy();
	m_ImageTable.Destroy();
	m_ImageGround.Destroy();
	m_ImageUserFace.Destroy();
	m_ImageWaitDistribute.Destroy();	

	//�ͷ�λͼ
	m_ImageEnter.DestroyImage();
	m_ImageCustomUser.DestroyImage();
	m_ImageCellNumber.DestroyImage();		
	m_ImageAnimation.DestroyImage();

	return;
}

//������Դ
bool CTableResource::LoadResource(LPCTSTR pszResDirectory, WORD wChairCount, bool bAvertCheatMode)
{
	//Ч�����
	ASSERT((pszResDirectory!=NULL)&&(wChairCount<=MAX_CHAIR));
	if ((pszResDirectory==NULL)||(wChairCount>MAX_CHAIR)) return false;

	//��Դ·��
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));
	if(bAvertCheatMode)
	{
		_sntprintf(m_szDirectory,CountArray(m_szDirectory),TEXT("%s\\%s\\AvertCheatMode"),szDirectory,pszResDirectory);

		//���Ŀ¼
		WIN32_FIND_DATA  wFindData;		
		HANDLE hFind = FindFirstFile(m_szDirectory, &wFindData);
		if ((hFind == INVALID_HANDLE_VALUE) || (wFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==0)
		{
			//������·��
			_sntprintf(m_szDirectory,CountArray(m_szDirectory),TEXT("%s\\AvertCheatMode"),szDirectory);
		}
		FindClose(hFind);
	}
	else
	{
		_sntprintf(m_szDirectory,CountArray(m_szDirectory),TEXT("%s\\%s"),szDirectory,pszResDirectory);
	}

	//����·��
	TCHAR szParameterFile[MAX_PATH]=TEXT("");
	_sntprintf(szParameterFile,CountArray(szParameterFile),TEXT("%s\\TableResource.INI"),m_szDirectory);

	//��Դ�ж�
	if(bAvertCheatMode)
	{
		//����·��
		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\Table.png"),m_szDirectory);

		//������Դ
		CPngImage ImageTable;
		if (ImageTable.LoadImage(szImagePath)==false)
		{
			//��Դ·��
			ZeroMemory(m_szDirectory, sizeof(m_szDirectory));
			_sntprintf(m_szDirectory,CountArray(m_szDirectory),TEXT("%s\\AvertCheatMode"),szDirectory);

			//����·��
			ZeroMemory(szParameterFile, sizeof(szParameterFile));
			_sntprintf(szParameterFile,CountArray(szParameterFile),TEXT("%s\\TableResource.INI"),m_szDirectory);
		}

		//����·��
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\WaitDistribute.bmp"),m_szDirectory);

		//������Դ
		if(FAILED(m_ImageWaitDistribute.Load(szImagePath)))
		{
			return false;
		}

		//����·��
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\AnimalDistribute.png"),m_szDirectory);

		//������Դ
		m_ImageAnimation.LoadImage(szImagePath);
	}

	//��ɫ����
	ReadColor(m_crName,TEXT("Color"),TEXT("Color_Name"),szParameterFile);
	ReadColor(m_crMember,TEXT("Color"),TEXT("Color_Member"),szParameterFile);
	ReadColor(m_crMaster,TEXT("Color"),TEXT("Color_Master"),szParameterFile);

	//λ������
	ReadPoint(m_ptLock,TEXT("Position"),TEXT("Point_Lock"),szParameterFile);
	ReadPoint(m_ptEnter,TEXT("Position"),TEXT("Point_Enter"),szParameterFile);
	ReadPoint(m_ptTableID,TEXT("Position"),TEXT("Point_TableID"),szParameterFile);

	//��ʾ����
	m_bShowUser=(GetPrivateProfileInt(TEXT("Attribute"),TEXT("ShowUser"),1L,szParameterFile)==TRUE);
	m_bShowChair=(GetPrivateProfileInt(TEXT("Attribute"),TEXT("ShowChair"),1L,szParameterFile)==TRUE);
	m_bShowTable=(GetPrivateProfileInt(TEXT("Attribute"),TEXT("ShowTable"),1L,szParameterFile)==TRUE);
	m_bShowFocus=(GetPrivateProfileInt(TEXT("Attribute"),TEXT("ShowFocus"),1L,szParameterFile)==TRUE);
	m_bShowEnter=(GetPrivateProfileInt(TEXT("Attribute"),TEXT("ShowEnter"),0L,szParameterFile)==TRUE);
	m_bCustomUser=(GetPrivateProfileInt(TEXT("Attribute"),TEXT("CustomUser"),0L,szParameterFile)==TRUE);
	m_bShowTableID=(GetPrivateProfileInt(TEXT("Attribute"),TEXT("ShowTableID"),1L,szParameterFile)==TRUE);
	m_bRenderImage=(GetPrivateProfileInt(TEXT("Attribute"),TEXT("RenderImage"),1L,szParameterFile)==TRUE);
	m_bShowCellScore=(GetPrivateProfileInt(TEXT("Attribute"),TEXT("ShowCellScore"),0L,szParameterFile)==TRUE);

	//��Ŀ����
	m_wTableItemCount=GetPrivateProfileInt(TEXT("Attribute"),TEXT("TableItemCount"),2L,szParameterFile);
	m_wChairItemCount=GetPrivateProfileInt(TEXT("Attribute"),TEXT("ChairItemCount"),wChairCount,szParameterFile);

	//������Ŀ
	m_wUserGenreCount=GetPrivateProfileInt(TEXT("Attribute"),TEXT("UserGenreCount"),1L,szParameterFile);
	m_wTableGenreCount=GetPrivateProfileInt(TEXT("Attribute"),TEXT("TableGenreCount"),1L,szParameterFile);
	m_wChairGenreCount=GetPrivateProfileInt(TEXT("Attribute"),TEXT("ChairGenreCount"),1L,szParameterFile);

	//��������
	ReadPoint(m_ptAnimation,TEXT("Animation"),TEXT("Point_DistributeAnimal"),szParameterFile);
	m_nAnimationFrame=GetPrivateProfileInt(TEXT("Animation"),TEXT("DistributeAnimalFrame"),0L,szParameterFile);		

	//������Դ
	if(UpdateResource(CSkinRenderManager::GetInstance())==false)
	{
		return false;
	}

	//����ߴ�
	if (m_ImageEnter.IsNull()==false)
	{
		m_SizeEnter.SetSize(m_ImageEnter.GetWidth()/4,m_ImageEnter.GetHeight());
	}

	//���ӳߴ�
	if (m_ImageChair.IsNull()==false)
	{
		m_SizeChair.SetSize(m_ImageChair.GetWidth()/m_wChairItemCount,m_ImageChair.GetHeight()/m_wChairGenreCount);
	}

	//���ӳߴ�
	if (m_ImageTable.IsNull()==false)
	{
		m_SizeTable.SetSize(m_ImageTable.GetWidth()/m_wTableItemCount,m_ImageTable.GetHeight()/m_wTableGenreCount);
	}

	//�û��ߴ�
	if (m_ImageCustomUser.IsNull()==false)
	{
		m_SizeChair.SetSize(m_ImageCustomUser.GetWidth()/m_wChairItemCount,m_ImageCustomUser.GetHeight()/m_wUserGenreCount);
	}

	//λ������
	if (m_bShowTable==true)
	{
		for (WORD i=0;i<wChairCount;i++)
		{
			//��������
			TCHAR szSubItem[64]=TEXT("");

			//�û�����
			if (m_bShowUser==true)
			{
				//����λ��
				_sntprintf(szSubItem,CountArray(szSubItem),TEXT("Rect_Name%d"),i+1);
				ReadRect(m_rcNameArray[i],TEXT("Position"),szSubItem,szParameterFile);

				//׼��λ��
				_sntprintf(szSubItem,CountArray(szSubItem),TEXT("Point_Ready%d"),i+1);
				ReadPoint(m_ptReadyArray[i],TEXT("Position"),szSubItem,szParameterFile);

				//�������
				_sntprintf(szSubItem,CountArray(szSubItem),TEXT("DrawStyle%d"),i+1);
				m_nDrawStyle[i]=GetPrivateProfileInt(TEXT("Position"),szSubItem,0L,szParameterFile);

				//����λ��
				POINT ChairPoint;
				_sntprintf(szSubItem,CountArray(szSubItem),TEXT("Point_Chair%d"),i+1);
				ReadPoint(ChairPoint,TEXT("Position"),szSubItem,szParameterFile);

				//���ӿ��
				m_rcChairArray[i].top=ChairPoint.y;
				m_rcChairArray[i].left=ChairPoint.x;
				m_rcChairArray[i].right=m_rcChairArray[i].left+m_SizeChair.cx;
				m_rcChairArray[i].bottom=m_rcChairArray[i].top+m_SizeChair.cy;
			}
		}
	}

	return true;
}

//������Դ
bool CTableResource::UpdateResource(CSkinRenderManager * pSkinRenderManager)
{
	//����·��
	TCHAR szImageGround[MAX_PATH]=TEXT("");
	_sntprintf(szImageGround,CountArray(szImageGround),TEXT("%s\\Ground.bmp"),m_szDirectory);

	//���ر���
	if (m_ImageGround.IsNull()==false)
	{
		m_ImageGround.Destroy();
	}

	//������Դ
	if (FAILED(m_ImageGround.Load(szImageGround)))
	{
		//m_ImageGround.LoadFromResource(AfxGetInstanceHandle(),IDB_TABLE_GROUND);
	}

	//��Ⱦ��Դ
	if (m_bRenderImage==true)
	{
		pSkinRenderManager->RenderImage(m_ImageGround);
	}

	//����λͼ
	if ((m_bShowEnter==true)&&(m_ImageEnter.IsNull()==true))
	{
		//����·��
		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\Enter.png"),m_szDirectory);

		//������Դ
		if (m_ImageEnter.LoadImage(szImagePath)==false) return false;
	}

	//�û�λͼ
	if ((m_bCustomUser==true)&&(m_ImageCustomUser.IsNull()==true))
	{
		//����·��
		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\CustomUser.png"),m_szDirectory);

		//������Դ
		if (m_ImageCustomUser.LoadImage(szImagePath)==false) return false;
	}

	//��עλͼ
	if ((m_bShowCellScore==true)&&(m_ImageCellNumber.IsNull()==true))
	{
		//����·��
		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\CellNumber.png"),m_szDirectory);

		//������Դ
		if (m_ImageCellNumber.LoadImage(szImagePath)==false)
		{
			//�����ڲ���Դ
			if (m_ImageCellNumber.LoadImage(AfxGetInstanceHandle(),TEXT("TABLE_CELL_NUMBER"))==false)
				return false;
		}
	}

	//��������
	if (m_bShowTable==true)
	{
		//����·��
		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\Table.png"),m_szDirectory);

		//������Դ
		CPngImage ImageTable;
		if (ImageTable.LoadImage(szImagePath)==false) return false;

		//������Դ
		if (m_ImageTable.IsNull()==true)
		{
			m_ImageTable.Create(ImageTable.GetWidth(),ImageTable.GetHeight(),32);
		}

		//��䱳��
		CImageDC TableDC(m_ImageTable);
		CDC * pTableDC=CDC::FromHandle(TableDC);
		m_ImageGround.DrawImageTile(pTableDC,0,0,ImageTable.GetWidth(),ImageTable.GetHeight());

		//��������
		ImageTable.DrawImage(pTableDC,0,0,ImageTable.GetWidth(),ImageTable.GetHeight(),0,0);
	}

	//��������
	if ((m_bShowChair==true)&&(m_bShowTable==true))
	{
		//����·��
		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\Chair.png"),m_szDirectory);

		//������Դ
		CPngImage ImageChair;
		if (ImageChair.LoadImage(szImagePath)==false) return false;

		//������Դ
		if (m_ImageChair.IsNull()==true)
		{
			m_ImageChair.Create(ImageChair.GetWidth(),ImageChair.GetHeight(),32);
		}

		//��䱳��
		CImageDC ChairDC(m_ImageChair);
		CDC * pChairDC=CDC::FromHandle(ChairDC);
		m_ImageGround.DrawImageTile(pChairDC,0,0,ImageChair.GetWidth(),ImageChair.GetHeight());

		//��������
		ImageChair.DrawImage(pChairDC,0,0,ImageChair.GetWidth(),ImageChair.GetHeight(),0,0);
	}

	//ͷ��
	{
		//������Դ
		CPngImage ImageUserFace;
		if (ImageUserFace.LoadImage(GetModuleHandle(AVATAR_CONTROL_DLL_NAME),TEXT("USER_FACE_IMAGE"))==false) return false;

		//������Դ
		if (m_ImageUserFace.IsNull()==true)
		{
			m_ImageUserFace.Create(ImageUserFace.GetWidth(),ImageUserFace.GetHeight(),32);
		}

		//��䱳��
		CImageDC UserFaceDC(m_ImageUserFace);
		CDC * pUserFaceDC=CDC::FromHandle(UserFaceDC);
		m_ImageGround.DrawImageTile(pUserFaceDC,0,0,ImageUserFace.GetWidth(),ImageUserFace.GetHeight());

		//��������
		ImageUserFace.DrawImage(pUserFaceDC,0,0,ImageUserFace.GetWidth(),ImageUserFace.GetHeight(),0,0);
	}

	return true;
}

//��ȡ��ֵ
LONG CTableResource::ReadStringValue(LPCTSTR & pszSring)
{
	//Ч�����
	ASSERT((pszSring!=NULL)&&(pszSring[0]!=0));
	if ((pszSring==NULL)||(pszSring[0]==0)) return 0L;

	//��������
	LONG lSymbol=1;
	
	//���÷���
	if(pszSring[0]=='-') lSymbol=-1;
	if(pszSring[0]==',' && pszSring[1]=='-') lSymbol=-1;

	//Ѱ�ҿ�ʼ
	while (((pszSring[0]>0)&&(pszSring[0]<TEXT('0')))||(pszSring[0]>TEXT('9'))) pszSring++;

	//��ȡ��ֵ
	LONG lValue=0L;
	while ((pszSring[0]>=TEXT('0'))&&(pszSring[0]<=TEXT('9')))
	{
		lValue=lValue*10L+pszSring[0]-TEXT('0');
		++pszSring;
	}

	return lSymbol*lValue;
}

//��ȡ����
bool CTableResource::ReadRect(RECT & ValueRect, LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszIniFile)
{
	//���ñ���
	TCHAR szReadData[64]=TEXT("");
	ZeroMemory(&ValueRect,sizeof(ValueRect));

	//��ȡ�ַ�
	GetPrivateProfileString(pszItem,pszSubItem,TEXT(""),szReadData,CountArray(szReadData),pszIniFile);
	if (szReadData[0]!=0)
	{
		//��ȡ����
		LPCTSTR pszString=szReadData;
		ValueRect.left=ReadStringValue(pszString);
		ValueRect.top=ReadStringValue(pszString);
		ValueRect.right=ReadStringValue(pszString);
		ValueRect.bottom=ReadStringValue(pszString);

		return true;
	}

	return false;
}

//��ȡ����
bool CTableResource::ReadPoint(POINT & ValuePoint, LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszIniFile)
{
	//���ñ���
	TCHAR szReadData[64]=TEXT("");
	ZeroMemory(&ValuePoint,sizeof(ValuePoint));

	//��ȡ�ַ�
	GetPrivateProfileString(pszItem,pszSubItem,TEXT(""),szReadData,CountArray(szReadData),pszIniFile);
	if (szReadData[0]!=0)
	{
		//��ȡ����
		LPCTSTR pszString=szReadData;
		ValuePoint.x=ReadStringValue(pszString);
		ValuePoint.y=ReadStringValue(pszString);

		return true;
	}

	return false;
}

//��ȡ��ɫ
bool CTableResource::ReadColor(COLORREF & ValueColor, LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszIniFile)
{
	//���ñ���
	TCHAR szReadData[64]=TEXT("");
	ZeroMemory(&ValueColor,sizeof(ValueColor));

	//��ȡ�ַ�
	GetPrivateProfileString(pszItem,pszSubItem,TEXT(""),szReadData,CountArray(szReadData),pszIniFile);
	if (szReadData[0]!=0)
	{
		//��ȡ����
		LPCTSTR pszString=szReadData;
		ValueColor=RGB(ReadStringValue(pszString),ReadStringValue(pszString),ReadStringValue(pszString));

		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CTableView::CTableView()
{
	//����ӿ�
	m_pITableViewFrame=NULL;

	//״̬����
	m_bMouseDown=false;
	m_wHoverChairID=INVALID_CHAIR;
	ZeroMemory(&m_TableAttribute,sizeof(m_TableAttribute));
	m_lCellScore=0;

	return;
}

//��������
CTableView::~CTableView()
{
}

//�ӿڲ�ѯ
VOID * CTableView::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableView,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableView,Guid,dwQueryVer);
	return NULL;
}


//��������
WORD CTableView::GetNullChairCount(WORD & wNullChairID)
{
	//���ñ���
	wNullChairID=INVALID_CHAIR;

	//Ѱ��λ��
	WORD wNullCount=0;
	for (WORD i=0;i<m_TableAttribute.wChairCount;i++)
	{
		if (m_TableAttribute.pIClientUserItem[i]==NULL)
		{
			//������Ŀ
			wNullCount++;

			//���ý��
			if (wNullChairID==INVALID_CHAIR) wNullChairID=i;
		}
	}

	return wNullCount;
}

//���ú���
VOID CTableView::InitTableView(WORD wTableID, WORD wChairCount,ITableViewFrame * pITableViewFrame)
{
	//��������
	m_TableAttribute.wTableID=wTableID;
	m_TableAttribute.wChairCount=wChairCount;

	//���ýӿ�
	m_pITableViewFrame=QUERY_OBJECT_PTR_INTERFACE(pITableViewFrame,ITableViewFrame);

	return;
}

//��ȡ�û�
IClientUserItem * CTableView::GetClientUserItem(WORD wChairID)
{
	//Ч�����
	ASSERT(wChairID<m_TableAttribute.wChairCount);
	if (wChairID>=m_TableAttribute.wChairCount) return NULL;

	//��ȡ�û�
	return m_TableAttribute.pIClientUserItem[wChairID];
}

//������Ϣ
bool CTableView::SetClientUserItem(WORD wChairID, IClientUserItem * pIClientUserItem)
{
	//Ч�����
	ASSERT(wChairID<m_TableAttribute.wChairCount);
	if (wChairID>=m_TableAttribute.wChairCount) return false;

	//�����û�
	m_TableAttribute.pIClientUserItem[wChairID]=pIClientUserItem;

	//���½���
	m_pITableViewFrame->UpdateTableView(m_TableAttribute.wTableID);

	return true;
}

//��갴��
VOID CTableView::SetMouseDown(bool bMouseDown)
{
	//���ñ�־
	if (m_bMouseDown!=bMouseDown)
	{
		//���ñ���
		m_bMouseDown=bMouseDown; 

		//���½���
		m_pITableViewFrame->UpdateTableView(m_TableAttribute.wTableID);
	}

	return;
}

//������
VOID CTableView::SetFocusFrame(bool bFocusFrame)
{
	//���ñ�־
	if (m_TableAttribute.bFocusFrame!=bFocusFrame)
	{
		//���ñ���
		m_TableAttribute.bFocusFrame=bFocusFrame;

		//���½���
		m_pITableViewFrame->UpdateTableView(m_TableAttribute.wTableID);
	}

	return;
}

//����λ��
VOID CTableView::SetHoverStatus(WORD wHoverChairID)
{
	//���ñ�־
	if (m_wHoverChairID!=wHoverChairID)
	{
		//���ñ���
		m_wHoverChairID=wHoverChairID;

		//���½���
		m_pITableViewFrame->UpdateTableView(m_TableAttribute.wTableID);
	}

	return;
}

//����״̬ 
VOID CTableView::SetTableStatus(bool bPlaying, bool bLocker,LONG lCellScore)
{
	//���ñ�־
	if ((m_TableAttribute.bLocker!=bLocker)||(m_TableAttribute.bPlaying!=bPlaying))
	{
		//���ñ���
		m_TableAttribute.bLocker=bLocker; 
		m_TableAttribute.bPlaying=bPlaying;		
	}
	m_lCellScore=lCellScore;
	//���½���
	m_pITableViewFrame->UpdateTableView(m_TableAttribute.wTableID);

	return;
}

//�滭����
VOID CTableView::DrawTableView(CDC * pDC, DWORD dwServerRule, CTableResource * pTableResource)
{
	//���Ӵ�С
	CSize SizeChair=pTableResource->m_SizeChair;
	CSize SizeTable=pTableResource->m_SizeTable;

	//��������
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//��������
	bool bMySelfTable=false;
	bool bAvertCheatMode=CServerRule::IsAllowAvertCheatMode(dwServerRule);

	TCHAR szScore[32]=TEXT("");
	_sntprintf(szScore,CountArray(szScore), TEXT("%ld"),m_lCellScore);

	//�滭����
	if ((bAvertCheatMode==false)||(m_TableAttribute.wTableID==0))
	{
		INT nXPosTable=(m_TableAttribute.bPlaying==true && pTableResource->m_wTableItemCount>1)?SizeTable.cx:0;
		pTableResource->m_ImageTable.BitBlt(pDC->m_hDC,0,0,SizeTable.cx,SizeTable.cy,nXPosTable,0);
	}
	else
	{
		INT nXPosTable=SizeTable.cx;
		pTableResource->m_ImageTable.BitBlt(pDC->m_hDC,0,0,SizeTable.cx,SizeTable.cy,nXPosTable,0);
	}

	//�滭����
	if ((pTableResource->m_bShowEnter==true)&&(pTableResource->m_ImageEnter.IsNull()==false))
	{
		//����λ��
		INT nImageIndex=0;
		if (m_TableAttribute.bPlaying==true) nImageIndex=1;
		if ((m_wHoverChairID==INDEX_ENTER_CHAIR)&&(m_TableAttribute.bPlaying==false)&&(m_bMouseDown==true)) nImageIndex=2;
		if ((m_wHoverChairID==INDEX_ENTER_CHAIR)&&(m_TableAttribute.bPlaying==false)&&(m_bMouseDown==false)) nImageIndex=3;

		//�滭��־
		CSize SizeEnter=pTableResource->m_SizeEnter;
		pTableResource->m_ImageEnter.DrawImage(pDC,SizeTable.cx/2+pTableResource->m_ptEnter.x,SizeTable.cy/2+pTableResource->m_ptEnter.y,SizeEnter.cx,SizeEnter.cy,SizeEnter.cx*nImageIndex,0);
	}

	//������Ϣ
	if(m_TableAttribute.wChairCount < MAX_CHAIR)
	{
		for (WORD i=0;i<m_TableAttribute.wChairCount;i++)
		{
			//��������
			CRect rcChair=pTableResource->m_rcChairArray[i];

			//�滭����
			if ((bAvertCheatMode==true)&&(pTableResource->m_bShowChair==true)&&(m_TableAttribute.wTableID==0))
			{
				ASSERT(pTableResource->m_ImageChair.IsNull()==false);
				pTableResource->m_ImageChair.BitBlt(pDC->m_hDC,rcChair.left,rcChair.top,SizeChair.cx,SizeChair.cy,SizeChair.cx*i,0);
			}

			//�滭����
			if ((bAvertCheatMode==false)&&(pTableResource->m_bShowChair==true)&&(m_TableAttribute.pIClientUserItem[i]==NULL))
			{
				ASSERT(pTableResource->m_ImageChair.IsNull()==false);
				pTableResource->m_ImageChair.BitBlt(pDC->m_hDC,rcChair.left,rcChair.top,SizeChair.cx,SizeChair.cy,SizeChair.cx*i,0);
			}

			//�滭�û�
			if (pTableResource->m_bShowUser==true)
			{
				//�滭�û�
				if ((bAvertCheatMode==false)&&(m_TableAttribute.pIClientUserItem[i]!=NULL))
				{
					//��ȡ�û�
					IClientUserItem * pIClientUserItem=m_TableAttribute.pIClientUserItem[i];
					tagCustomFaceInfo * pCustomFaceInfo=pIClientUserItem->GetCustomFaceInfo();

					//�Լ��ж�
					if (m_TableAttribute.pIClientUserItem[i]->GetUserID()==pGlobalUserData->dwUserID)
					{
						bMySelfTable=true;
					}

					//�滭ͷ��
					if (pTableResource->m_bCustomUser==false)
					{
						//����λ��
						INT nXExcursion=(rcChair.Width()-FACE_CX)/2;
						INT nYExcursion=(rcChair.Height()-FACE_CY)/2;

						//�滭ͷ��
						DrawUserAvatar(pDC,rcChair.left+nXExcursion,rcChair.top+nYExcursion,pIClientUserItem,pTableResource);
					}
					else
					{
						//�Զ�ͷ��
						INT nXImagePos=i*rcChair.Width();
						INT nYImagePos=(m_TableAttribute.pIClientUserItem[i]->GetGender()==GENDER_FEMALE)?rcChair.Height():0;
						pTableResource->m_ImageCustomUser.DrawImage(pDC,rcChair.left,rcChair.top,rcChair.Width(),rcChair.Height(),nXImagePos,nYImagePos);
					}

					//ͬλ�ж�
					WORD wTableID=pIClientUserItem->GetTableID();
					WORD wChairID=pIClientUserItem->GetChairID();
					bool bSameStation=(wTableID==m_TableAttribute.wTableID)&&(wChairID==i);

					//ͬ���־
					if ((bSameStation==true)&&(m_TableAttribute.bPlaying==false)&&(pIClientUserItem->GetUserStatus()==US_READY))
					{
						INT nXPos=pTableResource->m_ptReadyArray[i].x;
						INT nYPos=pTableResource->m_ptReadyArray[i].y;
						pTableResource->m_ImageReady.DrawImage(pDC,nXPos,nYPos);
					}

					//�û�����
					DrawTableName(pDC,i,pIClientUserItem->GetNickName(),pTableResource);
				}

				//����ģʽ
				if ((bAvertCheatMode==true)&&(pTableResource->m_bShowUser==true))
				{
					//�û�ͷ��
					if (m_TableAttribute.wTableID>0)
					{
						//����λ��
						INT nXExcursion=(rcChair.Width()-FACE_CX)/2;
						INT nYExcursion=(rcChair.Height()-FACE_CY)/2;

						//�滭ͷ��
						if (pTableResource->m_bCustomUser==false)
						{
							//ϵͳͷ��
							IFaceItemControl * pIFaceItemControl=CFaceItemControl::GetInstance();
							pIFaceItemControl->DrawFaceNormal(pDC,rcChair.left+nXExcursion,rcChair.top+nYExcursion,((m_TableAttribute.wTableID*m_TableAttribute.wChairCount)+i)%pIFaceItemControl->GetFaceCount());
						}
						else
						{
							//�Զ�ͷ��
							INT nXImagePos=i*rcChair.Width();
							INT nYImagePos=(((m_TableAttribute.wTableID*m_TableAttribute.wChairCount)+i)%2==GENDER_FEMALE)?rcChair.Height():0;
							pTableResource->m_ImageCustomUser.DrawImage(pDC,rcChair.left,rcChair.top,rcChair.Width(),rcChair.Height(),nXImagePos,nYImagePos);
						}
					}

					//�û�����
					WORD wTableID=m_TableAttribute.wTableID;
					DrawTableName(pDC,i,(wTableID==0)?TEXT("�������"):TEXT("����λ��"),pTableResource);
				}

				//�滭�߿�
				if ((bAvertCheatMode==false)||(m_TableAttribute.wTableID==0))
				{
					//�Զ��߿�
					if ((pTableResource->m_bCustomUser==true)&&(m_wHoverChairID==i))
					{
						pDC->Draw3dRect(rcChair.left,rcChair.top,rcChair.Width(),rcChair.Height(),RGB(125,125,125),RGB(125,125,125));
					}

					//ϵͳ�߿�
					if ((pTableResource->m_bCustomUser==false)&&((m_wHoverChairID==i)||(m_TableAttribute.pIClientUserItem[i]!=NULL)))
					{
						//����λ��
						INT nXExcursion=(rcChair.Width()-FACE_CX)/2;
						INT nYExcursion=(rcChair.Height()-FACE_CY)/2;

						//��������
						BYTE cbFrameKind=ITEM_FRAME_NORMAL;
						if ((m_wHoverChairID==i)&&(m_bMouseDown==false)) cbFrameKind=ITEM_FRAME_HOVER;
						if ((m_wHoverChairID==i)&&(m_bMouseDown==true)) cbFrameKind=ITEM_FRAME_HITDOWN;

						//�滭���
						IFaceItemControl * pIFaceItemControl=CFaceItemControl::GetInstance();
						pIFaceItemControl->DrawFaceItemFrame(pDC,rcChair.left+nXExcursion,rcChair.top+nYExcursion,cbFrameKind);
					}
				}
			}
		}

	}

	//�滭��ͼ
	if (m_TableAttribute.bLocker==true)
	{
		INT nXPos=pTableResource->m_ptLock.x;
		INT nYPos=pTableResource->m_ptLock.y;
		pTableResource->m_ImageLocker.DrawImage(pDC,nXPos,nYPos);
	}

	//���Ӻ���
	if (pTableResource->m_bShowTableID==true)
	{
		//��ȡ��С
		CSize SizeNumberBK=pTableResource->m_SizeNumberBK;
		CSize SizeNumberNB=pTableResource->m_SizeNumberNB;

		//��������
		WORD wItemCount=0;
		WORD wNumberTemp=m_TableAttribute.wTableID+1;
		WORD wTableNumber=m_TableAttribute.wTableID+1;

		//������Ŀ
		do
		{
			wItemCount++;
			wNumberTemp/=10;
		} while (wNumberTemp>0);

		//�滭����
		INT nXNumberBK=pTableResource->m_ptTableID.x-SizeNumberBK.cx/2;
		INT nYNumberBK=pTableResource->m_ptTableID.y-SizeNumberBK.cy/2;
		pTableResource->m_ImageNumberBK.DrawImage(pDC,nXNumberBK,nYNumberBK);

		//λ�ö���
		INT nYNumberNB=pTableResource->m_ptTableID.y-SizeNumberNB.cy/2;
		INT nXNumberNB=pTableResource->m_ptTableID.x+(wItemCount*SizeNumberNB.cx)/2-SizeNumberNB.cx;

		//�滭����
		for (INT i=0;i<wItemCount;i++)
		{
			//�滭����
			WORD wNumber=wTableNumber%10;
			pTableResource->m_ImageNumberNB.DrawImage(pDC,nXNumberNB,nYNumberNB,SizeNumberNB.cx,SizeNumberNB.cy,wNumber*SizeNumberNB.cx,0);

			//���ñ���
			wTableNumber/=10;
			nXNumberNB-=SizeNumberNB.cx;
		};
	}

	//�滭����
	if ((pTableResource->m_bShowFocus==true)&&(pParameterGlobal->m_bSalienceTable==true)&&(bMySelfTable==true))
	{
		pDC->Draw3dRect(0,0,SizeTable.cx,SizeTable.cy,RGB(125,125,125),RGB(125,125,125));
	}

	//�滭��ע
	if ((bAvertCheatMode==false)||(m_TableAttribute.wTableID==0))
	{
		if (m_lCellScore>0 && pTableResource->m_bShowCellScore==true)
		{
			DrawNumberString(pDC,pTableResource->m_ImageCellNumber,m_lCellScore,SizeTable.cx/2-5,SizeTable.cy/2,TA_CENTER);
		}
	}

	return;
}

//�滭����
VOID CTableView::DrawTableName(CDC * pDC, WORD wChairID, LPCTSTR pszNickName, CTableResource * pTableResource)
{
	//�û�����
	CRect rcName=pTableResource->m_rcNameArray[wChairID];

	//��ͼ����
	CRect rcDrawRect=rcName;
	INT nNameLength=lstrlen(pszNickName);
	INT nDrawStyte=pTableResource->m_nDrawStyle[wChairID];
	INT nDrawFormat=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS;
	INT nNameHeight=pDC->DrawText(pszNickName,nNameLength,&rcDrawRect,nDrawFormat|DT_CALCRECT);

	//����λ��
	if (nNameHeight<=12)
	{
		if (nDrawStyte&NT_LEFT) nDrawFormat|=DT_LEFT;
		else if (nDrawStyte&NT_RIGHT) nDrawFormat|=DT_RIGHT;
		else nDrawFormat|=(DT_CENTER|DT_SINGLELINE);
	}
	else nDrawFormat|=DT_LEFT;

	//����λ��
	INT nYPos=0;
	if (nDrawStyte&NT_BOTTOM) nYPos=rcName.Height()-nNameHeight;
	else if (nDrawStyte&NT_TOP) nYPos=0;
	else nYPos=(rcName.Height()-nNameHeight)/2;

	//����λ��
	rcDrawRect=rcName;
	rcDrawRect.left=rcName.left;
	rcDrawRect.right=rcName.right;
	rcDrawRect.top=__max(rcName.top,rcName.top+nYPos);
	rcDrawRect.bottom=__min(rcName.bottom,rcDrawRect.top+nNameHeight);

	//�û�����
	pDC->SetTextColor(pTableResource->m_crName);
	pDC->DrawText(pszNickName,nNameLength,&rcDrawRect,nDrawFormat);

	return;
}

//�滭ͷ��
VOID CTableView::DrawUserAvatar(CDC * pDC, INT nXPos, INT nYPos, IClientUserItem * pIClientUserItem, CTableResource * pTableResource)
{
	DrawUserAvatar(pDC, nXPos, nYPos, FACE_CX, FACE_CY, pIClientUserItem, pTableResource);

	return;
}

//�滭ͷ��
VOID CTableView::DrawUserAvatar(CDC * pDC, INT nXPos, INT nYPos, INT nWidth, INT nHeight, IClientUserItem * pIClientUserItem, CTableResource * pTableResource)
{
	//Ч�����
	ASSERT(pIClientUserItem!=NULL);
	if (pIClientUserItem==NULL) return;

	//��������
	tagUserInfo * pUserInfo=pIClientUserItem->GetUserInfo();
	tagCustomFaceInfo * pCustomFaceInfo=pIClientUserItem->GetCustomFaceInfo();

	//�滭ͷ��
	if ((pCustomFaceInfo->dwDataSize!=0L)&&(pIClientUserItem->GetCustomID()!=0L))
	{
		IFaceItemControl * pIFaceItemControl=CFaceItemControl::GetInstance();
		pIFaceItemControl->DrawFaceNormal(pDC,nXPos,nYPos,pCustomFaceInfo->dwCustomFace);
	}
	else
	{
		//WIN 7 ִ��
		if(CD2DEngine::GetD2DEngine())
		{
			//���ñ���
			WORD wLineCount=pTableResource->m_ImageUserFace.GetWidth()/FACE_CX;
			WORD wFullCount=wLineCount*pTableResource->m_ImageUserFace.GetHeight()/(FACE_CY*2);

			//��������
			WORD wFaceID=pIClientUserItem->GetFaceID();
			bool bOffLine=(pIClientUserItem->GetUserStatus()==US_OFFLINE);

			//�滭ͷ��
			if (bOffLine==false)
			{
				//��������
				if (wFaceID>=wFullCount) wFaceID=0;

				//�滭ͷ��
				INT nXImagePos=wFaceID%wLineCount;
				INT nYImagePos=(wFaceID/wLineCount)*2L;
				pTableResource->m_ImageUserFace.BitBlt(pDC->m_hDC,nXPos,nYPos,nWidth,nHeight,nXImagePos*FACE_CX,nYImagePos*FACE_CY);
			}
			else
			{
				//��������
				if (wFaceID>=wFullCount) wFaceID=0;

				//�滭ͷ��
				INT nXImagePos=wFaceID%wLineCount;
				INT nYImagePos=(wFaceID/wLineCount)*2L;
				pTableResource->m_ImageUserFace.BitBlt(pDC->m_hDC,nXPos,nYPos,nWidth,nHeight,nXImagePos*FACE_CX,nYImagePos*FACE_CY+FACE_CY);
			}
		}
		else
		{
			IFaceItemControl * pIFaceItemControl=CFaceItemControl::GetInstance();
			pIFaceItemControl->DrawFaceNormal(pDC,nXPos,nYPos,pIClientUserItem->GetFaceID());
		}
	}

	return;
}

//�滭����
VOID CTableView::DrawNumberString(CDC * pDC, CPngImage &PngNumber, LONG lNumber, INT nXPos, INT nYPos,BYTE cbAlign)
{
	//������Դ
	INT nNumberHeight=PngNumber.GetHeight();
	INT nNumberWidth=PngNumber.GetWidth()/10;

	//������Ŀ
	LONG lNumberCount=0;
	LONG lNumberTemp=lNumber;
	do
	{
		lNumberCount++;
		lNumberTemp/=10;
	} while (lNumberTemp>0);

	//λ�ö���
	INT nYDrawPos=nYPos-nNumberHeight/2;
	INT nXDrawPos=nXPos;
	if(cbAlign==TA_RIGHT)
		nXDrawPos+=(lNumberCount-1)*nNumberWidth;
	else if(cbAlign==TA_CENTER)
		nXDrawPos+=(lNumberCount-1)*nNumberWidth/2;

	//�滭����
	for (LONG i=0;i<lNumberCount;i++)
	{
		//�滭����
		LONG lCellNumber=(LONG)(lNumber%10L);
		PngNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,nNumberWidth,nNumberHeight,lCellNumber*nNumberWidth,0);

		//���ñ���
		lNumber/=10;
		nXDrawPos-=nNumberWidth;
	};

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CTableViewFrame::CTableViewFrame()
{
	//״̬����
	m_bHovering=false;
	m_bLMouseDown=false;
	m_bRMouseDown=false;
	m_bWaitDistribute=false;
	m_cbButtonDown=0;
	m_cbButtonHover=0;
	m_nAnimalFrameIndex=0;

	//���Ա���
	m_wTableCount=0;
	m_wChairCount=0;
	m_dwServerRule=0;
	m_wServerType=0;

	//������Ϣ
	m_nXExcursion=0;
	m_nYExcursion=0;
	m_nXTableCount=0;
	m_nYTableCount=0;

	//������Ϣ
	m_nScrollPos=0;
	m_nScrollPosMax=0;	
	m_nVisibleLineCount=0;

	//��������
	m_wDownChairID=INVALID_CHAIR;
	m_wDownTableID=INVALID_TABLE;

	//����ָ��
	m_pITableViewFrameSink=NULL;

	return;
}

//��������
CTableViewFrame::~CTableViewFrame()
{
	//ɾ������
	for (INT_PTR i=0;i<m_TableViewArray.GetCount();i++)
	{
		CTableView * pTableView=m_TableViewArray[i];
		if (pTableView!=NULL) SafeDelete(pTableView);
	}

	//ɾ������
	m_TableViewArray.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * CTableViewFrame::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableViewFrame,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableViewFrame,Guid,dwQueryVer);
	return NULL;
}

//��������
bool CTableViewFrame::CreateTableFrame(CWnd * pParentWnd, UINT uWndID, IUnknownEx * pIUnknownEx)
{
	//���ñ���
	m_bHovering=false;
	m_bLMouseDown=false;
	m_bRMouseDown=false;	
	m_cbButtonDown=0;
	m_cbButtonHover=0;
	m_wDownChairID=INVALID_CHAIR;
	m_wDownTableID=INVALID_TABLE;

	//���ýӿ�
	ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableViewFrameSink)!=NULL);
	m_pITableViewFrameSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableViewFrameSink);

	//��������
	CRect rcCreate(0,0,0,0);
	Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcCreate,pParentWnd,uWndID);

	return true;
}

//���ú���
bool CTableViewFrame::ConfigTableFrame(WORD wTableCount, WORD wChairCount, DWORD dwServerRule, WORD wServerType, WORD wServerID, LPCTSTR pszResDirectory)
{
	//Ч�����
	ASSERT(wChairCount<=MAX_CHAIR);
	ASSERT((pszResDirectory!=NULL)&&(pszResDirectory[0]!=0));

	//���ù���
	m_SkinScrollBar.InitScroolBar(this);

	//�������
	bool bAvertCheatMode=CServerRule::IsAllowAvertCheatMode(dwServerRule);

	//������Դ
	/* try
	{
		if (m_TableResource.LoadResource(pszResDirectory,wChairCount, bAvertCheatMode)==false)
		{
			return false;
		}
	}
	catch(...)
	{
		return false;
	} */

	//���ñ���
	m_wTableCount=wTableCount;
	m_wChairCount=wChairCount;
	m_dwServerRule=dwServerRule;
	m_wServerType=wServerType;
	m_TableViewArray.SetSize(m_wTableCount);
	ZeroMemory(m_TableViewArray.GetData(),m_wTableCount*sizeof(CTableView *));

	//��������
	for (WORD i=0;i<m_wTableCount;i++)
	{
		m_TableViewArray[i]=new CTableView;
		m_TableViewArray[i]->InitTableView(i,wChairCount,this);
	}

	//����λ��
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyControl(rcClient.Width(),rcClient.Height());

	//ע���¼�
	CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
	if (pPlatformEvent!=NULL) pPlatformEvent->RegisterEventWnd(m_hWnd);

	return true;
}

//��ȡ�û�
IClientUserItem * CTableViewFrame::GetClientUserItem(WORD wTableID, WORD wChairID)
{
	//��ȡ����
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//��ȡ�û�
	if (pITableView!=NULL)
	{
		return pITableView->GetClientUserItem(wChairID);
	}

	return NULL;
}

//������Ϣ
bool CTableViewFrame::SetClientUserItem(WORD wTableID, WORD wChairID, IClientUserItem * pIClientUserItem)
{
	ITableView * pITableView=GetTableViewItem(wTableID);
	if (pITableView!=NULL) pITableView->SetClientUserItem(wChairID,pIClientUserItem);
	return true;
}

//��Ϸ��־
bool CTableViewFrame::GetPlayFlag(WORD wTableID)
{
	//��ȡ����
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//��ȡ��־
	if (pITableView!=NULL)
	{
		return pITableView->GetPlayFlag();
	}

	return false;
}

//�����־
bool CTableViewFrame::GetLockerFlag(WORD wTableID)
{
	//��ȡ����
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//��ȡ��־
	if (pITableView!=NULL)
	{
		return pITableView->GetLockerFlag();
	}

	return false;
}

//������
VOID CTableViewFrame::SetFocusFrame(WORD wTableID, bool bFocusFrame)
{
	//��ȡ����
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//���ñ�־
	if (pITableView!=NULL) pITableView->SetFocusFrame(bFocusFrame);

	return;
}

//����״̬ 
VOID CTableViewFrame::SetTableStatus(WORD wTableID, bool bPlaying, bool bLocker,LONG lCellScore)
{
	//��ȡ����
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//���ñ�־
	if (pITableView!=NULL) pITableView->SetTableStatus(bPlaying,bLocker,lCellScore);

	return;
}

//����״̬ 
VOID CTableViewFrame::SetTableStatus(bool bWaitDistribute)
{
	m_bWaitDistribute=bWaitDistribute;	
	RedrawWindow();

	//���ö�ʱ��
	if(m_bWaitDistribute == true && m_TableResource.m_nAnimationFrame > 0)
	{
		m_nAnimalFrameIndex=0;
		SetTimer(IDI_WAIT_ANIMATION,TIME_WAIT_ANIMATION,NULL);
	}
	else
	{
		KillTimer(IDI_WAIT_ANIMATION);
	}

	return;
}

//���ӿ���
bool CTableViewFrame::VisibleTable(WORD wTableID)
{
	//Ч�����
	ASSERT(wTableID<m_wTableCount);
	if (wTableID>=m_wTableCount) return false;

	//����λ��
	INT nLastPos=m_nScrollPos;
	INT nXPos=(wTableID%m_nXTableCount)*m_TableResource.m_SizeTable.cx+m_nXExcursion;
	INT nYPos=(wTableID/m_nXTableCount)*m_TableResource.m_SizeTable.cy-m_nScrollPos;

	//����λ��
	CRect rcTable,rcClient;
	GetClientRect(&rcClient);
	rcTable.SetRect(nXPos,nYPos,nXPos+m_TableResource.m_SizeTable.cx,nYPos+m_TableResource.m_SizeTable.cy);

	//λ�õ���
	if (rcTable.top<0L) m_nScrollPos+=rcTable.top;
	if ((rcTable.top>0)&&(rcTable.bottom>rcClient.bottom)) m_nScrollPos+=rcTable.bottom-rcClient.bottom;
	m_nScrollPos=__max(0,__min(m_nScrollPos,m_nScrollPosMax-m_nVisibleLineCount*m_TableResource.m_SizeTable.cy));

	//������ͼ
	if (nLastPos!=m_nScrollPos)
	{
		SetScrollPos(SB_VERT,m_nScrollPos);
		ScrollWindow(0,nLastPos-m_nScrollPos,NULL,NULL);
	}

	return true;
}

//��������
bool CTableViewFrame::FlashGameTable(WORD wTableID)
{
	//��ȡ����
	ITableView * pITableView=GetTableViewItem(wTableID);

	//�����ж�
	if (pITableView==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	INT nXItem=wTableID%m_nXTableCount;
	INT nYItem=wTableID/m_nXTableCount;

	//����λ��
	CRect rcTable;
	rcTable.left=nXItem*m_TableResource.m_SizeTable.cx+m_nXExcursion;
	rcTable.top=nYItem*m_TableResource.m_SizeTable.cy-m_nScrollPos;
	rcTable.right=(nXItem+1)*m_TableResource.m_SizeTable.cx+m_nXExcursion;
	rcTable.bottom=(nYItem+1)*m_TableResource.m_SizeTable.cy+m_nYExcursion-m_nScrollPos;
	
	//�������
	INT nFlashTimes=20,nStepWidth=4;
	INT nExpandPos=nFlashTimes*nStepWidth;
	rcTable.InflateRect(nExpandPos,nExpandPos);

	//���´���
	UpdateWindow();

	//ѭ���滭
	CWindowDC WindowDC(this);
	CPen LinePen(PS_SOLID,0,GetSysColor(COLOR_BTNHILIGHT));

	//���û���
	WindowDC.SetROP2(R2_XORPEN);
	WindowDC.SelectObject(&LinePen);

	//ѭ���滭
	for (INT nTimes=nFlashTimes;nTimes>=0;nTimes--)
	{
		//�滭����
		WindowDC.MoveTo(rcTable.left,rcTable.top);
		WindowDC.LineTo(rcTable.right,rcTable.top);
		WindowDC.LineTo(rcTable.right,rcTable.bottom);
		WindowDC.LineTo(rcTable.left,rcTable.bottom);
		WindowDC.LineTo(rcTable.left,rcTable.top);

		//�滭��ͣ
		Sleep((nFlashTimes-nTimes)*2);

		//�滭����
		WindowDC.MoveTo(rcTable.left,rcTable.top);
		WindowDC.LineTo(rcTable.right,rcTable.top);
		WindowDC.LineTo(rcTable.right,rcTable.bottom);
		WindowDC.LineTo(rcTable.left,rcTable.bottom);
		WindowDC.LineTo(rcTable.left,rcTable.top);

		//���þ���
		rcTable.DeflateRect(nStepWidth,nStepWidth);
	}

	//�ͷ���Դ
	WindowDC.DeleteDC();
	LinePen.DeleteObject();

	return true;
}

//��������
bool CTableViewFrame::FlashGameChair(WORD wTableID, WORD wChairID)
{
	//��ȡ����
	ITableView * pITableView=GetTableViewItem(wTableID);

	//�����ж�
	if (pITableView==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	INT nXItem=wTableID%m_nXTableCount;
	INT nYItem=wTableID/m_nXTableCount;
	CRect rcChair=m_TableResource.m_rcChairArray[wChairID];

	//��ȡλ��
	INT nXTablePos=nXItem*m_TableResource.m_SizeTable.cx;
	INT nYTablePos=nYItem*m_TableResource.m_SizeTable.cy;
	rcChair.OffsetRect(nXTablePos+m_nXExcursion,nYTablePos+m_nYExcursion-m_nScrollPos);

	//�������
	INT nFlashTimes=20,nStepWidth=4;
	INT nExpandPos=nFlashTimes*nStepWidth;
	rcChair.InflateRect(nExpandPos,nExpandPos);

	//���´���
	UpdateWindow();

	//ѭ���滭
	CClientDC ClientDC(this);
	CPen LinePen(PS_SOLID,0,RGB(255,255,255));

	//���û���
	ClientDC.SetROP2(R2_XORPEN);
	CPen * pOldPen=ClientDC.SelectObject(&LinePen);

	//ѭ���滭
	for (INT nTimes=nFlashTimes;nTimes>=0;nTimes--)
	{
		//�滭����
		ClientDC.MoveTo(rcChair.left,rcChair.top);
		ClientDC.LineTo(rcChair.right,rcChair.top);
		ClientDC.LineTo(rcChair.right,rcChair.bottom);
		ClientDC.LineTo(rcChair.left,rcChair.bottom);
		ClientDC.LineTo(rcChair.left,rcChair.top);

		//�滭��ͣ
		Sleep((nFlashTimes-nTimes)*2);

		//�滭����
		ClientDC.MoveTo(rcChair.left,rcChair.top);
		ClientDC.LineTo(rcChair.right,rcChair.top);
		ClientDC.LineTo(rcChair.right,rcChair.bottom);
		ClientDC.LineTo(rcChair.left,rcChair.bottom);
		ClientDC.LineTo(rcChair.left,rcChair.top);

		//���þ���
		rcChair.DeflateRect(nStepWidth,nStepWidth);
	}

	//�ͷ���Դ
	ClientDC.SelectObject(pOldPen);

	return true;
}

//��������
bool CTableViewFrame::UpdateTableView(WORD wTableID)
{
	//��ȡ����
	ITableView * pITableView=GetTableViewItem(wTableID);
	if ((pITableView==NULL)||(m_nXTableCount==0)) return false;

	//��������
	INT nXItem=wTableID%m_nXTableCount;
	INT nYItem=wTableID/m_nXTableCount;

	//��������
	CRect rcTable;
	rcTable.left=nXItem*m_TableResource.m_SizeTable.cx+m_nXExcursion;
	rcTable.right=(nXItem+1)*m_TableResource.m_SizeTable.cx+m_nXExcursion;
	rcTable.top=nYItem*m_TableResource.m_SizeTable.cy+m_nYExcursion-m_nScrollPos;
	rcTable.bottom=(nYItem+1)*m_TableResource.m_SizeTable.cy+m_nYExcursion-m_nScrollPos;

	//��������
	RedrawWindow(&rcTable,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW|RDW_UPDATENOW);

	return true;
}

//��ȡ����
ITableView * CTableViewFrame::GetTableViewItem(WORD wTableID)
{
	//��ȡ����
	if (wTableID!=INVALID_TABLE)
	{
		//Ч�����
		ASSERT(wTableID<m_TableViewArray.GetCount());
		if (wTableID>=m_TableViewArray.GetCount()) return NULL;

		//��ȡ����
		ITableView * pITableView=m_TableViewArray[wTableID];

		return pITableView;
	}

	return NULL;
}

//��������
WORD CTableViewFrame::GetNullChairCount(WORD wTableID, WORD & wNullChairID)
{
	//��ȡ����
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//��ȡ״̬
	if (pITableView!=NULL)
	{
		return pITableView->GetNullChairCount(wNullChairID);
	}

	return 0;
}

//�ػ���Ϣ
VOID CTableViewFrame::OnPaint()
{
	//���� DC
	CPaintDC dc(this);

	//��ȡ����
	CRect rcClip;
	CRect rcClient;
	dc.GetClipBox(&rcClip);
	GetClientRect(&rcClient);
	//GetClientRect(&rcClip);

	//�滭����
	if ((m_TableViewArray.GetCount()>0L)&&(m_TableResource.m_bShowTable==true))
	{
		//�������
		CSize SizeTable=m_TableResource.m_SizeTable;

		//��������
		WORD wTableCount=m_wTableCount;
		WORD nXTableCount=m_nXTableCount;
		INT  nXExcursion=m_nXExcursion;
		INT  nYExcursion=m_nYExcursion;

		//�������
		bool bAvertCheatMode=CServerRule::IsAllowAvertCheatMode(m_dwServerRule);		
		if(bAvertCheatMode)
		{
			wTableCount=1;
			nXTableCount=1;

			if(bAvertCheatMode && m_bWaitDistribute)
			{
				SizeTable.cx=m_TableResource.m_ImageWaitDistribute.GetWidth();
				SizeTable.cy=m_TableResource.m_ImageWaitDistribute.GetHeight();
				nXExcursion=(__max(SizeTable.cx,rcClient.Width()) - SizeTable.cx)/2;
				nYExcursion=(__max(SizeTable.cy,rcClient.Height()) - SizeTable.cy)/2;
			}
		}

		//��������
		CImage ImageBuffer;
		ImageBuffer.Create(SizeTable.cx,SizeTable.cy,32);

		//���� DC
		CImageDC BufferDC(ImageBuffer);
		CDC * pDCBuffer=CDC::FromHandle(BufferDC);

		//���û���
		pDCBuffer->SetBkMode(TRANSPARENT);
		pDCBuffer->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

		//�滭׼��
		INT nXStartItem=(rcClip.left-nXExcursion)/SizeTable.cx;
		INT nYStartItem=(m_nScrollPos-nYExcursion+rcClip.top )/SizeTable.cy;
		INT nXConcludeItem=(rcClip.right-nXExcursion+SizeTable.cx-1)/SizeTable.cx;
		INT nYConcludeItem=(m_nScrollPos-nYExcursion+rcClip.bottom+SizeTable.cy-1)/SizeTable.cy;

		//�滭����
		for (INT nYIndex=nYStartItem;nYIndex<nYConcludeItem;nYIndex++)
		{
			for (INT nXIndex=nXStartItem;nXIndex<nXConcludeItem;nXIndex++)
			{
				//λ�ö���
				INT nIndex=nYIndex*m_nXTableCount+nXIndex;
				INT nXTablePos=nXIndex*SizeTable.cx+nXExcursion;
				INT nYTablePos=nYIndex*SizeTable.cy+nYExcursion-m_nScrollPos;

				if(SizeTable.cx > rcClient.Width())
				{
					nXTablePos = (rcClient.Width() - SizeTable.cx)/2;
				}

				//�滭����
				if ((nIndex>=0)&&(nXIndex<nXTableCount)&&(nIndex<wTableCount))
				{
					//�滭����
					if(bAvertCheatMode && m_bWaitDistribute)
					{
						m_TableResource.m_ImageWaitDistribute.BitBlt(pDCBuffer->GetSafeHdc(),0,0);

						//��������
						if(m_TableResource.m_ImageAnimation.IsNull() == false)
						{
							//��������
							CSize SizeAnimation;
							SizeAnimation.cy=m_TableResource.m_ImageAnimation.GetHeight();
							SizeAnimation.cx=m_TableResource.m_ImageAnimation.GetWidth()/m_TableResource.m_nAnimationFrame;

							//�滭����
							m_TableResource.m_ImageAnimation.DrawImage(pDCBuffer,m_TableResource.m_ptAnimation.x,m_TableResource.m_ptAnimation.y,SizeAnimation.cx,SizeAnimation.cy,m_nAnimalFrameIndex*SizeAnimation.cx,0,SizeAnimation.cx,SizeAnimation.cy);
						}
					}
					else
					{
						m_TableViewArray[nIndex]->DrawTableView(pDCBuffer,m_dwServerRule,&m_TableResource);
					}

					//�滭����
					dc.BitBlt(nXTablePos,nYTablePos,SizeTable.cx,SizeTable.cy,pDCBuffer,0,0,SRCCOPY);
				}
				else
				{
					//�滭��϶
					m_TableResource.m_ImageGround.DrawImageTile(&dc,nXTablePos,nYTablePos,SizeTable.cx,SizeTable.cy);
				}
			}
		}

		//�滭��϶
		if (nXExcursion>rcClip.left)
		{
			INT nTileWidth=nXExcursion-rcClip.left;
			m_TableResource.m_ImageGround.DrawImageTile(&dc,rcClip.left,rcClip.top,nTileWidth,rcClip.Height());
		}

		//�滭��϶
		if ((nYExcursion-m_nScrollPos)>rcClip.top)
		{
			INT nTileHeight=(nYExcursion-m_nScrollPos)-rcClip.top;
			m_TableResource.m_ImageGround.DrawImageTile(&dc,rcClip.left,rcClip.top,rcClip.Width(),nTileHeight);
		}
	}
	else 
	{
		//�滭��϶
		if (m_TableResource.m_ImageGround.IsNull()==false)
		{
			m_TableResource.m_ImageGround.DrawImageTile(&dc,rcClip);
		}
		else
		{
			dc.FillSolidRect(&rcClip,RGB(0,0,0));
		}
	}

	return;
}

//������Ϣ
VOID CTableViewFrame::OnNcDestroy()
{
	__super::OnNcDestroy();

	//ɾ������
	for (INT_PTR i=0;i<m_TableViewArray.GetCount();i++)
	{
		CTableView * pTableView=m_TableViewArray[i];
		if (pTableView!=NULL) SafeDelete(pTableView);
	}

	//ɾ������
	m_TableViewArray.RemoveAll();

	//������Դ
	m_TableResource.DestroyResource();
}

//ʱ����Ϣ
VOID CTableViewFrame::OnTimer(UINT nIDEvent)
{
	if(IDI_WAIT_ANIMATION == nIDEvent)
	{
		//��������
		m_nAnimalFrameIndex=(m_nAnimalFrameIndex+1)%m_TableResource.m_nAnimationFrame;

		//����֡�ߴ�
		INT nImageHeight=m_TableResource.m_ImageAnimation.GetHeight();
		INT nImageWidth=m_TableResource.m_ImageAnimation.GetWidth()/m_TableResource.m_nAnimationFrame;

		//��������
		CRect rcPaint; 
		rcPaint.left=m_TableResource.m_ptAnimation.x;
		rcPaint.top=m_TableResource.m_ptAnimation.y;
		rcPaint.right=m_TableResource.m_ptAnimation.x+nImageWidth;
		rcPaint.bottom=m_TableResource.m_ptAnimation.y+nImageHeight;

		//���½���
		InvalidateRect(&rcPaint);
	}
}

//λ����Ϣ
VOID CTableViewFrame::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//�����ؼ�
	RectifyControl(cx,cy);

	return;
}

//��Ϣ����
BOOL CTableViewFrame::PreTranslateMessage(MSG * pMsg)
{
	//��Ϣ����
	if (pMsg->message==WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_UP:
			{
				SendMessage(WM_VSCROLL,MAKELONG(SB_LINEUP,m_nScrollPos),NULL);
				return TRUE;
			}
		case VK_DOWN:
			{
				SendMessage(WM_VSCROLL,MAKELONG(SB_LINEDOWN,m_nScrollPos),NULL);
				return TRUE;
			}
		case VK_PRIOR:
			{
				SendMessage(WM_VSCROLL,MAKELONG(SB_PAGEUP,m_nScrollPos),NULL);
				return TRUE;
			}
		case VK_NEXT:
		case VK_SPACE:
			{
				SendMessage(WM_VSCROLL,MAKELONG(SB_PAGEDOWN,m_nScrollPos),NULL);
				return TRUE;
			}
		case VK_HOME:
			{
				SendMessage(WM_VSCROLL,MAKELONG(SB_TOP,m_nScrollPos),NULL);
				return TRUE;
			}
		case VK_END:
			{
				SendMessage(WM_VSCROLL,MAKELONG(SB_BOTTOM,m_nScrollPos),NULL);
				return TRUE;
			}
		}
	}

	return __super::PreTranslateMessage(pMsg);
}

//������Ϣ
VOID CTableViewFrame::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	//���ý���
	SetFocus();

	//��ȡ����
	CRect rcClient;
	GetClientRect(&rcClient);
	INT nLastPos=m_nScrollPos;

	//�ƶ�����
	switch (nSBCode)
	{
	case SB_TOP:
		{
			m_nScrollPos=0;
			break;
		}
	case SB_BOTTOM:
		{
			m_nScrollPos=m_nScrollPosMax-m_nVisibleLineCount*m_TableResource.m_SizeTable.cy;
			break;
		}
	case SB_LINEUP:
		{
			m_nScrollPos-=SCROLL_POS_LINE;
			break;
		}
	case SB_PAGEUP:
		{
			m_nScrollPos-=rcClient.bottom;
			break;
		}
	case SB_LINEDOWN:
		{
			m_nScrollPos+=SCROLL_POS_LINE;
			break;
		}
	case SB_PAGEDOWN:
		{
			m_nScrollPos+=rcClient.bottom;
			break;
		}
	case SB_THUMBTRACK:
		{
			m_nScrollPos=nPos;
			break;
		}
	}
	
	//����λ��
	m_nScrollPos=__min(m_nScrollPos,m_nScrollPosMax-rcClient.bottom);
	if (m_nScrollPos<0) m_nScrollPos=0;

	//��������
	if (nLastPos!=m_nScrollPos)
	{
		SetScrollPos(SB_VERT,m_nScrollPos);
		ScrollWindow(0,nLastPos-m_nScrollPos,NULL,NULL);
	}

	return;
}

//�����Ϣ
VOID CTableViewFrame::OnLButtonDblClk(UINT nFlags, CPoint MousePoint)
{
	//���ý���
	SetFocus();

	//�¼�֪ͨ
	m_pITableViewFrameSink->OnDButtonHitTable(m_wDownTableID);

	return;
}

//�����Ϣ
LRESULT CTableViewFrame::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	//���ñ���
	m_bHovering=false;

	//���ý���
	if (m_wDownTableID!=INVALID_TABLE) 
	{
		//���ñ���
		WORD wTableID=m_wDownTableID;
		m_wDownTableID=INVALID_TABLE;

		//���ý���
		ITableView * pITableView=GetTableViewItem(wTableID);
		if (pITableView!=NULL) pITableView->SetHoverStatus(INVALID_CHAIR);
	}

	return 0;
}

//�����Ϣ
VOID CTableViewFrame::OnLButtonDown(UINT nFlags, CPoint MousePoint)
{
	//���ý���
	SetFocus();

	//���´���
	if (m_cbButtonHover!=0)
	{
		//����˻�
		SetCapture();

		//���ñ���
		m_cbButtonDown=m_cbButtonHover;

		//���½���
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE);
	}

	//��������
	if ((m_bRMouseDown==false)&&(m_bLMouseDown==false))
	{
		//����˻�
		SetCapture();

		//���ñ���
		m_bLMouseDown=true;
		m_wDownTableID=SwitchTableID(MousePoint);

		//��ȡ����
		ITableView * pITableView=NULL;
		if (m_wDownTableID!=INVALID_TABLE) pITableView=GetTableViewItem(m_wDownTableID);

		//��������
		if (pITableView!=NULL)
		{
			//λ�ö���
			CPoint TablePoint;
			TablePoint.x=(MousePoint.x-m_nXExcursion)%m_TableResource.m_SizeTable.cx;
			TablePoint.y=(MousePoint.y-m_nYExcursion+m_nScrollPos)%m_TableResource.m_SizeTable.cy;

			//��������
			m_wDownChairID=SwitchChairID(TablePoint);

			//�������
			if (m_wDownChairID!=INVALID_CHAIR) 
			{
				pITableView->SetMouseDown(m_bLMouseDown);
				pITableView->SetHoverStatus(m_wDownChairID);
			}
		}
	}

	return;
}

//�����Ϣ
VOID CTableViewFrame::OnLButtonUp(UINT nFlags, CPoint MousePoint)
{
	//�������
	BYTE cbButtonDown=0;
	WORD wViewItemDown=INVALID_WORD;

	//����ж�
	if ((m_bLMouseDown==true)&&(m_bRMouseDown==false))
	{
		//�ͷŲ���
		ReleaseCapture();

		//���ñ���
		m_bLMouseDown=false;

		//��������
		WORD wChairID=INVALID_CHAIR;
		WORD wTableID=SwitchTableID(MousePoint);

		//��������
		if (m_wDownTableID!=INVALID_TABLE)
		{
			//��ȡ����
			ASSERT(GetTableViewItem(m_wDownTableID)!=NULL);
			ITableView * pITableView=GetTableViewItem(m_wDownTableID);

			//����״̬
			pITableView->SetMouseDown(false);
			pITableView->SetHoverStatus(INVALID_CHAIR);
		}

		//��ȡλ��
		if ((wTableID!=INVALID_TABLE)&&(wTableID==m_wDownTableID))
		{
			//��ȡ����
			ASSERT(GetTableViewItem(wTableID)!=NULL);
			ITableView * pITableView=GetTableViewItem(wTableID);

			//����λ��
			CPoint TablePoint;
			TablePoint.x=(MousePoint.x-m_nXExcursion)%m_TableResource.m_SizeTable.cx;
			TablePoint.y=(MousePoint.y-m_nYExcursion+m_nScrollPos)%m_TableResource.m_SizeTable.cy;

			//���Ӳ���
			wChairID=SwitchChairID(TablePoint);
		}

		//�������
		if ((wTableID==m_wDownTableID)&&(wChairID==m_wDownChairID))
		{
			//���ñ���
			m_wDownTableID=INVALID_TABLE;
			m_wDownChairID=INVALID_CHAIR;

			//�¼�֪ͨ
			m_pITableViewFrameSink->OnLButtonHitTable(wTableID,wChairID);
		}
		else
		{
			//���ñ���
			m_wDownTableID=INVALID_TABLE;
			m_wDownChairID=INVALID_CHAIR;
		}
	}

	return;
}

//�����Ϣ
VOID CTableViewFrame::OnRButtonDown(UINT nFlags, CPoint MousePoint)
{
	//���ý���
	SetFocus();

	//��������
	if ((m_bRMouseDown==false)&&(m_bLMouseDown==false))
	{
		//����˻�
		SetCapture();

		//���ñ���
		m_bRMouseDown=true;

		//���Ӳ���
		m_wDownTableID=SwitchTableID(MousePoint);

		//����״̬
		if (m_wDownTableID!=INVALID_TABLE)
		{
			//��ȡ����
			ASSERT(GetTableViewItem(m_wDownTableID)!=NULL);
			ITableView * pITableView=GetTableViewItem(m_wDownTableID);

			//����λ��
			CPoint TablePoint;
			TablePoint.x=(MousePoint.x-m_nXExcursion)%m_TableResource.m_SizeTable.cx;
			TablePoint.y=(MousePoint.y-m_nYExcursion+m_nScrollPos)%m_TableResource.m_SizeTable.cy;

			//���Ӳ���
			m_wDownChairID=SwitchChairID(TablePoint);

			//�������
			if (m_wDownChairID!=INVALID_CHAIR)
			{
				pITableView->SetMouseDown(m_bRMouseDown);
				pITableView->SetHoverStatus(m_wDownChairID);
			}
		}
	}

	return;
}

//�����Ϣ
VOID CTableViewFrame::OnRButtonUp(UINT nFlags, CPoint MousePoint)
{
	if (m_bRMouseDown==true)
	{
		//�ͷŲ���
		ReleaseCapture();

		//���ñ���
		m_bRMouseDown=false;

		//��������
		WORD wChairID=INVALID_CHAIR;
		WORD wTableID=SwitchTableID(MousePoint);

		//����״̬
		if (m_wDownTableID!=INVALID_TABLE)
		{
			//��ȡ����
			ASSERT(GetTableViewItem(m_wDownTableID)!=NULL);
			ITableView * pITableView=GetTableViewItem(m_wDownTableID);

			//����״̬
			pITableView->SetMouseDown(false);
			pITableView->SetHoverStatus(INVALID_CHAIR);
		}

		//��ȡλ��
		if ((wTableID!=INVALID_TABLE)&&(wTableID==m_wDownTableID))
		{
			//��ȡ����
			ASSERT(GetTableViewItem(wTableID)!=NULL);
			ITableView * pITableView=GetTableViewItem(wTableID);

			//����λ��
			CPoint TablePoint;
			TablePoint.x=(MousePoint.x-m_nXExcursion)%m_TableResource.m_SizeTable.cx;
			TablePoint.y=(MousePoint.y+m_nScrollPos)%m_TableResource.m_SizeTable.cy;

			//���Ӳ���
			wChairID=SwitchChairID(TablePoint);
		}

		//�������
		if ((wTableID==m_wDownTableID)&&(wChairID==m_wDownChairID))
		{
			//���ñ���
			m_wDownTableID=INVALID_TABLE;
			m_wDownChairID=INVALID_CHAIR;

			//�¼�֪ͨ
			m_pITableViewFrameSink->OnRButtonHitTable(wTableID,wChairID);
		}
		else
		{
			//���ñ���
			m_wDownTableID=INVALID_TABLE;
			m_wDownChairID=INVALID_CHAIR;
		}
	}

	return;
}

//�����Ϣ
BOOL CTableViewFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint MousePoint)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//���ù���
	INT nLastPos=m_nScrollPos;
	INT nMaxPos=m_nScrollPosMax-rcClient.bottom;
	m_nScrollPos=__max(__min(m_nScrollPos-zDelta/WHELL_TIMES,nMaxPos),0L);

	//��������
	SetScrollPos(SB_VERT,m_nScrollPos);
	ScrollWindow(0,nLastPos-m_nScrollPos,NULL,NULL);

	return TRUE;
}

//�����Ϣ
BOOL CTableViewFrame::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//����λ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��ȡ���
	CPoint Point;
	GetCursorPos(&Point);
	ScreenToClient(&Point);

	//�����ж�
	if (m_bHovering==false)
	{
		//���ñ���
		m_bHovering=true;

		//��������
		TRACKMOUSEEVENT TrackMouseEvent;
		ZeroMemory(&TrackMouseEvent,sizeof(TrackMouseEvent));

		//ע����Ϣ
		TrackMouseEvent.hwndTrack=m_hWnd;
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);

		//ע���¼�
		_TrackMouseEvent(&TrackMouseEvent);
	}

	//״̬�ж�
	if ((m_bRMouseDown==false)&&(m_bLMouseDown==false))
	{
		//��ȡ���
		CPoint MousePoint;
		GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);

		//��������
		WORD wNewChairID=INVALID_CHAIR;
		WORD wNewTableID=SwitchTableID(MousePoint);

		//��ȡ����
		if (wNewTableID!=INVALID_TABLE)
		{
			//��������
			CPoint TablePoint;
			TablePoint.x=(MousePoint.x-m_nXExcursion)%m_TableResource.m_SizeTable.cx;
			TablePoint.y=(MousePoint.y-m_nYExcursion+m_nScrollPos)%m_TableResource.m_SizeTable.cy;

			//��ȡ����
			wNewChairID=SwitchChairID(TablePoint);
		}
		
		//��������
		if (((wNewTableID!=m_wDownTableID)||(wNewChairID!=m_wDownChairID))&&((wNewTableID!=INVALID_CHAIR)||(m_wDownChairID!=INVALID_CHAIR)))
		{
			//��������
			if (m_wDownTableID!=INVALID_TABLE)
			{
				GetTableViewItem(m_wDownTableID)->SetHoverStatus(INVALID_CHAIR);
			}

			//��������
			if ((wNewTableID!=INVALID_TABLE)&&(wNewChairID!=INVALID_CHAIR))
			{
				GetTableViewItem(wNewTableID)->SetHoverStatus(wNewChairID);
			}
		}
		
		//���ñ���
		m_wDownTableID=wNewTableID;
		m_wDownChairID=wNewChairID;

		//���ù��
		if ((m_wDownTableID!=INVALID_TABLE)&&(m_wDownChairID!=INVALID_CHAIR))
		{
			//SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND_CUR)));
			return TRUE;
		}
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//���Ӳ���
WORD CTableViewFrame::SwitchTableID(POINT MousePoint)
{
	//״̬Ч��
	if (m_TableResource.m_SizeTable.cx==0) return INVALID_TABLE;
	if (m_TableResource.m_SizeTable.cy==0) return INVALID_TABLE;

	//λ�ü���
	INT nXItem=(MousePoint.x-m_nXExcursion)/m_TableResource.m_SizeTable.cx;
	INT nYItem=(MousePoint.y-m_nYExcursion+m_nScrollPos)/m_TableResource.m_SizeTable.cy;

	//����ж�
	if ((nYItem>=0)&&(nXItem>=0)&&(nYItem<m_nYTableCount)&&(nXItem<m_nXTableCount))
	{
		//���ý��
		WORD wResultItem=nYItem*m_nXTableCount+nXItem;

		//����ж�
		if (wResultItem<m_wTableCount)
		{
			bool bAvertCheatMode=CServerRule::IsAllowAvertCheatMode(m_dwServerRule);
			if ((wResultItem==0)||(bAvertCheatMode==false)) return wResultItem;
		}
	}

	return INVALID_TABLE;
}

//���Ӳ���
WORD CTableViewFrame::SwitchChairID(POINT MousePoint)
{
	//λ������
	if ((m_TableResource.m_bShowUser==true)||(m_TableResource.m_bShowChair==true))
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//�����ж�
			if (MousePoint.x<m_TableResource.m_rcChairArray[i].left) continue;
			if (MousePoint.x>m_TableResource.m_rcChairArray[i].right) continue;

			//�����ж�
			if (MousePoint.y<m_TableResource.m_rcChairArray[i].top) continue;
			if (MousePoint.y>m_TableResource.m_rcChairArray[i].bottom) continue;

			return i;
		}
	}

	//��������
	if (m_TableResource.m_bShowEnter==true)
	{
		//��������
		bool bOutSide=false;

		CRect rcClient;
		GetClientRect(&rcClient);

		//��������
		CSize SizeTable = m_TableResource.m_SizeTable;
		if(SizeTable.cx > rcClient.Width()) SizeTable.cx=rcClient.Width();

		//�����ж�
		if (MousePoint.x<SizeTable.cx/2+m_TableResource.m_ptEnter.x) bOutSide=true;
		if (MousePoint.x>SizeTable.cx/2+m_TableResource.m_ptEnter.x+m_TableResource.m_SizeEnter.cx) bOutSide=true;

		//�����ж�
		if (MousePoint.y<SizeTable.cy/2+m_TableResource.m_ptEnter.y) bOutSide=true;
		if (MousePoint.y>SizeTable.cy/2+m_TableResource.m_ptEnter.y+m_TableResource.m_SizeEnter.cy) bOutSide=true;

		//����ж�
		if (bOutSide==false)
		{
			return INDEX_ENTER_CHAIR;
		}
	}

	return INVALID_CHAIR;
}

//�����ؼ�
VOID CTableViewFrame::RectifyControl(INT nWidth, INT nHeight)
{
	//����λ��
	if ((m_wTableCount!=0)&&(m_TableResource.m_SizeTable.cx>0L)&&(m_TableResource.m_SizeTable.cy>0L))
	{
		//��������
		INT nViewWidth=__max(m_TableResource.m_SizeTable.cx,nWidth);
		INT nViewHeight=__max(m_TableResource.m_SizeTable.cy,nHeight);

		//��ͼ����
		m_nXTableCount=nViewWidth/m_TableResource.m_SizeTable.cx;
		m_nYTableCount=(m_wTableCount+m_nXTableCount-1)/m_nXTableCount;
		m_nVisibleLineCount=nViewHeight/m_TableResource.m_SizeTable.cy;

		//�������
		bool bAvertCheatMode=CServerRule::IsAllowAvertCheatMode(m_dwServerRule);
		if(bAvertCheatMode)
		{
			m_nYTableCount=1;
			m_nVisibleLineCount=1;
		}

		//ƫ��λ��
		if (m_wTableCount==1 || bAvertCheatMode)
		{
			m_nXExcursion=(nViewWidth-m_TableResource.m_SizeTable.cx)/2;
			m_nYExcursion=__max((m_nVisibleLineCount>=m_nYTableCount)?(nHeight-m_nYTableCount*m_TableResource.m_SizeTable.cy)/2:0,0);
		}
		else
		{
			m_nYExcursion=0L;
			m_nXExcursion=(nViewWidth-m_nXTableCount*m_TableResource.m_SizeTable.cx)/2;
		}
	}
	
	//��������
	m_nScrollPosMax=m_nYTableCount*m_TableResource.m_SizeTable.cy;
	m_nScrollPos=__max(0,__min(m_nScrollPos,m_nScrollPosMax-m_nVisibleLineCount*m_TableResource.m_SizeTable.cy));

	//���ù���
	SCROLLINFO ScrollInfoVert;
	ScrollInfoVert.cbSize=sizeof(ScrollInfoVert);
	ScrollInfoVert.fMask=SIF_RANGE|SIF_PAGE|SIF_POS;
	ScrollInfoVert.nMin=0;
	ScrollInfoVert.nMax=m_nScrollPosMax;
	ScrollInfoVert.nPage=nHeight;
	ScrollInfoVert.nPos=m_nScrollPos;
	SetScrollInfo(SB_VERT,&ScrollInfoVert,FALSE);

	return;
}

//�¼���Ϣ
LRESULT CTableViewFrame::OnMessagePlatformEvent(WPARAM wParam, LPARAM lParam)
{
	//�������
	if (wParam==EVENT_SKIN_CONFIG_UPDATE)
	{
		//��Ⱦ�ж�
		if (m_TableResource.m_bRenderImage==true)
		{
			//������Դ
			CSkinRenderManager * pSkinRenderManager=CSkinRenderManager::GetInstance();
			if (pSkinRenderManager!=NULL) m_TableResource.UpdateResource(pSkinRenderManager);

			//���½���
			RedrawWindow(NULL,NULL,RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASENOW);
		}

		return 0;
	}

	return 0L;
}

//�滭����
VOID CTableViewFrame::DrawNumberString(CDC * pDC, CPngImage &PngNumber, LONG lNumber, INT nXPos, INT nYPos,BYTE cbAlign)
{
	//������Դ
	INT nNumberHeight=PngNumber.GetHeight();
	INT nNumberWidth=PngNumber.GetWidth()/10;

	//������Ŀ
	LONG lNumberCount=0;
	LONG lNumberTemp=lNumber;
	do
	{
		lNumberCount++;
		lNumberTemp/=10;
	} while (lNumberTemp>0);

	//λ�ö���
	INT nYDrawPos=nYPos-nNumberHeight/2;
	INT nXDrawPos=nXPos;
	if(cbAlign==TA_RIGHT)
		nXDrawPos+=(lNumberCount-1)*nNumberWidth;
	else if(cbAlign==TA_CENTER)
		nXDrawPos+=(lNumberCount-1)*nNumberWidth/2;

	//�滭����
	for (LONG i=0;i<lNumberCount;i++)
	{
		//�滭����
		LONG lCellNumber=(LONG)(lNumber%10L);
		PngNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,nNumberWidth,nNumberHeight,lCellNumber*nNumberWidth,0);

		//���ñ���
		lNumber/=10;
		nXDrawPos-=nNumberWidth;
	};

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CMatchViewFrame::CMatchViewFrame()
{
	//״̬����
	m_bHovering=false;
	m_cbButtonDown=0;
	m_cbButtonHover=0;

	//���Ա���	
	m_wTableCount=0;
	m_wChairCount=0;
	m_wServerID=INVALID_WORD;

	//���Ա���
	m_pGameMatch=NULL;
	ZeroMemory(&m_MatchPosition,sizeof(m_MatchPosition));

	//����ָ��
	m_pITableViewFrameSink=NULL;

	//��������
	m_dwTotalUser=0;
	m_dwWaitting=0;
	m_cbMatchStatus=MS_NULL;
	m_cbUserMatchStatus=MUS_NULL;

	m_pBtMatchJoin=NULL;
	m_pBtMatchQuit=NULL;
	m_pBtMatchDetails=NULL;

	return;
}

//��������
CMatchViewFrame::~CMatchViewFrame()
{
	//ɾ������
	for (INT_PTR i=0;i<m_TableViewArray.GetCount();i++)
	{
		CTableView * pTableView=m_TableViewArray[i];
		if (pTableView!=NULL) SafeDelete(pTableView);
	}

	//ɾ������
	m_TableViewArray.RemoveAll();

	//�ͷ���Դ	
	if(m_ImageGround.IsNull()==false) m_ImageGround.Destroy();
	if(m_ImageStatus.IsNull()==false) m_ImageStatus.DestroyImage();	
	if(m_ImageNumber1.IsNull()==false) m_ImageNumber1.DestroyImage();
	if(m_ImageNumber2.IsNull()==false) m_ImageNumber2.DestroyImage();

	//�ͷ���Դ
	if(m_ImageMatchBack.IsNull()==false) m_ImageMatchBack.DestroyImage();
	if(m_ImageMatchText.IsNull()==false) m_ImageMatchText.DestroyImage();
	if(m_ImageMatchShow.IsNull()==false) m_ImageMatchShow.DestroyImage();

	return;
}

//�ӿڲ�ѯ
VOID * CMatchViewFrame::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableViewFrame,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableViewFrame,Guid,dwQueryVer);
	return NULL;
}

//��Ϣ����
void CMatchViewFrame::Notify(TNotifyUI &  msg)
{
	//��ȡ����
	CControlUI * pControlUI = msg.pSender;

	//����¼�
	if (lstrcmp(msg.sType, TEXT("click")) == 0)
	{
		//������ť
		if(pControlUI==m_pBtMatchJoin)
		{
			return m_pITableViewFrameSink->OnHitTableButton(BT_ENTER_GAME);
		}

		//������ť
		if(pControlUI==m_pBtMatchQuit)
		{
			return m_pITableViewFrameSink->OnHitTableButton(BT_LEAVE_MATCH);
		}

		//��Ϸ����
		if(pControlUI==m_pBtMatchDetails)
		{
			//��ȡ����
			ASSERT(CGlobalWebLink::GetInstance()!=NULL);
			CGlobalWebLink * pGlobalWebLink=CGlobalWebLink::GetInstance();			

			//�����ַ
			TCHAR szNavigation[256]=TEXT("");
			_sntprintf(szNavigation,CountArray(szNavigation),TEXT("%s/Match/MatchView.aspx?param=%d&num=%d&type=%d"),pGlobalWebLink->GetPlatformLink(),m_pGameMatch->dwMatchID,m_pGameMatch->dwMatchNO,m_pGameMatch->cbMatchType);

			//��ҳ��
			ShellExecute(NULL,TEXT("OPEN"),szNavigation,NULL,NULL,SW_NORMAL);
		}
	}
}

//��ʼ�滭
void CMatchViewFrame::OnBeginPaintWindow(HDC hDC)
{
	//���� DC
	CDC * pDC=CDC::FromHandle(hDC);

	//���û���
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));
	pDC->SelectObject(m_PaintManager.GetFont(1));

	//��ȡ����
	CRect rcClient;
	GetClientRect(&rcClient);

	//�滭��϶
	if (m_ImageGround.IsNull()==false)
	{
		m_ImageGround.DrawImageTile(pDC,rcClient);
	}
	else
	{
		pDC->FillSolidRect(&rcClient,RGB(0,0,0));
	}

	//����ͼƬ
	if(m_ImageMatchBack.IsNull()==false)
	{
		m_ImageMatchBack.DrawImage(pDC,(rcClient.Width()-m_ImageMatchBack.GetWidth())/2,(rcClient.Height()-m_ImageMatchBack.GetHeight())/2);
	}

	//��������
	INT nXStartPos=rcClient.Width()/2+m_MatchPosition.ptMatchShow.x;
	INT nYStartPos=rcClient.Height()/2+m_MatchPosition.ptMatchShow.y;

	//����չʾ
	if(m_ImageMatchShow.IsNull()==false)
	{
		m_ImageMatchShow.DrawImage(pDC,nXStartPos,nYStartPos);
	}

	//��������
	if(m_ImageMatchText.IsNull()==false)
	{
		m_ImageMatchText.DrawImage(pDC,nXStartPos+m_MatchPosition.ptMatchText.x,nYStartPos+m_MatchPosition.ptMatchText.y);
	}

	//������Ϣ
	if(m_pGameMatch!=NULL)
	{
		//��������
		CRect rcMatchInfo;
		DWORD dwFormat=DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_END_ELLIPSIS;

		//��ʱ����
		if(m_pGameMatch->cbMatchType==MATCH_TYPE_LOCKTIME)
		{
			//����ģʽ
			TCHAR szMatchMode[16]=TEXT("��ʱ��");
			rcMatchInfo=m_MatchPosition.rcMatchMode;
			rcMatchInfo.OffsetRect(nXStartPos,nYStartPos);
			DrawText(hDC,szMatchMode,lstrlen(szMatchMode),rcMatchInfo,dwFormat);
			
			//����ʱ��
			TCHAR szStartTime[32]=TEXT("");
			rcMatchInfo=m_MatchPosition.rcMatchStartTime;
			rcMatchInfo.OffsetRect(nXStartPos,nYStartPos);
			_sntprintf(szStartTime,CountArray(szStartTime),TEXT("%02d:%02d:%02d"),m_pGameMatch->MatchStartTime.wHour,m_pGameMatch->MatchStartTime.wMinute,m_pGameMatch->MatchStartTime.wSecond);
			DrawText(hDC,szStartTime,lstrlen(szStartTime),rcMatchInfo,dwFormat);

			//����ʱ��
			TCHAR szEndTime[32]=TEXT("");
			rcMatchInfo=m_MatchPosition.rcMatchEndTime;
			rcMatchInfo.OffsetRect(nXStartPos,nYStartPos);
			_sntprintf(szEndTime,CountArray(szEndTime),TEXT("%02d:%02d:%02d"),m_pGameMatch->MatchEndTime.wHour,m_pGameMatch->MatchEndTime.wMinute,m_pGameMatch->MatchEndTime.wSecond);
			DrawText(hDC,szEndTime,lstrlen(szEndTime),rcMatchInfo,dwFormat);	

			//��������
			TCHAR szMatchFee[32]=TEXT("");
			rcMatchInfo=m_MatchPosition.rcMatchFee;
			rcMatchInfo.OffsetRect(nXStartPos,nYStartPos);
			_sntprintf(szMatchFee,CountArray(szMatchFee),SCORE_STRING TEXT("%s"),m_pGameMatch->lMatchFee,m_pGameMatch->cbMatchFeeType==MATCH_FEE_TYPE_GOLD?TEXT("��Ϸ��"):TEXT("Ԫ��"));
			DrawText(hDC,szMatchFee,lstrlen(szMatchFee),rcMatchInfo,dwFormat);	

			//��������
			rcMatchInfo=m_MatchPosition.rcMatchCondition;
			rcMatchInfo.OffsetRect(nXStartPos,nYStartPos);
			LPCTSTR pszMatchCondition[]={TEXT("�������"),TEXT("�����Ա"),TEXT("�����Ա"),TEXT("�����Ա"),TEXT("�����Ա"),TEXT("VIP��Ա"),TEXT("������Ա")};						
			//��������		
			if(m_pGameMatch->cbMemberOrder<CountArray(pszMatchCondition))
			{
				BYTE cbMemberOrder=m_pGameMatch->cbMemberOrder;
				DrawText(hDC,pszMatchCondition[cbMemberOrder],lstrlen(pszMatchCondition[cbMemberOrder]),rcMatchInfo,dwFormat);
			}

			//���پ���
			TCHAR szMinPlayCount[16]=TEXT("");
			rcMatchInfo=m_MatchPosition.rcMinPlayCount;
			rcMatchInfo.OffsetRect(nXStartPos,nYStartPos);
			_sntprintf(szMinPlayCount,CountArray(szMinPlayCount),TEXT("%d ��"),m_pGameMatch->wMatchPlayCount);
			DrawText(hDC,szMinPlayCount,lstrlen(szMinPlayCount),rcMatchInfo,dwFormat);

			//��������ʱ
			if(m_ImageNumber1.IsNull()==false)
			{
				//��������
				SYSTEMTIME StartTime;	
				CTime CurrentTime = CTime::GetCurrentTime();		
				StartTime = m_pGameMatch->MatchStartTime;

				//����ʱ���
				DWORD dwCurrentStamp = CurrentTime.GetHour()*3600+CurrentTime.GetMinute()*60+CurrentTime.GetSecond();
				DWORD dwStartStamp = StartTime.wHour*3600+StartTime.wMinute*60+StartTime.wSecond;
				DWORD dwDiffStemp = dwStartStamp>dwCurrentStamp?dwStartStamp-dwCurrentStamp:0;

				//����ʱ��
				DWORD wDiffHour = dwDiffStemp/3600;
				DWORD wDiffMinute = (dwDiffStemp-wDiffHour*3600)/60;
				DWORD wDiffSecond = dwDiffStemp-wDiffHour*3600-wDiffMinute*60;

				//״̬�ж�
				if(m_cbMatchStatus==MS_NULL)			//����δ��ʼ
				{
					//��������
					TCHAR szFormatNum[16]=TEXT("");
					_sntprintf(szFormatNum,CountArray(szFormatNum),TEXT("%02d:%02d:%02d"),wDiffHour,wDiffMinute,wDiffSecond);

					//��������
					DrawNumber(pDC,nXStartPos+m_MatchPosition.ptNumber1.x,nYStartPos+m_MatchPosition.ptNumber1.y,&m_ImageNumber1,TEXT(":0123456789"),0,szFormatNum);
				}
				else if(m_cbMatchStatus==MS_MATCHING)	//����������	
				{
					//��������
					DrawWords(pDC,nXStartPos+m_MatchPosition.ptNumber1.x,nYStartPos+m_MatchPosition.ptNumber1.y,&m_ImageStatus,TEXT("�����������ѽ���"),TEXT("����������"));
				} 
				else if(m_cbMatchStatus==MS_MATCHEND)	//�����ѽ���
				{
					//��������
					DrawWords(pDC,nXStartPos+m_MatchPosition.ptNumber1.x,nYStartPos+m_MatchPosition.ptNumber1.y,&m_ImageStatus,TEXT("�����������ѽ���"),TEXT("�����ѽ���"));
				}
			}

			//��ǰ����
			if(m_ImageNumber2.IsNull()==false)
			{
				DrawNumber(pDC,nXStartPos+m_MatchPosition.ptNumber2.x,nYStartPos+m_MatchPosition.ptNumber2.y,&m_ImageNumber2,TEXT("0123456789"),m_dwWaitting,TEXT("%d"),DT_CENTER);
			}
		}

		//��ʱ����
		if(m_pGameMatch->cbMatchType==MATCH_TYPE_IMMEDIATE)
		{
			//����ģʽ
			TCHAR szMatchMode[16]=TEXT("��ʱ��");
			rcMatchInfo=m_MatchPosition.rcMatchMode;
			rcMatchInfo.OffsetRect(nXStartPos,nYStartPos);
			DrawText(hDC,szMatchMode,lstrlen(szMatchMode),rcMatchInfo,dwFormat);

			//��������
			TCHAR szStartUserCount[32]=TEXT("");
			rcMatchInfo=m_MatchPosition.rcStartCount;
			rcMatchInfo.OffsetRect(nXStartPos,nYStartPos);
			_sntprintf(szStartUserCount,CountArray(szStartUserCount),TEXT("��%d�˿���"),m_pGameMatch->wStartUserCount);
			DrawText(hDC,szStartUserCount,lstrlen(szStartUserCount),rcMatchInfo,dwFormat);

			//��������
			TCHAR szMatchFee[32]=TEXT("");
			rcMatchInfo=m_MatchPosition.rcMatchFee;
			rcMatchInfo.OffsetRect(nXStartPos,nYStartPos);
			_sntprintf(szMatchFee,CountArray(szMatchFee),SCORE_STRING TEXT("%s"),m_pGameMatch->lMatchFee,m_pGameMatch->cbMatchFeeType==MATCH_FEE_TYPE_GOLD?TEXT("��Ϸ��"):TEXT("Ԫ��"));
			DrawText(hDC,szMatchFee,lstrlen(szMatchFee),rcMatchInfo,dwFormat);	

			//��������
			rcMatchInfo=m_MatchPosition.rcMatchCondition;
			rcMatchInfo.OffsetRect(nXStartPos,nYStartPos);
			LPCTSTR pszMatchCondition[]={TEXT("�������"),TEXT("�����Ա"),TEXT("�����Ա"),TEXT("�����Ա"),TEXT("�����Ա"),TEXT("VIP��Ա"),TEXT("������Ա")};						
			if(m_pGameMatch->cbMemberOrder<CountArray(pszMatchCondition))
			{
				BYTE cbMemberOrder=m_pGameMatch->cbMemberOrder;
				DrawText(hDC,pszMatchCondition[cbMemberOrder],lstrlen(pszMatchCondition[cbMemberOrder]),rcMatchInfo,dwFormat);
			}

			//��Ϸ����
			TCHAR szPlayCount[16]=TEXT("");
			rcMatchInfo=m_MatchPosition.rcPalyCount;
			rcMatchInfo.OffsetRect(nXStartPos,nYStartPos);
			_sntprintf(szPlayCount,CountArray(szPlayCount),TEXT("%d ��"),m_pGameMatch->wMatchPlayCount);
			DrawText(hDC,szPlayCount,lstrlen(szPlayCount),rcMatchInfo,dwFormat);

			//��������
			if(m_ImageNumber1.IsNull()==false)
			{
				DrawNumber(pDC,nXStartPos+m_MatchPosition.ptNumber1.x,nYStartPos+m_MatchPosition.ptNumber1.y,&m_ImageNumber1,TEXT("0123456789"),m_dwWaitting,TEXT("%d"),DT_CENTER);
			}

			//�ȴ�����
			if(m_ImageNumber1.IsNull()==false)
			{
				DrawNumber(pDC,nXStartPos+m_MatchPosition.ptNumber2.x,nYStartPos+m_MatchPosition.ptNumber2.y,&m_ImageNumber1,TEXT("0123456789"),m_dwTotalUser-m_dwWaitting,TEXT("%d"),DT_CENTER);
			}
		}
	}
}

//��������
bool CMatchViewFrame::CreateTableFrame(CWnd * pParentWnd, UINT uWndID, IUnknownEx * pIUnknownEx)
{
	//���ñ���
	m_bHovering=false;	
	m_cbButtonDown=0;
	m_cbButtonHover=0;

	//���ýӿ�
	ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableViewFrameSink)!=NULL);
	m_pITableViewFrameSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableViewFrameSink);

	//��������
	CRect rcCreate(0,0,0,0);
	Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcCreate,pParentWnd,uWndID);

	return true;
}

//���ú���
bool CMatchViewFrame::ConfigTableFrame(WORD wTableCount, WORD wChairCount, DWORD dwServerRule, WORD wServerType, WORD wServerID,LPCTSTR pszResDirectory)
{
	//Ч�����
	ASSERT(wChairCount<=MAX_CHAIR);
	ASSERT((pszResDirectory!=NULL)&&(pszResDirectory[0]!=0));

	//���ñ���
	m_wServerID=wServerID;
	m_wTableCount=wTableCount;
	m_wChairCount=wChairCount;
	m_TableViewArray.SetSize(m_wTableCount);
	ZeroMemory(m_TableViewArray.GetData(),m_wTableCount*sizeof(CTableView *));

	//��������
	for (WORD i=0;i<m_wTableCount;i++)
	{
		m_TableViewArray[i]=new CTableView;
		m_TableViewArray[i]->InitTableView(i,wChairCount,this);
	}

	//������Դ
	if (LoadResource(pszResDirectory)==false)
	{
		return false;
	}

	//���ö�ʱ��
	if(m_pGameMatch->cbMatchType==MATCH_TYPE_LOCKTIME && m_ImageNumber1.IsNull()==false)
	{
		SetTimer(IDI_UPDATE_TIME,TIME_UPDATE_TIME,NULL);
	}

	return true;
}

//��ͼ����
bool CMatchViewFrame::OnEventViewData(WORD wCmdID, VOID * pData, WORD wDataSize)
{
	switch(wCmdID)
	{
	case SUB_GR_MATCH_NUM:			//��������
		{
			//����Ч��
			ASSERT(wDataSize==sizeof(CMD_GR_Match_Num));
			if(wDataSize!=sizeof(CMD_GR_Match_Num)) return false;

			//��ȡ����
			CMD_GR_Match_Num *pMatchNum=(CMD_GR_Match_Num*)pData;

			//���ñ���
			m_dwTotalUser=pMatchNum->dwTotal;
			m_dwWaitting=pMatchNum->dwWaitting;
			
			//���½���
			InvalidateRect(NULL);

			return true;
		}
	case SUB_GR_MATCH_STATUS:		//����״̬
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(BYTE));
			if(wDataSize!=sizeof(BYTE)) return false;

			//����״̬
			SetMatchStatus(*(BYTE*)pData);

			return true;
		}
	case SUB_GR_USER_MATCH_STATUS:	//��Ҳ���״̬
		{
			ASSERT(wDataSize==sizeof(CMD_GR_UserMatchStatus));
			if (wDataSize!=sizeof(CMD_GR_UserMatchStatus)) return false;

			//��ȡ����
			CMD_GR_UserMatchStatus *pUserMatchStatus=(CMD_GR_UserMatchStatus*)pData;

			SetUserMatchStatus(pUserMatchStatus->cbCurrStatus);

			return true;
		}
	}

	return false;
}

//��ȡ�û�
IClientUserItem * CMatchViewFrame::GetClientUserItem(WORD wTableID, WORD wChairID)
{
	//��ȡ����
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//��ȡ�û�
	if (pITableView!=NULL)
	{
		return pITableView->GetClientUserItem(wChairID);
	}

	return NULL;
}

//������Ϣ
bool CMatchViewFrame::SetClientUserItem(WORD wTableID, WORD wChairID, IClientUserItem * pIClientUserItem)
{
	ITableView * pITableView=GetTableViewItem(wTableID);
	if (pITableView!=NULL) pITableView->SetClientUserItem(wChairID,pIClientUserItem);
	return true;
}

//��Ϸ��־
bool CMatchViewFrame::GetPlayFlag(WORD wTableID)
{
	//��ȡ����
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//��ȡ��־
	if (pITableView!=NULL)
	{
		return pITableView->GetPlayFlag();
	}

	return false;
}

//�����־
bool CMatchViewFrame::GetLockerFlag(WORD wTableID)
{
	//��ȡ����
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//��ȡ��־
	if (pITableView!=NULL)
	{
		return pITableView->GetLockerFlag();
	}

	return false;
}

//������
VOID CMatchViewFrame::SetFocusFrame(WORD wTableID, bool bFocusFrame)
{
	//��ȡ����
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//���ñ�־
	if (pITableView!=NULL) pITableView->SetFocusFrame(bFocusFrame);

	return;
}

//����״̬ 
VOID CMatchViewFrame::SetTableStatus(WORD wTableID, bool bPlaying, bool bLocker,LONG lCellScore)
{
	//��ȡ����
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//���ñ�־
	if (pITableView!=NULL) pITableView->SetTableStatus(bPlaying,bLocker,lCellScore);

	return;
}

//����״̬ 
VOID CMatchViewFrame::SetTableStatus(bool bWaitDistribute)
{
	return;
}

//���ӿ���
bool CMatchViewFrame::VisibleTable(WORD wTableID)
{
	return true;
}

//��������
bool CMatchViewFrame::FlashGameTable(WORD wTableID)
{
	return true;
}

//��������
bool CMatchViewFrame::FlashGameChair(WORD wTableID, WORD wChairID)
{
	return true;
}

//��������
bool CMatchViewFrame::UpdateTableView(WORD wTableID)
{
	return true;
}

//��ȡ����
ITableView * CMatchViewFrame::GetTableViewItem(WORD wTableID)
{
	//��ȡ����
	if (wTableID!=INVALID_TABLE)
	{
		//Ч�����
		ASSERT(wTableID<m_TableViewArray.GetCount());
		if (wTableID>=m_TableViewArray.GetCount()) return NULL;

		//��ȡ����
		ITableView * pITableView=m_TableViewArray[wTableID];

		return pITableView;
	}

	return NULL;
}

//��������
WORD CMatchViewFrame::GetNullChairCount(WORD wTableID, WORD & wNullChairID)
{
	//��ȡ����
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//��ȡ״̬
	if (pITableView!=NULL)
	{
		return pITableView->GetNullChairCount(wNullChairID);
	}

	return 0;
}

//������Ϣ
VOID CMatchViewFrame::OnNcDestroy()
{
	//�رն�ʱ��
	KillTimer(IDI_UPDATE_TIME);

	//ɾ������
	for (INT_PTR i=0;i<m_TableViewArray.GetCount();i++)
	{
		CTableView * pTableView=m_TableViewArray[i];
		if (pTableView!=NULL) SafeDelete(pTableView);
	}

	//ɾ������
	m_TableViewArray.RemoveAll();

	//�ͷ���Դ	
	if(m_ImageGround.IsNull()==false) m_ImageGround.Destroy();
	if(m_ImageStatus.IsNull()==false) m_ImageStatus.DestroyImage();	
	if(m_ImageNumber1.IsNull()==false) m_ImageNumber1.DestroyImage();
	if(m_ImageNumber2.IsNull()==false) m_ImageNumber2.DestroyImage();

	//�ͷ���Դ
	if(m_ImageMatchBack.IsNull()==false) m_ImageMatchBack.DestroyImage();
	if(m_ImageMatchText.IsNull()==false) m_ImageMatchText.DestroyImage();
	if(m_ImageMatchShow.IsNull()==false) m_ImageMatchShow.DestroyImage();

    return __super::OnNcDestroy();
}

//λ����Ϣ
VOID CMatchViewFrame::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//�����ؼ�
	RectifyControl(cx,cy);

	return;
}

//�ж�Ŀ¼
bool CMatchViewFrame::IsDirectoryExist(LPCTSTR pszDirectofy)
{
	//��������
    WIN32_FIND_DATA wFindData;
    bool bValue=false;

	//����Ŀ¼
    HANDLE hFile = FindFirstFile(pszDirectofy, &wFindData);
    if ((hFile != INVALID_HANDLE_VALUE) && (wFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
       bValue=true;   
    }

	//�رվ��
    FindClose(hFile);

    return bValue;
}

//������Դ
bool CMatchViewFrame::LoadResource(LPCTSTR pszResDirectory)
{
	//��ȡ����
	CServerListData * pServerListData=CServerListData::GetInstance();
	CGameServerItem * pGameServerItem = pServerListData->SearchGameServer(m_wServerID);
	if(pGameServerItem==NULL) return false;

	//������Ϣ
	m_pGameMatch = &pGameServerItem->m_GameMatch;

	//����Ŀ¼
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//����Ŀ¼
	TCHAR szMatchDirectory[MAX_PATH]=TEXT("");
	_sntprintf(szMatchDirectory,CountArray(szMatchDirectory),TEXT("%s\\%s\\MatchMode_%d_%d"),szDirectory,pszResDirectory,m_pGameMatch->dwMatchID,m_pGameMatch->dwMatchNO);

	//�ж�Ŀ¼
	if(IsDirectoryExist(szMatchDirectory)==false) return false;

	//����·��
	TCHAR szParameterFile[MAX_PATH]=TEXT("");
	_sntprintf(szParameterFile,CountArray(szParameterFile),TEXT("%s\\MatchResource.INI"),szMatchDirectory);

	//��������
	CTableResource TableResource;

	//λ��ģʽ
	m_MatchPosition.cbPostionMode=GetPrivateProfileInt(TEXT("Attribute"),TEXT("PositionMode"),1L,szParameterFile);

	//����ģʽ
	if(m_MatchPosition.cbPostionMode==MATCH_PM_ABSOLUTE)
	{
		//λ������
		TableResource.ReadPoint(m_MatchPosition.ptMatchShow,TEXT("Position"),TEXT("Point_MatchShow"),szParameterFile);
		TableResource.ReadPoint(m_MatchPosition.ptMatchText,TEXT("Position"),TEXT("Point_MatchText"),szParameterFile);

		//λ������
		TableResource.ReadPoint(m_MatchPosition.ptMatchJoin,TEXT("Position"),TEXT("Point_MatchJoin"),szParameterFile);
		TableResource.ReadPoint(m_MatchPosition.ptMatchDetails,TEXT("Position"),TEXT("Point_MatchDetails"),szParameterFile);

		//λ������
		TableResource.ReadPoint(m_MatchPosition.ptNumber1,TEXT("Position"),TEXT("Point_Number1"),szParameterFile);
		TableResource.ReadPoint(m_MatchPosition.ptNumber2,TEXT("Position"),TEXT("Point_Number2"),szParameterFile);	

		//λ������
		TableResource.ReadRect(m_MatchPosition.rcMatchMode,TEXT("Position"),TEXT("Rect_MatchMode"),szParameterFile);
		TableResource.ReadRect(m_MatchPosition.rcMatchFee,TEXT("Position"),TEXT("Rect_MatchFee"),szParameterFile);		
		TableResource.ReadRect(m_MatchPosition.rcStartCount,TEXT("Position"),TEXT("Rect_StartCount"),szParameterFile);
		TableResource.ReadRect(m_MatchPosition.rcPalyCount,TEXT("Position"),TEXT("Rect_PlayCount"),szParameterFile);
		TableResource.ReadRect(m_MatchPosition.rcMinPlayCount,TEXT("Position"),TEXT("Rect_MinPlayCount"),szParameterFile);			TableResource.ReadRect(m_MatchPosition.rcMatchEndTime,TEXT("Position"),TEXT("Rect_MatchEndTime"),szParameterFile);
		TableResource.ReadRect(m_MatchPosition.rcMatchStartTime,TEXT("Position"),TEXT("Rect_MatchStartTime"),szParameterFile);
		TableResource.ReadRect(m_MatchPosition.rcMatchCondition,TEXT("Position"),TEXT("Rect_MatchCondition"),szParameterFile);
	}

	//���ģʽ.
	if(m_MatchPosition.cbPostionMode==MATCH_PM_RELATIVE)
	{
		//��������
		CPoint ptInfoOffset;

		//ƫ��λ��
		TableResource.ReadPoint(ptInfoOffset,TEXT("Position"),TEXT("Point_InfoOffset"),szParameterFile);

		//λ������
		TableResource.ReadPoint(m_MatchPosition.ptMatchShow,TEXT("Position"),TEXT("Point_MatchShow"),szParameterFile);
		TableResource.ReadPoint(m_MatchPosition.ptMatchText,TEXT("Position"),TEXT("Point_MatchText"),szParameterFile);	

		//λ������
		TableResource.ReadPoint(m_MatchPosition.ptMatchJoin,TEXT("Position"),TEXT("Point_MatchJoin"),szParameterFile);
		TableResource.ReadPoint(m_MatchPosition.ptMatchDetails,TEXT("Position"),TEXT("Point_MatchDetails"),szParameterFile);

		//λ������
		TableResource.ReadPoint(m_MatchPosition.ptNumber1,TEXT("Position"),TEXT("Point_Number1"),szParameterFile);
		TableResource.ReadPoint(m_MatchPosition.ptNumber2,TEXT("Position"),TEXT("Point_Number2"),szParameterFile);	

		//λ������
		TableResource.ReadRect(m_MatchPosition.rcMatchMode,TEXT("Position"),TEXT("Rect_MatchMode"),szParameterFile);
		
		//��������
		CRect rcMatchInfo=m_MatchPosition.rcMatchMode;

		//��ʱ����
		if(m_pGameMatch->cbMatchType==MATCH_TYPE_LOCKTIME)
		{
			//��ʼʱ��
			rcMatchInfo.OffsetRect(ptInfoOffset.x,ptInfoOffset.y);
			m_MatchPosition.rcMatchStartTime=rcMatchInfo;

			//����ʱ��
			rcMatchInfo.OffsetRect(ptInfoOffset.x,ptInfoOffset.y);
			m_MatchPosition.rcMatchEndTime=rcMatchInfo;

			//��������
			rcMatchInfo.OffsetRect(ptInfoOffset.x,ptInfoOffset.y);
			m_MatchPosition.rcMatchFee=rcMatchInfo;

			//��������
			rcMatchInfo.OffsetRect(ptInfoOffset.x,ptInfoOffset.y);
			m_MatchPosition.rcMatchCondition=rcMatchInfo;

			//���پ���
			rcMatchInfo.OffsetRect(ptInfoOffset.x,ptInfoOffset.y);
			m_MatchPosition.rcMinPlayCount=rcMatchInfo;
		}

		//��ʱ����
		if(m_pGameMatch->cbMatchType==MATCH_TYPE_IMMEDIATE)
		{
			//��������
			rcMatchInfo.OffsetRect(ptInfoOffset.x,ptInfoOffset.y);
			m_MatchPosition.rcStartCount=rcMatchInfo;

			//��������
			rcMatchInfo.OffsetRect(ptInfoOffset.x,ptInfoOffset.y);
			m_MatchPosition.rcMatchFee=rcMatchInfo;

			//��������
			rcMatchInfo.OffsetRect(ptInfoOffset.x,ptInfoOffset.y);
			m_MatchPosition.rcMatchCondition=rcMatchInfo;

			//��������
			rcMatchInfo.OffsetRect(ptInfoOffset.x,ptInfoOffset.y);
			m_MatchPosition.rcPalyCount=rcMatchInfo;
		}
	}

	//��������
	TCHAR szImagePath[MAX_PATH]=TEXT("");

	//��Դ���
	HINSTANCE hInstance=::AfxGetResourceHandle();

	//����ͼƬ
	_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\Ground.bmp"),szMatchDirectory);
	if (FAILED(m_ImageGround.Load(szImagePath)))
	{
		//m_ImageGround.LoadFromResource(hInstance,IDB_TABLE_GROUND);
	}	

	//����ͼƬ
	_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\IMAGE_MATCH_ BACK.png"),szMatchDirectory);
	if (m_ImageMatchBack.LoadImage(szImagePath)==false) return false;

    //չʾͼƬ
	_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\IMAGE_MATCH_SHOW.png"),szMatchDirectory);
	if (m_ImageMatchShow.LoadImage(szImagePath)==false) return false;

	//����ͼƬ
	_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\IMAGE_MATCH_TEXT.png"),szMatchDirectory);
	if (m_ImageMatchText.LoadImage(szImagePath)==false) return false;

	//����1ͼƬ
	_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\IMAGE_NUMBER1.png"),szMatchDirectory);
	if (m_ImageNumber1.LoadImage(szImagePath)==false) return false;

	//����2ͼƬ
	_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\IMAGE_NUMBER2.png"),szMatchDirectory);
	if (m_ImageNumber2.LoadImage(szImagePath)==false) return false;

	//״̬����
	_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\IMAGE_WORDS.png"),szMatchDirectory);
	if (m_ImageStatus.LoadImage(szImagePath)==false) return false;

	//������ť
	bool bSuccess=false;
	bSuccess=CreateControlUI(pszResDirectory);	

	return bSuccess;
}

//�����ؼ�
bool CMatchViewFrame::CreateControlUI(LPCTSTR pszResDirectory)
{
	//����Ŀ¼
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//��ϷĿ¼
	TCHAR szGameDirectory[MAX_PATH]=TEXT("");
	_sntprintf(szGameDirectory,CountArray(szGameDirectory),TEXT("%s\\%s"),szDirectory,pszResDirectory);

	//����Ŀ¼
	TCHAR szMatchDirectory[MAX_PATH]=TEXT("");
	_sntprintf(szMatchDirectory,CountArray(szMatchDirectory),TEXT("MatchMode_%d_%d"),m_pGameMatch->dwMatchID,m_pGameMatch->dwMatchNO);

	//����·��
	CStdString strResourcePath = m_PaintManager.GetResourcePath();
	m_PaintManager.SetResourcePath(szGameDirectory);

	//��ȡ����
	CContainerUI * pParent = static_cast<CContainerUI *>(m_PaintManager.GetRoot());

	//��������
	TCHAR szButtonImage[MAX_PATH]=TEXT("");

	//������ť
	m_pBtMatchJoin = CButtonUI::Create(&m_PaintManager,pParent,TEXT(""));
	m_pBtMatchQuit = CButtonUI::Create(&m_PaintManager,pParent,TEXT(""));
	m_pBtMatchDetails = CButtonUI::Create(&m_PaintManager,pParent,TEXT(""));

	//����ͼƬ
	_sntprintf(szButtonImage,CountArray(szButtonImage),TEXT("%s\\BT_MATCH_JOIN.png"),szMatchDirectory);
	m_pBtMatchJoin->SetStatusImage(szButtonImage);

	//����ͼƬ
	_sntprintf(szButtonImage,CountArray(szButtonImage),TEXT("%s\\BT_MATCH_QUIT.png"),szMatchDirectory);
	m_pBtMatchQuit->SetStatusImage(szButtonImage);

	//����ͼƬ
	_sntprintf(szButtonImage,CountArray(szButtonImage),TEXT("%s\\BT_MATCH_DETAILS.png"),szMatchDirectory);
	m_pBtMatchDetails->SetStatusImage(szButtonImage);

	//��ԭ·��
	m_PaintManager.SetResourcePath(strResourcePath);

	//�����ؼ�
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyControl(rcClient.Width(),rcClient.Height());

	return true;
}

//����״̬
VOID CMatchViewFrame::SetMatchStatus(BYTE cbMatchStatus)
{
	//����״̬
	m_cbMatchStatus=cbMatchStatus;

	if (m_cbMatchStatus==MS_MATCHEND)
	{
		CServerListData *pServerListData = CServerListData::GetInstance();
		if (pServerListData!=NULL)
		{
			CGameServerItem *pGameServerItem = pServerListData->SearchGameServer(m_wServerID);
			if (pGameServerItem!=NULL)
			{
				pGameServerItem->m_bSignuped=false;
			}
		}
	}

	//���½���
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyControl(rcClient.Width(),rcClient.Height());

	return;
}

//��Ҳ���״̬
VOID CMatchViewFrame::SetUserMatchStatus(BYTE cbUserMatchStatus)
{
	//����״̬
	m_cbUserMatchStatus=cbUserMatchStatus;

	if (m_cbUserMatchStatus==MUS_NULL)
	{
		CServerListData *pServerListData = CServerListData::GetInstance();
		if (pServerListData!=NULL)
		{
			CGameServerItem *pGameServerItem = pServerListData->SearchGameServer(m_wServerID);
			if (pGameServerItem!=NULL)
			{
				pGameServerItem->m_bSignuped=false;
			}
		}
	}

	//���½���
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyControl(rcClient.Width(),rcClient.Height());

	return;
}

//�����ؼ�
VOID CMatchViewFrame::RectifyControl(INT nWidth, INT nHeight)
{
	//�ж�ָ��
	if(m_pGameMatch==NULL) return;

	//��ʱ����
	if(m_pGameMatch->cbMatchType==MATCH_TYPE_LOCKTIME)
	{
		//��������
		m_pBtMatchJoin->SetVisible(true);
		m_pBtMatchQuit->SetVisible(false);
		m_pBtMatchDetails->SetVisible(true);
	}

	//��ʱ����
	if(m_pGameMatch->cbMatchType==MATCH_TYPE_IMMEDIATE)
	{
		//��������
		m_pBtMatchDetails->SetVisible(true);
		m_pBtMatchJoin->SetVisible(m_cbUserMatchStatus==MUS_NULL);		
		m_pBtMatchQuit->SetVisible(m_cbUserMatchStatus==MUS_SIGNUP||m_cbUserMatchStatus==MUS_MATCHING);	

		//ʹ������		
		m_pBtMatchDetails->SetEnabled(true);
		m_pBtMatchJoin->SetEnabled(m_pBtMatchJoin->IsVisible());		
		m_pBtMatchQuit->SetEnabled(m_cbUserMatchStatus==MUS_SIGNUP);	
	}

	//��������
	INT nXStartPos=nWidth/2+m_MatchPosition.ptMatchShow.x;
	INT nYStartPos=nHeight/2+m_MatchPosition.ptMatchShow.y;

	//�������
	CRect rcMatchJoin;
	rcMatchJoin.left=nXStartPos+m_MatchPosition.ptMatchJoin.x;
	rcMatchJoin.top=nYStartPos+m_MatchPosition.ptMatchJoin.y;
	rcMatchJoin.right=rcMatchJoin.left+m_pBtMatchJoin->GetFixedWidth();
	rcMatchJoin.bottom=rcMatchJoin.top+m_pBtMatchJoin->GetFixedHeight();

	//��������
	CRect rcMatchDetails;
	rcMatchDetails.left=nXStartPos+m_MatchPosition.ptMatchDetails.x;
	rcMatchDetails.top=nYStartPos+m_MatchPosition.ptMatchDetails.y;
	rcMatchDetails.right=rcMatchDetails.left+m_pBtMatchDetails->GetFixedWidth();
	rcMatchDetails.bottom=rcMatchDetails.top+m_pBtMatchDetails->GetFixedHeight();

	//����λ��
	m_pBtMatchJoin->SetPos(rcMatchJoin);
	m_pBtMatchQuit->SetPos(rcMatchJoin);
	m_pBtMatchDetails->SetPos(rcMatchDetails);

	return;
}

//��������
VOID CMatchViewFrame::DrawWords(CDC * pDC,INT nXPos,INT nYPos,CPngImage * pWordsImage,LPCTSTR pszWords,LPCTSTR pszText,UINT nFormat)
{
	//����У��
	if(pWordsImage==NULL) return;

	//��������
	CString strWords(pszWords);
	CSize SizeWord(pWordsImage->GetWidth()/strWords.GetLength(),pWordsImage->GetHeight());

	//��������	
	TCHAR szText[32]=TEXT("");
	lstrcpyn(szText,pszText,CountArray(szText));

	//����������
	if((nFormat&DT_LEFT)!=0) nXPos -= 0;
	if((nFormat&DT_CENTER)!=0) nXPos -= (lstrlen(szText)*SizeWord.cx)/2;
	if((nFormat&DT_RIGHT)!=0) nXPos -= lstrlen(szText)*SizeWord.cx;
	
	//����������
	if((nFormat&DT_TOP)!=0) nYPos -= 0;
	if((nFormat&DT_VCENTER)!=0) nYPos -= SizeWord.cy/2;
	if((nFormat&DT_BOTTOM)!=0) nYPos -= SizeWord.cy;

	//�滭����
	INT nIndex=0;
	for(INT i=0; i<lstrlen(szText);i++)
	{
		nIndex = strWords.Find(szText[i]);
		if(nIndex!=-1)
		{
			pWordsImage->DrawImage(pDC,nXPos,nYPos,SizeWord.cx,SizeWord.cy,SizeWord.cx*nIndex,0,SizeWord.cx,SizeWord.cy);
			nXPos += SizeWord.cx;
		}
	}

	return;
}

//��������
VOID CMatchViewFrame::DrawNumber(CDC * pDC,INT nXPos,INT nYPos,CPngImage * pNumberImage,LPCTSTR pszNumber,INT nNumber,LPCTSTR pszFormat,UINT nFormat)
{
	//����У��
	if(pNumberImage==NULL) return;

	//��������
	CString strNumber(pszNumber);
	CSize SizeNumber(pNumberImage->GetWidth()/strNumber.GetLength(),pNumberImage->GetHeight());

	//��������	
	TCHAR szValue[32]=TEXT("");
	_sntprintf(szValue,CountArray(szValue),pszFormat,nNumber);

	//����������
	if((nFormat&DT_LEFT)!=0) nXPos -= 0;
	if((nFormat&DT_CENTER)!=0) nXPos -= (lstrlen(szValue)*SizeNumber.cx)/2;
	if((nFormat&DT_RIGHT)!=0) nXPos -= lstrlen(szValue)*SizeNumber.cx;
	
	//����������
	if((nFormat&DT_TOP)!=0) nYPos -= 0;
	if((nFormat&DT_VCENTER)!=0) nYPos -= SizeNumber.cy/2;
	if((nFormat&DT_BOTTOM)!=0) nYPos -= SizeNumber.cy;

	//�滭����
	INT nIndex=0;
	for(INT i=0; i<lstrlen(szValue);i++)
	{
		nIndex = strNumber.Find(szValue[i]);
		if(nIndex!=-1)
		{
			pNumberImage->DrawImage(pDC,nXPos,nYPos,SizeNumber.cx,SizeNumber.cy,SizeNumber.cx*nIndex,0,SizeNumber.cx,SizeNumber.cy);
			nXPos += SizeNumber.cx;
		}
	}

	return;
}

//��ʱ����Ϣ
void CMatchViewFrame::OnTimer(UINT nIDEvent)
{
	__super::OnTimer(nIDEvent);

	if(IDI_UPDATE_TIME==nIDEvent)
	{
		//��ȡ����
		CRect rcClient;
		GetClientRect(&rcClient);

		//��ʼλ��
		INT nXStartPos=rcClient.Width()/2+m_MatchPosition.ptMatchShow.x;
		INT nYStartPos=rcClient.Height()/2+m_MatchPosition.ptMatchShow.y;

		//��������
		CRect rcRepaint;		
		rcRepaint.left=nXStartPos+m_MatchPosition.ptNumber1.x;
		rcRepaint.top=nYStartPos+m_MatchPosition.ptNumber1.y;
		rcRepaint.right=rcRepaint.left+200;
		rcRepaint.bottom=rcRepaint.top+30;

		//ˢ�½���
		RedrawWindow(&rcRepaint,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////