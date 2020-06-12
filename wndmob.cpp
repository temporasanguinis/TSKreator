#include "wndmob.h"
#pragma warning(push, 0)
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSpinBox>
#include <QCheckBox>
#include <QToolButton>
#include <QStandardItemModel>
#include <QTableView>
#pragma warning(pop)
#include "validator.h"
#include "config.h"
#include "constantname.h"
#include "selectobject.h"
#include "guiutils.h"
#include "kreatorsettings.h"

using namespace ts;


WndMob::WndMob( Area& ref_area, const Mob& ref_mob, QWidget* parent )
  : QDialog( parent ), Ui::GuiMob(),
    m_area( ref_area ),
    m_mob( ref_mob )
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::WndMob( const Area&, const Mob&, QWidget* ) called." );
#endif
  init();
  loadData();
  mp_leName->setFocus();
}

WndMob::~WndMob()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::~WndMob() called." );
#endif
  delete highLighter;
  delete m_ConditionModel;
  delete m_BehaviorModel;
}

void WndMob::init()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::init() called." );
#endif
  setupUi( this );
  m_BehaviorModel = new QStandardItemModel(mp_Behaviors);
  m_ConditionModel = new QStandardItemModel(mp_Conditions);
  if (!connect(m_BehaviorModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(somethingChanged()))) {
      qWarning("Cannot connect signal!");
  }
  if (!connect(m_ConditionModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(conditionChanged(QModelIndex, QModelIndex)))) {
      qWarning("Cannot connect signal!");
  }
  KreatorSettings::instance().loadGuiStatus( "MobWindow", this );
  highLighter = new Syntax(mp_teDescription->document());
  mp_teDescription->setFont( TS::GetFixedFont() );
  
  mp_tbAddBehavior->setIcon(TS::GetAddIcon());
  mp_tbDelBehavior->setIcon(TS::GetRemoveIcon());
  TS::SetUpArrow(mp_tbMoveUpBehavior);
  TS::SetDownArrow(mp_tbMoveDownBehavior);
  mp_tbAddCondition->setIcon(TS::GetAddIcon());
  mp_tbDelCondition->setIcon(TS::GetRemoveIcon());
  TS::SetUpArrow(mp_tbMoveUpCondition);
  TS::SetDownArrow(mp_tbMoveDownCondition);
  connect(mp_tbAddBehavior, SIGNAL(clicked()), this, SLOT(addBehavior()));
  connect(mp_tbDelBehavior, SIGNAL(clicked()), this, SLOT(removeBehavior()));
  connect(mp_tbMoveUpBehavior, SIGNAL(clicked()), this, SLOT(moveUpBehavior()));
  connect(mp_tbMoveDownBehavior, SIGNAL(clicked()), this, SLOT(moveDownBehavior()));
  connect(mp_tbAddCondition, SIGNAL(clicked()), this, SLOT(addCondition()));
  connect(mp_tbDelCondition, SIGNAL(clicked()), this, SLOT(removeCondition()));
  connect(mp_tbMoveUpCondition, SIGNAL(clicked()), this, SLOT(moveUpCondition()));
  connect(mp_tbMoveDownCondition, SIGNAL(clicked()), this, SLOT(moveDownCondition()));
  
  mp_tbActs->setIcon( TS::GetEditIcon() );
  mp_tbNewActs->setIcon( TS::GetEditIcon() );
  mp_tbAffects->setIcon( TS::GetEditIcon() );
  mp_tbAffects2->setIcon( TS::GetEditIcon() );
  mp_tbResistances->setIcon( TS::GetEditIcon() );
  mp_tbImmunities->setIcon( TS::GetEditIcon() );
  mp_tbSusceptibilities->setIcon( TS::GetEditIcon() );
  TS::SetLeftArrow( mp_pbPrevMob );
  TS::SetRightArrow( mp_pbNextMob );
  mp_pbInit->setIcon( TS::GetInitIcon() );

  mp_leVNumber->setValidator( Validator::vnumber() );

  int i;
  mp_comboRace->clear();
  for( i = 0; i < CHARACTER_RACE_END; i++ )
    mp_comboRace->insertItem( i, ConstantName::characterRace( i ) );

  mp_comboGender->clear();
  for( i = 0; i < CHARACTER_GENDER_END; i++ )
    mp_comboGender->insertItem( i, ConstantName::characterGender( i ).toLower() );

  mp_comboLoadPosition->clear();
  mp_comboDefaultPosition->clear();
  for( i = 0; i < CHARACTER_POSITION_END; i++ )
  {
    mp_comboLoadPosition->insertItem( i, ConstantName::characterPosition( i ).toLower() );
    mp_comboDefaultPosition->insertItem( i, ConstantName::characterPosition( i ).toLower() );
  }

  mp_comboType->clear();
  for( i = 0; i < MOB_TYPE_END; i++ )
    mp_comboType->insertItem( i, ConstantName::mobType( i ).toLower() );

  mp_leAlignment->setValidator( Validator::integer() );
  mp_leXPBonus->setValidator( Validator::integer() );
  mp_leHPBonus->setValidator( Validator::integer() );
  mp_leGold->setValidator( Validator::unsignedInteger() );

  connect( mp_pbInit, SIGNAL( clicked() ), this, SLOT( editInits() ));

  connect( mp_comboType, SIGNAL( activated( int ) ), this, SLOT( typeSelected( int ) ) );
  connect( mp_tbActs, SIGNAL( clicked() ), this, SLOT( editActs() ) );
  connect( mp_tbNewActs, SIGNAL( clicked() ), this, SLOT( editNewActs() ) );
  connect( mp_tbAffects, SIGNAL( clicked() ), this, SLOT( editAffects() ) );
  connect( mp_tbAffects2, SIGNAL( clicked() ), this, SLOT( editAffects2() ) );
  connect( mp_tbResistances, SIGNAL( clicked() ), this, SLOT( editResistances() ) );
  connect( mp_tbImmunities, SIGNAL( clicked() ), this, SLOT( editImmunities() ) );
  connect( mp_tbSusceptibilities, SIGNAL( clicked() ), this, SLOT( editSusceptibilities() ) );

  connect( mp_pbPrevMob, SIGNAL( clicked() ), this, SLOT( showPrevMob() ) );
  connect( mp_pbNextMob, SIGNAL( clicked() ), this, SLOT( showNextMob() ) );

  connect( mp_leVNumber, SIGNAL( textChanged( const QString& ) ), this, SLOT( somethingChanged() ) );
  connect( mp_leName, SIGNAL( textChanged( const QString& ) ), this, SLOT( somethingChanged() ) );
  connect( mp_leShort, SIGNAL( textChanged( const QString& ) ), this, SLOT( somethingChanged() ) );
  connect( mp_leLong, SIGNAL( textChanged( const QString& ) ), this, SLOT( somethingChanged() ) );
  connect( mp_comboRace, SIGNAL( activated( int ) ), this, SLOT( somethingChanged() ) );
  connect( mp_sbLevel, SIGNAL( valueChanged( int ) ), this, SLOT( somethingChanged() ) );
  connect( mp_leAlignment, SIGNAL( textChanged( const QString& ) ), this, SLOT( somethingChanged() ) );
  connect( mp_leXPBonus, SIGNAL( textChanged( const QString& ) ), this, SLOT( somethingChanged() ) );
  connect( mp_leHPBonus, SIGNAL( textChanged( const QString& ) ), this, SLOT( somethingChanged() ) );
  connect( mp_comboGender, SIGNAL( activated( int ) ), this, SLOT( somethingChanged() ) );
  connect( mp_sbAttacks, SIGNAL( valueChanged( int ) ), this, SLOT( somethingChanged() ) );
  connect( mp_sbArmor, SIGNAL( valueChanged( int ) ), this, SLOT( somethingChanged() ) );
  connect( mp_sbThac0, SIGNAL( valueChanged( int ) ), this, SLOT( somethingChanged() ) );
  connect( mp_leDamage, SIGNAL( textChanged( const QString& ) ), this, SLOT( somethingChanged() ) );
  connect( mp_comboDefaultPosition, SIGNAL( activated( int ) ), this, SLOT( somethingChanged() ) );
  connect( mp_comboLoadPosition, SIGNAL( activated( int ) ), this, SLOT( somethingChanged() ) );
  connect( mp_sbSpellPower, SIGNAL( valueChanged( int ) ), this, SLOT( somethingChanged() ) );
  connect( mp_sbAbsorbDam, SIGNAL( valueChanged( int ) ), this, SLOT( somethingChanged() ) );
  connect( mp_leGold, SIGNAL( textChanged( const QString& ) ), this, SLOT( somethingChanged() ) );
  connect( mp_teDescription, SIGNAL( textChanged() ), this, SLOT( somethingChanged() ) );
  connect( mp_teRoomSound, SIGNAL( textChanged() ), this, SLOT( somethingChanged() ) );
  connect( mp_teNearSound, SIGNAL( textChanged() ), this, SLOT( somethingChanged() ) );

  connect( mp_pbSave, SIGNAL( clicked() ), this, SLOT( saveData() ) );
  connect( mp_pbRestore, SIGNAL( clicked() ), this, SLOT( restoreData() ) );
  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( saveAndClose() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( close() ) );
  m_ConditionModel->clear();
  m_ConditionModel->setColumnCount(2);
  mp_Conditions->horizontalHeader()->setStretchLastSection(true);
  m_ConditionModel->setHeaderData(0, Qt::Orientation::Horizontal, "Condizione");
  m_ConditionModel->setHeaderData(1, Qt::Orientation::Horizontal, "Valore");
  mp_Conditions->setItemDelegate(new ConditionComboBoxDelegate(mp_Conditions));
  mp_Conditions->setModel(m_ConditionModel);

  m_BehaviorModel->clear();
  m_BehaviorModel->setColumnCount(4);

  //mp_Behaviors->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  mp_Behaviors->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  mp_Behaviors->verticalHeader()->setDefaultSectionSize(48);
  mp_Behaviors->horizontalHeader()->setStretchLastSection(true);
  m_BehaviorModel->setHeaderData(0, Qt::Orientation::Horizontal, "Tipo Evento");
  m_BehaviorModel->setHeaderData(1, Qt::Orientation::Horizontal, "Evento");
  m_BehaviorModel->setHeaderData(2, Qt::Orientation::Horizontal, "Tipo reazione");
  m_BehaviorModel->setHeaderData(3, Qt::Orientation::Horizontal, "Reazione");
  mp_Behaviors->setItemDelegate(new BehaviorComboBoxDelegate(mp_Behaviors));
  mp_Behaviors->setModel(m_BehaviorModel);
  
  mp_Behaviors->setColumnWidth(0, 190);
  mp_Behaviors->setColumnWidth(1, 190);
  mp_Behaviors->setColumnWidth(2, 270);
  mp_Conditions->setColumnWidth(0, 270);
  mp_Behaviors->setSelectionBehavior(QAbstractItemView::SelectRows);
  mp_Behaviors->setSelectionMode(QAbstractItemView::SingleSelection);
  mp_Conditions->setSelectionBehavior(QAbstractItemView::SelectRows);
  mp_Conditions->setSelectionMode(QAbstractItemView::SingleSelection);

  QItemSelectionModel* sm = mp_Conditions->selectionModel();
  connect(sm, SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
      this, SLOT(mp_Conditions_currentRowChanged(QModelIndex, QModelIndex)));

  QItemSelectionModel* sm2 = mp_Behaviors->selectionModel();
  if (!connect(sm2, SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
      this, SLOT(mp_Behaviors_currentRowChanged(QModelIndex, QModelIndex)))) {

  }

  //for (int row = 0; row < 4; ++row) {
  //    QList<QStandardItem*> rowData;
  //    rowData << (new QStandardItem(QString("%0").arg(row)));
  //    rowData << new QStandardItem(QString("%0").arg(row));
  //    rowData << new QStandardItem(QString("%0").arg(row));
  //    rowData << new QStandardItem(QString("%0").arg(row));
  //    m_BehaviorModel->appendRow(rowData);
  //    mp_Behaviors->openPersistentEditor(m_BehaviorModel->index(row, 0));
  //    mp_Behaviors->openPersistentEditor(m_BehaviorModel->index(row, 1));
  //    mp_Behaviors->openPersistentEditor(m_BehaviorModel->index(row, 2));
  //    mp_Behaviors->openPersistentEditor(m_BehaviorModel->index(row, 3));
  //}

  //for (int row = 0; row < 4; ++row) {
  //    QList<QStandardItem*> rowData;
  //    rowData << new QStandardItem(QString("%0").arg(row));
  //    rowData << new QStandardItem(QString("%0").arg(row));
  //    m_ConditionModel->appendRow(rowData);
  //    mp_Conditions->openPersistentEditor(m_ConditionModel->index(row, 0));
  //    mp_Conditions->openPersistentEditor(m_ConditionModel->index(row, 1));
  //}
}

void WndMob::conditionChanged(const QModelIndex current, QModelIndex previous) {
    if (current.isValid()) {
        auto v = m_ConditionModel->item(current.row(), current.column());
        auto str = v->data(Qt::EditRole).toString();
        behaviorConditionMap[currentBehaviorRow][current.row()][current.column()] = str;
    }
    somethingChanged();
}

void WndMob::refreshPanel()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::refreshPanel() called." );
#endif

  mp_leVNumber->setText( QString::number( m_mob.vnumber() ) );
  mp_leName->setText( m_mob.name() );
  mp_comboType->setCurrentIndex( m_mob.mobType() );
  typeSelected( m_mob.mobType() );
  mp_leShort->setText( m_mob.shortDescription() );
  mp_leLong->setText( m_mob.longDescription() );
  mp_comboRace->setCurrentIndex( m_mob.race() );
  mp_sbLevel->setValue( m_mob.mobLevel() );
  mp_leAlignment->setText( QString::number( m_mob.alignment() ) );
  mp_leXPBonus->setText( QString::number( m_mob.xpBonus() ) );
  mp_leHPBonus->setText( QString::number( m_mob.hitPointsBonus() ) );
  mp_comboGender->setCurrentIndex( m_mob.gender() );
  mp_sbAttacks->setValue( (int)m_mob.attacks() );
  mp_sbArmor->setValue( m_mob.armor() / 10 );
  mp_sbThac0->setValue( m_mob.thac0() );
  mp_leDamage->setText( m_mob.damage().toString() );
  mp_comboDefaultPosition->setCurrentIndex( m_mob.mobDefaultPosition() );
  mp_comboLoadPosition->setCurrentIndex( m_mob.position() );
  mp_sbSpellPower->setValue( m_mob.spellPower() );
  mp_sbAbsorbDam->setValue( m_mob.absorbDam() );
  mp_leGold->setText( QString::number( m_mob.gold() ) );
  mp_teDescription->setPlainText( m_mob.description() );
  mp_teRoomSound->setPlainText( m_mob.roomSound() );
  mp_teNearSound->setPlainText( m_mob.nearSound() );
}

void WndMob::refreshTitle()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::refreshTitle() called." );
#endif
  QString sTitle = "";
  sTitle.sprintf( "Mob #%ld - %s", m_mob.vnumber(), qPrintable( m_mob.name() ) );
  if( mp_pbSave->isEnabled() )
    sTitle.append( QString( " [modificato]" ) );
  setWindowTitle( sTitle );

  if( mp_comboType->currentIndex() == MOB_TYPE_MULTI_ATTACKS || mp_comboType->currentIndex() == MOB_TYPE_UNBASHABLE || mp_comboType->currentIndex() == MOB_TYPE_SOUND )
  {
      mp_tbAffects2->setEnabled( true );
  } else {
      mp_tbAffects2->setEnabled( false );
  }
}

void WndMob::somethingChanged()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::somethingChanged() called." );
#endif
  mp_pbSave->setEnabled( true );
  refreshTitle();
}

void WndMob::restoreData()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::restoreData() called." );
#endif
  if( m_area.hasMob( m_mob.vnumber() ) )
  {
    m_mob = m_area.mob( m_mob.vnumber() );
    loadData();
  }
  else
    QMessageBox::warning( this, TS::MESSAGE_BOX_WARNING,
                   TS::MESSAGE_WARNING_NO_VNUMBER.arg( m_mob.vnumber() ) );
}

void WndMob::loadData()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::loadData() called." );
#endif
  refreshPanel();
  refreshAverageHpAndXP();
  refreshActs();
  refreshNewActs();
  refreshAffects();
  refreshAffects2();
  refreshResistances();
  refreshImmunities();
  refreshSusceptibilities();
  showBehaviors();
  mp_pbSave->setEnabled( false );
  refreshTitle();
}

void WndMob::saveData()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::saveData() called." );
#endif
  VNumber old_vnum = m_mob.vnumber();
  VNumber new_vnum = mp_leVNumber->text().toLong();

  QString valid = validateBehaviors();
  if (valid!="") {
      QMessageBox::critical(this, "Errore", QString("Ci sono errori nei behavior del mob.\nRivedere e coreggere.\n\n%0").arg(valid));
      return;
  }
  else {
      applyBehaviors();
  }

  if( old_vnum != new_vnum )
  {
    if( m_area.hasMob( new_vnum )
      && QMessageBox::question( this, TS::MESSAGE_BOX_QUESTION,
                        TS::MESSAGE_ASK_OVERWRITE_VNUMBER.arg( new_vnum ),
                        trUtf8( "Si" ), trUtf8( "No" ), QString(), 1, 1 )
      )
      return;

    if( !QMessageBox::question( this, TS::MESSAGE_BOX_QUESTION,
                        TS::MESSAGE_ASK_UPDATE_REFERENCES.arg( old_vnum ).arg( new_vnum ),
                        trUtf8( "Si" ), trUtf8( "No" ), QString(), 0, 1 )
      )
      m_area.changeMobVNumber( old_vnum, new_vnum, true );

  }

  m_mob.setVNumber( new_vnum );
  m_mob.setName( mp_leName->text() );
  m_mob.setMobType( mp_comboType->currentIndex() );
  m_mob.setShortDescription( mp_leShort->text() );
  m_mob.setLongDescription( mp_leLong->text() );
  m_mob.setRace( mp_comboRace->currentIndex() );
  m_mob.setMobLevel( mp_sbLevel->value() );
  m_mob.setAlignment( mp_leAlignment->text().toInt() );
  m_mob.setHitPointsBonus(mp_leHPBonus->text().toInt());
  m_mob.setXpBonus( mp_leXPBonus->text().toLong() );
  m_mob.setGender( mp_comboGender->currentIndex() );
  m_mob.setAttacks( mp_sbAttacks->value() );
  m_mob.setArmor( mp_sbArmor->value() * 10 );
  m_mob.setHitRoll( 20 - mp_sbThac0->value() );
  m_mob.setDamage( Dice( mp_leDamage->text().simplified() ) );
  m_mob.setMobDefaultPosition( mp_comboDefaultPosition->currentIndex() );
  m_mob.setPosition( mp_comboLoadPosition->currentIndex() );
  m_mob.setSpellPower( mp_sbSpellPower->value() );
  m_mob.setAbsorbDam( mp_sbAbsorbDam->value() );
  m_mob.setGold( mp_leGold->text().toInt() );
  m_mob.setDescription( mp_teDescription->toPlainText() );
  m_mob.setRoomSound( mp_teRoomSound->toPlainText() );
  m_mob.setNearSound( mp_teNearSound->toPlainText() );


  m_area.addMob( m_mob );
  m_area.setMobsChanged();
  m_area.sortMobs();

  mp_pbSave->setEnabled( false );
  refreshTitle();
  refreshAverageHpAndXP();
  emit( dataSaved() );
}

void WndMob::saveAndClose()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::saveAndClose() called." );
#endif
  if( mp_pbSave->isEnabled() )
    saveData();
  close();
}

void WndMob::closeEvent( QCloseEvent* e )
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::closeEvent( QCloseEvent* ) called." );
#endif

  if( mp_pbSave->isEnabled() )
  {
    if( KreatorSettings::instance().applyWithoutAsking() )
    {
      saveData();
      e->accept();
    }
    else
    {
      switch( QMessageBox::question( this, TS::MESSAGE_BOX_QUESTION, TS::MESSAGE_ASK_SAVE,
                trUtf8( "Si" ), trUtf8( "No" ), trUtf8( "Annulla" ), 1, 2 ) )
      {
      case 0:
        saveData();
        e->accept();
        break;
      case 1:
        e->accept();
        break;
      default:
        e->ignore();
        break;
      }
    }
  }
  else
    e->accept();

  if( e->isAccepted() )
    KreatorSettings::instance().saveGuiStatus( "MobWindow", this );
}

void WndMob::typeSelected( int iType )
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::typeSelected( int ) called." );
#endif

  if( iType != m_mob.mobType() )
  {
    somethingChanged();
  }

  mp_teRoomSound->setEnabled( iType == MOB_TYPE_SOUND );
  mp_teNearSound->setEnabled( iType == MOB_TYPE_SOUND );
}

void WndMob::showNextMob()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::showNextMob() called." );
#endif
  QCloseEvent ce;
  closeEvent( &ce );
  if( !ce.isAccepted() )
    return;

  VNumber next_vnum = m_area.nextObjectInList( ObjectData::Object_Mob, m_mob.vnumber() );
  if( next_vnum != VNumberInvalid )
  {
    m_mob = m_area.mob( next_vnum );
    loadData();
    mp_pbPrevMob->setEnabled( true );
  }
  else
    mp_pbNextMob->setEnabled( false );
}

void WndMob::showPrevMob()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::showPrevMob() called." );
#endif

  QCloseEvent ce;
  closeEvent( &ce );
  if( !ce.isAccepted() )
    return;

  VNumber prev_vnum = m_area.prevObjectInList( ObjectData::Object_Mob, m_mob.vnumber() );
  if( prev_vnum != VNumberInvalid )
  {
    m_mob = m_area.mob( prev_vnum );
    loadData();
    mp_pbNextMob->setEnabled( true );
  }
  else
    mp_pbPrevMob->setEnabled( false );
}

void WndMob::refreshActs()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::refreshActs() called." );
#endif
  mp_leActs->setText( Utils::bitvector2string( m_mob.acts(), Eleuconf::getAllFlagsCaption(Eleuconf::mobActionsFlags) ).toLower() );
}

void WndMob::refreshNewActs()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::refreshNewActs() called." );
#endif
  mp_leNewActs->setText( Utils::bitvector2string( m_mob.extraFlags(), Eleuconf::getAllFlagsCaption(Eleuconf::mobActionsFlags, 1) ).toLower() );
}

void WndMob::refreshAffects()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::refreshAffects() called." );
#endif
  mp_leAffects->setText( Utils::bitvector2string( m_mob.affects(), Eleuconf::getAllFlagsCaption(Eleuconf::mobAffectionsFlags) ).toLower() );
}

void WndMob::refreshAffects2()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::refreshAffects2() called." );
#endif
  mp_leAffects2->setText( Utils::bitvector2string( m_mob.affects2(), Eleuconf::getAllFlagsCaption(Eleuconf::mobAffectionsFlags, 1) ).toLower() );
}

void WndMob::refreshResistances()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::refreshResistances() called." );
#endif
  mp_leResistances->setText( Utils::bitvector2string( m_mob.resistances(), Eleuconf::getAllFlagsCaption(Eleuconf::resFlags) ).toLower() );
}

void WndMob::refreshImmunities()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::refreshImmunities() called." );
#endif
  mp_leImmunities->setText( Utils::bitvector2string( m_mob.immunities(), Eleuconf::getAllFlagsCaption(Eleuconf::resFlags) ).toLower() );
}

void WndMob::refreshSusceptibilities()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::refreshSusceptibilities() called." );
#endif
  mp_leSusceptibilities->setText( Utils::bitvector2string( m_mob.susceptibilities(), Eleuconf::getAllFlagsCaption(Eleuconf::resFlags) ).toLower() );
}

void WndMob::editActs()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::editActs( int ) called." );
#endif
  BitVector vFlags = m_mob.acts();
  BitVector vRetFlags = SelectObject::mobActs( vFlags, this );
  if( vRetFlags == vFlags )
    return;
  m_mob.setActs( vRetFlags );
  refreshActs();
  somethingChanged();
}

void WndMob::editNewActs()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::editNewActs( int ) called." );
#endif
  BitVector vFlags = m_mob.extraFlags();
  BitVector vRetFlags = SelectObject::mobNewActs( vFlags, this );
  if( vRetFlags == vFlags )
      return;
  m_mob.setExtraFlags( vRetFlags );
  refreshNewActs();
  somethingChanged();
}

void WndMob::editAffects()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::editAffects( int ) called." );
#endif
  BitVector vFlags = m_mob.affects();
  BitVector vRetFlags = SelectObject::affectMobFlags( vFlags, this );
  if( vRetFlags == vFlags )
    return;
  m_mob.setAffects( vRetFlags );
  refreshAffects();
  somethingChanged();
}

void WndMob::editAffects2()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::editAffects2( int ) called." );
#endif
  BitVector vFlags = m_mob.affects2();
  BitVector vRetFlags = SelectObject::affect2Flags( vFlags, this );
  if( vRetFlags == vFlags )
    return;
  m_mob.setAffects2( vRetFlags );
  refreshAffects2();
  somethingChanged();
}

void WndMob::editResistances()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::editResistances( int ) called." );
#endif
  BitVector vFlags = m_mob.resistances();
  BitVector vRetFlags = SelectObject::damageImmunityType( vFlags, "Resistenze", this );
  if( vRetFlags == vFlags )
    return;
  m_mob.setResistances( vRetFlags );
  refreshResistances();
  somethingChanged();
}

void WndMob::editImmunities()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::editImmunities( int ) called." );
#endif
  BitVector vFlags = m_mob.immunities();
  BitVector vRetFlags = SelectObject::damageImmunityType( vFlags, "Immunita'",this );
  if( vRetFlags == vFlags )
    return;
  m_mob.setImmunities( vRetFlags );
  refreshImmunities();
  somethingChanged();
}

void WndMob::editSusceptibilities()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::editSusceptibilities( int ) called." );
#endif
  BitVector vFlags = m_mob.susceptibilities();
  BitVector vRetFlags = SelectObject::damageImmunityType( vFlags, "Suscettibilita'", this );
  if( vRetFlags == vFlags )
    return;
  m_mob.setSusceptibilities( vRetFlags );
  refreshSusceptibilities();
  somethingChanged();
}

void WndMob::editInits()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::editInits() called." );
#endif

    SelectObject::showInits(true, m_mob.vnumber(), m_area, this);
}

void WndMob::refreshAverageHpAndXP()
{
#if defined( KREATOR_DEBUG )
  qDebug( "WndMob::refreshAverageHpAndXP() called." );
#endif
  mp_leXP->setText( QString::number( m_mob.experience() ) );
  mp_leHP->setText( QString::number( m_mob.maxHitPoints() ) );
}

void addItem(QStandardItemModel* model, QString display, QString value, int index) {
    QStandardItem* item2 = new QStandardItem();
    item2->setData(display, Qt::DisplayRole);
    item2->setData(value, Qt::UserRole);
    model->setItem(index, item2);
}

ConditionComboBoxDelegate::ConditionComboBoxDelegate(QObject* parent)
    : QItemDelegate(parent)
{
}

QWidget* ConditionComboBoxDelegate::createEditor(QWidget* parent,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    if (index.column() != 0) {
        return QItemDelegate::createEditor(parent, option, index);
    }
    QComboBox* editor = new QComboBox(parent);
    QStandardItemModel* model = new QStandardItemModel(editor);
    addItem(model, "<nessuna>", "", 0);
    addItem(model, "Mob ha oggetto", "0", 1);
    addItem(model, "Mob non ha oggetto", "1", 2);
    addItem(model, "Mob VNUM e' vivo", "2", 3);
    addItem(model, "Mob VNUM non e' vivo", "3", 4);
    addItem(model, "Check persistenza", "4", 5);
    addItem(model, "PG ha Award", "5", 6);
    addItem(model, "PG Non ha Award", "6", 7);
    addItem(model, "Confronta ricordo 1", "7", 8);
    addItem(model, "Confronta ricordo 2", "8", 9);
    addItem(model, "Pg ha Soldi sopra", "9", 10);
    addItem(model, "Pg ha Soldi sotto", "10", 11);

    editor->setModel(model);
    if (index.column() == 0) {
        editor->setItemData(0, "Scegliendo <nessuno> e' come cancellare la condizione", Qt::ToolTipRole);
        editor->setItemData(1, "La condizione riesce se il mob ha un oggetto in inventario\nIl valore e' il VNUM dell'oggetto", Qt::ToolTipRole);
        editor->setItemData(2, "La condizione riesce se il mob NON ha un oggetto in inventario\nIl valore e' il VNUM dell'oggetto", Qt::ToolTipRole);
        editor->setItemData(3, "La condizione riesce se il mob con VNUM e' stato trovato nel Mud\nIl valore e' il VNUM del Mob", Qt::ToolTipRole);
        editor->setItemData(4, "La condizione riesce se il mob con VNUM NON e' stato trovato nel MUD\nIl valore e' il VNUM del Mob", Qt::ToolTipRole);
        editor->setItemData(5, "Per mob che danno XP, Elementi o Divini\nValori possibili:\n"
            "  3 : Mob non ha ancora dato Elementali al PG\n"
            "  2 : Mob non ha ancora dato Divini al PG\n"
            "  1 : Mob non ha ancora dato XP al PG\n"
            " -1 : Mob ha gia' dato XP al PG\n"
            " -2 : Mob ha gia' dato Divini al PG\n"
            " -3 : Mob ha gia' dato Elementali al PG\n\n"
            "Es: La reazione darebbe divini al PG ma il pg ha gia avuto divini da questo mob.\n"
            "In questo caso la condizione con il valore 3 lo preverrebbe.\n"
            "E per dire al pg in reazione tell o reazione Interrompi si userebbe -3.", Qt::ToolTipRole);
        editor->setItemData(6, "La condizione riesce se il PG ha l'award\nIl valore e' il NUM del Award", Qt::ToolTipRole);
        editor->setItemData(7, "La condizione riesce se il PG NON ha l'award\nIl valore e' il NUM del Award", Qt::ToolTipRole);
        editor->setItemData(8, "La condizione riesce se la memoria del mob nel campo 1 combacia\nIl valore e' il valore da confrontare con la memoria", Qt::ToolTipRole);
        editor->setItemData(9, "La condizione riesce se la memoria del mob nel campo 2 combacia\nIl valore e' il valore da confrontare con la memoria", Qt::ToolTipRole);
        editor->setItemData(10, "La condizione riesce se il PG ha almeno X soldi\nIl valore e' X da confrontare con i soldi del pg", Qt::ToolTipRole);
        editor->setItemData(11, "La condizione riesce se il PG NON ha almeno X soldi\nIl valore e' X da confrontare con i soldi del pg", Qt::ToolTipRole);
    }
    return editor;
}

void ConditionComboBoxDelegate::setEditorData(QWidget* editor,
    const QModelIndex& index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    if (index.column() == 0) {
        QComboBox* cBox = static_cast<QComboBox*>(editor);
        cBox->setCurrentIndex(cBox->findData(value));
    }
    else {
        QLineEdit* qL = static_cast<QLineEdit*>(editor);
        qL->setText(value);
    }
}

void ConditionComboBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
    const QModelIndex& index) const
{
    if (index.column() == 0) {
        QComboBox* cBox = static_cast<QComboBox*>(editor);
        QVariant value = cBox->itemData(cBox->currentIndex());

        model->setData(index, value, Qt::EditRole);
    }
    else {
        QLineEdit* qL = static_cast<QLineEdit*>(editor);
        model->setData(index, qL->text(), Qt::EditRole);
    }
}

void ConditionComboBoxDelegate::updateEditorGeometry(QWidget* editor,
    const QStyleOptionViewItem& option, const QModelIndex&/* index */) const
{
    editor->setGeometry(option.rect);
}


BehaviorComboBoxDelegate::BehaviorComboBoxDelegate(QObject* parent)
    : QItemDelegate(parent)
{
}

QWidget* BehaviorComboBoxDelegate::createEditor(QWidget* parent,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    if (index.column() != 0 && index.column() != 2) {
        if (index.column() == 3) {
            auto ret = new QTextEdit(parent);
            ret->setToolTip("Parametri alla reazione (es. se parla quello che dice)");
            return ret;
        }
        else {
            return QItemDelegate::createEditor(parent, option, index);
        }
    }
    QComboBox* editor = new QComboBox(parent);
    QStandardItemModel* model = new QStandardItemModel(editor);
    if (index.column() == 0) {
        addItem(model, "<nessuno>", "", 0);
        addItem(model, "PG dice", "0", 1);
        addItem(model, "PG da oggetto", "1", 2);
    }
    else if (index.column() == 2) {
        addItem(model, "<nessuno>", "", 0);
        addItem(model, "Parla", "0", 1);
        addItem(model, "Dai oggetto", "1", 2);
        addItem(model, "Echo in stanza", "2", 3);
        addItem(model, "Dai XP", "3", 4);
        addItem(model, "Dai Elementi", "4", 5);
        addItem(model, "Dai Divini", "5", 6);
        addItem(model, "Carica e dai random da range", "6", 7);
        addItem(model, "Distruggi oggetti", "7", 8);
        addItem(model, "Interrompi parsing", "8", 9);
        addItem(model, "Dai award numero al pg", "9", 10);
        addItem(model, "Togli award al pg", "10", 11);
        addItem(model, "Dai soldi al pg", "11", 12);
        addItem(model, "Prendi soldi dal pg", "12", 13);
        addItem(model, "Ricorda (memoria 1)", "13", 14);
        addItem(model, "Ricorda (memoria 2)", "14", 15);
    }
    editor->setModel(model);
    if (index.column() == 0) {
        editor->setItemData(0, "Scegliendo <nessuno> e' come cancellare l'evento o la reazione", Qt::ToolTipRole);
        editor->setItemData(1, "Scatta quando il pg parla al mob di tell o ask\nIn questo caso l'evento sono le parola chiavi\nStringa vuota scatta per ogni talk", Qt::ToolTipRole);
        editor->setItemData(2, "Scatta quando il pg da un oggetto al mob\nIn questo caso l'evento e' il VNUM dell'oggetto\n-1 scatta su tutti gli oggetti\nL'oggetto dato viene cancellato", Qt::ToolTipRole);
    }
    if (index.column() == 2) {
        editor->setItemData(0, "Scegliendo <nessuno> e' come cancellare l'evento o la reazione", Qt::ToolTipRole);
        editor->setItemData(1, "Il mob reagira' parlarndo al PG\nIn questo caso l'evento e' la frase da dire\nPuoi usare %m = nome del mob, %u = nome del pg", Qt::ToolTipRole);
        editor->setItemData(2, "Il mob reagira' dando un oggetto che ha in inventario\nIn questo caso l'evento e' il VNUM dell'oggetto", Qt::ToolTipRole);
        editor->setItemData(3, "Il mob reagira' facendo un echo nella stanza\nIn questo caso l'evento e' l' echo da fare\n(Shift+Enter per multilinea)\nPuoi usare %m = nome del mob, %u = nome del pg", Qt::ToolTipRole);
        editor->setItemData(4, "Il mob reagira' dando XP al PG\nIn questo caso l'evento e' la quantita di XP", Qt::ToolTipRole);
        editor->setItemData(5, "Il mob reagira' dando sigilli Elementali al PG\nIn questo caso l'evento e' la quantita di sigilli Elementali", Qt::ToolTipRole);
        editor->setItemData(6, "Il mob reagira' dando sigilli Divini al PG\nIn questo caso l'evento e' la quantita di sigilli Divini", Qt::ToolTipRole);
        editor->setItemData(7, "Il mob reagira' loadando un oggetto random da un range di VNUM e dandolo al PG\nIn questo caso l'evento e': VNUM RANGE\nEs.:\n10200 5\nIl che vuol dire loada un oggetto random da 10200 a 10205.", Qt::ToolTipRole);
        editor->setItemData(8, "Il mob reagira' distruggendo oggetti che ha in inventario\nIn questo caso l'evento sono i VNUM degli oggetti separati da spazio (massimo 5)", Qt::ToolTipRole);
        editor->setItemData(9, "Il mob reagira' interrompendo il parsing di sucessivi Behavior\nche altrimenti potrebbero matchare piu avanti (sotto)", Qt::ToolTipRole);
        editor->setItemData(10, "Il mob reagira' dando un award al pg\nIl numero dell'award e' il valore", Qt::ToolTipRole);
        editor->setItemData(11, "Il mob reagira' togliendo un award al pg\nIl numero dell'award e' il valore", Qt::ToolTipRole);
        editor->setItemData(12, "Il mob reagira' dando soldi al pg\nIl numero dei soldi e' il valore", Qt::ToolTipRole);
        editor->setItemData(13, "Il mob reagira' prendendo soldi al pg\nIl numero dei soldi e' il valore", Qt::ToolTipRole);
        editor->setItemData(14, "Il mob reagira' ricordando nel campo 1 il valore\nPuo' essere usato per poi usare condizioni su altre reazioni.", Qt::ToolTipRole);
        editor->setItemData(15, "Il mob reagira' ricordando nel campo 2 il valore\nPuo' essere usato per poi usare condizioni su altre reazioni.", Qt::ToolTipRole);
    }
    return editor;
}

void BehaviorComboBoxDelegate::setEditorData(QWidget* editor,
    const QModelIndex& index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    if (index.column() == 0 || index.column() == 2) {
        QComboBox* cBox = static_cast<QComboBox*>(editor);
        cBox->setCurrentIndex(cBox->findData(value));
    }
    else {
        QString className = editor->metaObject()->className();
        if (className == "QTextEdit") {
            QTextEdit* qL = static_cast<QTextEdit*>(editor);
            qL->setText(value);
        }
        else {
            QLineEdit* qL = static_cast<QLineEdit*>(editor);
            qL->setText(value);
        }
    }
}

void BehaviorComboBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
    const QModelIndex& index) const
{
    if (index.column() == 0 || index.column() == 2) {
        QComboBox* cBox = static_cast<QComboBox*>(editor);
        QVariant value = cBox->itemData(cBox->currentIndex());

        model->setData(index, value, Qt::EditRole);
    }
    else {
        QString className = editor->metaObject()->className();
        QString text;
        if (className == "QTextEdit") {
            QTextEdit* qL = static_cast<QTextEdit*>(editor);
            text = qL->toPlainText();
        }
        else {
            QLineEdit* qL = static_cast<QLineEdit*>(editor);
            text = qL->text();
        }
        model->setData(index, text, Qt::EditRole);
    }
}

void BehaviorComboBoxDelegate::updateEditorGeometry(QWidget* editor,
    const QStyleOptionViewItem& option, const QModelIndex&/* index */) const
{
    editor->setGeometry(option.rect);
}

void WndMob::addBehavior() {
    QList<QStandardItem*> rowData;
    rowData << (new QStandardItem(QString("")));
    rowData << new QStandardItem(QString(""));
    rowData << new QStandardItem(QString(""));
    rowData << new QStandardItem(QString(""));
    m_BehaviorModel->appendRow(rowData);
    int rowIndex = m_BehaviorModel->rowCount() - 1;
    mp_Behaviors->openPersistentEditor(m_BehaviorModel->index(rowIndex, 0));
    //mp_Behaviors->openPersistentEditor(m_BehaviorModel->index(rowIndex, 1));
    mp_Behaviors->openPersistentEditor(m_BehaviorModel->index(rowIndex, 2));
    //mp_Behaviors->openPersistentEditor(m_BehaviorModel->index(rowIndex, 3));
    mp_Behaviors->selectRow(rowIndex);
    std::vector<std::vector<QString>> stv;
    behaviorConditionMap[rowIndex] = stv;
    somethingChanged();
}
void WndMob::removeBehavior() {
    int rowIndex = -1;
    if (mp_Behaviors->selectionModel()->currentIndex().isValid()) {
        rowIndex = mp_Behaviors->selectionModel()->currentIndex().row();
    }
    if (rowIndex != -1) {
        behaviorConditionMap.erase(currentBehaviorRow);
        m_BehaviorModel->removeRow(rowIndex);
        somethingChanged();
    }
}
void WndMob::moveUpBehavior() {
    int rowIndex = -1;
    if (mp_Behaviors->selectionModel()->currentIndex().isValid()) {
        rowIndex = mp_Behaviors->selectionModel()->currentIndex().row();
    }
    if (rowIndex > 0) {
        auto tmp = behaviorConditionMap[rowIndex-1];
        behaviorConditionMap[rowIndex - 1] = behaviorConditionMap[rowIndex];
        behaviorConditionMap[rowIndex] = tmp;
        auto rw = m_BehaviorModel->takeRow(rowIndex);
        m_BehaviorModel->insertRow(rowIndex - 1, rw);
        mp_Behaviors->openPersistentEditor(m_BehaviorModel->index(rowIndex - 1, 0));
        mp_Behaviors->openPersistentEditor(m_BehaviorModel->index(rowIndex - 1, 2));
        mp_Behaviors->selectRow(rowIndex - 1);
        somethingChanged();
    }
}
void WndMob::moveDownBehavior() {
    int rowIndex = -1;
    if (mp_Behaviors->selectionModel()->currentIndex().isValid()) {
        rowIndex = mp_Behaviors->selectionModel()->currentIndex().row();
    }
    if (rowIndex > -1 && rowIndex < m_BehaviorModel->rowCount() - 1) {
        auto tmp = behaviorConditionMap[rowIndex + 1];
        behaviorConditionMap[rowIndex + 1] = behaviorConditionMap[rowIndex];
        behaviorConditionMap[rowIndex] = tmp;
        auto rw = m_BehaviorModel->takeRow(rowIndex);
        m_BehaviorModel->insertRow(rowIndex + 1, rw);
        mp_Behaviors->openPersistentEditor(m_BehaviorModel->index(rowIndex + 1, 0));
        mp_Behaviors->openPersistentEditor(m_BehaviorModel->index(rowIndex + 1, 2));
        mp_Behaviors->selectRow(rowIndex + 1);
        somethingChanged();
    }
}
void WndMob::addCondition() {
    if (currentBehaviorRow == -1) {
        QMessageBox::critical(this, "Errore", "Serve selezionare prima un behavior.");
        return;
    }
    QList<QStandardItem*> rowData;
    rowData << (new QStandardItem(QString("")));
    rowData << new QStandardItem(QString(""));
    m_ConditionModel->appendRow(rowData);
    int rowIndex = m_ConditionModel->rowCount() - 1;
    mp_Conditions->openPersistentEditor(m_ConditionModel->index(rowIndex, 0));
    //mp_Conditions->openPersistentEditor(m_ConditionModel->index(rowIndex, 1));
    mp_Conditions->selectRow(rowIndex);
    std::vector<QString> qsl = { "","" };
    behaviorConditionMap[currentBehaviorRow].push_back(qsl);
    somethingChanged();
}
void WndMob::removeCondition() {
    if (currentBehaviorRow == -1) {
        QMessageBox::critical(this, "Errore", "Serve selezionare prima un behavior.");
        return;
    }
    int rowIndex = -1;
    if (mp_Conditions->selectionModel()->currentIndex().isValid()) {
        rowIndex = mp_Conditions->selectionModel()->currentIndex().row();
    }
    if (rowIndex == -1) {
        QMessageBox::critical(this, "Errore", "Serve selezionare prima una condizione.");
        return;
    }
    if (rowIndex != -1) {
        m_ConditionModel->takeRow(rowIndex);
        auto &vec = behaviorConditionMap[currentBehaviorRow];
        vec.erase(vec.begin() + rowIndex);
        somethingChanged();
    }
}

void WndMob::moveUpCondition() {
    if (currentBehaviorRow == -1) {
        QMessageBox::critical(this, "Errore", "Serve selezionare prima un behavior.");
        return;
    }
    int rowIndex = -1;
    if (mp_Conditions->selectionModel()->currentIndex().isValid()) {
        rowIndex = mp_Conditions->selectionModel()->currentIndex().row();
    }
    if (rowIndex == -1) {
        QMessageBox::critical(this, "Errore", "Serve selezionare prima una condizione.");
        return;
    }
    if (rowIndex > 0) {
        auto tmp = behaviorConditionMap[currentBehaviorRow][rowIndex-1];
        behaviorConditionMap[currentBehaviorRow][rowIndex - 1] = behaviorConditionMap[currentBehaviorRow][rowIndex];
        behaviorConditionMap[currentBehaviorRow][rowIndex] = tmp;

        auto rw = m_ConditionModel->takeRow(rowIndex);
        m_ConditionModel->insertRow(rowIndex-1, rw);
        mp_Conditions->openPersistentEditor(m_ConditionModel->index(rowIndex - 1, 0));
        mp_Conditions->selectRow(rowIndex - 1);
    }
}
void WndMob::moveDownCondition() {
    if (currentBehaviorRow == -1) {
        QMessageBox::critical(this, "Errore", "Serve selezionare prima un behavior.");
        return;
    }
    int rowIndex = -1;
    if (mp_Conditions->selectionModel()->currentIndex().isValid()) {
        rowIndex = mp_Conditions->selectionModel()->currentIndex().row();
    }
    if (rowIndex == -1) {
        QMessageBox::critical(this, "Errore", "Serve selezionare prima una condizione.");
        return;
    }
    if (rowIndex > -1 && rowIndex < m_ConditionModel->rowCount()-1) {
        auto tmp = behaviorConditionMap[currentBehaviorRow][rowIndex + 1];
        behaviorConditionMap[currentBehaviorRow][rowIndex + 1] = behaviorConditionMap[currentBehaviorRow][rowIndex];
        behaviorConditionMap[currentBehaviorRow][rowIndex] = tmp;

        auto rw = m_ConditionModel->takeRow(rowIndex);
        m_ConditionModel->insertRow(rowIndex+1, rw);
        mp_Conditions->openPersistentEditor(m_ConditionModel->index(rowIndex+1, 0));
        mp_Conditions->selectRow(rowIndex + 1);
    }
}

void WndMob::mp_Behaviors_currentRowChanged(const QModelIndex& current, const QModelIndex& previous) {
    if (current.isValid()) {
        currentBehaviorRow = current.row();
        showConditionsForBehavior(currentBehaviorRow);
    }
    else {
        currentBehaviorRow = -1;
        m_ConditionModel->clear();
    }
}

void WndMob::mp_Conditions_currentRowChanged(const QModelIndex& current, const QModelIndex& previous) {
    if (current.isValid()) {
        currentConditionRow = current.row();
    }
    else {
        currentConditionRow = -1;
    }
}

void WndMob::showBehaviors() {
    behaviorConditionMap.clear();
    int counter = 0;
    for each (auto &b in m_mob.getBehaviors())
    {
        QString evType;
        QString ev;
        QString reactType;
        QString react;
        evType = QString::number(b.mb_Event);
        reactType = QString::number(b.mb_Reaction);
        switch (b.mb_Event)
        {
        case me_Talk:
            ev = QString(b.e_mb_String);
            break;
        case me_Give:
            ev = QString::number(b.e_mb_Long);
            break;
        default:
            break;
        }

        switch (b.mb_Reaction)
        {
        case mr_Talk:
        case mr_Emote:
            react = QString(b.r_mb_String);
            break;
        case mr_Give:
        case mr_Xp:
        case mr_Elementi:
        case ts::mr_GiveAward:
        case ts::mr_TakeAward:
        case ts::mr_GiveGold:
        case ts::mr_TakeGold:
        case ts::mr_Ricorda1:
        case ts::mr_Ricorda2:
            react = QString::number(b.r_mb_Long[0]);
            break;
        case mr_DestroyObject:
            for (size_t i = 0; i < MAX_BEHAVIOUR_CONDITIONS; i++)
            {
                if (b.r_mb_Long[i]>0) react += QString::number(b.r_mb_Long[i]) + " ";
            }
            react = react.trimmed();
            break;
        case mr_RangeGive:
            react = QString::number(b.r_mb_Long[0]) + " " + QString::number(b.r_mb_Range);
            break;
        case mr_Break:
            react = "";
            break;
        default:
            break;
        }
        behaviorConditionMap[counter] = std::vector<std::vector<QString>>();
        for each (auto &cond in b.conditions)
        {
            if (cond.active) {
                QString condType = QString::number(cond.conditionType);
                QString condVal = QString::number(cond.condition);
                behaviorConditionMap[counter].push_back(std::vector<QString>{
                    condType,
                    condVal
                });
            }
        }
        QList<QStandardItem*> rowData;
        rowData << (new QStandardItem(evType));
        rowData << new QStandardItem(ev);
        rowData << new QStandardItem(reactType);
        rowData << new QStandardItem(react);
        m_BehaviorModel->appendRow(rowData);
        int rowIndex = m_BehaviorModel->rowCount() - 1;
        mp_Behaviors->openPersistentEditor(m_BehaviorModel->index(rowIndex, 0));
        //mp_Behaviors->openPersistentEditor(m_BehaviorModel->index(rowIndex, 1));
        mp_Behaviors->openPersistentEditor(m_BehaviorModel->index(rowIndex, 2));
        //mp_Behaviors->openPersistentEditor(m_BehaviorModel->index(rowIndex, 3));
        counter++;
    }
    //mp_Behaviors->resizeRowsToContents();
}
void WndMob::showConditionsForBehavior(int behIndex) {
    //m_ConditionModel->clear();
    while (m_ConditionModel->rowCount())
    {
        m_ConditionModel->takeRow(0);
    }
    /*mp_Conditions->horizontalHeader()->setStretchLastSection(true);
    m_ConditionModel->setHeaderData(0, Qt::Orientation::Horizontal, "Condizione");
    m_ConditionModel->setHeaderData(1, Qt::Orientation::Horizontal, "Valore");
    mp_Conditions->setColumnWidth(0, 270);*/
    if (behaviorConditionMap.find(behIndex) != behaviorConditionMap.end()) {
        for each (auto &cond in behaviorConditionMap[behIndex])
        {
            QList<QStandardItem*> rowData;
            rowData << (new QStandardItem(cond[0]));
            rowData << new QStandardItem(cond[1]);
            m_ConditionModel->appendRow(rowData);
            int rowIndex = m_ConditionModel->rowCount() - 1;
            mp_Conditions->openPersistentEditor(m_ConditionModel->index(rowIndex, 0));
            //mp_Conditions->openPersistentEditor(m_ConditionModel->index(rowIndex, 1));
        }
    }
}

QString err(int behIndex, QString err, int condIndex = -1) {
    if (condIndex == -1)
        return QString("Behavior %0: %1").arg(behIndex).arg(err);
    else 
        return QString("Behavior %0: Condizione %1, Errore: %2").arg(behIndex).arg(condIndex).arg(err);
}

QString WndMob::validateBehaviors() {
    for (size_t i = 0; i < m_BehaviorModel->rowCount(); i++)
    {
        QString v1 = m_BehaviorModel->item(i, 0)->data(Qt::EditRole).toString();
        QString v2 = m_BehaviorModel->item(i, 1)->data(Qt::EditRole).toString();
        QString v3 = m_BehaviorModel->item(i, 2)->data(Qt::EditRole).toString();
        QString v4 = m_BehaviorModel->item(i, 3)->data(Qt::EditRole).toString();
        if (v1 != "" && (v3 == "")) return err(i, "Campi mancanti");
        if ((v3 != "" && v3 != "8") && v4 == "") return err(i, "Campi mancanti");

        MobEvents ev = (MobEvents)v1.toInt();
        MobReactions re = (MobReactions)v3.toInt();
        if (ev >= ts::me_SIZE) return err(i, "Evento sconosciuto");
        if (re >= ts::mr_SIZE) return err(i, "Reazione sconosciuta");;
        bool ok;
        switch (ev)
        {
        case ts::me_Talk:
            if (v2 != "" && v2.trimmed()=="") return err(i, "Mancano parole chiavi per l'evento. Se vuoi qualsiasi parola deve essere completamente vuoto.");
            break;
        case ts::me_Give:
            v2.toInt(&ok);
            if (!ok) return err(i, "Evento richiede un numero");
            break;
        }
        switch (re)
        {
        case ts::mr_Talk:
            if (v4 == "") return err(i, "Manca testo da parlare per la reazione talk");
            break;
        case ts::mr_Give:
            v4.toInt(&ok);
            if (!ok) return err(i, "Reazione richiede un numero");
            break;
        case ts::mr_Emote:
            if (v4 == "") return err(i, "Manca testo per la reazione emote");
            break;
        case ts::mr_Xp:
        case ts::mr_Elementi:
        case ts::mr_Divini:
        case ts::mr_GiveAward:
        case ts::mr_TakeAward:
        case ts::mr_GiveGold:
        case ts::mr_TakeGold:
        case ts::mr_Ricorda1:
        case ts::mr_Ricorda2:
            v4.toInt(&ok);
            if (!ok) return err(i, "Reazione richiede un numero");
            break;
        case ts::mr_RangeGive:
        {
            auto sp = v4.split(" ", QString::SkipEmptyParts);
            if (sp.size() != 2) return  err(i, "Reazione richiede due numeri");
            sp[0].toInt(&ok);
            if (!ok) return  err(i, "Prima parola reazione non e' un numero");
            sp[1].toInt(&ok);
            if (!ok) return err(i, "Seconda parola reazione non e' un numero");
        }
            break;
        case ts::mr_DestroyObject:
        {
            auto sp = v4.split(" ", QString::SkipEmptyParts);
            if (sp.length() > 10) return err(i, "Reazione ha massimo 10 numeri");
            for (size_t x = 0; x < sp.length(); x++)
            {
                sp[x].toInt(&ok);
                if (!ok) return err(i, "Reazione: una parola non e' un numero");
            }
        }
            break;
        case ts::mr_Break:
            if (v4 != "") return err(i, "Reazione: deve essere vuota");
            break;
        }
        if (behaviorConditionMap.find(i) == behaviorConditionMap.end()) return false;
        for (size_t x = 0; x < behaviorConditionMap[i].size(); x++)
        {
            if (behaviorConditionMap[i][x][0] != "" &&
                behaviorConditionMap[i][x][1] == "") {
                return err(i, "Mancano valori", x);
            }
            auto zz = (MobBehaviourCondition)behaviorConditionMap[i][x][0].toInt(&ok);
            if (!ok) return err(i, "Deve essere un numero", x);
            if (zz < 0 || zz >= ts::MobBehaviourCondition::mc_SIZE) return err(i, "Condizione sconosciuta", x);
            behaviorConditionMap[i][x][1].toInt(&ok);
            if (!ok) return err(i, "Deve essere un numero", x);
        }
    }
    return "";
}
void WndMob::applyBehaviors() {
    auto& beh = m_mob.getBehaviors();
    beh.clear();
    for (size_t i = 0; i < m_BehaviorModel->rowCount(); i++)
    {
        MobBehaviour b;
        memset(&b, 0, sizeof(MobBehaviour));
        QString v1 = m_BehaviorModel->item(i, 0)->data(Qt::EditRole).toString();
        QString v2 = m_BehaviorModel->item(i, 1)->data(Qt::EditRole).toString();
        QString v3 = m_BehaviorModel->item(i, 2)->data(Qt::EditRole).toString();
        QString v4 = m_BehaviorModel->item(i, 3)->data(Qt::EditRole).toString();

        MobEvents ev = (MobEvents)v1.toInt();
        b.mb_Event = ev;
        MobReactions re = (MobReactions)v3.toInt();
        b.mb_Reaction = re;
        switch (ev)
        {
        case ts::me_Talk:
            strcpy(b.e_mb_String, v2.toAscii());
            break;
        case ts::me_Give:
            b.e_mb_Long = v2.toInt();
            break;
        }
        switch (re)
        {
        case ts::mr_Talk:
            strcpy(b.r_mb_String, v4.toAscii());
            break;
        case ts::mr_Give:
            b.r_mb_Long[0] = v4.toInt();
            break;
        case ts::mr_Emote:
            strcpy(b.r_mb_String, v4.toAscii());
            break;
        case ts::mr_Xp:
        case ts::mr_Elementi:
        case ts::mr_Divini:
        case ts::mr_GiveAward:
        case ts::mr_TakeAward:
        case ts::mr_GiveGold:
        case ts::mr_TakeGold:
        case ts::mr_Ricorda1:
        case ts::mr_Ricorda2:
            b.r_mb_Long[0] = v4.toInt();
            break;
        case ts::mr_RangeGive:
        {
            auto sp = v4.split(" ", QString::SkipEmptyParts);
            
            b.r_mb_Long[0] = sp[0].toInt();
            b.r_mb_Range = sp[1].toInt();
            
        }
            break;
        case ts::mr_DestroyObject:
        {
            auto sp = v4.split(" ", QString::SkipEmptyParts);
            for (size_t i = 0; i < sp.length(); i++)
            {
                b.r_mb_Long[i] = sp[i].toInt();
            }
        }
            break;
        case ts::mr_Break:
            break;
        }
        int activeC = 0;
        if (behaviorConditionMap.find(i) == behaviorConditionMap.end()) continue;
        for (size_t x = 0; x < behaviorConditionMap[i].size(); x++)
        {
            if (behaviorConditionMap[i][x][0] != "" &&
                behaviorConditionMap[i][x][1] != "") {
                b.conditions[activeC].active = true;
                b.conditions[activeC].conditionType = (MobBehaviourCondition)behaviorConditionMap[i][x][0].toInt();
                b.conditions[activeC].condition = behaviorConditionMap[i][x][1].toLong();
                activeC++;
            }
        }
        beh.push_back(b);
    }
}