
#ifndef TS_VALIDATOR_H
#define TS_VALIDATOR_H
#pragma warning(push, 0)
#include <QIntValidator>
#pragma warning(pop)
#include "selectobject.h"

class SpellValidator : public QIntValidator
{
public:
    explicit SpellValidator(QObject* parent = nullptr)
        :QIntValidator(parent) {}

    QValidator::State validate(QString& str, int& i) const {
        if (!str.size() || QIntValidator::validate(str, i) == State::Acceptable) {
            return State::Acceptable;
        }
        int spell = SelectObject::findSpell(str);
        if (spell) {
            return State::Intermediate;
        }
        return State::Invalid;
    }

    virtual void fixup(QString& input)const override {
        input = QString::number(SelectObject::findSpell(input));
    }
};

class Validator
{
public:

    static const QValidator* instrumentType()
    {
        return new QIntValidator(0, 2, 0);
    }

    static const QValidator* spell()
    {
        if (!mp_spell)
            mp_spell = new SpellValidator();
        return mp_spell;
    }

  static const QIntValidator* vnumber()
  {
    if( !mp_vnumber )
      mp_vnumber = new QIntValidator( -1, 99999, 0 );
    return mp_vnumber;
  }

  static const QIntValidator* integer()
  {
    if( !mp_integer )
      mp_integer = new QIntValidator( 0 );
    return mp_integer;
  }

  static const QIntValidator* unsignedInteger()
  {
    if( !mp_unsignedInteger )
    {
      mp_unsignedInteger = new QIntValidator( 0 );
      mp_unsignedInteger->setBottom( 0 );
    }
    return mp_integer;
  }

  static void cleanUp()
  {
    qDebug( "Validator::cleanUp() called." );
    if( mp_vnumber )
    {
      delete mp_vnumber;
      mp_vnumber = NULL;
    }

    if( mp_integer )
    {
      delete mp_integer;
      mp_integer = NULL;
    }

    if( mp_unsignedInteger )
    {
      delete mp_unsignedInteger;
      mp_unsignedInteger = NULL;
    }
  }

protected:
  Validator();
  ~Validator() { cleanUp(); }

private:
  static QIntValidator* mp_vnumber;
  static QIntValidator* mp_integer;
  static QIntValidator* mp_unsignedInteger;
  static QValidator* mp_spell;

};

#endif // TS_VALIDATOR_H
