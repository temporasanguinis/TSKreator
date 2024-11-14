
#ifndef TS_MOB_H
#define TS_MOB_H

#include <QTextStream>
#include <stdio.h>
#include "characterdata.h"
#include <vector>


namespace ts
{
    enum MobBehaviourCondition
    { /* queste sono le condizioni usabili per attivare un behavior, piu' di una possibile in contemporanea */
        mc_HasObject, /* solo se il mob ha in inventario l'oggetto, usare piu di una volta se necessario (usa AND) */
        mc_NotHasObject, /* solo se il mob NON ha in inventario l'oggetto, usare piu di una volta se necessario (usa AND) */
        mc_MobDead,  /* se il mob con il vnum specificato non esiste, usare piu di una volta se necessario */
        mc_MobAlive,  /* se il mob con il vnum specificato non esiste, usare piu di una volta se necessario */
        mc_UnaVolta, /* Dare siggilli, xp o elementi puo avvenire una volta sola per un pg */
        mc_HaAward, /* specificare se serve award nr.X affinche' la condizione passi */
        mc_NonHaAward, /* specificare se deve avere award nr.X affinche la condizione passi */
        mc_Ricordo1, /* se il ricordo 1 del mob corrisponde a nr.X */
        mc_Ricordo2,/* se il ricordo 2 del mob corrisponde a nr.X */
        mc_PgHaSoldi, /* se il pg ha soldi X */
        mc_PgNonHaSoldi, /* se il pg NON ha soldi X */
        mc_PgHaPwpSopra, /* Se il PG ha pwp piu di X*/
        mc_PgHaPwpSotto, /* Se il PG NON ha pwp piu di X*/
        mc_RollD100, /* scatta se un dado da 100 e'sotto X */
        mc_StatCheck, /* roll riuscito su: save 0, str 1, int 2, wis 3, dex 4, con 5, chr 6 */
        mc_GlobalTrue, /* se variabile globale del mud (nome) e' TRUE */
        mc_GlobalFalse, /* se variabile globale (nome) e' FALSE */
        mc_PGHasClass, /* se pg ha classe: [numero classe] */
        mc_PGHasAlign, /* se pg ha allineamento: neutral = 0 good = 1 evil = 2 */
        mc_PGHasRace, /* se pg ha razza X (da lista razze) */
        mc_InRoom, /* se il mob si trova in room : vnum */
        mc_SIZE
    };

    enum MobEvents 
    { /* questi sono gli eventi che possono far scattare un behavior */
        me_Talk, /* reagisce a tell ed ask; contiene la lista di parole chiave, separate da spazi */
        me_Give,  /* reagisce al give; contiene il vnum dell'oggetto dato */
        me_Death, /* quando il mob muore: 1 == morto per mano pg, 0 altirmenti  */
        me_Time, /* quando l'ora e' X, se X == -1 ogni ora */
        me_Command, /* quando il pg da comando [numero comando]  */
        me_PlayerEnterLeave, /* quando il pg entra in stanza (1) o esce (0)  */
        me_Follow, /* quando il pg inizia/smette a seguire il mob : 1 = inizia, 0 = smette  */
        me_Arrives, /* quando il mob arriva in stanza VNUM  */
        me_PgOrder, /* quando pg ordina al mob, se vuoto qualsiasi cosa, altrimenti l'ordine deve contenere (testo) */
        me_Ricordo1Modificato, /* ricordo 1 e' diventato X (number) */
        me_Ricordo2Modificato, /* ricordo 2 e' diventato X (number) */
        me_SIZE
    };

    enum MobReactions 
    { /* queste sono le reazioni possibili per un evento del behavior */
        mr_Talk,  /* risponde al pc; contiene la frase da dire */
        mr_Give,  /* da' un oggetto al pc; contiene il vnum dell'oggetto da dare al pc (deve trovarsi nell'inv del mob) */
        mr_Emote, /* effettua un echo; contiene il testo dell'echo */
        mr_Xp,     /* assegna degli xp al pc; contiene il numero di xp */
        mr_Elementi, /* dai un tot di sigilli elementali al pg */
        mr_Divini, /* dai un tot di sigilli divini al pg */
        mr_RangeGive, /* Carica un oggetto random da un [vnum vnum+offset] lo da al PG */
        mr_DestroyObject, /* Distrugge un oggetto in inventario con (vnum specificato) se lo ha */
        mr_Break, /* smette di eseguire behavior successivi (ad esempio per caso fallito nei check precedenti - es. condizione memoria) */
        mr_GiveAward, /* dai award X al pg */
        mr_TakeAward, /* togli award X al pg */
        mr_GiveGold, /* dai gold X al pg */
        mr_TakeGold, /* prendi gold X dal pg */
        mr_Ricorda1, /* ricorda nel ricordo 1 numero X */
        mr_Ricorda2, /* ricorda nel ricordo 2 numero X */
        mr_Disappear, /* mob scompare scritta (testo), senza eseguire procedure morte */
        mr_Die, /* mob muore con scritta (testo)  */
        mr_Aggro, /* aggra il pg che gli si rivolge */
        mr_LoadMobVnum, /* carica mob VNUM in stanza VNUM (se il vnum della room e' 0 allora nella room dove si trova)  */
        mr_UnLoadMobVnum, /* togli mob VNUM dal mud */
        mr_LoadObjectInVnum, /* carica oggetto VNUM in room VNUM */
        mr_TransferMob, /* sposta mob in stanza VNUM */
        mr_TransferRoom, /* sposta i pg presenti in stanza VNUM */
        mr_TrackVnum, /* mob inizia a camminare / tracka verso VNUM (una room a round) */
        mr_GiveMedal, /* Dai numero X medaglie al valore al pg (anche negativo per togliere) */
        mr_Follow, /* segui (1) o smetti di seguire (0) il pg che si rivolge */
        mr_ExecCommand, /* esegui i comandi (comandi testuali) (per evento PgOrder lascia vuoto per eseguire ordine */
        mr_ChangeLong, /* cambia desctizione long al mob  (testo) */
        mr_ChangeSound, /* cambia mob sound in room al mob  (testo) */
        mr_ChangeNearSound, /* cambia mob sound in room adiacenti al mob (testo)  */
        mr_SetGlobalTRUE, /* setta TRUE variabile globale del mud (nome) */
        mr_SetGlobalFALSE, /* setta FALSE variabile globale del mud (nome) */
        mr_IncrementaRicordo1, /* Incrementa il numero nella memoria 1 di X (numero)*/
        mr_IncrementaRicordo2, /* Incrementa il numero nella memoria 2 di X (numero)*/
        mr_SIZE
    };

#define MAX_BEHAVIOUR_CONDITIONS 10
    typedef struct BehaviourCondition {
        bool active;
        enum MobBehaviourCondition conditionType;
        long condition;
        char s_condition[2048];
    } BehaviourCondition;

    typedef struct MobBehaviour_Struct
    {
        enum MobEvents mb_Event;
        char e_mb_String[2048];
        long e_mb_Long;
        enum MobReactions mb_Reaction;
        char r_mb_String[2048];
        long r_mb_Long[MAX_BEHAVIOUR_CONDITIONS];
        short int r_mb_Range;
        short int r_mbLag;
        struct BehaviourCondition conditions[MAX_BEHAVIOUR_CONDITIONS];
    } MobBehaviour;

  class Mob : public CharacterData
  {
  public:
    void init();
    void validate();

    Mob();
    Mob( VNumber );
    Mob( const Mob& );

    virtual ~Mob();

    Mob& operator=( const Mob& );

    const QString& roomSound() const { return m_roomSound; }
    const QString& nearSound() const { return m_nearSound; }
    BitVector acts() const { return m_acts; }
    bool hasAct( int flag ) const { return Bit::has( m_acts, flag ); }
    bool hasBehaviors() const { return m_mobBehaviours.size(); }
    int mobType() const { return m_mobType; }
    int mobLevel() const { return m_mobLevel; }
    int hitPointsBonus() const { return m_hitPointsBonus; }
    long xpBonus() const { return m_xpBonus; }
    int mobDefaultPosition() const { return m_mobDefaultPosition; }

    void setRoomSound( const QString& txt ) { m_roomSound = Utils::removeTilde( txt.trimmed() ); }
    void setNearSound( const QString& txt ) { m_nearSound = Utils::removeTilde( txt.trimmed() ); }
    void setAct( int flag ) { Bit::set( &m_acts, flag ); }
    void setActs( BitVector new_value ) { m_acts = new_value; }
    void removeAct( int flag ) { Bit::remove( &m_acts, flag ); }
    void setMobType( int new_value ) { m_mobType = new_value; }
    void setMobLevel( int new_value ) { m_mobLevel = new_value; }
    void setHitPointsBonus( int new_value ) { m_hitPointsBonus = new_value; generateAverageMaxHitPoints(); }
    void setXpBonus( long );
    void setMobDefaultPosition( int new_value ) { m_mobDefaultPosition = new_value; }

    void load( FILE* );
    void save( QTextStream& );
    std::vector<MobBehaviour>& getBehaviors() { return m_mobBehaviours; }
    bool readMobBehaviours(FILE *fp);

  private:
    void generateExperience();
    void generateAverageMaxHitPoints();

  protected:
    QString m_roomSound;
    QString m_nearSound;
    BitVector m_acts;
    int m_mobType;
    int m_mobLevel;
    int m_hitPointsBonus;
    long m_xpBonus;
    int m_mobDefaultPosition;
    std::vector<MobBehaviour> m_mobBehaviours;
  };

} // namespace ts

#endif // TS_MOB_H


