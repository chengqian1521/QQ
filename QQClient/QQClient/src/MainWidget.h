#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#pragma execution_character_set("utf-8")
#include <QGroupBox>
#include <QVBoxLayout>
#include <QToolBox>
#include <QToolButton>
#include <QTcpSocket>
#include <QListView>
#include <QCloseEvent>
#include <QMap>
#include <unordered_map>
#include "../src/common.h"
#include <QWidget>
#include "QQUser.h"
class ChatWidget;
class NetService;
class QLabel;

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = 0);
    virtual  ~MainWidget();  



protected:
	/*override*/
	virtual bool eventFilter(QObject *watched, QEvent *event) override;
	virtual void closeEvent(QCloseEvent *event)override;
	virtual void showEvent(QShowEvent* ev)override;
	virtual void mousePressEvent(QMouseEvent *event)override;
	virtual void mouseMoveEvent(QMouseEvent *event)override;
	virtual void mouseReleaseEvent(QMouseEvent *event)override;

private:
	void initView();
	void initTitleView(QVBoxLayout* mainVBoxLayout);
	void initTopPanel(QVBoxLayout* mainVBoxLayout);
	void initFriendPanel(QVBoxLayout* mainVBoxLayout);
	void initFriendList(QHBoxLayout* friendPanelLayout);

	inline QQUser* findFriendInfoByQQ(int qq) {
		auto it = _qqToFriendInfoMap.find(qq);
		if (it == _qqToFriendInfoMap.end())
			return nullptr;
		return &it->second;
	}
	inline ChatWidget* findChatWidgetByQQ(int qq) {
		auto it = _qqToCharWidgetMap.find(qq);
		if (it == _qqToCharWidgetMap.end())
			return nullptr;

		return it->second;
	}
	inline void addQQUserToMap(int qq, const QQUser& u) {
		_qqToFriendInfoMap[qq] = u;
	}
	inline void addChatWidgetToMap(int qq, ChatWidget* chatWidget) {
		_qqToCharWidgetMap[qq] = chatWidget;
	}
public:
	int _qq;
	QString _qqName;
public:
	QLabel  *_qqNameLabel;
	QLabel    *_headLabel;

	bool _isMousrPressInTop = false;

	QPoint  _prePos;//用于按住窗口时的移动

	QWidget* _friendList;
	QVBoxLayout* _friendListLayout;

	bool _isFriendListInitEnd = false;

	
	std::unordered_map<int, ChatWidget* > _qqToCharWidgetMap;
	std::unordered_map<int, QQUser>       _qqToFriendInfoMap;
};

#endif // MAINWINDOW_H
