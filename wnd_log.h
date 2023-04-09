#ifndef TS_WNDLOG_H
#define TS_WNDLOG_H
#pragma warning(push, 0)
#include <QDialog>
#include "ui_guilog.h"
#pragma warning(pop)
#include "syntax.h"

class WndLog : public QDialog, private Ui::GuiLog
{
	Q_OBJECT

public:
	WndLog(QWidget*);
	virtual ~WndLog();
	void Log(QString);
};

#endif // TS_WNDLOG_H
