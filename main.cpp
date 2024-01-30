
#include <QFile>
#include <QTextStream>
#include <QSplashScreen>
#include <QTimer>
#include <QDateTime>
#include <stdio.h>
#include "wndmainwindow.h"
#include "tskreator.h"
#include "kreatorsettings.h"
#include "guiversion.h"
#include "settings.h"
#include <windows.h>
#include <QMessageBox>

QFile *pLogFile = 0;


void initLogFile()
{
  if( !pLogFile )
  {
     if(!ts::Settings::logsDirectory().isEmpty() && ts::Settings::logsDirectory().compare(".")) {
        pLogFile = new QFile( ts::Settings::logsDirectory(), 0 );
        pLogFile->open( QIODevice::WriteOnly );
     }
  }
}

void closeLogFile()
{
  if( pLogFile )
  {
    pLogFile->flush();
    pLogFile->close();
    delete pLogFile;
    pLogFile = 0;
  }
}

WndMainWindow* mainWin = NULL;

void showMessage(QString msg, int error = 0) {
    if (!mainWin) return;
    QMessageBox msgBox(mainWin);
    msgBox.setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
    msgBox.setWindowIcon(mainWin->windowIcon());
    msgBox.setWindowTitle(error ? (QString("Errore") + (error > 1 ? QString(" FATALE") : QString(""))) : QString("Messaggio"));
    msgBox.setIcon(error == 2 ? QMessageBox::Icon::Critical : error ? QMessageBox::Icon::Warning : QMessageBox::Icon::Information);
    msgBox.setText(msg);
    msgBox.exec();
}
void TsKreatorMessageHandler( QtMsgType type, const QMessageLogContext &ctx, const QString &msg )
{
  QString sTxt = "";
  BOOL isDebug = FALSE;
  switch( type )
  {
  case QtDebugMsg:
      isDebug = TRUE;
    sTxt = QString("[Debug] %1").arg(msg);
    break;
  case QtWarningMsg:
      if (!msg.contains("too many profiles")) {
          sTxt = QString("[Warning] %1").arg(msg);
      }
    break;
  case QtCriticalMsg:
    sTxt = QString( "[Critical]: %1").arg(msg );
    showMessage(sTxt, 1);
    break;
  case QtFatalMsg:
    sTxt = QString( "[Fatal] %1").arg(msg );
    showMessage(sTxt, 2);
    //abort();
  default:
    sTxt = QString( "[Info]: %1").arg(msg );
    break;
  }

  if( !sTxt.isEmpty() )
  {
      if (mainWin && !isDebug) mainWin->log(sTxt);
    if( pLogFile )
    {
      QTextStream stream( pLogFile );
      stream << sTxt << endl << flush;
    }
    else
    {
      sTxt += QString( "\n" );
      fprintf( stderr, sTxt.toUtf8().data() );
    }
  }
}



int main( int argc, char ** argv )
{
  
  bool dpi = SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
  //QApplication app(argc, argv);
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QFont font("Segoe UI");
  font.setStyleHint(QFont::SansSerif);
  font.setPointSize(8);
  QApplication::setFont(font);

  qInstallMessageHandler( TsKreatorMessageHandler );  
  TsKreator appTsKreator( argc, argv );
  initLogFile();
  QString sLogHeader = QString( "Starting log at " ) + QDateTime::currentDateTime().toString( "dd/MM/yyyy hh:mm:ss" );
  qDebug( sLogHeader.toUtf8().data() );
  WndMainWindow wndMain( 0 );
 
  QPixmap pixSplash(":/images/logo.png");
  QSplashScreen *pSplash = new QSplashScreen( pixSplash, Qt::WindowStaysOnTopHint );
  pSplash->connect( &wndMain, SIGNAL( messageShowed( const QString&, int, const QColor& ) ),
  pSplash, SLOT( showMessage( const QString&, int, const QColor& ) ) );

  pSplash->show();
  wndMain.connect( &wndMain, SIGNAL( windowClosed() ), &appTsKreator, SLOT( closeTsKreator() ) );
  int iDelay = 2200;
  if( argc > 1 )
  {
    QString area_path( argv[1] );
    wndMain.loadArea( area_path );
    iDelay = 1000;
  }
  else
  {
    if( KreatorSettings::instance().checkAreasAtStartup() )
    {
      wndMain.loadAreas();
    }
  }

  QTimer::singleShot( iDelay, pSplash, SLOT( close() ) );
  mainWin = &wndMain;
  QTimer::singleShot( iDelay + 200, &wndMain, SLOT( show() ) );

  int iAppRet = appTsKreator.exec();
  mainWin = NULL;
  delete pSplash;
  qDebug( "TS Kreator exit with code #%d.", iAppRet );
  closeLogFile();
  return iAppRet;
}
