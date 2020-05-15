
#ifndef TS_WNDCREATEAREA_H
#define TS_WNDCREATEAREA_H
#pragma warning(push, 0)
#include <QDialog>
#include <QString>
#include "ui_guicreatearea.h"
#pragma warning(pop)


class WndCreateArea : public QDialog, Ui::GuiCreateArea
{
  Q_OBJECT

public:
  WndCreateArea( const QString& file_name, QWidget* parent );
  ~WndCreateArea();

signals:
  void areaCreated( const QString& );

protected slots:
  void createArea();

};

#endif  // TS_WNDCREATEAREA_H

