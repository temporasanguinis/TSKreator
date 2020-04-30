
#include "zoneinitlist.h"
#include "exception.h"

namespace ts
{
#define CMNT_ZONEBEGIN "Inizializzazione parametri zona."
#define CMNT_ZONEEND "Fine inizializzazione parametri zona."
#define CMNT_MOBBEGIN "Inizializzazione dei Mobs."
#define CMNT_MOBEND "Fine inizializzazione dei Mobs."
#define CMNT_ITEMBEGIN "Inizializzazione degli Oggetti."
#define CMNT_ITEMEND "Fine inizializzazione degli Oggetti."
#define CMNT_DOORSBEGIN "Inizializzazione delle Porte."
#define CMNT_DOORSEND  "Fine inizializzazione delle Porte."

  void ZoneInitList::reset()
  {
	if (!m_questorCommands.empty())
		m_questorCommands.clear();
    if( !m_mobs.empty() )
      m_mobs.clear();
    if( !m_items.empty() )
      m_items.clear();
    if( !m_doors.empty() )
      m_doors.clear();
    m_lastMobInit = ZoneCommandGroup();
    m_lastItemInit = ZoneCommandGroup();
  }

  ZoneInitList::ZoneInitList()
  {
    reset();
  }

  ZoneInitList::~ZoneInitList()
  {
  }

  ZoneInitList::ZoneInitList( const ZoneInitList& zil )
  {
	m_questorCommands = zil.m_questorCommands;
    m_mobs = zil.m_mobs;
    m_items = zil.m_items;
    m_doors = zil.m_doors;
    m_lastMobInit = zil.m_lastMobInit;
    m_lastItemInit = zil.m_lastItemInit;
  }

  ZoneInitList& ZoneInitList::operator=( const ZoneInitList& zil )
  {
    if( this != &zil )
    {
	  m_questorCommands = zil.m_questorCommands;
      m_mobs = zil.m_mobs;
      m_items = zil.m_items;
      m_doors = zil.m_doors;
      m_lastMobInit = zil.m_lastMobInit;
      m_lastItemInit = zil.m_lastItemInit;
    }
    return *this;
  }

  bool CommandGroupIDLessThan(const ts::ZoneCommandGroup& s1, const ts::ZoneCommandGroup& s2)
  {
      long loc1 = s1.parent().argument(ts::ZoneCommand::Argument::Argument3);
      long loc2 = s2.parent().argument(ts::ZoneCommand::Argument::Argument3);
      long vnum1 = s1.parent().argument(ts::ZoneCommand::Argument::Argument1);
      long vnum2 = s2.parent().argument(ts::ZoneCommand::Argument::Argument1);
      if (vnum1 < vnum2) {
          return true;
      }
      else if (vnum1 == vnum2) {
          return loc1 < loc2;
      }
      return false;
  }
  
  bool MobInitIDLessThan(const ts::MobInit& s1, const ts::MobInit& s2)
  {
      long loc1 = s1.master().parent().argument(ts::ZoneCommand::Argument::Argument3);
      long loc2 = s2.master().parent().argument(ts::ZoneCommand::Argument::Argument3);
      long vnum1 = s1.master().parent().argument(ts::ZoneCommand::Argument::Argument1);
      long vnum2 = s2.master().parent().argument(ts::ZoneCommand::Argument::Argument1);
      if (vnum1 < vnum2) {
          return true;
      }
      else if (vnum1 == vnum2) {
          if (loc1 < loc2) {
              return true;
          }
          else if (loc1 == loc2) {
              return s1.id() < s2.id();
          }
      }
      return false;
  }

  void ZoneInitList::setCommandList( const ZoneCommandList& zcl )
  {
    reset();

    ZoneCommandList::const_iterator it = zcl.begin();

    while( it != zcl.end() )
    {
      parseCommand( *it );
      ++it;
    }
    flushBuffer();
	qDebug("ZoneInitList: sorting Questor Inits...");
	qSort(m_questorCommands);
    qDebug( "ZoneInitList: sorting Mob Inits..." );
    qSort( m_mobs.begin(), m_mobs.end(), MobInitIDLessThan );
    qDebug( "ZoneInitList: sorting Item Inits..." );
    qSort( m_items.begin(), m_items.end(), CommandGroupIDLessThan);
    qDebug( "ZoneInitList: sorting Door Inits..." );
    qSort( m_doors );
    qDebug( "ZoneInitList: sorting completed." );
  }

  void ZoneInitList::flushBuffer()
  {
    if( !m_lastMobInit.isNull() )
    {
      bool append_mob_as_follower = false;
      if( m_lastMobInit.parent().isMobFollower() )
      {
        if( m_mobs.empty() )
        {
          qWarning( "Error in ZoneInitList: follower found without a master... fixed!" );
          ZoneCommand zc = m_lastMobInit.parent();
          zc.setType( QChar( 'M' ) );
          m_lastMobInit.setParent( zc );
        }
        else
          append_mob_as_follower = true;
      }

      if( !append_mob_as_follower )
      {
        MobInit mi;
        mi.setMaster( m_lastMobInit );
        m_mobs.append( mi );
      }
      else
        m_mobs.last().addFollower( m_lastMobInit );

      m_lastMobInit.init();
    }

    if( !m_lastItemInit.isNull() )
    {
      m_items.append( m_lastItemInit );
      m_lastItemInit.init();
    }
  }

  bool isInternalComment(QString& comment) {
      bool isInternal = false;
      isInternal |= comment == CMNT_ZONEBEGIN;
      isInternal |= comment == CMNT_ZONEEND;
      isInternal |= comment == CMNT_MOBBEGIN;
      isInternal |= comment == CMNT_MOBEND;
      isInternal |= comment == CMNT_ITEMBEGIN;
      isInternal |= comment == CMNT_ITEMEND;
      isInternal |= comment == CMNT_DOORSBEGIN;
      isInternal |= comment == CMNT_DOORSEND;
      return isInternal;
  }

  int lastAddType = -1;
  void ZoneInitList::parseCommand( const ZoneCommand& zc )
  {
	if (zc.isQuestorCommand()) 
	{
        lastAddType = 0;
		m_questorCommands.append(zc);
	}
    else if( zc.isMobLoad() || zc.isMobFollower() )
    {
        if (!zc.isMobFollower()) {
            lastAddType = 1;
            flushBuffer();
            m_lastMobInit.setParent(zc);
        }
        else {
            m_lastMobInit.addSon(zc);
        }
    }
    else if( zc.isMobFear() || zc.isMobHate() || zc.isMobEquip() || zc.isMobGive() )
    {
      m_lastMobInit.addSon( zc );
    }
    else if( zc.isItemPut() )
    {
        if (!m_lastItemInit.isNull()) {
            m_lastItemInit.addSon(zc);
        }
        else if (!m_lastMobInit.isNull()) {
            m_lastMobInit.addSon(zc);
        }
        else {
            throw Exception(ts::Exception::Runtime, "Item PUT senza MOB o senza ITEM LOAD");
        }
    }
    else if( zc.isItemLoad() )
    {
        lastAddType = 2;
      flushBuffer();
      m_lastItemInit.setParent( zc );
    }
    else if( zc.isDoorInit() )
    {
        lastAddType = 3;
      flushBuffer();
      m_doors.append( zc );
    }
    else if (zc.isOnlyComment() && lastAddType>-1 && zc.comment() != "" && !isInternalComment(zc.comment())) {
        ZoneCommand emptyLine;
        emptyLine.setType('*');
        emptyLine.setComment("");
        switch (lastAddType) {
        case 0:
            m_questorCommands.append(emptyLine);
            m_questorCommands.append(zc);
            break;
        case 1:
            m_lastMobInit.addSon(emptyLine);
            m_lastMobInit.addSon(zc);
            break;
        case 2:
            m_lastItemInit.addSon(emptyLine);
            m_lastItemInit.addSon(zc);
            break;
        case 3:
            m_doors.append(emptyLine);
            m_doors.append(zc);
            break;
        }
    }
  }

  ZoneCommandList ZoneInitList::commandList() const
  {
    qDebug( "ZoneInitList::commandList() : creating command list..." );
    ZoneCommandList zcl;
    ZoneCommand cmd_comment;

    zcl.addCommand( 1 );  // Init the list

	zcl.appendComment(CMNT_ZONEBEGIN);
	questor_const_iterator qitm = m_questorCommands.begin();
	while (qitm != m_questorCommands.end())
	{
		if ((*qitm).isQuestorCommand())
		{;
			zcl.appendCommand(*qitm);
		}	
		++qitm;
	}
	zcl.appendComment(CMNT_ZONEEND);
	zcl.appendComment("");

    zcl.appendComment( CMNT_MOBBEGIN );

    mobs_const_iterator itm = m_mobs.begin();
    while( itm != m_mobs.end() )
    {
      if( (*itm).isMobLoad() )
      {
        zcl.appendCommandGroup( (*itm).master() );
        if( (*itm).hasFollowers() )
        {
          MobInit::followers_const_iterator itf = (*itm).followersBegin();
          while( itf != (*itm).followersEnd() )
          {
            zcl.appendCommandGroup( *itf );
            ++itf;
          }
        }
      }
      ++itm;
    }

    zcl.appendComment( CMNT_MOBEND);
    zcl.appendComment( "" );

    zcl.appendComment( CMNT_ITEMBEGIN );

    items_const_iterator iti = m_items.begin();
    while( iti != m_items.end() )
    {
      if( (*iti).parent().isItemLoad() )
        zcl.appendCommandGroup( *iti );
      ++iti;
    }

    zcl.appendComment( CMNT_ITEMEND );
    zcl.appendComment( "" );
    zcl.appendComment( CMNT_DOORSBEGIN );

    doors_const_iterator itd = m_doors.begin();
    while( itd != m_doors.end() )
    {
      if( (*itd).isDoorInit() )
        zcl.appendCommand( *itd );
      ++itd;
    }

    zcl.appendComment( CMNT_DOORSEND );
    //zcl.appendComment( "" );

    zcl.resetCommandsId();
    qDebug( "ZoneInitList::commandList() : command list created." );
    return zcl;
  }

}  // namespace ts
