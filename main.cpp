
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

void TsKreatorMessageHandler( QtMsgType type, const QMessageLogContext &ctx, const QString &msg )
{
  QString sTxt = "";

  switch( type )
  {
  case QtDebugMsg:
    sTxt = QString("[Debug] %1").arg(msg);
    break;
  case QtWarningMsg:
    sTxt = QString( "[Warning] %1").arg(msg );
    break;
  case QtCriticalMsg:
    sTxt = QString( "[Critical]: %1").arg(msg );
    break;
  case QtFatalMsg:
    sTxt = QString( "[Fatal] %1").arg(msg );
    abort();
  default:
    sTxt = QString( "[Info]: %1").arg(msg );
    break;
  }

  if( !sTxt.isEmpty() )
  {
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
  
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
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
  //pSplash->setFixedWidth(468);
  //pSplash->setFixedHeight(100);
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
  QTimer::singleShot( iDelay + 200, &wndMain, SLOT( show() ) );

  int iAppRet = appTsKreator.exec();
  delete pSplash;
  qDebug( "TS Kreator exit with code #%d.", iAppRet );
  closeLogFile();
  return iAppRet;
}
