#include "wndtester.h"
#include "wndroom.h"
#include "wnditem.h"
#include "wndzone.h"
#include "wndmob.h"
#pragma warning(push, 0)
#include <QScrollBar>
#include <QCheckBox>
#include <QCloseEvent>
#include <QMessageBox>
#include <QTextBrowser>
#include <QTextOption>
#pragma warning(pop)
#include "eleuconf.h"
#include "kreatorsettings.h"
#include <typeinfo>

using namespace ts;

const QString listExits[] =
    {
        "<font color=\"#00FF00\">Nord</font>",
        "<font color=\"yellow\">Est</font>",
        "<font color=\"blue\">Sud</font>",
        "<font color=\"#FF48FF\">Ovest</font>",
        "<font color=\"#00FFFF\">Alto</font>",
        "<font color=\"white\">Basso</font>"
    };

#define FG_BLACK         "<font style=\"display:inline-block;white-space:pre-wrap;font-family:Consolas;\"  color=black size=4>"
#define FG_RED           "<font style=\"display:inline-block;white-space:pre-wrap;font-family:Consolas;\"  color=#800000 size=4>"
#define FG_GREEN         "<font style=\"display:inline-block;white-space:pre-wrap;font-family:Consolas;\"  color=#00B300 size=4>"
#define FG_BROWN         "<font style=\"display:inline-block;white-space:pre-wrap;font-family:Consolas;\"  color=#808000 size=4>"
#define FG_BLUE          "<font style=\"display:inline-block;white-space:pre-wrap;font-family:Consolas;\"  color=#000080 size=4>"
#define FG_MAGENTA       "<font style=\"display:inline-block;white-space:pre-wrap;font-family:Consolas;\"  color=#800080 size=4>"
#define FG_CYAN          "<font style=\"display:inline-block;white-space:pre-wrap;font-family:Consolas;\"  color=#008080 size=4>"
#define FG_LT_GRAY       "<font style=\"display:inline-block;white-space:pre-wrap;font-family:Consolas;\"  color=#C0C0C0 size=4>"
#define FG_DK_GRAY       "<font style=\"display:inline-block;white-space:pre-wrap;font-family:Consolas;\"  color=#808080 size=4>"
#define FG_LT_RED        "<font style=\"display:inline-block;white-space:pre-wrap;font-family:Consolas;\"  color=#FF0000 size=4>"
#define FG_LT_GREEN      "<font style=\"display:inline-block;white-space:pre-wrap;font-family:Consolas;\"  color=#00FF00 size=4>"
#define FG_YELLOW        "<font style=\"display:inline-block;white-space:pre-wrap;font-family:Consolas;\"  color=#FFFF00 size=4>"
#define FG_LT_BLUE       "<font style=\"display:inline-block;white-space:pre-wrap;font-family:Consolas;\"  color=#0000FF size=4>"
#define FG_LT_MAGENTA    "<font style=\"display:inline-block;white-space:pre-wrap;font-family:Consolas;\"  color=#FF00FF size=4>"
#define FG_LT_CYAN       "<font style=\"display:inline-block;white-space:pre-wrap;font-family:Consolas;\"  color=#00FFFF size=4>"
#define FG_WHITE         "<font style=\"display:inline-block;white-space:pre-wrap;font-family:Consolas;\"  color=#FFFFFF size=4>"

WndTester::WndTester(Area *ref_area, QWidget* parent)
        : QDialog(parent), Ui::GuiTester()
{
#if defined( KREATOR_DEBUG )
    qDebug("WndTester::WndTester( QWidget* ) called.");
#endif
    m_area_data = ref_area;
    m_area = *(ref_area);
    /*
    if (typeid(WndArea) == typeid(*parent))
    {
        //QMessageBox::information( this, "Kreator", "OK");
        this->parentwnd = dynamic_cast<WndArea*>(parent);
    } else {
        //QMessageBox::information( this, "Kreator", QString(typeid(WndArea).name()) + " " + QString(typeid(*parent).name()));
    }*/
    parentwnd = (WndArea*)parent;
    init();
    le_Input->setFocus();
    if (m_area.rooms().length()) {
        go(m_area.rooms().at(0).vnumber());
    }
    else {
        textBrowser->append(QString("<font color=\"red\" size=4>").append("NON CI SONO ROOMS IN AREA").append("</font>"));
    }
}

WndTester::~WndTester()
{
#if defined( KREATOR_DEBUG )
    qDebug("WndTester::~WndTester() called.");
#endif
}

void WndTester::init()
{
#if defined( KREATOR_DEBUG )
    qDebug("WndTester::init() called.");
#endif
    setupUi(this);
    KreatorSettings::instance().loadGuiStatus("TesterWindow", this);
    textBrowser->setFontPointSize(12);
    scrollBar = textBrowser->verticalScrollBar();

    //connect(le_Input, SIGNAL(returnPressed()), pb_Invia, SLOT(animateClick()));
    connect(le_Input, SIGNAL(returnPressed()), le_Input, SLOT(selectAll()));
    connect(pb_Invia, SIGNAL(clicked()), this, SLOT(mudChangeText()));

    //QTextOption option;
    //option.setFlags(QTextOption::IncludeTrailingSpaces);
    //textBrowser->document()->setDefaultTextOption(option);
    //textBrowser->setStyleSheet("span, p, li {"
    //    " white-space: pre-wrap; "
    //    " display: inline-block;"
    //    " unicode-bidi: embed;"
    //    " font-family: monospace;"
    //    "}");
    //textBrowser->setHtml(""
    //    "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
    //    "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">"
    //    "p, li { white-space: pre-wrap;display: block;font-family: monospace; }"
    //    "</style></head><body style=\" font-family:'Consolas'; font-size:9pt; font-weight:400; font-style:normal;\">"
    //    "</body></html>"
    //);
    textBrowser->append(ParseAnsiColors("$c0009Digita help per avere informazioni sui comandi disponibili"));
}

void WndTester::go(VNumber vnum)
{
#if defined( KREATOR_DEBUG )
    qDebug("WndTester::go(VNumber vnum) called.");
#endif

    if (m_area.hasRoom(vnum)) {
        cr = vnum;
        QString buf;
        Room rp = m_area.room(vnum);
        auto &zn = m_area.zone(floor(vnum / 100));
        if (zn.hasNewFlag(19)) {
            int x, y, z;
            x = rp.getX();
            y = rp.getY();
            z = rp.getZ();
            statusbar->showMessage(QString("Vnum: %1, X:%2, Y:%3, Z:%4").arg(vnum).arg(x).arg(y).arg(z));
        }
        else {
            statusbar->showMessage(QString("Vnum: %1").arg(vnum));
        }
        buf = QString("<font color=\"yellow\" size=4>").append(ParseAnsiColors(rp.name())).append("</font>");
        if (rp.sectorType() == ROOM_SECTOR_TELEPORT) {
            buf.append(ParseAnsiColors(" $c0015 (TELEPORT)"));
        }
        textBrowser->append(buf);
        textBrowser->append(QString("<font color=\"lightgray\" size=4>").append(ParseAnsiColors(rp.description())).append("</font>"));

        buf.clear();
        for (int i = 0; i < rp.exits().size();i++) {
            buf.append(' ' + listExits[rp.exits().at(i).direction()]);
            if (rp.exits().at(i).isClosed())
                buf.append(" (closed)");
            if (rp.exits().at(i).hasFlag(EXIT_FLAG_SECRET))
                buf.append(ParseAnsiColors(" $c0009(secret)"));
            if (rp.exits().at(i).isLocked())
                buf.append(" (locked)");
        }
        if (buf.isEmpty())
            textBrowser->append(QString("<font color=\"lightgray\" size=4>Uscite visibili: Nessuna</font>"));
        else
            textBrowser->append(QString("<font color=\"lightgray\" size=4>Uscite visibili:").append(buf).append("</font>"));

        checkObjInit(vnum);
        checkMobInit(vnum);

        textBrowser->append(ParseAnsiColors("$c0015-----------------"));
        textBrowser->append(ParseAnsiColors("$c0007Uscite visibili:"));

        buf.clear();
        for (int i = 0; i < rp.exits().size();i++) {
            if (m_area.hasRoom(rp.exits().at(i).toRoom())) {
                buf = QString("$c0007 %1 - %2 #%3").arg(rp.exits().at(i).name()).arg(m_area.room(rp.exits().at(i).toRoom()).name()).arg(rp.exits().at(i).toRoom());
                if (rp.exits().at(i).isClosed())
                    buf.append(" (closed)");
                if (rp.exits().at(i).isLocked())
                    buf.append(" (locked)");
                if (rp.exits().at(i).hasFlag(EXIT_FLAG_SECRET))
                    buf.append(" (secret)");
                textBrowser->append(ParseAnsiColors(buf));
            }
        }
        textBrowser->append(ParseAnsiColors("$c0015-----------------"));
        parentwnd->selectRoom(vnum);
    } else
        textBrowser->append(QString("<font color=\"red\" size=4>").append("Quella stanza non esiste").append("</font>"));
}

const Mob * WndTester::findMobInRoom(VNumber room, QString name, bool *ok) {
    for (int i = 0; i < m_area.zones().size(); i++) {
        ZoneCommandList zcl = m_area.zones().at(i).commandList();
        ZoneCommandList::const_iterator it = zcl.begin();
        while (it != zcl.end()) {
            qWarning(qPrintable(it->toString(false)));
            if (it->isMobLoad() && it->argument(ZoneCommand::Argument3) == room) {
                if (m_area.hasMob(it->argument(ZoneCommand::Argument1))) {
                    if (m_area.mob(it->argument(ZoneCommand::Argument1)).isname(name)) {
                        *ok = true;
                        return &(m_area.mob(it->argument(ZoneCommand::Argument1)));
                    }
                }
            }
            it++;
        }
    }
    *ok = false;
    return NULL;
}

const Item * WndTester::findObjInRoom(VNumber room, QString name, bool *ok) {

    for (int i = 0; i < m_area.zones().size(); i++) {
        ZoneCommandList zcl = m_area.zones().at(i).commandList();
        ZoneCommandList::const_iterator it = zcl.begin();
        while (it != zcl.end()) {
            qWarning(qPrintable(it->toString(false)));
            if (it->isItemLoad() && it->argument(ZoneCommand::Argument3) == room) {
                if (m_area.hasItem(it->argument(ZoneCommand::Argument1))) {
                    if (m_area.item(it->argument(ZoneCommand::Argument1)).isname(name)) {
                        *ok = true;
                        return &m_area.item(it->argument(ZoneCommand::Argument1));
                    }
                }
            }
            it++;
        }
    }
    *ok = false;
    return NULL;
}

bool WndTester::viewKeyowrd(VNumber room, QString key) {
    if (!m_area_data->hasRoom(room)) {
        textBrowser->append(QString("<font color=\"red\" size=4>").append("Questa room non esiste!"));
        return false;
    }
    Room r = m_area_data->room(room);
    if (r.canFindExtraDescription(key)) {
        auto extra = r.findExtraDescription(key);
        textBrowser->append(QString("<font color=\"yellow\" size=4>").append("Vedi la: ").append(extra.keys()));
        textBrowser->append(QString("<font color=\"yellow\" size=4>").append(extra.description()));
        return true;
    }
    return false;
}
void WndTester::mudChangeText()
{
#if defined( KREATOR_DEBUG )
    qDebug("WndTester::mudChangeText() called.");
#endif
    QString vv = le_Input->text();
    QString buf = vv.section(' ', 0, 0);
    QString buf2 = vv.section(' ', 1, 1);
    QString buf3 = vv.section(' ', 2, 2);
    buf = buf.toLower();
    buf2 = buf2.toLower();
    buf3 = buf3.toLower();

    textBrowser->append(QString("<font color=\"yellow\" size=4>").append(buf).append(" ").append(buf2));

    if (QString("go").startsWith(buf)) {
        bool ok;
        VNumber vnum = buf2.toLong(&ok, 10);
        if (ok) {
            go(vnum);
        }
        else {
            VNumber minVnum = m_area.zones().first().minVNumber();
            VNumber maxVnum = m_area.zones().last().maxVNumber();
            for (size_t i = minVnum; i < maxVnum; i++)
            {
                if (m_area.hasRoom(i)) {
                    const Mob* mob = findMobInRoom(i, buf2, &ok);
                    if (ok) {
                        go(i);
                        return;
                    }
                }
            }
            textBrowser->append(ParseAnsiColors(QString("$c0009Non trovo: ").append(buf2)));
        }
    } else if (QString("look").startsWith(buf)) {
        if (buf2 == "s" || buf2 == "w" || buf2 == "n" || buf2 == "e" || buf2 == "u" || buf2 == "s" ||
            buf2 == "a" || buf2 == "b" || buf2 == "o" || buf2 == "d") {
            int dir = -1;
            if (QString("south").startsWith(buf2) || QString("sud").startsWith(buf2))
                dir = EXIT_DIRECTION_SOUTH;
            else if (QString("est").startsWith(buf2) || QString("east").startsWith(buf2))
                dir = EXIT_DIRECTION_EAST;
            else if (QString("nord").startsWith(buf2) || QString("north").startsWith(buf2))
                dir = EXIT_DIRECTION_NORTH;
            else if (QString("west").startsWith(buf2) || QString("ovest").startsWith(buf2))
                dir = EXIT_DIRECTION_WEST;
            else if (QString("up").startsWith(buf2) || QString("alto").startsWith(buf2))
                dir = EXIT_DIRECTION_UP;
            else if (QString("down").startsWith(buf2) || QString("basso").startsWith(buf2))
                dir = EXIT_DIRECTION_DOWN;

            Room rp = m_area.room(cr);
            if (!rp.hasExit(dir)) {
                textBrowser->append(ParseAnsiColors(QString("$c0009Non vedi nulla in quella direzione.")));
            }
            else {
                textBrowser->append(ParseAnsiColors(QString("$c0009Guardi verso ").append(buf2.toUpper())));
                Exit exitp = rp.exit(dir);

                if (exitp.hasDoor() && !exitp.doorKey())
                    textBrowser->append(ParseAnsiColors(QString("$c0010.. attraverso un'uscita (%1)").arg(exitp.keys())));

                if (exitp.hasDoor() && exitp.doorKey())
                    textBrowser->append(ParseAnsiColors(QString("$c0010.. attraverso un'uscita (%1) chiusa dalla chiave: %2").arg(exitp.keys()).arg(exitp.doorKey())));

                VNumber crold = cr;
                go(exitp.toRoom());
                cr = crold;
                statusbar->showMessage(QString("Vnum: %1").arg(cr));
            }
        }
        else {
            const Mob* mob;
            const Item* item;
            bool ok;
            if (buf2!="" && viewKeyowrd(cr, buf2)) {
                return;
            }
            else if (buf.length() && buf2.trimmed().length() && &(mob = (findMobInRoom(cr, buf2.trimmed(), &ok))) && ok) {
                showMob(mob, cr);
            }
            else if (buf.length() && buf2.trimmed().length() && &(item = (findObjInRoom(cr, buf2.trimmed(), &ok))) && ok) {
                showObj(item, cr);
            }
            else if (!buf2.trimmed().length()) {
                go(cr);
            }
            else {
                textBrowser->append(QString("<font color=\"red\" size=4>").append("Non vedo: ").append(buf2));
            }
        }
    }
    else if (QString("cstat").startsWith(buf)) {
        const Mob *mob;
        bool ok;
        if (buf.length() && buf2.trimmed().length() && &(mob = findMobInRoom(cr, buf2.trimmed(), &ok)) && ok) {
            statMob(mob, cr);
        }
        else if (!buf2.trimmed().length()) {
            textBrowser->append(QString("<font color=\"red\" size=4>").append("Chi vuoi stattare?"));
        }
        else {
            textBrowser->append(QString("<font color=\"red\" size=4>").append("Non vedo: ").append(buf2));
        }
    }
    else if (QString("ostat").startsWith(buf)) {
        bool ok;
        const Item* item;
        if (buf.length() && buf2.trimmed().length() && &(item = (findObjInRoom(cr, buf2.trimmed(), &ok))) && ok) {
            statObj(item, cr);
        }
        else if (!buf2.trimmed().length()) {
            textBrowser->append(QString("<font color=\"red\" size=4>").append("Cosa vuoi stattare?"));
        }
        else {
            textBrowser->append(QString("<font color=\"red\" size=4>").append("Non vedo: ").append(buf2));
        }
    } else if (QString("rstat").startsWith(buf)) {
        rstat();
    } else if (QString("redit").startsWith(buf)) {
        bool ok;
        VNumber vnum = buf2.toLong(&ok);
        editRoom(ok ? vnum : cr);
    }
    else if (QString("medit").startsWith(buf)) {
        bool ok;
        VNumber mob = buf2.toLong(&ok);
        if (ok) {
            editMob(mob);
        }
        else
        {
            textBrowser->append(QString("<font color=\"red\" size=4>").append("Formato: medit vnum"));
        }
    }
    else if (QString("oedit").startsWith(buf)) {
        bool ok;
        VNumber mob = buf2.toLong(&ok);
        if (ok) {
            editObj(mob);
        }
        else
        {
            textBrowser->append(QString("<font color=\"red\" size=4>").append("Formato: oedit vnum"));
        }
    } else if (QString("south").startsWith(buf) || QString("sud").startsWith(buf))
        do_move(EXIT_DIRECTION_SOUTH);
    else if (QString("est").startsWith(buf) || QString("east").startsWith(buf))
        do_move(EXIT_DIRECTION_EAST);
    else if (QString("nord").startsWith(buf) || QString("north").startsWith(buf))
        do_move(EXIT_DIRECTION_NORTH);
    else if (QString("west").startsWith(buf) || QString("ovest").startsWith(buf))
        do_move(EXIT_DIRECTION_WEST);
    else if (QString("up").startsWith(buf) || QString("alto").startsWith(buf))
        do_move(EXIT_DIRECTION_UP);
    else if (QString("down").startsWith(buf) || QString("basso").startsWith(buf))
        do_move(EXIT_DIRECTION_DOWN);
    else if (QString("help").startsWith(buf))
        help();
    else if (QString("teleport").startsWith(buf))
        teleport();
    else if (QString("init").startsWith(buf)) {
        const Mob* mob;
        const Item* item;
        bool ok;
        if (buf2 != "mob" && buf2!="obj") {
            buf3 = buf2;
            buf2 = "";
        }
        if (buf.length() && (buf2=="mob"||buf2=="") && buf3.trimmed().length() && &(mob = (findMobInRoom(cr, buf3.trimmed(), &ok))) && ok) {
            initMob(mob, cr);
        }
        else if (buf.length() && (buf2=="obj"||buf2=="") && buf3.trimmed().length() && &(item = (findObjInRoom(cr, buf3.trimmed(), &ok))) && ok) {
            initObj(item, cr);
        }
        else {
            textBrowser->append(QString("<font color=\"red\" size=4>").append("Non vedo: &lt;").append(buf3).append("&gt;"));
        }
    }
    else textBrowser->append(ParseAnsiColors("$c0007Cosa?"));

    scrollBar->setValue(scrollBar->maximum() + scrollBar->pageStep());
}

int WndTester::inverseDirection(int dir) {
	switch (dir) {
                case EXIT_DIRECTION_NORTH: //N
                        dir=EXIT_DIRECTION_SOUTH;
			break;
                case EXIT_DIRECTION_EAST: //E
                        dir=EXIT_DIRECTION_WEST;
			break;
                case EXIT_DIRECTION_SOUTH: //S
                        dir=EXIT_DIRECTION_NORTH;
			break;
                case EXIT_DIRECTION_WEST: //W
                        dir=EXIT_DIRECTION_EAST;
			break;
                case EXIT_DIRECTION_UP: //U
                        dir=EXIT_DIRECTION_DOWN;
			break;
                case EXIT_DIRECTION_DOWN: //D
                        dir=EXIT_DIRECTION_UP;
			break;
	}
	return dir;
}

void WndTester::do_move(int cmd)
{
#if defined( KREATOR_DEBUG )
    qDebug("WndTester::do_move(int cmd) called.");
#endif
    Room rp = m_area.room(cr);
    if (!rp.hasExit(cmd)) {
        if (!chkCreateRooms->isChecked()) {
            textBrowser->append("<font color=\"red\" size=4>Non puoi andare in quella direzione.</font>");
            return;
        }
        else {
		// stanza corrente e la nuova da creare
		Room m_room = m_area_data->room(cr);
                Room new_room( m_area_data->firstFreeRoomVNumber(cr) );

		// assegna la zona alla room
		if( KreatorSettings::instance().assignZoneToRooms() )
			new_room.setZone( m_area_data->zoneReferenced( new_room.vnumber() ) );

		// uscita da qui alla nuova room
		Exit pToNew(cmd, m_room.vnumber());
		pToNew.setToRoom(new_room.vnumber());

		// uscita dalla nuova room a qui
		Exit pFromNew = pToNew.reversed();

		// aggingi uscite ad ambedue le room (corrente e nuova)
		m_room.addExit(pToNew);
		new_room.addExit(pFromNew);

		// aggiunge la room all area
		m_area_data->addRoom( new_room );
		m_area_data->addRoom( m_room );

		// mostra dialogo per edit della nuova room
                editRoom( new_room.vnumber() );

		m_area = *(m_area_data);
		go(new_room.vnumber());
		return;
		// vai nella nuova room
	}
    }
    Exit exitp = rp.exit(cmd);
    if (exitp.hasKeyHole() && exitp.doorKey())
        textBrowser->append(ParseAnsiColors(QString("$c0010Attraversi un'uscita chiusa dalla chiave: %1").arg(exitp.doorKey())));

    if (chkAssegnaPosizioni->checkState() == Qt::CheckState::PartiallyChecked || 
        chkAssegnaPosizioni->checkState() == Qt::CheckState::Checked) {
        if (m_area_data->hasRoom(exitp.toRoom())) {
            int x = 0, y = 0, z = 0;
            auto r = m_area_data->room(exitp.toRoom());
            if (chkAssegnaPosizioni->checkState() == Qt::CheckState::Checked ||
                (chkAssegnaPosizioni->checkState() == Qt::CheckState::PartiallyChecked &&
                 r.getX() == 0 && r.getY() == 0 && r.getZ() == 0)) {
                x = rp.getX();
                y = rp.getY();
                z = rp.getZ();
                switch (exitp.direction())
                {
                case EXIT_DIRECTION_DOWN:
                    z--;
                    break;
                case EXIT_DIRECTION_UP:
                    z++;
                    break;
                case EXIT_DIRECTION_NORTH:
                    y++;
                    break;
                case EXIT_DIRECTION_EAST:
                    x++;
                    break;
                case EXIT_DIRECTION_SOUTH:
                    y--;
                    break;
                case EXIT_DIRECTION_WEST:
                    x--;
                    break;
                default:
                    break;
                }
                r.setPos(x, y, z);
                m_area_data->addRoom(r);
                m_area = *(m_area_data);
                parentwnd->somethingChanged();
            }
        }
    }
    go(exitp.toRoom());
}

void WndTester::editRoom(VNumber room) {
    if (!m_area_data->hasRoom(room)) {
        textBrowser->append(QString("<font color=\"red\" size=4>").append("Questa room non esiste!"));
        return;
    }
    WndRoom *pWndRoom = new WndRoom( *(m_area_data), m_area_data->room( room ), this );
    pWndRoom->somethingChanged();
    if (parentwnd) {
        connect( pWndRoom, SIGNAL( dataSaved() ), parentwnd, SLOT(refreshView() ) );
        connect( pWndRoom, SIGNAL( dataSaved() ), parentwnd, SLOT( somethingChanged() ) );
        connect( pWndRoom, SIGNAL( dataSaved() ), this, SLOT( roomCreated() ) );
    }
    pWndRoom->setModal(true);
    pWndRoom->show();
}

void WndTester::editMob(VNumber mob) {
    if (!m_area_data->hasMob(mob)) {
        textBrowser->append(QString("<font color=\"red\" size=4>").append("Questo mob non esiste!"));
        return;
    }
    WndMob* pWndMob = new WndMob(*(m_area_data), m_area_data->mob(mob), this);
    pWndMob->somethingChanged();
    if (parentwnd) {
        connect(pWndMob, SIGNAL(dataSaved()), parentwnd, SLOT(refreshView()));
        connect(pWndMob, SIGNAL(dataSaved()), parentwnd, SLOT(somethingChanged()));
        connect(pWndMob, SIGNAL(dataSaved()), this, SLOT(roomCreated()));
    }
    pWndMob->setModal(true);
    pWndMob->show();
}

void WndTester::editObj(VNumber mob) {
    if (m_area_data->hasItem(mob)) {
        textBrowser->append(QString("<font color=\"red\" size=4>").append("Questo ggetto non esiste!"));
        return;
    }
    WndItem* pWndMob = new WndItem(*(m_area_data), m_area_data->item(mob), this);
    pWndMob->somethingChanged();
    if (parentwnd) {
        connect(pWndMob, SIGNAL(dataSaved()), parentwnd, SLOT(refreshView()));
        connect(pWndMob, SIGNAL(dataSaved()), parentwnd, SLOT(somethingChanged()));
        connect(pWndMob, SIGNAL(dataSaved()), this, SLOT(roomCreated()));
    }
    pWndMob->setModal(true);
    pWndMob->show();
}

void WndTester::roomCreated() {
    // prendi copia dell area dal pointer originale (il tester non lavora a vivo sul pointer)
    m_area = *(m_area_data);
    go(cr);
}

QString WndTester::ParseAnsiColors(const QString &text)
{
#if defined( KREATOR_DEBUG )
    qDebug("WndTester::ParseAnsiColors(QString txt) called.");
#endif
    QString txt = text;
    int i;

    for (i = txt.indexOf(QRegExp("\\$c[0-9]{4,4}"));i != -1; i = txt.indexOf(QRegExp("\\$c[0-9]{4,4}"))) {

        QString tmp = txt.mid(i + 2, i + 4);
        tmp = ansi_parse(tmp);
        txt.replace(i, 6, tmp);
    }
    for (i = 0; i < txt.size(); i++) {
        if (txt.at(i) == '\n') {
            txt.replace(i, 1, "<br>");
        }
    }

    return txt;
}

QString WndTester::ansi_parse(const QString &code)
{
#if defined( KREATOR_DEBUG )
    qDebug("WndTester::ansi_parse(QString code) called.");
#endif
    QString f;

    switch (code[2].unicode()) {
        case '0':
            switch (code[3].unicode()) {
                case '0':
                    f = FG_BLACK;
                    break;
                case '1':
                    f = FG_RED;
                    break;
                case '2':
                    f = FG_GREEN;
                    break;
                case '3':
                    f = FG_BROWN;
                    break;
                case '4':
                    f = FG_BLUE;
                    break;
                case '5':
                    f = FG_MAGENTA;
                    break;
                case '6':
                    f = FG_CYAN;
                    break;
                case '7':
                    f = FG_LT_GRAY;
                    break;
                case '8':
                    f = FG_DK_GRAY;
                    break;
                case '9':
                    f = FG_LT_RED;
                    break;
                default:
                    f = FG_DK_GRAY;
                    break;
            }
            break;

        case '1':
            switch (code[3].unicode()) {
                case '0':
                    f = FG_LT_GREEN;
                    break;
                case '1':
                    f = FG_YELLOW;
                    break;
                case '2':
                    f = FG_LT_BLUE;
                    break;
                case '3':
                    f = FG_LT_MAGENTA;
                    break;
                case '4':
                    f = FG_LT_CYAN;
                    break;
                case '5':
                    f = FG_WHITE;
                    break;
                default:
                    f = FG_LT_GREEN;
                    break;
            }
            break;

        default:
            f = FG_LT_RED;
            break;
    }

    return f;
}

void WndTester::help()
{
#if defined( KREATOR_DEBUG )
    qDebug("WndTester::help() called.");
#endif
    textBrowser->append(ParseAnsiColors("$c0014----------------------------------------------------------"));
    textBrowser->append(ParseAnsiColors("$c0010look [dir/key/mob/obj]: $c0014guarda qualcosa"));
    textBrowser->append(ParseAnsiColors("$c0010go vnum/nome: $c0014vai nella stanza vnum o dal mob con nome"));
    textBrowser->append(ParseAnsiColors("$c0010teleport: $c0014attiva la teleport"));
    textBrowser->append(ParseAnsiColors("$c0010rstat: $c0014stat della stanza"));
    textBrowser->append(ParseAnsiColors("$c0010cstat key: $c0014stat di un mob nella stanza"));
    textBrowser->append(ParseAnsiColors("$c0010ostat key: $c0014stat di un oggetto nella stanza"));
    textBrowser->append(ParseAnsiColors("$c0010redit: $c0014edit della stanza"));
    textBrowser->append(ParseAnsiColors("$c0010medit vnum: $c0014edit del mob"));
    textBrowser->append(ParseAnsiColors("$c0010oedit vnum: $c0014edit del oggetto"));
    textBrowser->append(ParseAnsiColors("$c0010init [mob/obj] key: $c0014edit dell'init del mob o oggetto"));
    textBrowser->append(ParseAnsiColors("$c0014----------------------------------------------------------"));
}

void WndTester::teleport()
{
#if defined( KREATOR_DEBUG )
    qDebug("WndTester::teleport() called.");
#endif

    Room rp = m_area.room(cr);

    if (rp.sectorType() == ROOM_SECTOR_TELEPORT) {
        go(rp.teleportToRoom());
    }
    else {
        textBrowser->append(ParseAnsiColors(QString("$c0009Non sei in room teleport.")));
    }
}

void WndTester::statMob(const Mob *mob, VNumber room)
{
#if defined( KREATOR_DEBUG )
    qDebug("WndTester::statMob(Mob) called.");
#endif
    int i;
    QString sVnum = QString::number(mob->vnumber());
    QString mName = mob->name();
    QString mShort = mob->shortDescription();
    QString mLong = mob->longDescription();
    QString mDesc = mob->description();
    QString str = ParseAnsiColors("$c0007"
        "$c0010VNum:     $c0014" + sVnum + "\n" +
        "$c0010Short:    $c0014" + mShort + "\n"
        "$c0010Long:     $c0014" + mLong + "\n"
        "$c0010Acts:     $c0014" +
        Utils::bitvector2string(mob->acts(), Eleuconf::getAllFlagsCaption(Eleuconf::mobActionsFlags)).toLower() + "\n"
        "$c0010New Acts: $c0014" +
        Utils::bitvector2string(mob->extraFlags(), Eleuconf::getAllFlagsCaption(Eleuconf::mobActionsFlags, 1)).toLower() + "\n"
        "$c0010Affects:  $c0014" +
        Utils::bitvector2string(mob->affects(), Eleuconf::getAllFlagsCaption(Eleuconf::mobAffectionsFlags)).toLower() + "\n"
        "$c0010Affects2: $c0014" +
        Utils::bitvector2string(mob->affects2(), Eleuconf::getAllFlagsCaption(Eleuconf::mobAffectionsFlags, 1)).toLower() + "\n"
        "$c0010Res:      $c0014" +
        Utils::bitvector2string(mob->resistances(), Eleuconf::getAllFlagsCaption(Eleuconf::resFlags)).toLower() + "\n"
        "$c0010Immu:     $c0014" +
        Utils::bitvector2string(mob->immunities(), Eleuconf::getAllFlagsCaption(Eleuconf::resFlags)).toLower() + "\n"
        "$c0010Susce:    $c0014" +
        Utils::bitvector2string(mob->susceptibilities(), Eleuconf::getAllFlagsCaption(Eleuconf::resFlags)).toLower() + "\n"
        "$c0010Attacks:  $c0014" + QString::number(mob->attacks()) + "$c0010, Damage: $c0014" + mob->damage().toString() + "$c0010, HitPoints: $c0014" + QString::number(mob->maxHitPoints())
    );
    textBrowser->append(ParseAnsiColors("$c0014- Statti il mob-------------------------------------------"));
    textBrowser->append(str);
    bool inMob = false;
    for (i = 0; i < m_area.zones().size(); i++) {
        ZoneCommandList zcl = m_area.zones().at(i).commandList();
        ZoneCommandList::const_iterator it = zcl.begin();
        while (it != zcl.end()) {
            qWarning(qPrintable(it->toString(false)));
            if (it->isMobLoad() && it->argument(ZoneCommand::Argument3) == room && it->argument(ZoneCommand::Argument1) == mob->vnumber()) {
                inMob = true;
            }
            if (inMob) {
                if (it->isMobLoad()) {
                    textBrowser->append(ParseAnsiColors("$c0010ZoneInit: $c0014" + QString::number(it->id())));
                }
                else if (it->hasParent()) {
                }
                else {
                    textBrowser->append(ParseAnsiColors("$c0010ZoneEnd:  $c0014" + QString::number(it->id())));
                    inMob = false;
                }
            }
            it++;
        }
    }
    textBrowser->append(ParseAnsiColors("$c0014----------------------------------------------------------"));
}

void WndTester::statObj(const Item* mob, VNumber room)
{
#if defined( KREATOR_DEBUG )
    qDebug("WndTester::statObj(Mob) called.");
#endif
    int i;
    QString sVnum = QString::number(mob->vnumber());
    QString mName = mob->name();
    QString mShort = mob->shortDescription();
    QString str = ParseAnsiColors("$c0007"
        "$c0010VNum:     $c0014" + sVnum + "\n" +
        "$c0010Short:    $c0014" + mShort + "\n$c0007\n"
    );
    textBrowser->append(ParseAnsiColors("$c0014- Statti l'oggetto ---------------------------------------"));
    textBrowser->append(str);
    bool inMob = false;
    for (i = 0; i < m_area.zones().size(); i++) {
        ZoneCommandList zcl = m_area.zones().at(i).commandList();
        ZoneCommandList::const_iterator it = zcl.begin();
        while (it != zcl.end()) {
            qWarning(qPrintable(it->toString(false)));
            if (it->isItemLoad() && it->argument(ZoneCommand::Argument3) == room && it->argument(ZoneCommand::Argument1) == mob->vnumber()) {
                inMob = true;
            }
            if (inMob) {
                if (it->isItemLoad()) {
                    textBrowser->append(ParseAnsiColors("$c0010ZoneInit: $c0014" + QString::number(it->id())));
                }
                else if (it->hasParent()) {
                }
                else {
                    textBrowser->append(ParseAnsiColors("$c0010ZoneEnd:  $c0014" + QString::number(it->id())));
                    inMob = false;
                }
            }
            it++;
        }
    }

    textBrowser->append(ParseAnsiColors("$c0014----------------------------------------------------------"));
}

void WndTester::PrintCommand(QTextBrowser *t, ZoneCommand zc) {
    QString str = m_area.generateComment(zc);
    t->append(ParseAnsiColors("$c0007  " + str));
}

void WndTester::showMob(const Mob * mob, VNumber room)
{
#if defined( KREATOR_DEBUG )
    qDebug("WndTester::showMob(Mob) called.");
#endif
    int i;
    QString mDesc = mob->description().trimmed();
    if (mDesc != "") mDesc += "\n";
    QString mShort = mob->shortDescription();
    QString mKey = mob->name();
    QString sVnum = QString::number(mob->vnumber());
    QString str = ParseAnsiColors("$c0007"
        "$c0010Key:   $c0014" + mKey + "\n"
        "$c0010Short: $c0014" + mShort + "\n$c0007" +
        mDesc +
        "$c0010Il Mob #" + sVnum + " indossa:"
    );
    textBrowser->append(ParseAnsiColors("$c0014- Guardi il mob-------------------------------------------"));
    textBrowser->append(str);
    bool inMob = false;
    for (i = 0; i < m_area.zones().size(); i++) {
        ZoneCommandList zcl = m_area.zones().at(i).commandList();
        ZoneCommandList::const_iterator it = zcl.begin();
        while (it != zcl.end()) {
            qWarning(qPrintable(it->toString(false)));
            if (it->isMobLoad() && it->argument(ZoneCommand::Argument3) == room && it->argument(ZoneCommand::Argument1) == mob->vnumber()) {
                inMob = true;
            }
            if (inMob) {
                if (it->isMobLoad()) {
                    //textBrowser->append(ParseAnsiColors("$c0010ZoneInit: $c0014" + QString::number(it->id())));
                }
                else if (it->hasParent()) {
                    PrintCommand(textBrowser, *it);
                }
                else {
                    //textBrowser->append(ParseAnsiColors("$c0010ZoneEnd:  $c0014" + QString::number(it->id())));
                    inMob = false;
                }
            }
            it++;
        }
    }

    textBrowser->append(ParseAnsiColors("$c0014----------------------------------------------------------"));
}

void WndTester::showObj(const Item* item, VNumber room)
{
#if defined( KREATOR_DEBUG )
    qDebug("WndTester::showObj(Mob) called.");
#endif
    int i;
    QString mDesc = item->longDescription().trimmed();
    if (mDesc != "") mDesc += "\n";
    QString mShort = item->shortDescription();
    QString mKey = item->name();
    QString sVnum = QString::number(item->vnumber());
    QString str = ParseAnsiColors("$c0007"
        "$c0010Key:   $c0014" + mKey + "\n"
        "$c0010Short: $c0014" + mShort + "\n$c0007" +
        mDesc +
        "$c0010L'oggetto #" + sVnum + " contiene:"
    );
    textBrowser->append(ParseAnsiColors("$c0014- Guardi l'oggetto ---------------------------------------"));
    textBrowser->append(str);
    bool inMob = false;
    for (i = 0; i < m_area.zones().size(); i++) {
        ZoneCommandList zcl = m_area.zones().at(i).commandList();
        ZoneCommandList::const_iterator it = zcl.begin();
        while (it != zcl.end()) {
            qWarning(qPrintable(it->toString(false)));
            if (it->isItemLoad() && it->argument(ZoneCommand::Argument3) == room && it->argument(ZoneCommand::Argument1) == item->vnumber()) {
                inMob = true;
            }
            if (inMob) {
                if (it->isItemLoad()) {
                    //textBrowser->append(ParseAnsiColors("$c0010ZoneInit: $c0014" + QString::number(it->id())));
                }
                else if (it->hasParent()) {
                    PrintCommand(textBrowser, *it);
                }
                else {
                    //textBrowser->append(ParseAnsiColors("$c0010ZoneEnd:  $c0014" + QString::number(it->id())));
                    inMob = false;
                }
            }
            it++;
        }
    }
    textBrowser->append(ParseAnsiColors("$c0014----------------------------------------------------------"));
}

void WndTester::checkObjInit(VNumber vnum)
{
#if defined( KREATOR_DEBUG )
    qDebug("WndTester::CheckMobInit(VNumber) called.");
#endif
    int i;
    for (i = 0; i < m_area.zones().size(); i++) {
        ZoneCommandList zcl = m_area.zones().at(i).commandList();
        ZoneCommandList::const_iterator it = zcl.begin();
        while (it != zcl.end()) {
            qWarning(qPrintable(it->toString(false)));
            if (it->isItemLoad() && it->argument(ZoneCommand::Argument3) == vnum) {
                textBrowser->append(ParseAnsiColors("$c0007" + m_area.item(it->argument(ZoneCommand::Argument1)).longDescription() + " (Item #" + QString::number(it->argument(ZoneCommand::Argument1)) + ", Init line:" + QString::number(it->id()) + ")"));
            }
            it++;
        }
    }
}
void WndTester::checkMobInit(VNumber vnum)
{
#if defined( KREATOR_DEBUG )
    qDebug("WndTester::CheckMobInit(VNumber) called.");
#endif
    int i;
    for (i = 0; i < m_area.zones().size() ; i++) {
        ZoneCommandList zcl = m_area.zones().at(i).commandList();
        ZoneCommandList::const_iterator it = zcl.begin();
        while (it != zcl.end()) {
            qWarning(qPrintable(it->toString(false)));
            if (it->isMobLoad() && it->argument(ZoneCommand::Argument3) == vnum) {
                textBrowser->append(ParseAnsiColors("$c0007" + m_area.mob(it->argument(ZoneCommand::Argument1)).longDescription() + " (Mob #" + QString::number(it->argument(ZoneCommand::Argument1)) + ", Init line:" + QString::number(it->id()) + ")"));
            }
            it++;
        }
    }
}

void WndTester::rstat()
{
    Room rp = m_area.room(cr);

    QString buf = QString("$c0005Room name: $c0011%1$c0005, Zona : $c0014%2$c0005. V-Number : $c0010%3").arg(rp.name()).arg(rp.zone()).arg(rp.vnumber());
    textBrowser->append(ParseAnsiColors(buf));

    textBrowser->append(ParseAnsiColors("$c0005Sector type : $c0014" + ConstantName::roomSector(rp.realSectorType())));

    buf = "$c0005Room flags: $c0014" + Utils::bitvector2string(rp.flags(), Eleuconf::getAllFlagsCaption(Eleuconf::roomFlags));
    textBrowser->append(ParseAnsiColors(buf));


    textBrowser->append(ParseAnsiColors("$c0005Descrizione:"));

    textBrowser->append(ParseAnsiColors("$c0007" + rp.description()));

    buf = "$c0005Extra description keywords(s):$c0007 ";

    if (rp.extraDescriptions().size() > 0) {
        buf += "\n\r";
        for (int i = 0; i < rp.extraDescriptions().size(); i++) {
            buf += rp.extraDescriptions().at(i).keys() + "\n\r";
        }
        buf += "\n\r";
    } else {
        buf += " Nessuna\n\r";
    }
    textBrowser->append(ParseAnsiColors(buf));

    textBrowser->append(ParseAnsiColors("$c0005------- $c0014Uscite $c0005-------"));
    for (int i = 0; i < rp.exits().size(); i++) {
        buf = QString("\n\r$c0005Direzione $c0015%1").arg(rp.exits().at(i).name());
        if (!rp.exits().at(i).keys().isEmpty()) {
            buf += "Keyword : $c0014" + rp.exits().at(i).keys();
        }
        textBrowser->append(ParseAnsiColors(buf));

        if (!rp.exits().at(i).description().isEmpty()) {
            textBrowser->append(ParseAnsiColors("$c0005Descrizione:"));
            textBrowser->append(ParseAnsiColors("$c0007" + rp.exits().at(i).description()));
        }

        buf = "$c0005Exit flag: $c0015" + Utils::bitvector2string(rp.exits().at(i).flags(), Eleuconf::getAllFlagsCaption(Eleuconf::exitFlags));
        textBrowser->append(ParseAnsiColors(buf));

        buf = QString("$c0005Key num: $c0015%1").arg(rp.exits().at(i).doorKey());
        textBrowser->append(ParseAnsiColors(buf));

        buf = QString("$c0005Alla stanza: $c0015%1").arg(rp.exits().at(i).toRoom());
        textBrowser->append(ParseAnsiColors(buf));

        if (rp.exits().at(i).openCommand() > 0) {
            buf = QString("$c0005OpenCommand: $c0015%1").arg(rp.exits().at(i).openCommand());
            textBrowser->append(ParseAnsiColors(buf));
        }
    }
    textBrowser->append("");
    return;
}

void WndTester::closeEvent(QCloseEvent* e)
{
    e->accept();
    KreatorSettings::instance().saveGuiStatus("TesterWindow", this);
}

void WndTester::editZone(const Zone &zone, VNumber line) {
    WndZone* pWnd = new WndZone(*m_area_data, zone, this);
    pWnd->somethingChanged();
    if (parentwnd) {
        connect(pWnd, SIGNAL(dataSaved()), parentwnd, SLOT(refreshView()));
        connect(pWnd, SIGNAL(dataSaved()), parentwnd, SLOT(somethingChanged()));
        connect(pWnd, SIGNAL(dataSaved()), this, SLOT(roomCreated()));
    }
    pWnd->setModal(true);
    pWnd->selectItemId(line);
    pWnd->refreshView();
    pWnd->show();
}

void WndTester::initMob(const Mob* mob, VNumber room)
{
#if defined( KREATOR_DEBUG )
    qDebug("WndTester::initMob(Mob) called.");
#endif
    for (int i = 0; i < m_area_data->zones().size(); i++) {
        ZoneCommandList zcl = m_area_data->zones().at(i).commandList();
        ZoneCommandList::const_iterator it = zcl.begin();
        while (it != zcl.end()) {
            qWarning(qPrintable(it->toString(false)));
            if (it->isMobLoad() && it->argument(ZoneCommand::Argument3) == room && it->argument(ZoneCommand::Argument1) == mob->vnumber()) {
                editZone(m_area_data->zones().at(i), it->id());
                return;
            }
            it++;
        }
    }
    textBrowser->append(ParseAnsiColors("$c0009Il mob non ha init, devi prima generarlo nel *.zon."));
}

void WndTester::initObj(const Item* mob, VNumber room)
{
#if defined( KREATOR_DEBUG )
    qDebug("WndTester::initObj(Mob) called.");
#endif
    for (int i = 0; i < m_area_data->zones().size(); i++) {
        ZoneCommandList zcl = m_area_data->zones().at(i).commandList();
        ZoneCommandList::const_iterator it = zcl.begin();
        while (it != zcl.end()) {
            qWarning(qPrintable(it->toString(false)));
            if (it->isItemLoad() && it->argument(ZoneCommand::Argument3) == room && it->argument(ZoneCommand::Argument1) == mob->vnumber()) {
                editZone(m_area_data->zones().at(i), it->id());
                return;
            }
            it++;
        }
    }
    textBrowser->append(ParseAnsiColors("$c0009Oggetto non ha init, devi prima generarlo nel *.zon."));
}