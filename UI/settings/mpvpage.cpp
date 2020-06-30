#include "mpvpage.h"
#include <QLabel>
#include <QSyntaxHighlighter>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QSettings>
#include "globalobjects.h"
namespace
{
    class OptionHighLighter : public QSyntaxHighlighter
    {
    public:
        OptionHighLighter(QTextDocument *parent):QSyntaxHighlighter(parent)
        {
            optionNameFormat.setForeground(QColor(41,133,199));
            optionContentFormat.setForeground(QColor(245,135,31));
            commentFormat.setForeground(QColor(142,142,142));
            optionRe.setPattern("\\s*([^#=]+)=?(.*)\n?");
            commentRe.setPattern("\\s*#.*\n?");
        }
        // QSyntaxHighlighter interface
    protected:
        virtual void highlightBlock(const QString &text)
        {
            int index= commentRe.indexIn(text);
            if(index!=-1)
            {
                setFormat(index, commentRe.matchedLength(), commentFormat);
                return;
            }
            index = optionRe.indexIn(text);
            if (index>-1)
            {
                QStringList opList = optionRe.capturedTexts();
                int opNamePos = optionRe.pos(1), opContentPos = optionRe.pos(2);
                if (opNamePos>-1)
                {
                    setFormat(opNamePos, opList.at(1).length(), optionNameFormat);
                }
                if (opContentPos>-1)
                {
                    setFormat(opContentPos + 1, opList.at(2).length() - 1, optionContentFormat);
                }
            }
        }
    private:
        QTextCharFormat optionNameFormat;
        QTextCharFormat optionContentFormat;
        QTextCharFormat commentFormat;
        QRegExp optionRe,commentRe;
    };
}
MPVPage::MPVPage(QWidget *parent) : SettingPage(parent)
{
    QLabel *tipLable=new QLabel(tr("One option per line, without the leading \"--\"\n"
                                   "The changes will take effect when KikoPlay is restarted"),this);
    tipLable->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    parameterEdit=new QTextEdit(this);
    QFont paramFont("Consolas",10);
    parameterEdit->setFont(paramFont);
    new OptionHighLighter(parameterEdit->document());
    parameterEdit->setPlainText(GlobalObjects::appSetting->value("Play/MPVParameters",
                                                                 "#Make sure the danmu is smooth\n"
                                                                 "vf=lavfi=\"fps=fps=60:round=down\"\n"
                                                                 "hwdec=auto").toString());
    QObject::connect(parameterEdit,&QTextEdit::textChanged,[this](){
       paramChange=true;
    });
    QVBoxLayout *dialogVLayout=new QVBoxLayout(this);
    dialogVLayout->setContentsMargins(0, 0, 0, 0);
    dialogVLayout->addWidget(tipLable);
    dialogVLayout->addWidget(parameterEdit);
}

void MPVPage::onAccept()
{
    if(paramChange) GlobalObjects::appSetting->setValue("Play/MPVParameters",parameterEdit->toPlainText());
}

void MPVPage::onClose()
{

}
