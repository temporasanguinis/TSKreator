#ifndef TS_WNDEXIT_H
#define TS_WNDEXIT_H
#pragma warning(push, 0)
#include <QDialog>
#include "ui_guiexit.h"
#pragma warning(pop)
#include "area.h"
#include "syntax.h"

using namespace ts;


class WndExit : public QDialog, private Ui::GuiExit
{
  Q_OBJECT

public:
  WndExit( Area&, Room&, const Exit&, QWidget* );
  virtual ~WndExit();

signals:
  void dataSaved();

protected slots:
  void saveData();
  void restoreData();
  void somethingChanged();
  void saveAndClose();
  void directionChanged( int );
  void editFlags();
  void editToRoom();
  void editKey();
  void openReverseExit();

protected:
  void init();
  void closeEvent( QCloseEvent* );
  void loadData();
  void refreshTitle();
  void refreshPanel();
  void refreshNames();
  void refreshFlags();
  void refreshToRoom();
  void refreshKey();
  void refreshOpenCommand();
  void refreshInit();

private:
  Area& m_area;
  Room& m_room;
  Exit m_exit;
  QString m_originalName;
  ZoneCommand m_doorInit;
  Syntax* highLighter = NULL;
};

#endif // TS_WNDEXIT_H
