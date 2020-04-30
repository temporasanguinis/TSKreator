
#include "wndmap.h"
#include <QHBoxLayout>
#include "kreatorsettings.h"

namespace ts
{
GlMap::GlMap(QList<Room> rooms, QWidget *parent)
    : QGLWidget(parent)
{
    m_rooms = rooms;
    object = 0;
    xrot = 3;
    yrot = 0;
    zrot = 0;
}

GlMap::~GlMap()
{
}

void GlMap::initializeGL()
{
    texture[0] = bindTexture(QPixmap("bookmark.png"), GL_TEXTURE_2D);
    object = makeobject();
    glShadeModel( GL_SMOOTH );
    glClearColor( 0.0, 0.0, 0.0, 0.5 );
    glClearDepth( 1.0 );
    glEnable( GL_DEPTH_TEST );
    glEnable(GL_TEXTURE_2D);                        // Enable Texture Mapping ( NEW )
    glDepthFunc( GL_LEQUAL );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );


}

void WndMap::closeEvent(QCloseEvent* e)
{
    e->accept();
    KreatorSettings::instance().saveGuiStatus("MapWindow", this);
}

void GlMap::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear The Screen And The Depth Buffer
    glLoadIdentity();                       // Reset The View
    glCallList(object);
}

void GlMap::resizeGL(int w, int h)
{
    glViewport( 0, 0, (GLint)w, (GLint)h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, w, 0, h, 0.0, 15.0 );
    //glFrustum(-1000.0, 1000.0, -1000.0, 1000.0, 5.0, 15.0);
    glMatrixMode( GL_MODELVIEW );
    //glLoadIdentity();
}

GLuint GlMap::makeobject()
{
    GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);

    glTranslatef(  0.0,  0.0, -15.0 );
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    for(int i = 0; i < m_rooms.size(); i++) {
        struct glCoords gC;
        auto rm = m_rooms.at(i);
        if(!objMap.contains(rm.vnumber()) &&
            rm.getX() && rm.getY()) {
            glBegin(GL_QUADS);
            gC.aX = (rm.getX()) * 100.0 - 25;
            gC.bX = (rm.getX()) * 100.0 + 25;
            gC.aY = (rm.getY()) * 100.0 - 25;
            gC.bY = (rm.getY()) * 100.0 + 25;
            
            glTexCoord2f( 0.0, 0.0 ); glVertex3f(gC.aX, gC.aY,  0.2 );
            glTexCoord2f( 1.0, 0.0 ); glVertex3f(gC.bX, gC.aY,  0.2 );
            glTexCoord2f( 1.0, 1.0 ); glVertex3f(gC.bX, gC.bY,  0.2 );
            glTexCoord2f( 0.0, 1.0 ); glVertex3f(gC.aX, gC.bY,  0.2 );
            /*
            glTexCoord2f(0.0, 0.0); glVertex3f(-110.2, -110.2, 0.2);
            glTexCoord2f(1.0, 0.0); glVertex3f(110.2, -110.2, 0.2);
            glTexCoord2f(1.0, 1.0); glVertex3f(110.2, 110.2, 0.2);
            glTexCoord2f(0.0, 1.0); glVertex3f(-110.2, 110.2, 0.2);
            */
            glEnd();
            objMap[m_rooms.at(i).vnumber()] = gC;
        }

//         for(int k = 0; k < m_rooms.at(i).exits() ; k++) {
//
//         }
//         glBindTexture(GL_TEXTURE_2D, texture[0]);
//         glBegin(GL_QUADS);
//         glTexCoord2f( 0.0, 0.0 ); glVertex3f( -0.2, -0.2,  0.2 );
//         glTexCoord2f( 1.0, 0.0 ); glVertex3f(  0.2, -0.2,  0.2 );
//         glTexCoord2f( 1.0, 1.0 ); glVertex3f(  0.2,  0.2,  0.2 );
//         glTexCoord2f( 0.0, 1.0 ); glVertex3f( -0.2,  0.2,  0.2 );
//         glEnd();
    }

//     qglColor(Qt::white);
//     glBindTexture(GL_TEXTURE_2D, texture[0]);               // Select Our Texture
//
//     glBegin(GL_QUADS);
//         // Front Face
//     glTexCoord2f( 0.0, 0.0 ); glVertex3f( -0.2, -0.2,  0.2 );
//     glTexCoord2f( 1.0, 0.0 ); glVertex3f(  0.2, -0.2,  0.2 );
//     glTexCoord2f( 1.0, 1.0 ); glVertex3f(  0.2,  0.2,  0.2 );
//     glTexCoord2f( 0.0, 1.0 ); glVertex3f( -0.2,  0.2,  0.2 );
//
//     glEnd();
//
//     glBindTexture(GL_TEXTURE_2D, texture[1]);               // Select Our Texture
//
//     glBegin(GL_LINES);
//     glVertex3f(0.2f, 0.0f, 0.2f); // origin of the line
//     glVertex3f(0.4f, 0.0f, 0.2f); // ending point of the line
//
//     glEnd();
//
//     glBindTexture(GL_TEXTURE_2D, texture[0]);               // Select Our Texture
//
//     glBegin(GL_QUADS);
//         // Front Face
//     glTexCoord2f( 0.0, 0.0 ); glVertex3f(  0.4,  -0.2,  0.2 );
//     glTexCoord2f( 1.0, 0.0 ); glVertex3f(  0.8,  -0.2,  0.2 );
//     glTexCoord2f( 1.0, 1.0 ); glVertex3f(  0.8,  0.2,  0.2 );
//     glTexCoord2f( 0.0, 1.0 ); glVertex3f(  0.4,  0.2,  0.2 );
//
//     glEnd();

    glEndList();
    return list;
}

WndMap::WndMap(Area *ar, QWidget* parent)
{
    map = new GlMap(ar->rooms(), parent);
    QHBoxLayout *lay = new QHBoxLayout(parent);
    lay->addWidget(map);
    setLayout(lay);
    KreatorSettings::instance().loadGuiStatus("MapWindow", this);
    //resize(parent->width(), parent->height());
    
}

}
