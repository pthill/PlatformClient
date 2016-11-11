#ifndef USER_LIST_CONTROL_HEAD_FILE
#define USER_LIST_CONTROL_HEAD_FILE

#pragma once

#include "ShareControlHead.h"

//////////////////////////////////////////////////////////////////////////////////

//��������
#define	MAX_COLUMN					32									//�������

//���ȶ���
#define USER_STATUS_CX				20									//�б�����

//��������
#define INDEX_STATUS				0									//״̬����
#define COUNT_STATUS				6									//״̬��Ŀ
#define INDEX_NORMAL				12									//��������
#define INDEX_FRIEND				13									//��������
#define INDEX_MEMBER				15									//��������
#define INDEX_MASTER				20									//��Ա����
#define INDEX_INVALID				0xFFFF								//��Ч����

//////////////////////////////////////////////////////////////////////////////////

//Ĭ����ɫ
#define COLOR_SELECT_TX				RGB(255,234,2)						//������ɫ
#define COLOR_SELECT_BK				RGB(85,72,53)						//������ɫ
#define COLOR_NORMAL_TX				RGB(255,255,255)					//������ɫ
#define COLOR_NORMAL_BK				RGB(21,6,0)							//������ɫ

//Ĭ����ɫ
#define COLOR_MYSELF_TX				RGB(255,167,132)					//������ɫ
#define COLOR_MYSELF_BK				RGB(85,72,53)						//������ɫ
#define COLOR_MASTER_TX				RGB(255,255,255)					//������ɫ
#define COLOR_MASTER_BK				RGB(90,45,27)						//������ɫ
#define COLOR_MEMBER_TX				RGB(67,44,12)						//������ɫ
#define COLOR_MEMBER_BK				RGB(255,157,2)						//������ɫ
#define COLOR_ANDROID_TX			RGB(255,255,255)					//������ɫ
#define COLOR_ANDROID_BK			RGB(255,0,0)						//������ɫ

//////////////////////////////////////////////////////////////////////////////////

//��ɫ����
struct tagColorUserList
{
	//������ɫ
	COLORREF						crSelectTX;							//������ɫ
	COLORREF						crSelectBK;							//������ɫ
	COLORREF						crNormalTX;							//������ɫ
	COLORREF						crNormalBK;							//������ɫ

	//������ɫ	
	COLORREF						crMyselfTX;							//������ɫ
	COLORREF						crMyselfBK;							//������ɫ
	COLORREF						crMasterTX;							//������ɫ
	COLORREF						crMasterBK;							//������ɫ
	COLORREF						crMemberTX;							//������ɫ
	COLORREF						crMemberBK;							//������ɫ
	COLORREF						crAndroidTX;						//������ɫ
	COLORREF						crAndroidBK;						//������ɫ
};

//////////////////////////////////////////////////////////////////////////////////

//�û��б�
class SHARE_CONTROL_CLASS CUserListControl : public CSkinListCtrl
{
	//״̬����
protected:
	bool							m_bCreateColumn;					//��������
	DWORD							m_dwServerRule;						//�������
	DWORD							m_dwMySelfUserID;					//�Լ���Ϣ

	//�б���Ϣ
protected:
	WORD							m_wColumnCount;						//�б���Ŀ
	BYTE							m_cbDataDescribe[MAX_COLUMN];		//��������

	//�ӿڱ���
protected:
	IGameLevelParser *				m_pIGameLevelParser;				//�ȼ��ӿ�
	IGameUserManager *				m_pIGameUserManager;				//�û��ӿ�

	//��Դ����
protected:
	tagColorUserList				m_ColorUserList;					//��ɫ����
	static CImageList				m_ImageListStatus;					//״̬λͼ
	static CPngImage				m_ImageUserStatus;					//״̬ͼƬ

	//��������
public:
	//���캯��
	CUserListControl();
	//��������
	virtual ~CUserListControl();

	//���غ���
public:
	//�ؼ���
	virtual VOID PreSubclassWindow();

	//���ݿ���
protected:
	//��ȡλ��
	virtual INT GetInsertIndex(VOID * pItemData);
	//��������
	virtual INT SortItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex, bool bAscendSort);
	//�����ַ�
	virtual VOID ConstructString(VOID * pItemData, WORD wColumnIndex, LPTSTR pszString, WORD wMaxCount);

	//�滭����
protected:
	//��ȡ��ɫ
	virtual VOID GetItemColor(LPDRAWITEMSTRUCT lpDrawItemStruct, COLORREF&crColorText, COLORREF&crColorBack);
	//�滭����
	virtual VOID DrawCustomItem(CDC * pDC, LPDRAWITEMSTRUCT lpDrawItemStruct, CRect&rcSubItem, INT nColumnIndex);

	//��չ����
protected:
	//�û�����
	virtual IClientUserItem * GetClientUserItem(VOID * pItemData);

	//���ú���
public:
	//���ýӿ�
	bool SetServerRule(DWORD dwServerRule);
	//���ýӿ�
	bool SetMySelfUserID(DWORD dwMySelfUserID);
	//���ýӿ�
	bool SetGameLevelParser(IUnknownEx * pIUnknownEx);
	//���ýӿ�
	bool SetGameUserManager(IUnknownEx * pIUnknownEx);

	//�б�����
public:
	//������ɫ
	bool SetColorUserList(tagColorUserList&ColorUserList);
	//�����б�
	bool SetColumnDescribe(tagColumnItem ColumnItem[], WORD wColumnCount);

	//��Ϣӳ��
protected:
	//������Ϣ
	VOID OnNcDestroy();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif