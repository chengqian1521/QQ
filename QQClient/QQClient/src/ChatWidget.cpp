

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
    
    //���������С����ť��Ч
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint & ~Qt::WindowMinimizeButtonHint);

    setWindowFlags(windowFlags() | Qt::Tool);//���ò�������������


    //���ñ�����ɫ
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(Qt::darkCyan));
    setPalette(palette);
    initView();//��ʼ����صĿؼ�
}

ChatWidget::~ChatWidget()
{

}

void ChatWidget::initView()//��ʼ����صĿؼ�
{

	_textBrowser = new QTextBrowser;

    //����textBrowser������ɫ��ͼƬ
    _textBrowser->setStyleSheet("background-image: url(:/QQClient/Resources/2.jpg);");//��������Ϊ2.jpg
   

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(_textBrowser);

    _fontComboBox = new QFontComboBox;
    _fontComboBox->setToolTip(tr("����"));


    _fontFizeComboBox = new QComboBox;
    _fontFizeComboBox->setToolTip(tr("�ֺ�"));
    _fontFizeComboBox->setEditable(true);
	for(int i=8;i<=22;++i)
		_fontFizeComboBox->addItem(QString::number(i));
    _fontFizeComboBox->setCurrentIndex(4);

 
	_addRudeToolButton = new QToolButton;
    _addRudeToolButton->setToolTip(tr("�Ӵ�"));
    _addRudeToolButton->setText(tr("�Ӵ�"));
    _addRudeToolButton->setCheckable(true);
    _addRudeToolButton->setAutoRaise(true);
    _addRudeToolButton->setIcon(QPixmap(":/QQClient/Resources/bold.png"));

    _italicToolButton = new QToolButton;
    _italicToolButton->setToolTip(tr("��б"));
    _italicToolButton->setText(tr("��б"));
    _italicToolButton->setCheckable(true);
    _italicToolButton->setAutoRaise(true);
    _italicToolButton->setIcon(QPixmap(":/QQClient/Resources/italic.png"));


    _underLineToolButton = new QToolButton;
    _underLineToolButton->setToolTip(tr("�»���"));
    _underLineToolButton->setText(tr("�»���"));
    _underLineToolButton->setCheckable(true);
    _underLineToolButton->setAutoRaise(true);
    _underLineToolButton->setIcon(QPixmap(":/QQClient/Resources/under.png"));

    _fontColorToolButton = new QToolButton;
    _fontColorToolButton->setToolTip(tr("��ɫ"));
    _fontColorToolButton->setText(tr("��ɫ"));
    _fontColorToolButton->setAutoRaise(true);
    _fontColorToolButton->setIcon(QPixmap(":/QQClient/Resources/color.png"));


    _showMainViewToolButton = new QToolButton;
    _showMainViewToolButton->setText(tr("��ʾ������"));
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
    _sendButton->setText(tr("����"));

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(_inputLineEdit);
    bottomLayout->addWidget(_sendButton);

    QVBoxLayout *mainlayout = new QVBoxLayout;

    mainlayout->addLayout(topLayout);
    mainlayout->addLayout(midLayout);
    mainlayout->addLayout(bottomLayout);
    mainlayout->setSizeConstraint(QLayout::SetFixedSize);//���ô��ڴ�С���ܸı�
    setLayout(mainlayout);

    _textBrowser->setCurrentFont(_fontComboBox->font());//����textBrowser��Ĭ������
    _textBrowser->setFontPointSize(_fontFizeComboBox->currentText().toDouble());//����textBrowser��Ĭ���ֺ�
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
    if (_sendButton->isEnabled())//���pushButtonû�б�ң���ô�Ϳ��Ե���on_pushButton_clicked()����
        onSendButtonClicked();
}

void ChatWidget::onSendButtonClicked()
{

    if(_inputLineEdit->text().isEmpty())//���lineEdit�ؼ�����Ϊ�գ���ʾ�û����ܷ��Ϳ���Ϣ
    {
        QMessageBox::information(this, tr("ע��"), tr("���ܷ��Ϳ���Ϣ"));
		_inputLineEdit->setFocus();
		return;
    }
    
    
       	
	if (!netService->sendNormalMessage(_myQQ,_friendQQ, _inputLineEdit->text())) {
		QMessageBox::information(this, "err", "send err");
		return;
	}
	addMessageToBrowser(tr("�ҵ���Ϣ"), _inputLineEdit->text());//��Ҫ���͵���Ϣ���ݼ���textBrowser�ؼ���
	_inputLineEdit->clear();//������ɺ󣬽�lineEdit�ؼ��������
}

void ChatWidget::addMessageToBrowser(const QString& delivername, const QString&  msg)//��textBrowser�����Ϣ
{
    //�õ���ǰʱ�䣬����ʱ���ʽ��Ϊ"yyyy-MM-dd hh:ss:ss"��ʽ���ַ���
	QString sTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:ss:ss");
    _textBrowser->append("[" + delivername + "]" + sTime);
    _textBrowser->append(msg);
    
    //����ϢtextBrowser����Ϣ������ֹ�����ʱ���Զ����������·�
    _textBrowser->verticalScrollBar()->setValue(_textBrowser->verticalScrollBar()->maximum());
}

void ChatWidget::onFontComboBoxCurrentFontChanged(const QFont &f)//�޸�textBrowser����
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

void ChatWidget::onComboBoxCurrentIndexChanged(const QString &arg1)//�޸�textBrowser�ֺ�
{
    _textBrowser->setFontPointSize(arg1.toDouble());
    _inputLineEdit->setFocus();
}

void ChatWidget::onAddRudeToolButtonClicked(bool checked)//�޸�textBrowser�����Ƿ�Ӵ�
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

void ChatWidget::onItalicToolButtonClicked(bool checked)//�޸�textBrowser�����Ƿ�б��
{
    _textBrowser->setFontItalic(checked);
    _inputLineEdit->setFocus();
}

void ChatWidget::onUnderlineToolButtonClicked(bool checked)//�޸�textBrowser�����Ƿ��»���
{
    _textBrowser->setFontUnderline(checked);
    _inputLineEdit->setFocus();
}

void ChatWidget::onFontColorToolButtonClicked()//�޸�textBrowser������ɫ
{
    color = QColorDialog::getColor(color,this);
    if(color.isValid())
    {
        _textBrowser->setTextColor(color);
        _inputLineEdit->setFocus();
    }
}

