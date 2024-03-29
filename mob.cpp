#include "mob.h"
#include <QFile>
#include <QRegExp>
#include "constantname.h"
#include "random.h"
#include "dice.h"

#include <iostream>

using namespace std;

namespace ts
{

    void Mob::init()
    {
        m_name = defaultName();
        setShortDescription(defaultName());
        setLongDescription(defaultName() + QString(" is here."));
        m_roomSound = "";
        m_nearSound = "";
        m_acts = 0;
        m_mobType = MOB_TYPE_MULTI_ATTACKS;
        m_mobLevel = 1;
        m_hitPointsBonus = 0;
        m_xpBonus = 0;
        m_mobDefaultPosition = CHARACTER_POSITION_STANDING;
        m_mobBehaviours.clear();
    }


    Mob::Mob()
            : CharacterData(ObjectData::Object_Mob)
    {
        init();
    }

    Mob::Mob(VNumber vnum)
            : CharacterData(ObjectData::Object_Mob, vnum)
    {
        init();
    }

    Mob::Mob(const Mob& m)
            : CharacterData((CharacterData)m)
    {
        m_roomSound = m.m_roomSound;
        m_nearSound = m.m_nearSound;
        m_acts = m.m_acts;
        m_mobType = m.m_mobType;
        m_mobLevel = m.m_mobLevel;
        m_hitPointsBonus = m.m_hitPointsBonus;
        m_xpBonus = m.m_xpBonus;
        m_mobDefaultPosition = m.m_mobDefaultPosition;
        m_mobBehaviours = m.m_mobBehaviours;
    }

    Mob::~Mob()
    {}

    Mob& Mob::operator=(const Mob& m)
    {
        if (this != &m) {
            CharacterData::operator=((CharacterData)m);
            m_roomSound = m.m_roomSound;
            m_nearSound = m.m_nearSound;
            m_acts = m.m_acts;
            m_mobType = m.m_mobType;
            m_mobLevel = m.m_mobLevel;
            m_hitPointsBonus = m.m_hitPointsBonus;
            m_xpBonus = m.m_xpBonus;
            m_mobDefaultPosition = m.m_mobDefaultPosition;
            m_mobBehaviours = m.m_mobBehaviours;
        }
        return *this;
    }

    void Mob::validate()
    {
        if (!hasAct(MOB_ACT_ISNPC))
            setAct(MOB_ACT_ISNPC);
        /*
        if( !hasAct( MOB_ACT_POLYSELF ) )
        {
          qWarning( "%s has flag %s set, removed.", dumpObject().toUtf8().data(),
            ConstantName::mobAct( MOB_ACT_POLYSELF ).toUtf8().data() );
          removeAct( MOB_ACT_POLYSELF );
        }
        */
    }

    static QChar GetMobTypeToSave(int id)
    {
        switch (id) {
            case MOB_TYPE_NEW:
                return 'N';
            case MOB_TYPE_MULTI_ATTACKS:
                return 'A';
            case MOB_TYPE_UNBASHABLE:
                return 'B';
            case MOB_TYPE_SOUND:
                return 'L';
            case MOB_TYPE_SIMPLE:
                return 'S';
            case MOB_TYPE_DETAILED:
                return 'D';
            default:
                return 'A';
        }
    }

    static int GetMobTypeFromLoad(char cid)
    {
        switch (cid) {
            case 'N':
                return MOB_TYPE_NEW;
            case 'A':
                return MOB_TYPE_MULTI_ATTACKS;
            case 'B':
                return MOB_TYPE_UNBASHABLE;
            case 'L':
                return MOB_TYPE_SOUND;
            case 'S':
                return MOB_TYPE_SIMPLE;
            case 'D':
                return MOB_TYPE_DETAILED;
            default:
                return MOB_TYPE_MULTI_ATTACKS;
        }
    }

    void Mob::load(FILE* pFile)
    {
        long lTmp;

        setName(Utils::readString(pFile, defaultName()));
        setShortDescription(Utils::readString(pFile, defaultName()));
        setLongDescription(Utils::readString(pFile, defaultName() + QString(" is here.")));
        setDescription(Utils::readString(pFile));

        QString sErrorMessage = "";
        sErrorMessage = dumpObject();

        lTmp = Utils::readNewFlags(pFile, &m_extraFlags);
        m_acts = lTmp;

        setAffects(Utils::readNumber(pFile, sErrorMessage + QString(" (MobAffects)")));
        setAlignment(Utils::readNumber(pFile, sErrorMessage + QString(" (MobAlignment)")));

        char cMobType;
        fscanf(pFile, " %c ", &cMobType);

        m_mobType = GetMobTypeFromLoad(cMobType);


        if (m_mobType == MOB_TYPE_SIMPLE || m_mobType == MOB_TYPE_MULTI_ATTACKS ||
                m_mobType == MOB_TYPE_NEW || m_mobType == MOB_TYPE_UNBASHABLE || m_mobType == MOB_TYPE_SOUND) {

            fscanf(pFile, " \n");

            if (m_mobType == MOB_TYPE_MULTI_ATTACKS || m_mobType == MOB_TYPE_UNBASHABLE || m_mobType == MOB_TYPE_SOUND) {
                setAttacks((float)Utils::readNumber(pFile, sErrorMessage + QString(" (Number of Attacks)")));
                setAffects2(Utils::readNumberInLine(pFile, sErrorMessage + QString(" (Affects2) ")));

                 if ((signed)m_affects2 == -1L)
                     m_affects2 = 0;

                /* Cambio gli shield dagli acts agli affect */
                if (hasExtraFlags(6)) {
                    removeExtraFlags(6);
                    addAffect2(AFFECT2_FIRESHIELD);
                }

                if (hasExtraFlags(7)) {
                    removeExtraFlags(7);
                    addAffect2(AFFECT2_FROSTSHIELD);
                }

                if (hasExtraFlags(8)) {
                    removeExtraFlags(8);
                    addAffect2(AFFECT2_ELECTRICSHIELD);
                }

                if (hasExtraFlags(9)) {
                    removeExtraFlags(9);
                    addAffect2(AFFECT2_ENERGYSHIELD);
                }

                if (hasExtraFlags(10)) {
                    removeExtraFlags(10);
                    addAffect2(AFFECT2_ACIDSHIELD);
                }
            }

            lTmp = Utils::readNumber(pFile, sErrorMessage + QString(" (MobLevel)"), 1);

            if (lTmp == -1) {
                setMobLevel(Utils::readNumber(pFile, sErrorMessage + QString(" (MobLevel)"), 1));
                setSpellPower(Utils::readNumber(pFile, sErrorMessage + QString(" (SpellPower)")));
                setAbsorbDam(Utils::readNumber(pFile, sErrorMessage + QString(" (AbsorbDam)")));
            } else
                setMobLevel(lTmp);

            setStrength((int)Utils::Min(10 + Random::number(0, (int)Utils::Max(1, mobLevel() / 5)), 18));
            setDexterity((int)Utils::Min(10 + Random::number(0, (int)Utils::Max(1, mobLevel() / 5)), 18));
            setConstitution((int)Utils::Min(10 + Random::number(0, (int)Utils::Max(1, mobLevel() / 5)), 18));
            setIntelligence((int)Utils::Min(10 + Random::number(0, (int)Utils::Max(1, mobLevel() / 5)), 18));
            setWisdom((int)Utils::Min(10 + Random::number(0, (int)Utils::Max(1, mobLevel() / 5)), 18));
            setCharisma((int)Utils::Min(10 + Random::number(0, (int)Utils::Max(1, mobLevel() / 5)), 18));

            setHitRoll((20 - Utils::readNumber(pFile, sErrorMessage + QString(" (HitRoll)"))));

            lTmp = Utils::readNumber(pFile, sErrorMessage + QString(" (Armor)"), 10);
            setArmor((lTmp *(m_mobType == MOB_TYPE_SIMPLE ? 1 : 10)));

            if (m_mobType == MOB_TYPE_SIMPLE) {
                setMaxHitPoints(Utils::readDice(pFile).roll());
            } else {
                setHitPointsBonus(Utils::readNumber(pFile, sErrorMessage + QString(" (MobHitPointsBonus)")));
                //setMaxHitPoints( ( Random::d8( mobLevel() ) + hitPointsBonus() ) );
            }
            setHitPoints(maxHitPoints());

            setDamage(Utils::readDice(pFile));
            fscanf(pFile, " \n");

            lTmp = Utils::readNumber(pFile, sErrorMessage + QString(" (Gold1)"), 0);
            bool read_also_race = (lTmp == -1);

            if (lTmp == -1)
                setGold(Utils::readNumber(pFile, sErrorMessage + QString(" (Gold2)"), 0));
            else
                setGold(lTmp);

            lTmp = Utils::readNumber(pFile, sErrorMessage + QString(" (Experience)"), 0);
            setXpBonus(lTmp);

            if (read_also_race) {
                setRace(Utils::readNumber(pFile, sErrorMessage + QString(" (Race)"), 0));
                /*
                Race( RaceID( fread_number( pFile ) ) );
                if( IsGiant() )
                abilities.str += number( 1, 4 );
                if( IsSmall() )
                abilities.str -= 1;
                */
            }

            setPosition(Utils::readNumber(pFile, sErrorMessage + QString(" (Position)"), CHARACTER_POSITION_STANDING));
            m_mobDefaultPosition = Utils::readNumber(pFile, sErrorMessage + QString(" (Position)"), CHARACTER_POSITION_STANDING);

            lTmp = Utils::readNumber(pFile, sErrorMessage + QString(" (Gender)"), CHARACTER_GENDER_NEUTRAL);
            if (lTmp < 3) {
                setGender(lTmp);
                resetResistances();
                resetImmunities();
                resetSusceptibilities();
            } else if (lTmp < 6) {
                setGender((lTmp - 3));
                setResistances(Utils::readNumber(pFile, sErrorMessage + QString(" (Resistances)")));
                setImmunities(Utils::readNumber(pFile, sErrorMessage + QString(" (Immunities)")));
                setSusceptibilities(Utils::readNumber(pFile, sErrorMessage + QString(" (Susceptibilities)")));
            } else {
                setGender(CHARACTER_GENDER_NEUTRAL);
                resetResistances();
                resetImmunities();
                resetSusceptibilities();
            }

            if (m_mobType == MOB_TYPE_SOUND) {
                setRoomSound(Utils::readString(pFile));
                setNearSound(Utils::readString(pFile));
            } else {
                setRoomSound("");
                setNearSound("");
            }
            /*
                   if( m_mobType == MOB_TYPE_UNBASHABLE )
                     setAct( MOB_ACT_HUGE );

                player.iClass = 0;

                TimeBirth() = time( 0 );
                TimePlayed() = 0;
                LastLogoff() = time(0);
                Weight( 200 );
                Height( 198 );

                for( int i = 0; i < 3; i++ )
                  GET_COND( this, i ) = -1;
                    */

        } else {
            /* The old monsters are down below here */

            fscanf(pFile, "\n");

            setStrength((Utils::readNumber(pFile, sErrorMessage + QString(" (Strength)"), 10)));
            setIntelligence((Utils::readNumber(pFile, sErrorMessage + QString(" (Intelligence)"), 10)));
            setWisdom((Utils::readNumber(pFile, sErrorMessage + QString(" (Wisdom)"), 10)));
            setDexterity((Utils::readNumber(pFile, sErrorMessage + QString(" (Dexterity)"), 10)));
            setConstitution((Utils::readNumber(pFile, sErrorMessage + QString(" (Constitution)"), 10)));
            setCharisma((int)Utils::Min(10 + Random::number(0, (int)Utils::Max(1, mobLevel() / 5)), 18));

            lTmp = Utils::readNumber(pFile, sErrorMessage + QString(" (MobHitPointsMin)"));
            setMaxHitPoints(Random::number(lTmp, Utils::readNumber(pFile, sErrorMessage + QString(" (MobHitPointsMax)"))));
            setHitPoints(maxHitPoints());

            lTmp = Utils::readNumber(pFile, sErrorMessage + QString(" (Armor)"), 10);
            setArmor((lTmp * 10));

            lTmp = Utils::readNumber(pFile, sErrorMessage + QString(" (MobManaPointsMin)"));
            setMaxManaPoints(Random::number(lTmp, Utils::readNumber(pFile, sErrorMessage + QString(" (MobManaPointsMax)"))));
            setManaPoints(maxManaPoints());

            lTmp = Utils::readNumber(pFile, sErrorMessage + QString(" (MobMovePointsMin)"));
            setMaxMovePoints(Random::number(lTmp, Utils::readNumber(pFile, sErrorMessage + QString(" (MobMovePointsMax)"))));
            setMovePoints(maxMovePoints());

            setGold(Utils::readNumber(pFile, sErrorMessage + QString(" (Gold)"), 0));
            setExperience(Utils::readNumber(pFile, sErrorMessage + QString(" (Experience)"), 1));
            setPosition(Utils::readNumber(pFile, sErrorMessage + QString(" (Position)"), CHARACTER_POSITION_STANDING));
            m_mobDefaultPosition = Utils::readNumber(pFile, sErrorMessage + QString(" (Position)"), CHARACTER_POSITION_STANDING);

            setGender(Utils::readNumber(pFile, sErrorMessage + QString(" (Gender)"), CHARACTER_GENDER_NEUTRAL));

            /*
            player.iClass = fread_number( pFile );

            fscanf(pFile, " %ld ", &lTmp);
            Level( WARRIOR_LEVEL_IND, lTmp );


            fscanf(pFile, " %ld ", &lTmp);
            TimeBirth() = time(0);
            TimePlayed() = 0;
            LastLogoff() = time(0);

            fscanf(pFile, " %ld ", &lTmp);
            Weight( lTmp );

            fscanf(pFile, " %ld \n", &lTmp);
            Height( lTmp );

            for( int i = 0; i < 3; i++ )
            {
            fscanf(pFile, " %ld ", &lTmp);
            GET_COND( this, i ) = lTmp;
            }
            fscanf(pFile, " \n ");

            for( int j = 0; j < 5; j++ )
            {
            fscanf(pFile, " %ld ", &lTmp);
            specials.apply_saving_throw[j] = lTmp;
            }

            fscanf(pFile, " \n ");
            */
            /* Set the damage as some standard 1d4 */
            /*
            points.damroll = 0;
            specials.damnodice = 1;
            specials.damsizedice = 6;
            */
            /* Calculate THAC0 as a formular of Level */

            //  points.hitroll = MAX( 1, Level( WARRIOR_LEVEL_IND ) - 3 );
        }

        if (readMobBehaviours(pFile)) {
            qDebug("Mob %d ha behavior", vnumber());
        }

        qDebug("%s loading completed.", dumpObject().toUtf8().data());
        /*
          tmpabilities = abilities;

          for( int i = 0; i < MAX_WEAR; i++ )
            equipment[ i ] = NULL;

          desc = NULL;

          SET_BIT( specials.act, ACT_ISNPC );

          generic = 0;
          commandp = 0;
          commandp2 = 0;
          waitp = 0;

          RemoveAct( ACT_SCRIPT );

          for( unsigned j = 0; j < scripts.size(); j++ )
          {
            if( scripts[ j ].iVNum == VNum () )
            {
              AddAct( ACT_SCRIPT );
              script = j;
              break;
            }
          }



          if( points.gold > Level( WARRIOR_LEVEL_IND ) * 1500 )
            mudlog( LOG_MOBILES, "%s has gold > level * 1500 (%d)", Description().c_str(), points.gold );

          SetRacialStuff();




          points.mana = getMaxManaPoints();
          points.move = getMaxMovePoints();

          return bc;
            */
    }

    QString BehaviorToString(MobBehaviour &beh) {
        QString ret = "E" + QString::number((int)beh.mb_Event) + "\n";
        switch (beh.mb_Event) {

        case me_Talk:
        case me_PgOrder:
        {
            auto str = QString(beh.e_mb_String).replace("\r\n", "").replace("\r", "").replace("\n", "").trimmed();
            if (str == "") {
                ret += str + "~\n";
            }
            else {
                ret += str + " ~\n";
            }
        }
            break;

        case me_Give:
        case me_Death:
        case me_Time:
        case me_Command:
        case me_PlayerEnterLeave:
        case me_Follow:
        case me_Arrives:
        case me_Ricordo1Modificato:
        case me_Ricordo2Modificato:

            ret += QString::number(beh.e_mb_Long) + "\n";

            break;

        default:
            qWarning("Tipo evento per behavior sconosciuto");
            throw std::exception("Tipo evento non riconosciuto");
            return "";
        }

        for (size_t i = 0; i < MAX_BEHAVIOUR_CONDITIONS; i++)
        {
            if (beh.conditions[i].active) {
                ret += "C" + QString::number((int)beh.conditions[i].conditionType) + "\n";
                ret += QString(beh.conditions[i].s_condition) + "\n";
            }
        }

        ret += "R" + QString::number((int)beh.mb_Reaction);
        if (beh.r_mbLag) {
            ret += "L" + QString::number((int)beh.r_mbLag);
        }
        ret +="\n";

        switch (beh.mb_Reaction) {

        case mr_Talk:
        case mr_Disappear:
        case mr_Die:
        case mr_ExecCommand:
        case mr_ChangeLong:
        case mr_ChangeSound:
        case mr_ChangeNearSound:
        case mr_SetGlobalTRUE:
        case mr_SetGlobalFALSE:
        case mr_Aggro:
        case mr_Emote:
            ret += QString(beh.r_mb_String) + "\n~\n";
            break;

        case mr_RangeGive:

            ret += QString::number(beh.r_mb_Long[0]) + " " + QString::number(beh.r_mb_Range) + "\n";
            break;

        case mr_Give:
        case mr_Elementi:
        case mr_Divini:
        case mr_GiveAward:
        case mr_TakeAward:
        case mr_GiveGold:
        case mr_TakeGold:
        case mr_Ricorda1:
        case mr_Ricorda2:
        case mr_Xp:
        case mr_TrackVnum: /* mob inizia a camminare verso VNUM   */
        case mr_TransferMob: /* sposta mob in VNUM   */
        case mr_GiveMedal: /* Da numero X medaglei al pg (anche negativo) */
        case mr_UnLoadMobVnum: /* togli mob VNUM dal mud  */
        case mr_Follow: /* segui (1) o smetti di seguire (0)*/
        case mr_IncrementaRicordo1:
        case mr_IncrementaRicordo2:
            ret += QString::number(beh.r_mb_Long[0]) + "\n";
            break;
        case mr_DestroyObject:
        case mr_LoadMobVnum:
        case mr_LoadObjectInVnum:
        case mr_TransferRoom:
        {
            QString buffer = "";
            for (size_t i = 0; i < MAX_BEHAVIOUR_CONDITIONS; i++)
            {
                if (beh.r_mb_Long[i])
                buffer += QString::number(beh.r_mb_Long[i]) + " ";
            }
            buffer = buffer.trimmed();
            ret += buffer + "\n";
        }
            break;
        case mr_Break:
            break;
        default:
            qWarning("Mob condition sconosciuto");
            throw std::exception("MOb reaction sconosciuto");
            return "";
        }

        return ret;
    }

    QString BehaviorsToString(vector<MobBehaviour> &beh) {
        QString ret;
        for (size_t i = 0; i < beh.size(); i++)
        {
            ret += BehaviorToString(beh[i]);
        }
        return ret;
    }

    void Mob::save(QTextStream& stream)
    {
        stream << "#" << m_vnumber << endl << flush;
        stream << m_name << "~" << endl << flush;
        stream << m_shortDescription << "~" << endl << flush;
        stream << m_longDescription << endl << "~" << endl << flush;
        stream << m_description << endl << "~" << endl << flush;

        Utils::writeNewFlags(stream, acts(), extraFlags());
        /*
        BitVector btTmp = acts();
        if( m_mobType == MOB_TYPE_UNBASHABLE )
          Bit::remove( &btTmp, MOB_ACT_HUGE );

        Utils::saveBitVector( stream, btTmp );
        stream << " " << flush;
        */
        Utils::saveBitVector(stream, m_affects);
        stream << " " << m_alignment << " " << flush;
        stream << GetMobTypeToSave(m_mobType) << flush;

        if (m_mobType == MOB_TYPE_MULTI_ATTACKS || m_mobType == MOB_TYPE_UNBASHABLE
                || m_mobType == MOB_TYPE_SOUND) {
            stream << " " << m_attacks << flush;
            if (affects2())
                stream << " " << affects2() << flush;
        }

        stream << endl << flush;
        stream << "-1 " << m_mobLevel << " " << m_spellPower << " " << m_absorbDam << " " << (20 - m_hitRoll) << " " << flush;
        stream << (m_armor / 10) << " " << m_hitPointsBonus << " " << m_damage.toString() << endl << flush;
        stream << "-1 " << flush;
        stream << m_gold << " " << m_xpBonus << " " << m_race << endl << flush;
        stream << m_position << " " << m_mobDefaultPosition << " " << flush;
        if (m_resistances > 0 || m_immunities > 0 || m_susceptibilities > 0) {
            stream << (m_gender + 3) << " " << flush;
            Utils::saveBitVector(stream, m_resistances);
            stream << " " << flush;
            Utils::saveBitVector(stream, m_immunities);
            stream << " " << flush;
            Utils::saveBitVector(stream, m_susceptibilities);
            stream << " " << endl << flush;
        } else {
            stream << m_gender << endl << flush;
        }

        if (m_mobType == MOB_TYPE_SOUND) {
            stream << m_roomSound << endl << "~" << endl << flush;
            stream << m_nearSound << endl << "~" << endl << flush;
        }

        QString behaviorsString = BehaviorsToString(m_mobBehaviours);
        stream << behaviorsString;

        stream << flush;

        qDebug("%s saving completed.", dumpObject().toUtf8().data());
    }

    void Mob::generateExperience()
    {
        int aiBase[ 52 ] =
            {
                5,     10,    20,    35,   60,   90,   150,   225,    600,    900,
                1100,   1300,  1550,  1800,  2100,  2400,  2700,  3000,   3500,   4000,
                4500,   5000,  6000,  7000,  8000,  9000, 10000, 12000,  14000,  16000,
                20000,  22000, 22000, 22000, 22000, 32000, 32000, 32000,  32000,  32000,
                42000,  42000, 52000, 52000, 72000, 72000, 92000, 92000, 122000, 122000,
                150000, 200000
            };

        int aiHit[ 52 ] =
            {
                1,   1,   2,   3,   4,   5,   6,   8,  12,  14,
                15,  16,  17,  18,  19,  20,  23,  25,  28,  30,
                33,  35,  40,  45,  50,  55,  60,  70,  80,  90,
                100, 120, 120, 120, 120, 140, 140, 140, 140, 140,
                160, 160, 180, 180, 200, 200, 225, 225, 250, 250,
                275, 300
            };

        int aiFlags[ 52 ] =
            {
                10,     15,    20,    25,    30,    40,    75,   125,   175,   300,
                450,   700,   700,   950,  950,  1250,  1250,  1550,  1550,  2100,
                2100,  2600,  3000,  3500,  4000,  4500,  5000,  6000,  7000,  8000,
                10000, 12000, 12000, 12000, 12000, 14000, 14000, 14000, 14000, 14000,
                16000, 16000, 20000, 20000, 24000, 24000, 28000, 28000, 32000, 32000,
                36000, 40000
            };

        if (m_xpBonus > 100)
            qWarning("%s has an Experience Bonus > 100 (%ld).", dumpObject().toUtf8().data(), m_xpBonus);

        int iLevel = mobLevel();

        if (iLevel < 0) {
            setExperience(1);
            return;
        }

        if (iLevel > 51)
            iLevel = 51;

        int iExp = aiBase[ iLevel ] + ((aiHit[ iLevel ] / 2) * this->maxHitPoints()) + (aiFlags[ iLevel ] * m_xpBonus);

        setExperience(iExp);
    }

    void Mob::setXpBonus(long new_value)
    {
        m_xpBonus = new_value;
        if (m_mobType == MOB_TYPE_SIMPLE)
            setExperience(new_value);
        else if (new_value < 0)
            setExperience(-new_value);
        else
            generateExperience();

        if (hasAct(MOB_ACT_WIMPY))
            setExperience((m_experience - (m_experience / 10)));

        if (hasAct(MOB_ACT_AGGRESSIVE))
            setExperience((m_experience + (m_experience / 10)));

        if (hasAct(MOB_ACT_META_AGGRESSIVE) || !hasAct(MOB_ACT_WIMPY))
            setExperience((m_experience + (m_experience / 2)));

    }

    void Mob::generateAverageMaxHitPoints()
    {
        int iHp = static_cast<int>((m_mobLevel * 4.5) + m_hitPointsBonus);
        setMaxHitPoints(iHp);
    }

    static void FreeMobBehaviour(MobBehaviour& mb)
    {
        /*if (mb.mb_Event == me_Talk && mb.e_mb_String)
            free(mb.e_mb_String);

        if ((mb.mb_Reaction == mr_Talk || mb.mb_Reaction ==
            mr_Emote) && mb.r_mb_String)
            free(mb.r_mb_String);*/
    }

    bool Mob::readMobBehaviours(FILE *fp)
    {
        long cur = ftell(fp);
        char TmpBuf[4096];
        bool any = false;
        while (fscanf(fp, "%s", TmpBuf) && *TmpBuf == 'E') {
            MobBehaviour mb;
            mb.r_mbLag = 0;
            mb.mb_Event = MobEvents::me_SIZE;
            mb.e_mb_Long = 0;
            strcpy(mb.e_mb_String, "");
            for (size_t i = 0; i < MAX_BEHAVIOUR_CONDITIONS; i++)
            {
                mb.conditions[i].active = false;
                mb.r_mb_Long[i] = 0;
            }
            mb.r_mb_Range = 0;
            strcpy(mb.r_mb_String, "");

            switch (mb.mb_Event = (MobEvents)atoi(TmpBuf + 1)) {

            case me_Talk:
            case me_PgOrder:
            {
                auto val = (Utils::readString(fp).toLower().trimmed());
                strcpy(mb.e_mb_String, val.toLatin1());
            }
                break;

            case me_Time:
                if (!fscanf(fp, "%s", TmpBuf)) {
                    qCritical("ReadMobBehaviours(%s): syntax error", m_name.toUtf8().data());
                    FreeMobBehaviour(mb);
                    return any;
                }

                mb.e_mb_Long = !*TmpBuf ? -1 : atol(TmpBuf);
                break;
            case me_Death:
            case me_Command:
            case me_PlayerEnterLeave:
            case me_Follow:
            case me_Arrives:
            case me_Ricordo1Modificato:
            case me_Ricordo2Modificato:
            case me_Give:

                if (!fscanf(fp, "%s", TmpBuf)) {
                    qCritical("ReadMobBehaviours(%s): syntax error", m_name.toUtf8().data());
                    FreeMobBehaviour(mb);
                    return any;
                }

                mb.e_mb_Long = atol(TmpBuf);

                break;

            default:
                qCritical("ReadMobBehaviours(%s): syntax error", m_name.toUtf8().data());
                FreeMobBehaviour(mb);
                return any;
            }
            int numCondizioni = 0;
        again:
            if (!(fscanf(fp, "%s", TmpBuf))) {
                qCritical("ReadMobBehaviours(%s): syntax error", m_name.toUtf8().data());
                FreeMobBehaviour(mb);
                return any;
            }
            if (*TmpBuf != 'R' && *TmpBuf != 'C') {
                qCritical("ReadMobBehaviours(%s): syntax error", m_name.toUtf8().data());
                FreeMobBehaviour(mb);
                return any;
            }

            if (*TmpBuf == 'C') {
                if (numCondizioni>MAX_BEHAVIOUR_CONDITIONS-1)
                {
                    qCritical("ReadMobBehaviours(%s): syntax error, troppe condizioni", m_name.toUtf8().data());
                    FreeMobBehaviour(mb);
                    return any;
                }
                int tipoCondizione = atoi(TmpBuf + 1);
                if (!(fscanf(fp, "%s", TmpBuf)))
                {
                    qCritical("ReadMobBehaviours(%s): syntax error", m_name.toUtf8().data());
                    FreeMobBehaviour(mb);
                    return any;
                }
                long condizione = atol(TmpBuf);
                mb.conditions[numCondizioni].active = true;
                mb.conditions[numCondizioni].conditionType = (MobBehaviourCondition)tipoCondizione;
                mb.conditions[numCondizioni].condition = condizione;
                strcpy(mb.conditions[numCondizioni].s_condition, TmpBuf);
                numCondizioni++;
                goto again;
            }

            char* lag = 0;
            if ((lag = strstr(TmpBuf, "L"))++) {
                mb.r_mbLag = (short)atoi(lag);
            }
            switch (mb.mb_Reaction = (MobReactions)atoi(TmpBuf + 1)) {

            case mr_Talk:
            case mr_Disappear:
            case mr_Die:
            case mr_ExecCommand:
            case mr_ChangeLong:
            case mr_ChangeSound:
            case mr_ChangeNearSound:
            case mr_SetGlobalTRUE:
            case mr_SetGlobalFALSE:
            case mr_Aggro:
            case mr_Emote:
            {
                auto val = (Utils::readString(fp).trimmed());
                strcpy(mb.r_mb_String, val.toLatin1());
            }
                
                break;

            case mr_RangeGive:
                if (fgets(TmpBuf, 4096, fp) == NULL) {
                    qCritical("ReadMobBehaviours(%s): syntax error", m_name.toUtf8().data());
                    FreeMobBehaviour(mb);
                    return any;
                }
                if (fgets(TmpBuf, 4096, fp) == NULL) {
                    qCritical("ReadMobBehaviours(%s): syntax error", m_name.toUtf8().data());
                    FreeMobBehaviour(mb);
                    return any;
                }
                {
                    QString tmp(TmpBuf);
                    tmp = tmp.trimmed();
                    auto split = tmp.split(' ', QString::SkipEmptyParts, Qt::CaseInsensitive);
                    if (split.length() != 2) {
                        qCritical("ReadMobBehaviours(%s): syntax error", m_name.toUtf8().data());
                        FreeMobBehaviour(mb);
                        return any;
                    }
                    long n1 = split.at(0).toLong(0, 10);
                    long n2 = split.at(1).toLong(0, 10);
                    mb.r_mb_Long[0] = n1;
                    mb.r_mb_Range = n2;
                }
                break;

            case mr_Give:
            case mr_Elementi:
            case mr_Divini:
            case mr_Xp:

                if (!fscanf(fp, "%s", TmpBuf)) {
                    qCritical("ReadMobBehaviours(%s): syntax error", m_name.toUtf8().data());
                    FreeMobBehaviour(mb);
                    return any;
                }

                mb.r_mb_Long[0] = atol(TmpBuf);
                if (mb.mb_Reaction == mr_Divini && mb.r_mb_Long[0] > 10) {
                    qWarning("Caricato Mob %d che ha Behavior di pagare divini: %d", vnumber(), mb.r_mb_Long[0]);
                }
                if (mb.mb_Reaction == mr_Elementi && mb.r_mb_Long[0] > 100) {
                    qWarning("Caricato Mob %d che ha Behavior di pagare elementi: %d", vnumber(), mb.r_mb_Long[0]);
                }
                break;
            case mr_DestroyObject:
            case mr_LoadMobVnum:
            case mr_LoadObjectInVnum:
            case mr_TransferRoom:
                if(fgets(TmpBuf, 4096, fp) == NULL) {
                    qCritical("ReadMobBehaviours(%s): syntax error", m_name.toUtf8().data());
                    FreeMobBehaviour(mb);
                    return any;
                }
                if (fgets(TmpBuf, 4096, fp) == NULL) {
                    qCritical("ReadMobBehaviours(%s): syntax error", m_name.toUtf8().data());
                    FreeMobBehaviour(mb);
                    return any;
                }
                {
                    QString tmp(TmpBuf);
                    tmp = tmp.trimmed();
                    auto valori = tmp.split(' ', QString::SkipEmptyParts, Qt::CaseInsensitive);
                    for (size_t i = 0; i < MAX_BEHAVIOUR_CONDITIONS; i++)
                    {
                        if (valori.length()>i) {
                            mb.r_mb_Long[i] = valori.at(i).toLong();
                        }
                    }
                }
                break;
            case mr_Break:
                mb.r_mb_Long[0] = 0;
                break;
            case mr_GiveGold:
            case mr_TakeGold:
            case mr_GiveAward:
            case mr_TakeAward:
            case mr_Ricorda1:
            case mr_Ricorda2:
            case mr_TrackVnum: /* mob inizia a camminare verso VNUM   */
            case mr_TransferMob: /* sposta mob in VNUM   */
            case mr_GiveMedal: /* Da numero X medaglei al pg (anche negativo) */
            case mr_UnLoadMobVnum: /* togli mob VNUM dal mud  */
            case mr_Follow: /* segui (1) o smetti di seguire (0)*/
            case mr_IncrementaRicordo1:
            case mr_IncrementaRicordo2:
                if (fgets(TmpBuf, 4096, fp) == NULL) {
                    qCritical("ReadMobBehaviours(%s): syntax error", m_name.toUtf8().data());
                    FreeMobBehaviour(mb);
                    return any;
                }
                if (fgets(TmpBuf, 4096, fp) == NULL) {
                    qCritical("ReadMobBehaviours(%s): syntax error", m_name.toUtf8().data());
                    FreeMobBehaviour(mb);
                    return any;
                }
                mb.r_mb_Long[0] = atol(TmpBuf);
                break;
            default:
                qCritical("ReadMobBehaviours(%s): syntax error", m_name.toUtf8().data());
                FreeMobBehaviour(mb);
                return any;
            }

            m_mobBehaviours.push_back(mb);
            any = true;
            cur = ftell(fp);
        }
        fseek(fp, cur, SEEK_SET);
        return any;
    }

} // namespace ts
