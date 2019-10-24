#ifndef TS_TSKREATOR_H
#define TS_TSKREATOR_H
#include <qmetaobject.h>

#include <QApplication>



class TsKreator : public QApplication
{
  Q_OBJECT

public:
  TsKreator( int & argc, char ** argv );
  ~TsKreator();

public slots:
  void closeTsKreator();

protected:
  void init();
  void cleanUp();

};

#endif // TS_TSKREATOR_H
