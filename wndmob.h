#ifndef TS_WNDMOB_H
#define TS_WNDMOB_H
#pragma warning(push, 0)
#include <QDialog>
#include <QStandardItemModel>
#include "ui_guimob.h"
#include <QItemDelegate>
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
  void addBehavior();
  void removeBehavior();
  void moveUpBehavior();
  void moveDownBehavior();
  void addCondition();
  void removeCondition();
  void moveUpCondition();
  void moveDownCondition();
  void mp_Behaviors_currentRowChanged(const QModelIndex& current, const QModelIndex& previous);
  void mp_Conditions_currentRowChanged(const QModelIndex& current, const QModelIndex& previous);
  void conditionChanged(const QModelIndex current, QModelIndex previous);
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
  void showBehaviors();
  void showConditionsForBehavior(int);
  QString validateBehaviors();
  void applyBehaviors();
private:
  Area& m_area;
  Mob m_mob;
  Syntax* highLighter = NULL;
  QStandardItemModel *m_BehaviorModel;
  QStandardItemModel *m_ConditionModel;
  int currentBehaviorRow = -1;
  int currentConditionRow = -1;
  std::map<int, std::vector<std::vector<QString>>> behaviorConditionMap;
};

class ConditionComboBoxDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    ConditionComboBoxDelegate(QObject* parent = 0);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
        const QModelIndex& index) const;

    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    void setModelData(QWidget* editor, QAbstractItemModel* model,
        const QModelIndex& index) const;

    void updateEditorGeometry(QWidget* editor,
        const QStyleOptionViewItem& option, const QModelIndex& index) const;
};
class BehaviorComboBoxDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    BehaviorComboBoxDelegate(QObject* parent = 0);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
        const QModelIndex& index) const;

    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    void setModelData(QWidget* editor, QAbstractItemModel* model,
        const QModelIndex& index) const;

    void updateEditorGeometry(QWidget* editor,
        const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // TS_WNDMOB_H
