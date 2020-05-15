#ifndef TS_WNDMOB_H
#define TS_WNDMOB_H
#pragma warning(push, 0)
#include <QDialog>
#include "ui_guimob.h"
#pragma warning(pop)
#include "area.h"
#include "syntax.h"

using namespace ts;


class WndMob : public QDialog, private Ui::GuiMob
{
 Q_OBJECT

public:
  WndMob( Area&, const Mob&, QWidget* );
  virtual ~WndMob();

signals:
  void dataSaved();

public slots:
    void somethingChanged();

protected slots:
  void saveData();
  void restoreData();
  void saveAndClose();
  void showNextMob();
  void showPrevMob();
  void typeSelected( int );
  void editActs();
  void editNewActs();
  void editAffects();
  void editAffects2();
  void editResistances();
  void editImmunities();
  void editSusceptibilities();
  void editInits();

protected:
  void init();
  void closeEvent( QCloseEvent* );
  void loadData();
  void refreshTitle();
  void refreshPanel();
  void refreshActs();
  void refreshNewActs();
  void refreshAffects();
  void refreshAffects2();
  void refreshResistances();
  void refreshImmunities();
  void refreshSusceptibilities();
  void refreshAverageHpAndXP();

private:
  Area& m_area;
  Mob m_mob;
  Syntax* highLighter = NULL;
};

#endif // TS_WNDMOB_H
