
#ifndef LOGINDLG_H
#define LOGINDLG_H
#pragma execution_character_set("utf-8")
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPixmapCache>
#include <QCloseEvent>

class QLineEdit;
class MainWidget;

class LoginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWidget(QWidget *parent = 0);
    virtual bool eventFilter(QObject *, QEvent *)override;
    virtual void mousePressEvent(QMouseEvent *event)override;
    virtual void mouseMoveEvent(QMouseEvent *event)override;
    virtual void mouseReleaseEvent(QMouseEvent *event)override;
    virtual void keyPressEvent(QKeyEvent *event)override;
public:
	void onRegisterLinkClicked();
signals:

public slots:

private:
    void initView();
public:
    QPixmap*  _close_button_hover ;
    QPixmap*  _close_button_normal;

    bool _isMousrPressInTop=false;

	QPoint  _prePos;//用于按住窗口时的移动
	
	bool _isAreadySendLoginMsg = false;

 
    QLineEdit *_lineEditUserQQ;
    QLineEdit *_lineEditPasswd;
    QPushButton *_loginButton;
    MainWidget*  _mainWidget;

private:
    void onClickLoginCallback();

public:
    int _qq;
    QString _pwd;

};

#endif // LOGINDLG_H
