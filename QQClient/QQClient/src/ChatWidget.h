
#ifndef WIDGET1_H
#define WIDGET1_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <QPushButton>
#include <QTextBrowser>
#include <QFontComboBox>
#include <QComboBox>
#include <QToolButton>
#include <QLineEdit>
#include <QColor>


class ChatWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChatWidget(int myQQ,int friendQQ, const QString& name, QWidget *parent=0);
    virtual ~ChatWidget();
    void addMessageToBrowser(const QString& delivername, const QString&  msg);
    
	QPushButton *_sendButton;

private slots:
    void onSendButtonClicked();
    void onFontComboBoxCurrentFontChanged(const QFont &f);
    void onComboBoxCurrentIndexChanged(const QString &arg1);
    void onAddRudeToolButtonClicked(bool checked);
    void onItalicToolButtonClicked(bool checked);
    void onUnderlineToolButtonClicked(bool checked);
    void onFontColorToolButtonClicked();
    void onInputLineEditEnterKeyPressed();
  


private:
    void initView();

    QTextBrowser      *_textBrowser;
    QFontComboBox    *_fontComboBox;
    QComboBox    *_fontFizeComboBox;
    QToolButton *_addRudeToolButton;
    QToolButton  *_italicToolButton;
    QToolButton *_underLineToolButton;
    QToolButton *_fontColorToolButton;
    QToolButton *_showMainViewToolButton;
    QLineEdit *_inputLineEdit;

	int _myQQ;
    int _friendQQ;
    QString _friendName;
    QColor color;
};

#endif // WIDGET1_H
