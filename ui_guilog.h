/********************************************************************************
** Form generated from reading UI file 'guilog.ui'
**
** Created by: Qt User Interface Compiler version 5.13.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUILOG_H
#define UI_GUILOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GuiLog
{
public:
    QWidget *centralWidget;
    QVBoxLayout *vboxLayout;
    QTextEdit *mp_LogTxt;

    void setupUi(QDialog *GuiLog)
    {
        if (GuiLog->objectName().isEmpty())
            GuiLog->setObjectName(QString::fromUtf8("GuiLog"));
        GuiLog->resize(617, 519);
        GuiLog->setMinimumSize(QSize(617, 519));
        GuiLog->setMaximumSize(QSize(617, 519));
        centralWidget = new QWidget(GuiLog);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->setGeometry(QRect(0, 0, 617, 519));
        vboxLayout = new QVBoxLayout(centralWidget);
        vboxLayout->setSpacing(6);
        vboxLayout->setContentsMargins(5, 5, 5, 5);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        vboxLayout->setContentsMargins(9, 9, 9, 9);
        mp_LogTxt = new QTextEdit(centralWidget);
        mp_LogTxt->setObjectName(QString::fromUtf8("mp_LogTxt"));
        mp_LogTxt->setReadOnly(true);

        vboxLayout->addWidget(mp_LogTxt);


        retranslateUi(GuiLog);

        QMetaObject::connectSlotsByName(GuiLog);
    } // setupUi

    void retranslateUi(QDialog *GuiLog)
    {
        GuiLog->setWindowTitle(QCoreApplication::translate("GuiLog", "Log", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GuiLog: public Ui_GuiLog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUILOG_H
