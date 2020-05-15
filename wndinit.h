#ifndef TS_WNDINIT_H
#define TS_WNDINIT_H
#pragma warning(push, 0)
#include <QDialog>
#include "ui_guiinit.h"
#pragma warning(pop)
#include "area.h"
#include "wndzone.h"

using namespace ts;


class WndInit : public QDialog, private Ui::GuiInit
{
  Q_OBJECT

public:
  WndInit( bool mob, VNumber vnu, Area&, QWidget* );
  virtual ~WndInit();

signals:
  void dataSaved();

protected slots:
  void saveData();
  void restoreData();
  void somethingChanged();
  void saveAndClose();
  void editInit();
  void editInit( QTreeWidgetItem*, int );
  void removeInit();
  void addInit();
  void copyInit();

protected:
  void init();
  void closeEvent( QCloseEvent* );
  void loadData();
  void refreshTitle();
  void refreshView();

private:
  Area& m_area;
  Zone m_zone;
  VNumber m_vnum;
  bool m_mob;
  QList<int> m_association;

};

#endif // TS_WNDINIT_H
