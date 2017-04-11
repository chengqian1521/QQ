

#include <QMessageBox>
#include <QDateTime>
#include <QScrollBar>
#include <QColorDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "utils.h"
#include <QDebug>
#include "ChatWidget.h"
#include "NetService.h"


ChatWidget::ChatWidget(int myQQ, int friendQQ,const QString& friendName, QWidget *parent) :
    QWidget(parent),_myQQ(myQQ),_friendQQ(friendQQ),_friendName(friendName)
{
   

    setWindowTitle(friendName);

	QPixmap headImage(":/QQClient/Resources/"+QString::number(utils::getImageIndexByQString(friendName))+".png");

    setWindowIcon(headImage);
    
    //设置最大化最小化按钮无效
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint & ~Qt::WindowMinimizeButtonHint);

    setWindowFlags(windowFlags() | Qt::Tool);//设置不在任务栏出现


    //设置背景颜色
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(Qt::darkCyan));
    setPalette(palette);
    initView();//初始化相关的控件
}

ChatWidget::~ChatWidget()
{

}

void ChatWidget::initView()//初始化相关的控件
{

	_textBrowser = new QTextBrowser;

    //设置textBrowser背景颜色或图片
    _textBrowser->setStyleSheet("background-image: url(:/QQClient/Resources/2.jpg);");//背景设置为2.jpg
   

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(_textBrowser);

    _fontComboBox = new QFontComboBox;
    _fontComboBox->setToolTip(tr("字体"));


    _fontFizeComboBox = new QComboBox;
    _fontFizeComboBox->setToolTip(tr("字号"));
    _fontFizeComboBox->setEditable(true);
	for(int i=8;i<=22;++i)
		_fontFizeComboBox->addItem(QString::number(i));
    _fontFizeComboBox->setCurrentIndex(4);

 
	_addRudeToolButton = new QToolButton;
    _addRudeToolButton->setToolTip(tr("加粗"));
    _addRudeToolButton->setText(tr("加粗"));
    _addRudeToolButton->setCheckable(true);
    _addRudeToolButton->setAutoRaise(true);
    _addRudeToolButton->setIcon(QPixmap(":/QQClient/Resources/bold.png"));

    _italicToolButton = new QToolButton;
    _italicToolButton->setToolTip(tr("倾斜"));
    _italicToolButton->setText(tr("倾斜"));
    _italicToolButton->setCheckable(true);
    _italicToolButton->setAutoRaise(true);
    _italicToolButton->setIcon(QPixmap(":/QQClient/Resources/italic.png"));


    _underLineToolButton = new QToolButton;
    _underLineToolButton->setToolTip(tr("下划线"));
    _underLineToolButton->setText(tr("下划线"));
    _underLineToolButton->setCheckable(true);
    _underLineToolButton->setAutoRaise(true);
    _underLineToolButton->setIcon(QPixmap(":/QQClient/Resources/under.png"));

    _fontColorToolButton = new QToolButton;
    _fontColorToolButton->setToolTip(tr("颜色"));
    _fontColorToolButton->setText(tr("颜色"));
    _fontColorToolButton->setAutoRaise(true);
    _fontColorToolButton->setIcon(QPixmap(":/QQClient/Resources/color.png"));


    _showMainViewToolButton = new QToolButton;
    _showMainViewToolButton->setText(tr("显示主窗口"));
    _showMainViewToolButton->setAutoRaise(true);

    QHBoxLayout *midLayout = new QHBoxLayout;
    midLayout->addWidget(_fontComboBox);
    midLayout->addWidget(_fontFizeComboBox);
    midLayout->addWidget(_addRudeToolButton);
    midLayout->addWidget(_italicToolButton);
    midLayout->addWidget(_underLineToolButton);
    midLayout->addWidget(_fontColorToolButton);
    midLayout->addWidget(_showMainViewToolButton);


	_inputLineEdit = new QLineEdit;

    _sendButton = new QPushButton;
    _sendButton->setText(tr("发送"));

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(_inputLineEdit);
    bottomLayout->addWidget(_sendButton);

    QVBoxLayout *mainlayout = new QVBoxLayout;

    mainlayout->addLayout(topLayout);
    mainlayout->addLayout(midLayout);
    mainlayout->addLayout(bottomLayout);
    mainlayout->setSizeConstraint(QLayout::SetFixedSize);//设置窗口大小不能改变
    setLayout(mainlayout);

    _textBrowser->setCurrentFont(_fontComboBox->font());//设置textBrowser的默认字体
    _textBrowser->setFontPointSize(_fontFizeComboBox->currentText().toDouble());//设置textBrowser的默认字号
    _inputLineEdit->setFocus();

    connect(_fontComboBox, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(onFontComboBoxCurrentFontChanged(const QFont &)));
    connect(_fontFizeComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onComboBoxCurrentIndexChanged(const QString &)));
    connect(_addRudeToolButton, SIGNAL(clicked(bool)), this, SLOT(onAddRudeToolButtonClicked(bool)));
    connect(_italicToolButton, SIGNAL(clicked(bool)), this, SLOT(onItalicToolButtonClicked(bool)));
    connect(_underLineToolButton, SIGNAL(clicked(bool)), this, SLOT(onUnderlineToolButtonClicked(bool)));
    connect(_fontColorToolButton, SIGNAL(clicked()), this, SLOT(onFontColorToolButtonClicked()));
    connect(_inputLineEdit, SIGNAL(returnPressed()), this, SLOT(onInputLineEditEnterKeyPressed()));
    connect(_sendButton, SIGNAL(clicked()), this, SLOT(onSendButtonClicked()));

}

void ChatWidget::onInputLineEditEnterKeyPressed()
{
    if (_sendButton->isEnabled())//如果pushButton没有变灰，那么就可以调用on_pushButton_clicked()函数
        onSendButtonClicked();
}

void ChatWidget::onSendButtonClicked()
{

    if(_inputLineEdit->text().isEmpty())//如果lineEdit控件内容为空，提示用户不能发送空消息
    {
        QMessageBox::information(this, tr("注意"), tr("不能发送空消息"));
		_inputLineEdit->setFocus();
		return;
    }
    
    
       	
	if (!netService->sendNormalMessage(_myQQ,_friendQQ, _inputLineEdit->text())) {
		QMessageBox::information(this, "err", "send err");
		return;
	}
	addMessageToBrowser(tr("我的消息"), _inputLineEdit->text());//将要发送的消息内容加入textBrowser控件中
	_inputLineEdit->clear();//发送完成后，将lineEdit控件内容清空
}

void ChatWidget::addMessageToBrowser(const QString& delivername, const QString&  msg)//向textBrowser添加消息
{
    //得到当前时间，并把时间格式化为"yyyy-MM-dd hh:ss:ss"形式的字符串
	QString sTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:ss:ss");
    _textBrowser->append("[" + delivername + "]" + sTime);
    _textBrowser->append(msg);
    
    //当消息textBrowser中消息过多出现滚动条时，自动滚动到最下方
    _textBrowser->verticalScrollBar()->setValue(_textBrowser->verticalScrollBar()->maximum());
}

void ChatWidget::onFontComboBoxCurrentFontChanged(const QFont &f)//修改textBrowser字体
{
    _textBrowser->setCurrentFont(f);
    _textBrowser->setFontPointSize(_fontFizeComboBox->currentText().toDouble());

    if(_addRudeToolButton->isChecked())
    {
        _textBrowser->setFontWeight(QFont::Bold);
    }
    else
    {
        _textBrowser->setFontWeight(QFont::Normal);
    }

    if(_italicToolButton->isChecked())
    {
        _textBrowser->setFontItalic(true);
    }
    else
    {
        _textBrowser->setFontItalic(false);
    }

    if(_underLineToolButton->isChecked())
    {
        _textBrowser->setFontUnderline(true);
    }
    else
    {
        _textBrowser->setFontUnderline(false);
    }

    _textBrowser->setTextColor(color);
    _inputLineEdit->setFocus();

}

void ChatWidget::onComboBoxCurrentIndexChanged(const QString &arg1)//修改textBrowser字号
{
    _textBrowser->setFontPointSize(arg1.toDouble());
    _inputLineEdit->setFocus();
}

void ChatWidget::onAddRudeToolButtonClicked(bool checked)//修改textBrowser字体是否加粗
{
    if(checked)
    {
        _textBrowser->setFontWeight(QFont::Bold);
    }
    else
    {
        _textBrowser->setFontWeight(QFont::Normal);
    }
    _inputLineEdit->setFocus();
}

void ChatWidget::onItalicToolButtonClicked(bool checked)//修改textBrowser字体是否斜体
{
    _textBrowser->setFontItalic(checked);
    _inputLineEdit->setFocus();
}

void ChatWidget::onUnderlineToolButtonClicked(bool checked)//修改textBrowser字体是否下划线
{
    _textBrowser->setFontUnderline(checked);
    _inputLineEdit->setFocus();
}

void ChatWidget::onFontColorToolButtonClicked()//修改textBrowser字体颜色
{
    color = QColorDialog::getColor(color,this);
    if(color.isValid())
    {
        _textBrowser->setTextColor(color);
        _inputLineEdit->setFocus();
    }
}

