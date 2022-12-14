#ifndef TS_WNDSETTINGS_H
#define TS_WNDSETTINGS_H
#pragma warning(push, 0)
#include <QDialog>
#include "ui_guisettings.h"
#pragma warning(pop)

class QLineEdit;


class WndSettings : public QDialog, private Ui::GuiSettings
{
  Q_OBJECT

public:
  WndSettings( QWidget* parent );
  virtual ~WndSettings();

protected slots:
  void saveData();
  void somethingChanged();
  void saveAndClose();
  void editZonesDir();
  void editRoomsDir();
  void editMobsDir();
  void editItemsDir();
  void editShopsDir();
  void editNotesDir();
  void editLogsDir();
  void editCustomTheme();

protected:
  void init();
  void closeEvent( QCloseEvent* );
  void loadData();
  void refreshPanel();
  void selectDir( QLineEdit*, const QString& msg );
  void selectFile(QLineEdit*, const QString& msg);


};

#endif // TS_WNDSETTINGS_H

