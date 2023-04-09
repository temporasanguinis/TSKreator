#include "wnd_log.h"
#pragma warning(push, 0)
#include <QLineEdit>
#include <QCheckBox>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QCloseEvent>
#pragma warning(pop)
#include "validator.h"
#include "config.h"
#include "constantname.h"
#include "selectobject.h"
#include "guiutils.h"
#include "kreatorsettings.h"


WndLog::WndLog(QWidget* parent)
    : QDialog(parent)
{
#if defined( KREATOR_DEBUG )
    qDebug("WndLog::WndLog( QWidget* ) called.");
#endif
    setupUi(this);
    setWindowTitle("TS Kreator LOG");
    TS::SetKreatorLogo(this);
}

WndLog::~WndLog()
{
#if defined( KREATOR_DEBUG )
    qDebug("WndLog::~WndLog() called.");
#endif
}

void WndLog::Log(QString txt) {
    mp_LogTxt->setPlainText(
        txt + "\n" +
        mp_LogTxt->toPlainText()
    );
}