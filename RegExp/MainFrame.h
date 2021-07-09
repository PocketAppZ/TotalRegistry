#pragma once

#include "VirtualListView.h"
#include "Registry.h"
#include "AppSettings.h"
#include "CommandManager.h"
#include "FindDlg.h"
#include "IMainFrame.h"

enum class NodeType {
	None = 0,
	Key = 1,
	Predefined = 2,
	RegistryRoot = 0x10,
	StandardRoot = 0x20,
	Hive = 0x40,
	Link = 0x80,
	Loaded = 0x100,
	Machine = 0x200,
	AccessDenied = 0x400,
};
DEFINE_ENUM_FLAG_OPERATORS(NodeType);

class CMainFrame :
	public CFrameWindowImpl<CMainFrame>,
	public CAutoUpdateUI<CMainFrame>,
	public CVirtualListView<CMainFrame>,
	public IMainFrame,
	public CMessageFilter,
	public CIdleHandler {
public:
	DECLARE_FRAME_WND_CLASS(L"RegExpWndClass", IDR_MAINFRAME)

	CMainFrame() : m_FindDlg(this) {}

	const UINT WM_BUILD_TREE = WM_APP + 11;
	const UINT WM_FIND_UPDATE = WM_APP + 12;
	const UINT WM_RUN = WM_APP + 13;
	const UINT TreeId = 123;

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	void RunOnUiThread(std::function<void()> f);

	// IMainFrame
	AppSettings& GetSettings() override;
	void OnFindNext(PCWSTR path, PCWSTR name, void* data) override;
	void OnFindStart();
	void OnFindEnd(bool cancelled);

	CString GetColumnText(HWND, int row, int col) const;

	int GetRowImage(HWND, int row) const;
	void DoSort(const SortInfo* si);
	bool IsSortable(HWND, int col) const;
	BOOL OnRightClickList(HWND, int row, int col, const POINT&);
	BOOL OnDoubleClickList(HWND, int row, int col, const POINT& pt);

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		//NOTIFY_CODE_HANDLER(TVN_GETDISPINFO, OnGetTreeDispInfo)
		NOTIFY_CODE_HANDLER(NM_KILLFOCUS, OnFocusChanged)
		NOTIFY_CODE_HANDLER(NM_SETFOCUS, OnFocusChanged)
		NOTIFY_CODE_HANDLER(TVN_ITEMEXPANDING, OnTreeItemExpanding)
		NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnTreeSelChanged)
		NOTIFY_CODE_HANDLER(LVN_ITEMCHANGED, OnListItemChanged)
		NOTIFY_CODE_HANDLER(TVN_BEGINLABELEDIT, OnTreeBeginEdit)
		NOTIFY_CODE_HANDLER(TVN_ENDLABELEDIT, OnTreeEndEdit)
		NOTIFY_HANDLER(TreeId, NM_RCLICK, OnTreeContextMenu)
		NOTIFY_CODE_HANDLER(TVN_KEYDOWN, OnTreeKeyDown)
		NOTIFY_CODE_HANDLER(LVN_KEYDOWN, OnListKeyDown)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_BUILD_TREE, OnBuildTree)
		MESSAGE_HANDLER(WM_FIND_UPDATE, OnFindUpdate)
		MESSAGE_HANDLER(WM_RUN, OnRunOnUIThread)
		COMMAND_ID_HANDLER(ID_FILE_RUNASADMIN, OnRunAsAdmin)
		COMMAND_ID_HANDLER(IDM_EXIT, OnExit)
		COMMAND_ID_HANDLER(ID_VIEW_REFRESH, OnViewRefresh)
		COMMAND_ID_HANDLER(ID_OPTIONS_SHOWEXTRAHIVES, OnShowExtraHives)
		COMMAND_ID_HANDLER(ID_EDIT_READONLY, OnEditReadOnly)
		COMMAND_ID_HANDLER(ID_OPTIONS_ALWAYSONTOP, OnAlwaysOnTop)
		COMMAND_ID_HANDLER(ID_VIEW_SHOWKEYSINLIST, OnShowKeysInList)
		COMMAND_ID_HANDLER(ID_NEW_KEY, OnNewKey)
		COMMAND_ID_HANDLER(ID_EDIT_FIND, OnEditFind)
		COMMAND_ID_HANDLER(ID_SEARCH_FINDNEXT, OnSearchFindNext)
		COMMAND_ID_HANDLER(ID_EDIT_UNDO, OnEditUndo)
		COMMAND_ID_HANDLER(ID_EDIT_REDO, OnEditRedo)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAbout)
		CHAIN_MSG_MAP(CAutoUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CVirtualListView<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

private:
	struct FindData {
		PCWSTR Path;
		PCWSTR Name;
		PVOID Data;
	};

	enum class ColumnType {
		Name, Type, Value, Details, Size, TimeStamp
	};
	enum class Operation {
		None,
		RenameKey,
		CreateKey,
	};

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnFindUpdate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBuildTree(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTreeSelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnRunAsAdmin(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnListItemChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnViewRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTreeItemExpanding(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnShowExtraHives(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnShowKeysInList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAlwaysOnTop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFocusChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnNewKey(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEditReadOnly(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTreeBeginEdit(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnTreeEndEdit(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnEditUndo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEditRedo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTreeContextMenu(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnTreeKeyDown(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnListKeyDown(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnEditFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnRunOnUIThread(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSearchFindNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void InitCommandBar();
	void InitToolBar(CToolBarCtrl& tb, int size = 24);
	HTREEITEM BuildTree(HTREEITEM hRoot, HKEY hKey, PCWSTR name = nullptr);
	void InitTree();
	CString GetNodePath(HTREEITEM hItem, HKEY* pKey = nullptr) const;
	CString GetFullNodePath(HTREEITEM hItem) const;
	NodeType GetNodeData(HTREEITEM hItem) const;
	void SetNodeData(HTREEITEM hItem, NodeType type);
	void ExpandItem(HTREEITEM hItem);
	HKEY GetKeyFromNode(HTREEITEM hItem) const;
	CTreeItem InsertKeyItem(HTREEITEM hParent, PCWSTR name, NodeType type = NodeType::Key);
	HTREEITEM FindItemByPath(PCWSTR path) const;
	void InvokeTreeContextMenu(const CPoint& pt);
	CString GetKeyDetails(const RegistryItem& item) const;
	int GetKeyImage(const RegistryItem& item) const;

	void UpdateUI();
	void UpdateList(bool force = false);

	Registry m_Registry;
	CommandManager m_CmdMgr;
	mutable CRegKey m_CurrentKey;
	CString m_CurrentPath;
	CCommandBarCtrl m_CmdBar;
	CSplitterWindow m_MainSplitter;
	CMultiPaneStatusBarCtrl m_StatusBar;
	CListViewCtrl m_List;
	mutable CTreeViewCtrlEx m_Tree;
	CListViewCtrl m_Details;
	std::vector<RegistryItem> m_Items;
	CTreeItem m_hLocalRoot, m_hStdReg, m_hRealReg;
	NodeType m_CurrentNodeType{ NodeType::None };
	int m_CurrentSelectedItem{ -1 };
	AppSettings m_Settings;
	Operation m_CurrentOperation{ Operation::None };
	CFindDlg m_FindDlg;
	bool m_ReadOnly{ true };
	bool m_UpdateNoDelay{ false };
};