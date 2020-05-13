#ifndef WNDMAP_H
#define WNDMAP_H

#include <QMap>
#include <QGLWidget>
#include <QCloseEvent>
#include <QMessageBox>

#include "room.h"
#include "area.h"
#include "wndarea.h"

namespace ts
{
    
    typedef enum dirType {
        None = 0,
        Normal = 1,
        OneWay = 2
    } dirType;

    struct glCoords {
        QRect rect = QRect();
        VNumber vnum = 0;
        float aX,aY,bX,bY,Z = 0.0f;
        QString title = "";
        QString subtitle = "";
        QString text = "";
        dirType bNorth = None;
        QRect north = QRect();
        dirType bEast = None;
        QRect east = QRect();
        dirType bSouth = None;
        QRect south = QRect();
        dirType bWest = None;
        QRect west = QRect();
        dirType bUp = None;
        dirType bDown = None;
        bool bSelected = false;
    };

#ifndef D3DVECTOR_DEFINED
    typedef struct _D3DVECTOR {
        float x;
        float y;
        float z;
    } D3DVECTOR;
#define D3DVECTOR_DEFINED
#endif

    class GlMap : public QGLWidget
    {
        Q_OBJECT
    public:
    signals:
        void doubleClicked(VNumber vnum);

        public:
            void setRooms(QList<const Room*> rooms) {
                m_rooms = rooms;
                roomMap.clear();
                for (auto& rm : rooms)
                {
                    roomMap[rm->vnumber()] = rm;
                }
                ResetObjects();
            }
            void ResetObjects();
            void setPos(GLfloat x, GLfloat y, GLfloat z) {
                offsetX = x;
                offsetY = y;
                bool changed = offsetZ != z;
                offsetZ = z;
                if (changed) {
                    ResetObjects();
                }
            };
            bool anySelected() {
                for (auto &r : objMap)
                {
                    if (r.bSelected) return true;
                }
                return false;
            }
            bool moveSelected(GLfloat x, GLfloat y, GLfloat z);
            void getPos(GLfloat &x, GLfloat &y, GLfloat &z) {
                x = offsetX;
                y = offsetY;
                z = offsetZ;
            };
            void setRot(GLfloat x, GLfloat y, GLfloat z) {
                xrot = x;
                yrot = y;
                zrot = z;
            };
            void setZoom(GLfloat x) {
                zoomRatio = x;
            };
            GLfloat getZoom() {
                return zoomRatio;
            };
            explicit GlMap(QList<const Room*> rooms, QWidget *parent = 0);
            ~GlMap();
        protected:
            void paintEvent(QPaintEvent* event);
            void paintGL();
            void initializeGL();
            void resizeGL(int w, int h);
            GLuint CreateRoomViewData();
            bool RoomLower(const Room* r1, const Room* r2);
            inline void GlMap::transformPoint(GLdouble out[4], const GLdouble m[16], const GLdouble in[4]);
            inline GLint GlMap::project(GLfloat objx, GLfloat objy, GLfloat objz,
                const GLdouble model[16], const GLdouble proj[16],
                const GLint viewport[4],
                GLdouble* winx, GLdouble* winy, GLdouble* winz);
            void GlMap::renderText(QPainter *painter, D3DVECTOR& textPosWorld, glCoords &obj);
            bool GlMap::renderRoom(QPainter* painter, glCoords& gc);
            bool GlMap::renderRoomLinks(QPainter* painter, glCoords& gc, QPen normal, QPen oneWay);
            void DrawLink(int dir, QRect& var, GLdouble  model[4][4], GLdouble  proj[4][4], GLint  view[4], GLdouble& textPosX, GLdouble& textPosY, GLdouble& textPosZ, GLdouble& textPosX2, GLdouble& textPosY2, GLdouble& textPosZ2, int height, QPainter* painter);
            void transformScreenToModel(GLdouble winx, GLdouble winy, GLdouble winz, GLdouble& X, GLdouble& Y, GLdouble& Z);
            virtual void mousePressEvent(QMouseEvent*);
            virtual void mouseReleaseEvent(QMouseEvent*);
            virtual void mouseMoveEvent(QMouseEvent*);
            void GlMap::drawTextAtWorld(GLdouble x, GLdouble y, GLdouble z, QPainter* painter,
                GLdouble model[4][4], GLdouble proj[4][4],
                GLint viewport[4], QString text);
            void GlMap::drawRectAtWorld(QPainter* painter,
                GLdouble model[4][4], GLdouble proj[4][4],
                GLint viewport[4], QRect r, QColor color, float expandPercent, bool border);
            void GlMap::drawLineAtWorld(QPainter* painter,
                GLdouble model[4][4], GLdouble proj[4][4],
                GLint viewport[4], QPoint from, QPoint to, QColor color);
    protected:
            void mouseDoubleClickEvent(QMouseEvent*) {
                if (hovering) {
                    emit doubleClicked(hovering->vnum);
                }
            }
    private:
            GLfloat      xrot;                               // X Rotation ( NEW )
            GLfloat     yrot;                               // Y Rotation ( NEW )
            GLfloat     zrot;                               // Z Rotation ( NEW )
            GLfloat xspeed;                                 // X Rotation Speed
            GLfloat yspeed;                                 // Y Rotation Speed
            GLuint      texture[3];                         // Storage For One Texture ( NEW )
            GLuint filter;
            GLuint object;
            QMap<VNumber, glCoords> objMap;
            QMap<VNumber, const Room*> roomMap;
            QList<const Room*> m_rooms;
            GLfloat offsetY = 0;
            GLfloat offsetX = 0;
            GLfloat offsetZ = 0;
            GLfloat zoomRatio = 1;
            QFont font = QFont("Arial", 18);
            QFont font2 = QFont("Arial", 12);
            QFont font3 = QFont("Arial", 14);
            int mouseDownX, mouseDownY;
            int mouseMoveX, mouseMoveY;
            int bMouseDown = 0;
            const glCoords* hovering = NULL;
    };

    class WndMap : public QWidget
    {
        Q_OBJECT

        public:
            WndMap(Area *ar, WndArea* parent);
            void setRoomsChanged() { area->setRoomsChanged(); }
            WndArea* getWndArea() { return parent; }
            void Refresh() {
                QList<const Room*> rooms;
                CreateRooms(rooms);
                map->setRooms(rooms);
            }
        public slots:
            void doubleClicked(VNumber vnum);
            void somethingChanged();
    protected:
        void CreateRooms(QList<const Room*>& rooms);
        void closeEvent(QCloseEvent* e);
        virtual void wheelEvent(QWheelEvent*);
        virtual void keyPressEvent(QKeyEvent*);
        void mouseMoveEvent(QMouseEvent* event);


    private:
            GlMap *map = NULL;
            WndArea* parent = NULL;
            Area* area;
    };
}
#endif
