#include "wndmainwindow.h"
#pragma warning(push, 0)
#include <QStyleFactory>
#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QDir>
#include <QHeaderView>
#include <QTextStream>
#include <QToolBar>
#include <QTimer>
#include <QStatusBar>
#pragma warning(pop)
#include "wndarea.h"
#include "config.h"
#include "wndsettings.h"
#include "guiversion.h"
#include "version.h"
#include "guiutils.h"
#include "settings.h"
#include "wndcreatearea.h"
#include "constantname.h"
#include "kreatorsettings.h"
#include "eleuconf.h"
#include <iostream>
#include <QTextEdit>
#include <QtGui>

using namespace ts;


WndMainWindow::WndMainWindow( QWidget* parent )
  : QMainWindow( parent ), Ui::GuiMainWindow()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMainWindow::WndMainWindow( QWidget* parent, const char* name ) called." );
#endif
  init();
}

WndMainWindow::~WndMainWindow()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMainWindow::~WndMainWindow() called." );
#endif
}

void WndMainWindow::closeMainWindow()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMainWindow::closeMainWindow() called." );
#endif
  close();
}

void WndMainWindow::closeEvent( QCloseEvent* e )
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMainWindow::closeEvent( QCloseEvent* ) called." );
#endif
  e->accept();

  if( e->isAccepted() )
  {
    KreatorSettings::instance().saveGuiStatus( "MainWindow", this );
    emit( windowClosed() );
  }
}

#define COLUMN_VNUMBER    0
#define COLUMN_INDEX      1
#define COLUMN_NAME       2
#define COLUMN_PATH       3
#define COLUMN_MINMAX     4
#define COLUMN_REPOP_TIME 5
#define COLUMN_REPOP_MODE 6
#define COLUMN_FLAGS      7
#define COLUNM_MAX        8


void WndMainWindow::init()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMainWindow::init() called." );
#endif
  setupUi( this );
  (void) statusBar();
  setWindowTitle( "TS Kreator" );
  TS::SetKreatorLogo( this );
  KreatorSettings::instance().loadGuiStatus( "MainWindow", this );

  if(KreatorSettings::instance().kreatorTheme() != -1) {
    QApplication::setStyle(QStyleFactory::create(QStyleFactory::keys().at(KreatorSettings::instance().kreatorTheme())));
  } else {
#ifdef Q_WS_WIN32
      if(QStyleFactory::keys().contains("cleanlooks", Qt::CaseInsensitive))
          QApplication::setStyle(QStyleFactory::create("cleanlooks"));
      else
          QApplication::setStyle(QStyleFactory::create("windows"));
#elif defined( Q_WS_MAC )
      //da testare se funziona su os x
      QApplication::setStyle(QStyleFactory::create("macintosh"));
#elif defined( Q_WS_X11 )
      QApplication::setStyle(QStyleFactory::create("plastique"));
#endif
  }

  QString ColumnName[ COLUNM_MAX ];
  ColumnName[ COLUMN_VNUMBER    ] = trUtf8( "# Zona" );
  ColumnName[ COLUMN_INDEX]       = trUtf8("#");
  ColumnName[ COLUMN_NAME       ] = trUtf8( "Nome" );
  ColumnName[ COLUMN_PATH       ] = trUtf8( "Path" );
  ColumnName[ COLUMN_MINMAX     ] = trUtf8( "VNumbers" );
  ColumnName[ COLUMN_REPOP_TIME ] = trUtf8( "Intervallo" );
  ColumnName[ COLUMN_REPOP_MODE ] = trUtf8( "Repop" );
  ColumnName[ COLUMN_FLAGS      ] = trUtf8( "Flags" );

  mp_twObjectList->setColumnCount( COLUNM_MAX );
  QStringList labels;
  for( int i = 0; i < COLUNM_MAX; i++ )
    labels << ColumnName[ i ];
  mp_twObjectList->setHeaderLabels( labels );
  mp_twObjectList->hideColumn( COLUMN_PATH );

  QHeaderView* pHeader = mp_twObjectList->header();
  pHeader->resizeSection( COLUMN_VNUMBER, 80 );
  pHeader->resizeSection(COLUMN_INDEX, 30);
  pHeader->resizeSection( COLUMN_NAME, 270 );
  pHeader->setStretchLastSection( true );
  pHeader->setSortIndicator( COLUMN_VNUMBER, Qt::AscendingOrder );
  pHeader->setSortIndicatorShown( true );
  pHeader->setSectionsClickable( true );

  mp_popupFile = menuBar()->addMenu( trUtf8( "File" ) );
  mp_barFile = addToolBar( trUtf8( "File" ) );
  mp_barFile->setIconSize( QSize(16, 16) );

  mp_actRefreshAreas = new QAction( QIcon(":/images/searchareas.png"), trUtf8( "Genera lista aree" ), this );
  mp_actRefreshAreas->setStatusTip( trUtf8( "Genera la lista delle aree" ) );
  connect( mp_actRefreshAreas, SIGNAL( triggered() ), this, SLOT( loadAreas() ) );
  mp_popupFile->addAction( mp_actRefreshAreas );
  mp_barFile->addAction( mp_actRefreshAreas );

  mp_popupFile->addSeparator();
  mp_barFile->addSeparator();

  mp_actCreateArea = new QAction( QIcon(":/images/newarea.png"), trUtf8( "Nuova area..." ), this );
  mp_actCreateArea->setStatusTip( trUtf8( "Crea una nuova area" ) );
  connect( mp_actCreateArea, SIGNAL( triggered() ), this, SLOT( createArea() ) );
  mp_popupFile->addAction( mp_actCreateArea );
  mp_barFile->addAction( mp_actCreateArea );

  mp_actLoadArea = new QAction( QIcon(":/images/loadarea.png"), trUtf8( "Carica..." ), this );
  mp_actLoadArea->setStatusTip( trUtf8( "Carica un'area" ) );
  connect( mp_actLoadArea, SIGNAL( triggered() ), this, SLOT( loadArea() ) );
  mp_popupFile->addAction( mp_actLoadArea );
  mp_barFile->addAction( mp_actLoadArea );

  mp_mru = mp_popupFile->addMenu( QIcon(":/images/loadarea.png"), trUtf8( "File aperti di recente" ) );
  QList<QString> tmpList = KreatorSettings::instance().mru();
  for(int i = 0; i < tmpList.size(); ++i) {
	  QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(tmpList.at(i)));
	  mp_recentFileActs[i] = new QAction(this);
      mp_recentFileActs[i]->setText(text);
      mp_recentFileActs[i]->setData(tmpList.at(i));
	  mp_recentFileActs[i]->setStatusTip(tmpList.at(i));
	  connect (mp_recentFileActs[i], SIGNAL( triggered() ), this, SLOT( openRecentFile() ) );
	  mp_mru->addAction( mp_recentFileActs[i] );
  }


  mp_popupFile->addSeparator();
  mp_barFile->addSeparator();

  mp_actSettings = new QAction( QIcon(":/images/settings.png"), trUtf8( "Preferenze..." ), this );
  mp_actSettings->setStatusTip( trUtf8( "Apri la finestra delle preferenze" ) );
  connect( mp_actSettings, SIGNAL( triggered() ), this, SLOT( showSettings() ) );
  mp_popupFile->addAction( mp_actSettings );
  mp_barFile->addAction( mp_actSettings );

  mp_popupFile->addSeparator();
  mp_barFile->addSeparator();

  mp_actQuit = new QAction( QIcon(":/images/quit.png"), trUtf8( "Esci" ), this );
  mp_actQuit->setStatusTip( trUtf8( "Esci da TS Kreator" ) );
  connect( mp_actQuit, SIGNAL( triggered() ), this, SLOT( closeMainWindow() ) );
  mp_popupFile->addAction( mp_actQuit );
  mp_barFile->addAction( mp_actQuit );

  mp_popupHelp = menuBar()->addMenu( trUtf8( "?" ) );

  //mp_popupHelp->addAction( trUtf8( "Suggerimenti..." ) );
  //mp_popupHelp->addAction( trUtf8( "Aiuto..." ) );
  //mp_popupHelp->addSeparator();
  mp_popupHelp->addAction( QIcon( ":/images/info.png" ), trUtf8( "Informazioni..." ), this, SLOT( showAbout() ) );

  connect( mp_twObjectList, SIGNAL( itemDoubleClicked( QTreeWidgetItem *, int ) ),
    this, SLOT( loadArea( QTreeWidgetItem*, int ) ) );
  connect( mp_twObjectList, SIGNAL( itemSelectionChanged() ), this, SLOT( areaSelectionChanged() ) );
  connect( mp_leFilter, SIGNAL( textEdited( const QString& ) ), this, SLOT( filterList() ) );
}

void WndMainWindow::showMessage( const QString& msg, int timeout )
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMainWindow::showMessage( const QString&, int ) called." );
#endif
  statusBar()->showMessage( msg, timeout );
  emit( messageShowed( msg ) );
  QApplication::processEvents();
}

void WndMainWindow::areaSelectionChanged()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMainWindow::areaSelectionChanged() called." );
#endif
  QList<QTreeWidgetItem*> wiList = mp_twObjectList->selectedItems();
  if( wiList.empty() )
    return;
  QTreeWidgetItem* item = wiList.first();
  showMessage( trUtf8( "File: " ) + item->text( COLUMN_PATH ) );
}

void WndMainWindow::loadAreas()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMainWindow::loadAreas() called." );
#endif
  QApplication::processEvents();
  QTimer::singleShot( 500, this, SLOT( refreshAreaView() ) );
}

void WndMainWindow::createItem(int index, const QString& zone_path )
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMainWindow::createItem( const QString ) called." );
#endif

  VNumber vnum;
  FILE *pFile;

  if( ( pFile = fopen( qPrintable( zone_path ), "r" ) ) )
  {
    char szBuffer[ 255 ];

    while( fgets( szBuffer, sizeof( szBuffer ), pFile ) )
    {
      if( *szBuffer == '#' )
      {
        vnum = -1;
        sscanf( szBuffer, "#%ld", &vnum );

        if( vnum >= VNumberInvalid && vnum < VNumberTop )
        {
          Zone new_zone( vnum );
          new_zone.loadHeaderOnly( pFile );

          QTreeWidgetItem *item = new QTreeWidgetItem( mp_twObjectList );
          item->setIcon( COLUMN_VNUMBER, QIcon( ":/images/world.png" ) );
          item->setText( COLUMN_VNUMBER, Utils::vnumber2string( new_zone.vnumber(), 3 ) );
          item->setText( COLUMN_INDEX, QString::number(index));
          item->setText( COLUMN_NAME, new_zone.name() );
          item->setText( COLUMN_PATH, zone_path );
          item->setText( COLUMN_MINMAX, Utils::vnumber2string( new_zone.minVNumber() ) + QString( " - " ) + Utils::vnumber2string( new_zone.maxVNumber() ) );
          item->setText( COLUMN_REPOP_TIME, Utils::vnumber2string( new_zone.repopInterval(), 3 ) + QString( " min." ) );
          item->setText( COLUMN_REPOP_MODE, ConstantName::zoneRepopMode( new_zone.repopMode() ).toLower() );
          item->setText( COLUMN_FLAGS, Utils::bitvector2string( new_zone.newFlags(), Eleuconf::getAllFlagsCaption(Eleuconf::zoneFlags) ).toLower() );

          QString msg = "";
          msg.sprintf( "Controllo la Zona #%ld in %s", new_zone.vnumber(), qPrintable( zone_path ) );
          showMessage( msg );
        }
      }
    }
    fclose( pFile );
  }
}

void WndMainWindow::refreshAreaView()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMainWindow::checkAreas() called." );
#endif

  QString sAreaDir = Settings::zonesDirectory();

  if( sAreaDir.isEmpty() || sAreaDir == "." )
  {
    QMessageBox::information( this, TS::MESSAGE_BOX_INFORMATION,
      trUtf8( "Per favore seleziona le cartelle dove vuoi salvare le aree." ) );
    showSettings();
    return;
  }

  QDir areaDir( sAreaDir );
  if( !areaDir.exists() )
  {
    QMessageBox::warning( this, TS::MESSAGE_BOX_WARNING,
      trUtf8( "La cartella delle zone non esiste." ) );
    showSettings();
    return;
  }

  QApplication::setOverrideCursor( Qt::WaitCursor );
  mp_twObjectList->clear();
  showMessage( trUtf8( "Creo la lista delle aree esistenti..." ) );

  QFileInfoList fiList = areaDir.entryInfoList( QDir::Files );

  QList<QFileInfo>::const_iterator it = fiList.begin();
  int cnt = 0;
  while( it != fiList.end() )
  {
    if( (*it).suffix() == Settings::zoneSuffix() )
      createItem(cnt, (*it).absoluteFilePath() );
    ++it;
    cnt++;
  }

  mp_twObjectList->sortItems(COLUMN_MINMAX, Qt::AscendingOrder );

  for (int i = 0; i < mp_twObjectList->topLevelItemCount(); i++)
  {
      mp_twObjectList->topLevelItem(i)->setText(COLUMN_INDEX, QString::number(i));
  }

  showMessage(trUtf8("Pronto."));

  checkBootFile();

  for (int i = 0; i < COLUMN_FLAGS; i++)
      mp_twObjectList->resizeColumnToContents(i);

  QApplication::restoreOverrideCursor();
  
}

void WndMainWindow::showSettings()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMainWindow::showPageSettings() called." );
#endif
  WndSettings *pSettingsDialog = new WndSettings( this );
  pSettingsDialog->exec();
  delete pSettingsDialog;
}

void WndMainWindow::createArea()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMainWindow::createArea() called." );
#endif
  QString sFileName = QFileDialog::getSaveFileName( this, trUtf8( "Crea un nuovo file" ),
                                     Settings::zonesDirectory(), trUtf8( "Zone File (*.zon)" ) );

  if( sFileName.isNull() || sFileName.isEmpty() )
    return;

  QString zone_suffix = QString( "." ) + Settings::zoneSuffix();
  if( !sFileName.endsWith( zone_suffix ) )
    sFileName.append( zone_suffix );

  WndCreateArea* pwCreateArea = new WndCreateArea( sFileName, this );
  connect( pwCreateArea, SIGNAL( areaCreated( const QString& ) ),
    this, SLOT( loadArea( const QString& ) ) );
  pwCreateArea->show();
  pwCreateArea->setFixedSize( pwCreateArea->size() );
  pwCreateArea->exec();
}

void WndMainWindow::loadArea()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMainWindow::loadArea() called." );
#endif
  QString sFilter = "";
  sFilter.sprintf( "Area Files (*.%s *.%s *.%s *.%s)", Settings::roomSuffix().toUtf8().data(),
    Settings::mobSuffix().toUtf8().data(), Settings::itemSuffix().toUtf8().data(), Settings::zoneSuffix().toUtf8().data() );

  QList<QString> tmpList = KreatorSettings::instance().mru();
  QString file_selected;
  if(!tmpList.isEmpty()) {
	  QDir tmpDir(tmpList.at(0));
	  file_selected = QFileDialog::getOpenFileName( this, trUtf8( "Seleziona il file da aprire" ),
                            tmpDir.absolutePath(), sFilter );
  } else {
	  file_selected = QFileDialog::getOpenFileName( this, trUtf8( "Seleziona il file da aprire" ),
                            Settings::zonesDirectory(), sFilter );
  }
  if(!file_selected.isEmpty()){
	  tmpList.prepend( file_selected );
	  while(tmpList.size() > 5) {
		  tmpList.removeLast();
	  }
	  KreatorSettings::instance().setMru(tmpList);
	  mp_mru->clear();
	  for(int i = 0; i < tmpList.size(); ++i) {
		  QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(tmpList.at(i)));
		  mp_recentFileActs[i] = new QAction(this);
		  mp_recentFileActs[i]->setText(text);
		  mp_recentFileActs[i]->setData(tmpList.at(i));
		  mp_recentFileActs[i]->setStatusTip(tmpList.at(i));
		  connect (mp_recentFileActs[i], SIGNAL( triggered() ), this, SLOT( openRecentFile() ) );
		  mp_mru->addAction( mp_recentFileActs[i] );
	  }
  }

  loadArea( file_selected );
}

void WndMainWindow::loadArea( const QString& file_selected )
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMainWindow::loadArea( const QString& ) called." );
#endif
  if( file_selected.isNull() || file_selected.isEmpty() )
    return;
qDebug( "loadArea( const QString& ) 1" );
  WndArea* pwArea = new WndArea( this, "WndArea" );
  qDebug( "loadArea( const QString& ) 2" );
  pwArea->show();
  qDebug( "loadArea( const QString& ) 3" );
  pwArea->loadArea( file_selected );
  qDebug( "loadArea( const QString& ) 4" );
}

class MyMSG : public QMessageBox {
protected:
    void MyMSG::resizeEvent(QResizeEvent* Event)
    {
        QMessageBox::resizeEvent(Event);
        //this->setFixedWidth(myFixedWidth);
        //this->setFixedHeight(myFixedHeight);
    }
private:
    int myFixedWidth = 450;
    int myFixedHeight = 250;
};

void WndMainWindow::showAbout()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMainWindow::showAbout() called." );
#endif

  QString sAbout = "";
  sAbout.sprintf( "TS Kreator di Carlo(Otacon/Brandon)\n"
          "basato su LeU Kreator.\nAmpliato da Redis(Martinus) e Marino(Auriel)");

  //QMessageBox::about( this, TS::MESSAGE_BOX_ABOUT, sAbout );
  QMessageBox msgBox(this);
  msgBox.setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
  //msgBox.setParent(this);
  QFile fl(":/about/VERSION.txt");
  fl.open(QFile::ReadOnly);
  QTextStream in(&fl);
  in.setCodec("UTF-8");
  QString version = in.readAll();

  msgBox.setWindowIcon(this->windowIcon());
  msgBox.setWindowTitle(TS::MESSAGE_BOX_ABOUT);
  msgBox.setIcon(QMessageBox::Icon::Information);
  msgBox.setText(sAbout);
  msgBox.setInformativeText("Versione " + (VERSION) + " (" + __DATE__ + ")");
  msgBox.setDetailedText(version);
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.setDefaultButton(QMessageBox::Ok);
  msgBox.setEscapeButton(QMessageBox::Ok);
  msgBox.setFixedWidth(300);
  

  QSpacerItem* horizontalSpacer = new QSpacerItem
  (300, 0,
      QSizePolicy::Minimum, QSizePolicy::Expanding);
  QGridLayout* layout = (QGridLayout*)msgBox.layout();
  layout->addItem(horizontalSpacer, layout->rowCount(),0, 1, layout->columnCount());

  QAbstractButton* detailsButton = NULL;

  auto buttons = msgBox.findChildren<QAbstractButton*>();
  if (!buttons.isEmpty()) {
      detailsButton = buttons[1];
  }

  QList<QTextEdit*> textBoxes = msgBox.findChildren<QTextEdit*>();
  if (textBoxes.size())
      textBoxes[0]->setFixedHeight(250);
  msgBox.setVisible(false);
  msgBox.showMinimized();
  //msgBox.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);

  // If we have found the details button, then click it to expand the
  // details area.
  if (detailsButton) {
      detailsButton->click();
  }
  



  //msgBox.setStyleSheet("QLabel{align:left;width: 100px;min-width: 100px;max-width: 100px;}");



  msgBox.setWindowState(Qt::WindowState::WindowActive | Qt::WindowState::WindowNoState);
  buttons = msgBox.findChildren<QAbstractButton*>();
  //buttons[0]->setHidden(true);
  buttons[1]->setHidden(true);
  //buttons[0]->setVisible(false);
  buttons[1]->setVisible(false);
  msgBox.setFocus();
  int x_offset = (WndMainWindow::geometry().width() - 300) / 2;
  int y_offset = (WndMainWindow::geometry().height() - msgBox.geometry().height()) / 2;

  msgBox.setGeometry(
      WndMainWindow::geometry().x() + x_offset,
      WndMainWindow::geometry().y() + y_offset,
      300,
      msgBox.geometry().height());

  msgBox.exec();
}

void WndMainWindow::loadArea( QTreeWidgetItem* item, int )
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMainWindow::loadArea( QTreeWidgetItem*, int ) called." );
#endif
  if( !item )
    return;

  WndArea* pwArea = new WndArea( this, "WndArea" );
  pwArea->show();
  pwArea->loadArea( item->text( COLUMN_PATH ) );
}

void WndMainWindow::openRecentFile()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMainWindow::openRecentFile() called." );
#endif
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        loadArea(action->data().toString());
}

QString WndMainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void WndMainWindow::filterList()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMainWindow::filterList() called." );
#endif
  for( int i = 0; i < mp_twObjectList->topLevelItemCount(); i++)
  {
      if(mp_twObjectList->topLevelItem(i)->text(COLUMN_NAME).contains(mp_leFilter->text(), Qt::CaseInsensitive))
          mp_twObjectList->topLevelItem(i)->setHidden( false );
      else
          mp_twObjectList->topLevelItem(i)->setHidden( true );
  }
}

void WndMainWindow::checkBootFile()
{
    QString sAreaDir = Settings::zonesDirectory();

    FILE* filePointer = NULL;
    const int bufferLength = 255;
    char buffer[bufferLength];
    QStringList lines;
    snprintf(buffer, sizeof(buffer), "%s\\%s", qPrintable(sAreaDir), "boot");
    filePointer = fopen(buffer, "r");
    if (filePointer) {
        while (fgets(buffer, bufferLength, filePointer)) {
            buffer[strcspn(buffer, "#\r\n")] = 0;
            if (buffer[0]) {
                lines.append(QString::fromLatin1(buffer));
            }
        }

        fclose(filePointer);

        int lastAreaIndex = -1;
        int lastAreaVnum = -1;
        bool error = FALSE;
        QString lastAreaName = "";
        QString errorMessage = "";
        for (int i = 0; i < mp_twObjectList->topLevelItemCount(); i++)
        {
            QRegExp nomefile("\\/(\\w*)\\.zon");
            int pos = nomefile.indexIn(mp_twObjectList->topLevelItem(i)->text(COLUMN_PATH));
            if (pos > -1) {
                QString nomeZona = nomefile.cap(1);
                int areaIndex = lines.indexOf(nomeZona);
                if (areaIndex > -1 && areaIndex < lastAreaIndex)
                {
                    errorMessage = "Zona " + nomeZona + ".zon (file:" + mp_twObjectList->topLevelItem(i)->text(COLUMN_INDEX) +", boot:" + QString::number(areaIndex) + ")" + " nella lista aree non e' in ordine boot (precedente: " + lastAreaName + ".zon (file:" + QString::number(lastAreaVnum) + ", boot:" + QString::number(lastAreaIndex) + ")";
                    error = TRUE;
                    break;
                }
                else
                {
                    lastAreaVnum = mp_twObjectList->topLevelItem(i)->text(COLUMN_INDEX).toInt();
                    lastAreaIndex = areaIndex;
                    lastAreaName = nomeZona;
                }
            }
        }

        if (error) {
            showMessage(errorMessage);
        }
    }
}
