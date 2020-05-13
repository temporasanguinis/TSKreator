
#include "wndmap.h"
#include <QHBoxLayout>
#include <QCoreApplication>
#include <qalgorithms.h>
#include "kreatorsettings.h"
#include "exception.h"
#include "wndroom.h"
#include <map>

namespace ts
{
    GlMap::GlMap(QList<const Room*> rooms, QWidget* parent)
        : QGLWidget(parent)
    {
        setRooms(rooms);
        object = 0;
        xrot = 0;
        yrot = 0;
        zrot = 0;
        setAutoFillBackground(false);
        setMouseTracking(true);
    }
#define matsize 4
    void reverseMatrix(GLdouble A[matsize][matsize], GLdouble I[matsize][matsize]) {
        GLdouble temp;
        int i, j, k;

        for (i = 0; i < matsize; i++)									//automatically initialize the unit matrix, e.g.
            for (j = 0; j < matsize; j++)								//	-       -
                if (i == j)										// | 1  0  0 |
                    I[i][j] = 1;									// | 0  1  0 |
                else											// | 0  0  1 |
                    I[i][j] = 0;									//  -       -

        for (k = 0; k < matsize; k++)									//by some row operations,and the same  row operations of
        {														//Unit mat. I gives the inverse of matrix A
            temp = A[k][k];										//'temp' stores the A[k][k] value so that  A[k][k] will not change
            for (j = 0; j < matsize; j++)								//during the operation A[i][j]/=A[k][k] when i = j = k
            {
                A[k][j] /= temp;									//it performs the following row operations to make A to unit matrix
                I[k][j] /= temp;									//R0=R0/A[0][0],similarly for I also R0 = R0 / A[0][0]
            }													//R1=R1-R0*A[1][0] similarly for I
            for (i = 0; i < matsize; i++)								//R2=R2-R0*A[2][0]		,,
            {
                temp = A[i][k];									//R1=R1/A[1][1]
                for (j = 0; j < matsize; j++)							//R0=R0-R1*A[0][1]
                {												//R2=R2-R1*A[2][1]
                    if (i == k)
                        break;									//R2=R2/A[2][2]
                    A[i][j] -= A[k][j] * temp;						//R0=R0-R2*A[0][2]
                    I[i][j] -= I[k][j] * temp;						//R1=R1-R2*A[1][2]
                }
            }
        }

        //printf("The inverse of the matrix is:");				//Print the matrix I that now contains the inverse of mat.A
        //    for (i = 0; i < matsize; i++)
        //    {
        //        for (j = 0; j < matsize; j++)
        //            printf("%f	", I[i][j]);
        //        printf("");
        //    }
    }

    inline void GlMap::transformPoint(GLdouble out[4], const GLdouble m[16], const GLdouble in[4])
    {
#define M(row,col)  m[col*4+row]
        out[0] =
            M(0, 0) * in[0] + M(0, 1) * in[1] + M(0, 2) * in[2] + M(0, 3) * in[3];
        out[1] =
            M(1, 0) * in[0] + M(1, 1) * in[1] + M(1, 2) * in[2] + M(1, 3) * in[3];
        out[2] =
            M(2, 0) * in[0] + M(2, 1) * in[1] + M(2, 2) * in[2] + M(2, 3) * in[3];
        out[3] =
            M(3, 0) * in[0] + M(3, 1) * in[1] + M(3, 2) * in[2] + M(3, 3) * in[3];
#undef M
    }

    inline GLint GlMap::project(GLfloat objx, GLfloat objy, GLfloat objz,
        const GLdouble model[16], const GLdouble proj[16],
        const GLint viewport[4],
        GLdouble* winx, GLdouble* winy, GLdouble* winz)
    {

        //reverseMatrix((GLdouble(*)[matsize])model);
        //reverseMatrix((GLdouble(*)[matsize])proj);
        GLdouble in[4], out[4];

        in[0] = objx;
        in[1] = objy;
        in[2] = objz;
        in[3] = 1.0;

        transformPoint(out, model, in);
        transformPoint(in, proj, out);

        if (in[3] == 0.0)
            return GL_FALSE;

        in[0] /= in[3];
        in[1] /= in[3];
        in[2] /= in[3];

        *winx = viewport[0] + (1 + in[0]) * viewport[2] / 2;
        *winy = viewport[1] + (1 + in[1]) * viewport[3] / 2;

        *winz = (1 + in[2]) / 2;
        return GL_TRUE;
    }

    void GlMap::transformScreenToModel(GLdouble winx, GLdouble winy, GLdouble winz, GLdouble& X, GLdouble& Y, GLdouble& Z) {
        GLdouble iModel[4][4];
        GLdouble iProj[4][4];
        GLdouble model[4][4], proj[4][4];
        GLint view[4];

        glGetDoublev(GL_MODELVIEW_MATRIX, &model[0][0]);
        glGetDoublev(GL_PROJECTION_MATRIX, &proj[0][0]);
        glGetIntegerv(GL_VIEWPORT, &view[0]);

        winy = view[3] - winy;
        X = ((winx - view[0]) / (view[2] / 2)) - 1;
        Y = ((winy - view[1]) / (view[3] / 2)) - 1;
        Z = (winz * 2) - 1;

        reverseMatrix(model, iModel);
        reverseMatrix(proj, iProj);

        GLdouble in[4] = { X, Y, Z, 1 };
        GLdouble out[4] = { 0, 0, 0, 0 };
        transformPoint(out, &iProj[0][0], in);
        transformPoint(in, &iModel[0][0], out);

        X = in[0];
        Y = in[1];
        Z = in[2];
        GLdouble mul = in[3];
        X *= mul;
        Y *= mul;
        Z *= mul;
    }

    bool GlMap::renderRoom(QPainter* painter, glCoords& var) {
        painter->setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing);
        D3DVECTOR v;
        v.x = var.aX + (var.bX - var.aX) / 2;
        v.y = var.bY - (var.bY - var.aY) / 2;
        v.z = 0;
        renderText(painter, v, var);
        return true;
    }

    bool GlMap::renderRoomLinks(QPainter* painter, glCoords& var, QPen normal, QPen oneWay) {
        painter->setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing);

        int width = this->width();
        int height = this->height();

        GLdouble model[4][4], proj[4][4];
        GLint view[4];
        glGetDoublev(GL_MODELVIEW_MATRIX, &model[0][0]);
        glGetDoublev(GL_PROJECTION_MATRIX, &proj[0][0]);
        glGetIntegerv(GL_VIEWPORT, &view[0]);


        GLdouble textPosX = 0, textPosY = 0, textPosZ = 0;
        GLdouble textPosX2 = 0, textPosY2 = 0, textPosZ2 = 0;
        if (var.bNorth) {
            if (var.bNorth == OneWay) {
                painter->setPen(oneWay);
            }
            else {
                painter->setPen(normal);
            }
            DrawLink(0, var.north, model, proj, view, textPosX, textPosY, textPosZ, textPosX2, textPosY2, textPosZ2, height, painter);
        }
        if (var.bEast) {
            if (var.bEast == OneWay) {
                painter->setPen(oneWay);
            }
            else {
                painter->setPen(normal);
            }
            DrawLink(1, var.east, model, proj, view, textPosX, textPosY, textPosZ, textPosX2, textPosY2, textPosZ2, height, painter);
        }
        if (var.bSouth) {
            if (var.bSouth == OneWay) {
                painter->setPen(oneWay);
            }
            else {
                painter->setPen(normal);
            }
            DrawLink(2, var.south, model, proj, view, textPosX, textPosY, textPosZ, textPosX2, textPosY2, textPosZ2, height, painter);
        }
        if (var.bWest) {
            if (var.bWest == OneWay) {
                painter->setPen(oneWay);
            }
            else {
                painter->setPen(normal);
            }
            DrawLink(3, var.west, model, proj, view, textPosX, textPosY, textPosZ, textPosX2, textPosY2, textPosZ2, height, painter);
        }
        if (var.bUp && var.Z == offsetZ) {
            QPainterPath path;
            QPoint pts[4];
            project(var.aX+2, (GLdouble)var.bY - 12, 0,
                &model[0][0], &proj[0][0], &view[0],
                &textPosX, &textPosY, &textPosZ);
            textPosY = height - textPosY;
            pts[0] = QPoint(textPosX, textPosY);

            pts[3] = QPoint(textPosX, textPosY);
            project((GLdouble)var.aX + 12, (GLdouble)var.bY - 12 + 10, 0,
                &model[0][0], &proj[0][0], &view[0],
                &textPosX, &textPosY, &textPosZ);
            textPosY = height - textPosY;
            pts[1] = QPoint(textPosX, textPosY);
            project((GLdouble)var.aX + 22, (GLdouble)var.bY - 12 + 0, 0,
                &model[0][0], &proj[0][0], &view[0],
                &textPosX, &textPosY, &textPosZ);
            textPosY = height - textPosY;
            pts[2] = QPoint(textPosX, textPosY);
            path.moveTo(pts[0]);
            path.lineTo(pts[1]);
            path.lineTo(pts[2]);
            path.lineTo(pts[3]);
            painter->fillPath(path, QBrush(normal.color()));
        }
        if (var.bDown && var.Z == offsetZ) {
            QPainterPath path;
            QPoint pts[4];
            project(var.aX+2, (GLdouble)var.bY - 35, 0,
                &model[0][0], &proj[0][0], &view[0],
                &textPosX, &textPosY, &textPosZ);
            textPosY = height - textPosY;
            pts[0] = QPoint(textPosX, textPosY);
            pts[3] = QPoint(textPosX, textPosY);
            project((GLdouble)var.aX + 12, (GLdouble)var.bY - 35 - 10, 0,
                &model[0][0], &proj[0][0], &view[0],
                &textPosX, &textPosY, &textPosZ);
            textPosY = height - textPosY;
            pts[1] = QPoint(textPosX, textPosY);
            project((GLdouble)var.aX + 22, (GLdouble)var.bY - 35 + 0, 0,
                &model[0][0], &proj[0][0], &view[0],
                &textPosX, &textPosY, &textPosZ);
            textPosY = height - textPosY;
            pts[2] = QPoint(textPosX, textPosY);
            path.moveTo(pts[0]);
            path.lineTo(pts[1]);
            path.lineTo(pts[2]);
            path.lineTo(pts[3]);
            painter->fillPath(path, QBrush(normal.color()));
        }
        return true;
    }

    void GlMap::drawLineAtWorld(QPainter* painter,
        GLdouble model[4][4], GLdouble proj[4][4],
        GLint viewport[4], QPoint from, QPoint to, QColor color) {

        GLdouble textPosX1 = 0, textPosY1 = 0, textPosZ1 = 0;
        GLdouble textPosX2 = 0, textPosY2 = 0, textPosZ2 = 0;
        project(from.x(), from.y(), 1,
            &model[0][0], &proj[0][0], &viewport[0],
            &textPosX1, &textPosY1, &textPosZ1);
        textPosY1 = this->height() - textPosY1; // y is inverted

        project(to.x(), to.y(), 1,
            &model[0][0], &proj[0][0], &viewport[0],
            &textPosX2, &textPosY2, &textPosZ2);
        textPosY2 = this->height() - textPosY2; // y is inverted

        /*QColor trc(0xFF, 0, 0, 0x80);
        QPen transRed(trc);
        painter->setPen(transRed);
        QBrush rb(trc);
        painter->setBrush(rb);*/
        //painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter->drawLine(textPosX1, textPosY1, textPosX2, textPosY2);
    }

    void GlMap::DrawLink(int dir, QRect& var, GLdouble  model[4][4], GLdouble  proj[4][4], GLint  view[4], GLdouble& textPosX, GLdouble& textPosY, GLdouble& textPosZ, GLdouble& textPosX2, GLdouble& textPosY2, GLdouble& textPosZ2, int height, QPainter* painter)
    {
        /*project(var.topLeft().x(), var.topLeft().y(), 0,
            &model[0][0], &proj[0][0], &view[0],
            &textPosX, &textPosY, &textPosZ);
        project(var.bottomRight().x(), var.bottomRight().y(), 0,
            &model[0][0], &proj[0][0], &view[0],
            &textPosX2, &textPosY2, &textPosZ2);
        textPosY = height - textPosY;
        textPosY2 = height - textPosY2;
        painter->drawLine(textPosX, textPosY, textPosX2, textPosY2);*/
        QPoint current = var.topLeft();
        QPoint next;
        switch (dir)
        {
        case 0:
            next = current;
            next.setY(next.y() + 10);
            break;
        case 1:
            next = current;
            next.setX(next.x() + 10);
            break;
        case 2:
            next = current;
            next.setY(next.y() - 10);
            break;
        case 3:
            next = current;
            next.setX(next.x() - 10);
            break;
        default:
            return;
        }

        drawLineAtWorld(painter, model, proj, view,
            current,
            next, Qt::blue);
        drawLineAtWorld(painter, model, proj, view,
            next,
            var.bottomRight(), Qt::blue);

        if (var.topLeft() == var.bottomRight()) {
            double x, y, z;
            project(next.x(), next.y(), 1,
                &model[0][0], &proj[0][0], &view[0],
                &x, &y, &z);
            y = this->height() - y;
            painter->drawEllipse(QPoint(x, y), (int)ceil(7*zoomRatio), (int)ceil(7*zoomRatio));
        }
    }

    void GlMap::drawTextAtWorld(GLdouble x, GLdouble y, GLdouble z, QPainter* painter,
        GLdouble model[4][4], GLdouble proj[4][4],
        GLint viewport[4], QString text) {
        GLdouble textPosX = 0, textPosY = 0, textPosZ = 0;
        project(x, y, z,
            &model[0][0], &proj[0][0], &viewport[0],
            &textPosX, &textPosY, &textPosZ);
        textPosY = this->height() - textPosY; // y is inverted
        painter->drawText(textPosX, textPosY, text);
    }

    void GlMap::drawRectAtWorld(QPainter* painter,
        GLdouble model[4][4], GLdouble proj[4][4],
        GLint viewport[4], QRect r, QColor color, float expandPercent, bool border) {
        GLdouble textPosX1 = 0, textPosY1 = 0, textPosZ1 = 0;
        GLdouble textPosX2 = 0, textPosY2 = 0, textPosZ2 = 0;
        r.adjust(r.width()*-expandPercent, r.height() *-expandPercent, r.width() * expandPercent, r.height() * expandPercent);
        project(r.x(), r.y(), 1,
            &model[0][0], &proj[0][0], &viewport[0],
            &textPosX1, &textPosY1, &textPosZ1);
        textPosY1 = this->height() - textPosY1; // y is inverted

        project(r.right(), r.bottom(), 1,
            &model[0][0], &proj[0][0], &viewport[0],
            &textPosX2, &textPosY2, &textPosZ2);
        textPosY2 = this->height() - textPosY2; // y is inverted

        painter->fillRect(QRect(textPosX1, textPosY1, textPosX2 - textPosX1, textPosY2 - textPosY1), color);
        if (border) painter->drawRect(QRect(textPosX1, textPosY1, textPosX2 - textPosX1, textPosY2 - textPosY1));
    }

    void GlMap::renderText(QPainter* painter, D3DVECTOR& textPosWorld, glCoords &obj)
    {
        GLdouble model[4][4], proj[4][4];
        GLint view[4];
        glGetDoublev(GL_MODELVIEW_MATRIX, &model[0][0]);
        glGetDoublev(GL_PROJECTION_MATRIX, &proj[0][0]);
        glGetIntegerv(GL_VIEWPORT, &view[0]);
    
        painter->setPen(Qt::black);
        if (obj.bSelected) {
            painter->setPen(Qt::red);
            QRect r(textPosWorld.x - 25.0f, textPosWorld.y - 25.0f, 50, 50);
            drawRectAtWorld(painter, model, proj, view, r, Qt::yellow, 0.15f, true);
        }

        painter->setPen(Qt::lightGray);
        painter->setFont(font2);
        drawTextAtWorld((GLdouble)textPosWorld.x - 0.0f,
            (GLdouble)textPosWorld.y+10,
            textPosWorld.z, painter, model, proj, view,
            obj.title);

        QColor color = QColor(255, 255, 255, 127);
        painter->setPen(Qt::black);
        if (obj.bSelected) {
            painter->setPen(Qt::red);
        }

        painter->setFont(font);
        drawTextAtWorld((GLdouble)textPosWorld.x - 0.0f,
            (GLdouble)textPosWorld.y-10,
            textPosWorld.z, painter, model, proj, view,
            obj.subtitle);

        if (false /*hovering == &obj*/) {
            painter->setFont(font2);
            painter->setPen(Qt::black);
            QFontMetrics fm(font2);
            int pixelsWide = fm.width(obj.text);
            int pixelsHigh = fm.height();
            QRect r(textPosWorld.x - 40.0f, textPosWorld.y - 50.0f, pixelsWide, pixelsHigh);
            drawRectAtWorld(painter, model, proj, view, r, Qt::white, 0.10f, true);
            drawTextAtWorld((GLdouble)textPosWorld.x - 40.0f,
                (GLdouble)textPosWorld.y - 47,
                textPosWorld.z, painter, model, proj, view,
                obj.text);

        }
    }

    GlMap::~GlMap()
    {
    }

    void GlMap::ResetObjects() {
        hovering = NULL;
        std::map<VNumber, bool> selVnums;
        for (auto& r : objMap)
        {
            if (r.bSelected) {
                selVnums[r.vnum] = true;
            }
        }
        objMap.clear();
        object = CreateRoomViewData();
        for (auto& r : objMap)
        {
            if (selVnums[r.vnum]) {
                r.bSelected = true;
            }
        }
        repaint();
    }
#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif
    void GlMap::initializeGL()
    {
        //texture[0] = bindTexture(QPixmap("bookmark.png"), GL_TEXTURE_2D);
        //glShadeModel(GL_SMOOTH);
        //glEnable(GL_MULTISAMPLE);
        glClearColor(0.0, 0.0, 0.0, 0.5);
        glClearDepth(1.0);
        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_TEXTURE_2D);                        // Enable Texture Mapping ( NEW )
        glDisable(GL_TEXTURE_2D);
        //glDepthFunc(GL_LEQUAL);
        //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ResetObjects();
    }

    void WndMap::closeEvent(QCloseEvent* e)
    {
        e->accept();
        KreatorSettings::instance().saveGuiStatus("MapWindow", this);
    }

    void GlMap::paintGL()
    {

    }

    void GlMap::paintEvent(QPaintEvent* event) {
        makeCurrent();
        GLclampf red = 194.0f / 255.0f;
        GLclampf green = 212.0f / 255.0f;
        GLclampf blue = 230.0f / 255.0f;
        glClearColor(red, green, blue, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear The Screen And The Depth Buffer
        glLoadIdentity();                       // Reset The View
        glScalef(zoomRatio, zoomRatio, 1.0);
        glTranslatef(offsetX * 100 / zoomRatio, offsetY * 100 / zoomRatio, 0.0);
        glCallList(object);
        font.setPointSize(14 * zoomRatio);
        font2.setPointSize(10 * zoomRatio);
        QPainter painter(this);
        int any = 0;
        for (auto& var : objMap)
        {
            QPen normal = QPen(Qt::blue, 3);
            QPen oneWay = QPen(Qt::lightGray, 2);
            if (var.Z != this->offsetZ) {
                double diffMul = 125;
                for (size_t i = 0; i < abs(var.Z+offsetZ); i++)
                {
                    diffMul -= diffMul / 2;
                }
                normal = QPen(QColor(0,0,255, 10), 2);
                oneWay = QPen(QColor(127, 127, 127, 10), 1);
            }
            renderRoomLinks(&painter, var, normal, oneWay);
        }
        for (auto& var : objMap)
        {
            if (var.Z != this->offsetZ) continue;
            renderRoom(&painter, var);
            any++;
        }

        painter.setFont(font3);
        painter.setPen(Qt::black);
        painter.fillRect(QRect(10, 10, 105, 30), QColor(255, 255, 255, 127));
        painter.drawText(15, 30, QString("Livello: %1").arg(QString::number(offsetZ)));
        painter.fillRect(QRect(10, 10 + 1 * 35, 120, 30), QColor(255, 255, 255, 127));
        painter.drawText(15, 30 + 1 * 35, QString("Rooms: %1").arg(QString::number(any)));
        painter.fillRect(QRect(10, 10 + 2 * 35, 150, 30), QColor(255, 255, 255, 127));
        painter.drawText(15, 30 + 2 * 35, QString("Offset: %1, %2").arg(QString::number((int)offsetX)).arg(QString::number((int)offsetY)));
        GLdouble x, y, z;
        transformScreenToModel(mouseMoveX, mouseMoveY, 0, x, y, z);
        painter.fillRect(QRect(10, 10 + 3 * 35, 150, 30), QColor(255, 255, 255, 127));
        x = ceil((x - 50) / 100);
        y = ceil((y - 50) / 100);
        painter.drawText(15, 30 + 3 * 35, QString("Coord: %1,%2").arg(QString::number(x)).arg(QString::number(y)));
        if (bMouseDown == Qt::LeftButton) {
            int x = min(mouseDownX, mouseMoveX);
            int y = min(mouseDownY, mouseMoveY);
            int x2 = max(mouseDownX, mouseMoveX);
            int y2 = max(mouseDownY, mouseMoveY);
            painter.fillRect(QRect(x, y, x2 - x, y2 - y), QColor(255, 255, 255, 127));
        }

        if (hovering) {
            painter.setFont(font3);
            painter.setPen(Qt::black);
            QFontMetrics fm(font3);
            int pixelsWide = fm.width(hovering->text);
            int pixelsHigh = fm.height();
            QRect r(mouseMoveX-3+20, mouseMoveY- pixelsHigh-3+40, pixelsWide+6, pixelsHigh+6);
            painter.fillRect(r, QColor(255, 255, 200, 255));
            painter.drawRect(r);
            painter.drawText(mouseMoveX+20, mouseMoveY-5+40, hovering->text);
        }
        painter.end();
    }

    void GlMap::resizeGL(int w, int h)
    {
        glViewport(0, 0, (GLint)w, (GLint)h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-w / 2, w / 2, -h / 2, h / 2, 0.0, 15.0);
        //glFrustum(-1000.0, 1000.0, -1000.0, 1000.0, 5.0, 15.0);
        glMatrixMode(GL_MODELVIEW);
        //glLoadIdentity();
    }

    bool GlMap::RoomLower(const Room* r1, const Room* r2) {
        return (r1)->getZ() < (r2)->getZ();
    }

    GLuint GlMap::CreateRoomViewData()
    {
        GLuint list = glGenLists(1);
        glNewList(list, GL_COMPILE);
        glTranslatef(offsetX + 0.0, offsetY + 0.0, -15.0);
        //glBindTexture(GL_TEXTURE_2D, texture[0]);
        /*QList<const Room*> rooms(m_rooms);
        qSort(rooms.begin(), rooms.end(), &GlMap::RoomLower);*/
        QList<const Room*> rooms(m_rooms);
        qSort(rooms.begin(), rooms.end(),
            [](const Room* a, const Room* b) -> bool { return a->getZ() < b->getZ(); });

        for (int i = 0; i < rooms.size(); i++) {
            struct glCoords gC;
            //gC.bUp = gC.bDown = gC.bNorth = gC.bEast = gC.bSouth = gC.bWest = 0;
            auto rm = rooms.at(i);
            if (!objMap.contains(rm->vnumber()) && (true || offsetZ == rm->getZ() || offsetZ-1 == rm->getZ())) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
                double diff = abs(rm->getZ() - (double)offsetZ);
                double alpha = 1.0;
                for (size_t i = 0; i < diff; i++)
                {
                    alpha /= 2;
                }
                if (offsetZ == rm->getZ())
                    glColor4f(1.0, 1.0, 1.0, 1);
                else
                {
                    if (rm->getZ() > offsetZ) {
                        glColor4f(1.0, .5, 0.0, alpha);
                    }
                    else {
                        glColor4f(0.0, .5, 1.0, alpha);
                    }
                }

                glBegin(GL_QUADS);
                double levelDelta = 10.0 * (rm->getZ() - (double)offsetZ);

                gC.aX = (rm->getX()) * 100.0 - 25 + levelDelta;
                gC.bX = (rm->getX()) * 100.0 + 25 + levelDelta;
                gC.aY = (rm->getY()) * 100.0 - 25 + levelDelta;
                gC.bY = (rm->getY()) * 100.0 + 25 + levelDelta;
                gC.Z = (rm->getZ());

                for (auto& re : rm->exits()) {
                    float addX1 = 0, addX2 = 0, addY1 = 0, addY2 = 0;
                    if (roomMap.contains(re.toRoom())) {
                        class QRect* rect = NULL;
                        auto r = roomMap[re.toRoom()];
                        const Exit* other = NULL;
                        bool loop = rm->vnumber() == re.toRoom();
                        for (auto& re2 : r->exits()) {
                            if (re2.directionReversed() == re.direction() && re2.toRoom() == rm->vnumber()) {
                                other = &re2;
                                break;
                            }
                        }
                        switch (re.direction())
                        {
                        case EXIT_DIRECTION_NORTH:
                            gC.bNorth = other ? Normal : OneWay;
                            addX1 = 0+levelDelta;
                            addY1 = 25 + levelDelta;
                            addX2 = 0 + levelDelta;
                            addY2 = -25 + levelDelta;
                            rect = &gC.north;
                            break;
                        case EXIT_DIRECTION_EAST:
                            gC.bEast = other ? Normal : OneWay;
                            addX1 = 25 + levelDelta;
                            addY1 = 0 + levelDelta;
                            addX2 = -25 + levelDelta;
                            addY2 = 0 + levelDelta;
                            rect = &gC.east;
                            break;
                        case EXIT_DIRECTION_WEST:
                            gC.bWest = other ? Normal : OneWay;
                            addX1 = -25 + levelDelta;
                            addY1 = 0 + levelDelta;
                            addX2 = 25 + levelDelta;
                            addY2 = 0 + levelDelta;
                            rect = &gC.west;
                            break;
                        case EXIT_DIRECTION_SOUTH:
                            gC.bSouth = other ? Normal : OneWay;
                            addX1 = 0 + levelDelta;
                            addY1 = -25 + levelDelta;
                            addX2 = 0 + levelDelta;
                            addY2 = 25 + levelDelta;
                            rect = &gC.south;
                            break;
                        case EXIT_DIRECTION_UP:
                            gC.bUp = other ? Normal : OneWay;
                            break;
                        case EXIT_DIRECTION_DOWN:
                            gC.bDown = other ? Normal : OneWay;
                            break;
                        default:
                            break;
                        }
                        
                        if (rect) {
                            if (other)
                                *rect = QRect(QPoint((rm->getX()) * 100.0 + addX1, (rm->getY()) * 100.0 + addY1), QPoint((r->getX()) * 100.0 + addX2, (r->getY()) * 100.0 + addY2));
                            else if (loop)
                                *rect = QRect(QPoint((rm->getX()) * 100.0 + addX1, (rm->getY()) * 100.0 + addY1), QPoint((rm->getX()) * 100.0 + addX1, (rm->getY()) * 100.0 + addY1));
                            else
                                *rect = QRect(QPoint((rm->getX()) * 100.0 + addX1, (rm->getY()) * 100.0 + addY1), QPoint((r->getX()) * 100.0, (r->getY()) * 100.0));
                        }
                    }

                }

                glTexCoord2f(0.0, 0.0); glVertex3f((GLfloat)gC.aX, (GLfloat)gC.aY, 0.2f);
                glTexCoord2f(1.0, 0.0); glVertex3f((GLfloat)gC.bX, (GLfloat)gC.aY, 0.2f);
                glTexCoord2f(1.0, 1.0); glVertex3f((GLfloat)gC.bX, (GLfloat)gC.bY, 0.2f);
                glTexCoord2f(0.0, 1.0); glVertex3f((GLfloat)gC.aX, (GLfloat)gC.bY, 0.2f);

                auto room = rooms.at(i);
                gC.title = QString::number(room->zone());
                gC.subtitle = QString::number(room->vnumber() - room->zone()*100);
                gC.text = room->name();
                gC.vnum = room->vnumber();
                glEnd();
                objMap[room->vnumber()] = gC;
            }
        }

        glEndList();
        return list;
    }

    void WndMap::somethingChanged() {
        QList<const Room*> rooms;
        CreateRooms(rooms);
        map->setRooms(rooms);
        map->ResetObjects();
    }

    void WndMap::doubleClicked(VNumber vnum) {
        auto ar = getWndArea();
        WndRoom* pWndRoom = new WndRoom(*area, area->room(vnum), this);
        connect(pWndRoom, SIGNAL(dataSaved()), ar, SLOT(refreshRoomsView()));
        connect(pWndRoom, SIGNAL(dataSaved()), ar, SLOT(somethingChanged()));
        connect(pWndRoom, SIGNAL(dataSaved()), this, SLOT(somethingChanged()));
        pWndRoom->setModal(true);
        pWndRoom->show();
    }

    void WndMap::CreateRooms(QList<const Room*>& rooms) {
        bool hasZeroZeroZero = false;
        for (auto& r : this->area->rooms())
        {
            if (this->area->hasZone(r.zone())) {
                auto z = this->area->zone(r.zone());
                if (z.hasNewFlag(19)) {
                    bool isZero = r.getX() == 0 && r.getY() == 0 && r.getZ() == 0;
                    if (!isZero || !hasZeroZeroZero) {
                        rooms.append(&r);
                    }
                    hasZeroZeroZero |= isZero;
                }
            }
        }
    }

    WndMap::WndMap(Area* ar, WndArea* parent)
    {
        this->area = ar;
        this->parent = parent;
        QList<const Room*> rooms;
        CreateRooms(rooms);
        map = new GlMap(rooms, parent);
        /*QObject::connect(map, &GlMap::doubleClicked,
            this, &WndMap::doubleClicked);*/
        if (!connect(map, SIGNAL(doubleClicked(VNumber)), this, SLOT(doubleClicked(VNumber)))) {
            throw Exception(ts::Exception::Runtime, "Cannot connect signal");
        }

        QHBoxLayout* lay = new QHBoxLayout(parent);
        lay->addWidget(map);
        setLayout(lay);
        KreatorSettings::instance().loadGuiStatus("MapWindow", this);
        map->setMouseTracking(true);
        setMouseTracking(true);
        //resize(parent->width(), parent->height());
        if (rooms.size()) {
            this->window()->setWindowTitle("Mappa - usa tastierino numerico per muoverti o Freccette + PgUp/PgDown, mouse scroll per zoom");
        }
        else {
            this->window()->setWindowTitle("Mappa - Nessuna room (La zona deve avere flag HasMap per poter disegnare le room)");
        }

    }

    void WndMap::wheelEvent(QWheelEvent* ev) {
        QWidget::wheelEvent(ev);
        if (ev->delta()) {
            float multi = ev->delta() > 0 ? 1.1 : 0.9;
            float zm = map->getZoom() * multi;
            if (zm > 0.1 && zm < 10) {
                map->setZoom(zm);
                map->repaint();
            }
        }
    }

    void GlMap::mouseMoveEvent(QMouseEvent* ev) {
        //QWidget::mouseMoveEvent(ev);
        mouseMoveX = ev->pos().x();
        mouseMoveY = ev->pos().y();
        auto lastHover = hovering;
        hovering = NULL;
        GLdouble x2, y2, z2;
        transformScreenToModel(mouseMoveX, mouseMoveY, 0, x2, y2, z2);
        QRect clickRect = QRect(x2, y2, 1, 1);
        for (auto& v : objMap)
        {
            QRect roomRect(v.aX, v.bY, v.bX - v.aX, v.aY - v.bY);
            if (clickRect.intersects(roomRect)) {
                hovering = &v;
                break;
            }
        }
    
        if (bMouseDown == Qt::RightButton) {
            GLdouble x = ev->pos().x();
            GLdouble y = ev->pos().y();
            x -= mouseDownX;
            y -= mouseDownY;
            GLdouble z = 1.0;
            GLdouble x2 = 0, y2 = 0, z2 = 0;
            float x1, y1, z1;
            getPos(x1, y1, z1);
            z = z1;

            transformScreenToModel(x, y, 0, x2, y2, z2);
            /*float x1, y1, z1;
            getPos(x1, y1, z1);*/
            /*GLdouble model[4][4], proj[4][4];
            GLint view[4];
            glGetDoublev(GL_MODELVIEW_MATRIX, &model[0][0]);
            glGetDoublev(GL_PROJECTION_MATRIX, &proj[0][0]);
            glGetIntegerv(GL_VIEWPORT, &view[0]);
            project(x, y, z, &model[0][0], &proj[0][0], &view[0], &x, &y, &z);*/
            x2 = ((x2 - 50) / 100);
            y2 = ((y2 - 50) / 100);
            GLdouble newX = (GLdouble)x1 + x2/100;
            GLdouble newY = (GLdouble)y1 + y2/100;
            GLdouble newX1 = (GLdouble)x1 - x2 / 100;
            GLdouble newY1 = (GLdouble)y1 - y2 / 100;
            //mouseDownX = ev->pos().x();
            //mouseDownY = ev->pos().y();
            setPos(newX, newY, z);
        }
        repaint();
    }

    void GlMap::mousePressEvent(QMouseEvent* ev) {
        QWidget::mousePressEvent(ev);
        bMouseDown = 0;
        mouseDownX = ev->pos().x();
        mouseDownY = ev->pos().y();
        if (ev->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier)) {
            bMouseDown = ev->button();
        }
        else {
            bMouseDown = Qt::RightButton;
        }
    }

    void GlMap::mouseReleaseEvent(QMouseEvent* ev) {
        QWidget::mouseReleaseEvent(ev);
        for (auto& v : objMap)
        {
            v.bSelected = false;
        }
        if ((bMouseDown==Qt::LeftButton || (mouseDownX== ev->pos().x() && mouseDownY == ev->pos().y())) && ev->button() == Qt::LeftButton) {
            GLdouble x = ev->pos().x();
            GLdouble y = ev->pos().y();
            GLdouble z = 1.0;
            GLdouble x2, y2, z2;
            int rX = min(mouseDownX, x + 1);
            int rY = min(mouseDownY, y + 1);
            int rX2 = max(mouseDownX, x + 1);
            int rY2 = max(mouseDownY, y + 1);
            QRect clickRect(rX, rY, rX2 - rX, rY2 - rY);
            transformScreenToModel(rX, rY, z, x2, y2, z2);
            rX = x2;
            rY = y2;
            transformScreenToModel(rX2, rY2, z, x2, y2, z2);
            rX2 = x2;
            rY2 = y2;
            clickRect = QRect(rX, rY, rX2 - rX, rY2 - rY);
            for (auto& v : objMap)
            {
                v.bSelected = false;
                QRect roomRect(v.aX, v.bY, v.bX - v.aX, v.aY - v.bY);
                if (clickRect.intersects(roomRect)) {
                    v.bSelected = true;
                }
            }
        }
        bMouseDown = 0;
        repaint();
    }

    void WndMap::keyReleaseEvent(QKeyEvent* ev) {
        QWidget::keyReleaseEvent(ev);
        if (ev->key()) {
            int key = ev->key();
            QString sKey = QKeySequence(key).toString();
            float x, y, z;
            map->getPos(x, y, z);
            if (sKey == "PgUp" || sKey == "9") {
                if (map->anySelected()) {
                    if (map->moveSelected(0, 0, +1)) {
                        map->setPos(x, y, z + 1);
                    }
                }
                else {
                    map->setPos(x, y, z + 1);
                }
            }
            else if (sKey == "PgDown" || sKey == "3") {
                if (map->anySelected()) {
                    if (map->moveSelected(0, 0, -1)) {
                        map->setPos(x, y, z - 1);
                    }
                }
                else {
                    map->setPos(x, y, z - 1);
                }
            }
            else if (sKey == "Left" || sKey == "4") {
                if (map->anySelected()) {
                    map->moveSelected(-1, 0, 0);
                }
                else {
                    map->setPos(x + 0.1, y, z);
                }
            }
            else if (sKey == "Right" || sKey == "6") {
                if (map->anySelected()) {
                    map->moveSelected(1, 0, 0);
                }
                else {
                    map->setPos(x - 0.1, y, z);
                }
            }
            else if (sKey == "Up" || sKey == "8") {
                if (map->anySelected()) {
                    map->moveSelected(0, 1, 0);
                }
                else {
                    map->setPos(x, y - 0.1, z);
                }
            }
            else if (sKey == "Down" || sKey == "2") {
                if (map->anySelected()) {
                    map->moveSelected(0, -1, 0);
                }
                else {
                    map->setPos(x, y + 0.1, z);
                }
            }
            else if (sKey == "Clear" || sKey == "5") {
                map->setPos(0, 0, 0);
                map->setZoom(1.0);
            }
            else if (sKey == "-") {
                float zm = map->getZoom() * 0.9;
                if (zm > 0.1 && zm < 10) {
                    map->setZoom(zm);
                }
            }
            else if (sKey == "+") {
                float zm = map->getZoom() * 1.1;
                if (zm > 0.1 && zm < 10) {
                    map->setZoom(zm);
                }
            }
            map->repaint();
        }
    }

    void WndMap::mouseMoveEvent(QMouseEvent* event)
    {
        return;
        QWidget* child = childAt(event->pos());
        QGLWidget* glwidget = qobject_cast<QGLWidget*>(child);
        if (glwidget)
        {
            QMouseEvent* glevent = new QMouseEvent(event->type(), glwidget->mapFromGlobal(event->globalPos()), event->button(), event->buttons(), event->modifiers());
            QCoreApplication::postEvent(glwidget, glevent);
        }
    }
}