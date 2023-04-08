
#ifndef TS_WNDMAINWINDOW_H
#define TS_WNDMAINWINDOW_H
#pragma warning(push, 0)
#include <QMainWindow>
#include "ui_guimainwindow.h"
#pragma warning(pop)

class QAction;
class QMenu;
class WndArea;
class QColor;
class QString;


class WndMainWindow : public QMainWindow, Ui::GuiMainWindow
{
  Q_OBJECT

  public:
    WndMainWindow( QWidget* parent );
    ~WndMainWindow();

    void log(QString log);

    QMenu* helpMenu() { return mp_popupHelp; }

  signals:
    void windowClosed();
    void messageShowed( const QString&, int alignment = Qt::AlignCenter | Qt::AlignBottom, const QColor& color = Qt::black );

  public slots:
    void loadAreas();
    void showMessage( const QString&, int timeout = 0 );
    void loadArea( const QString& );

  protected slots:
    void createArea();
    void loadArea();
    void showSettings();
    void closeMainWindow();
    void showAbout();
    void loadArea( QTreeWidgetItem*, int );
    void refreshAreaView();
    void areaSelectionChanged();
    void openRecentFile();
    void filterList();
    void checkBootFile();

  protected:
    void init();
    void closeEvent( QCloseEvent* );
    void createItem(int index, const QString& );
	QString strippedName(const QString &fullFileName);

  protected:
    QMenu* mp_popupFile;
    QMenu* mp_popupHelp;
	QMenu* mp_mru;
    QToolBar* mp_barFile;
    QAction* mp_actRefreshAreas;
    QAction* mp_actCreateArea;
    QAction* mp_actLoadArea;
    QAction* mp_actSettings;
    QAction* mp_actQuit;
	QAction* mp_recentFileActs[5];

};

#endif // TS_WNDMAINWINDOW_H

