#ifndef TS_WNDITEM_H
#define TS_WNDITEM_H
#pragma warning(push, 0)
#include <QDialog>
#include "ui_guiitem.h"
#pragma warning(pop)
#include "area.h"

using namespace ts;


class WndItem : public QDialog, private Ui::GuiItem
{
  Q_OBJECT

public:
  WndItem( Area&, const Item&, QWidget* );
  virtual ~WndItem();

public slots:
    void somethingChanged();

signals:
  void dataSaved();

protected slots:
  void saveData();
  void restoreData();
  void saveAndClose();
  void typeSelected( int );
  void showNextItem();
  void showPrevItem();
  void editValues();
  void editAffects();
  void editFlags();
  void editNewFlags();
  void editWearPositions();
  void editExtraDescription();
  void editExtraDescription( QListWidgetItem* );
  void removeExtraDescription();
  void addExtraDescription();
  void refreshExtraDescriptions();
  void refreshValues();
  void refreshAffects();

protected:
  void init();
  void closeEvent( QCloseEvent* );
  void loadData();
  void refreshTitle();
  void refreshPanel();
  void refreshFlags();
  void refreshNewFlags();
  void refreshWearPositions();

private:
  Area& m_area;
  Item m_item;

};

#endif // TS_WNDITEM_H
