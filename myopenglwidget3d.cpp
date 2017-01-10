#include "myopenglwidget3d.h"
#include <QtOpenGL/glut.h>
#include <math.h>
#include<QDebug>
const double pi=3.14159265359;
MyOpenGLWidget3D::MyOpenGLWidget3D(QWidget *parent) : QOpenGLWidget(parent)
{
    range_x=20.0;
    range_y=20.0;
    range_z=20.0;

    m_width=20;
    m_height=20;
    m_scalevalue=1.0;
}

void MyOpenGLWidget3D::initializeGL()
{
    initializeOpenGLFunctions();
//    bSetupPixelFormat();
    glClearColor(0.0, 0.0, 0,0);
}

void MyOpenGLWidget3D::resizeGL(GLint w, GLint h)
{
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    if(h==0)h=1;

    InitLighting();
    glMatrixMode(GL_PROJECTION);//进入投影变换状态
    glLoadIdentity();          //重新进行投影变换
    glOrtho(-range_x, range_x, -range_y, range_y, -range_z, range_z);//  (20.0f, (GLdouble)w/(GLdouble)h, 1.0, 40.0f);
//    gluPerspective(20.0f, (GLdouble)w/(GLdouble)h, 20.0, 60.0f);
//    glFrustum(-range_x, range_x, -range_y, range_y, eyepos[2]-range_z,eyepos[2]+range_z);
    glMatrixMode(GL_MODELVIEW);//结束投影变换
    glLoadIdentity();
    glViewport(0, 0, w, h);  //设置视口
//    gluLookAt(0,0,40,0,0,0,0,1,0);

    m_width=w;
    m_height=h;
    m_scalevalue=1.0;
    curTransformMatrix.setToIdentity();
}

void MyOpenGLWidget3D::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glTranslated(2.5, 0.0, 0.0);
    glColor3d(1.0, 0.0, 0.0);
//    glLoadIdentity();
//    gluLookAt(eyepos[0], eyepos[1], eyepos[2], centerpos[0], centerpos[1], centerpos[2], updir[0], updir[1], updir[2]);
//   DrawCubic();
   glutWireCube(5);
    glFlush();
}

void MyOpenGLWidget3D::bSetupPixelFormat()
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);
}

void MyOpenGLWidget3D::InitLighting()
{
    glClearColor(1.0, 1.0, 1.0, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    GLfloat ambientlight[]={0.0,0.0,0.0,1.0};
    GLfloat diffuselight[]={0.9,0.9,0.9,1.0};
    GLfloat specularlight[]={0.9,0.9,0.9,1.0};
    GLfloat position[]={1.0,1.0,1.0,0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientlight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuselight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularlight);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);//启用法向正规化，用于glscale之后坐标系缩放后的法向异常。
}
//绘制立方体
void MyOpenGLWidget3D::DrawCubic()
{
    glColor3d(1.0,0.0,0);
    glBegin(GL_POLYGON);
    glNormal3d(-1.0,-1.0,1.0);
    glVertex3d(-5.0,-5.0,5.0);
    glNormal3d(1.0,-1.0,1.0);
    glVertex3d(5.0,-5.0,5.0);
    glNormal3d(1.0,1.0,1.0);
    glVertex3d(5.0,5.0,5.0);
    glNormal3d(-1.0,1.0,1.0);
    glVertex3d(-5.0,5.0,5.0);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3d(-1.0,-1.0,1.0);
    glVertex3d(-5.0,-5.0,5.0);
    glNormal3d(1.0,-1.0,1.0);
    glVertex3d(5.0,-5.0,5.0);
    glNormal3d(1.0,-1.0,-1.0);
    glVertex3d(5.0,-5.0,-5.0);
    glNormal3d(-1.0,-1.0,-1.0);
    glVertex3d(-5.0,-5.0,-5.0);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3d(1.0,1.0,1.0);
    glVertex3d(5.0,5.0,5.0);
    glNormal3d(1.0,-1.0,1.0);
    glVertex3d(5.0,-5.0,5.0);
    glNormal3d(1.0,-1.0,-1.0);
    glVertex3d(5.0,-5.0,-5.0);
    glNormal3d(1.0,1.0,-1.0);
    glVertex3d(5.0,5.0,-5.0);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3d(1.0,1.0,1.0);
    glVertex3d(5.0,5.0,5.0);
    glNormal3d(-1.0,1.0,1.0);
    glVertex3d(-5.0,5.0,5.0);
    glNormal3d(-1.0,1.0,-1.0);
    glVertex3d(-5.0,5.0,-5.0);
    glNormal3d(1.0,1.0,-1.0);
    glVertex3d(5.0,5.0,-5.0);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3d(-1.0,-1.0,1.0);
    glVertex3d(-5.0,-5.0,5.0);
    glNormal3d(-1.0,1.0,1.0);
    glVertex3d(-5.0,5.0,5.0);
    glNormal3d(-1.0,1.0,-1.0);
    glVertex3d(-5.0,5.0,-5.0);
    glNormal3d(-1.0,-1.0,-1.0);
    glVertex3d(-5.0,-5.0,-5.0);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3d(1.0,-1.0,-1.0);
    glVertex3d(5.0,-5.0,-5.0);
    glNormal3d(1.0,1.0,-1.0);
    glVertex3d(5.0,5.0,-5.0);
    glNormal3d(-1.0,1.0,-1.0);
    glVertex3d(-5.0,5.0,-5.0);
    glNormal3d(-1.0,-1.0,-1.0);
    glVertex3d(-5.0,-5.0,-5.0);
    glEnd();
}

void MyOpenGLWidget3D::LoadSetupView(int viewindex)
{
    m_scalevalue=1.0;
    makeCurrent();
    glLoadIdentity();
    curTransformMatrix.setToIdentity();
    switch(viewindex)
    {
    case 0: //Dafault View
        break;
    case 1://Front View
        break;
    case 2: //Back View
        glRotated(180, 1.0, 0.0, 0.0);
        curTransformMatrix.rotate(180, 1.0, 0.0, 0.0);
        break;
    case 3: //Left View
        glRotated(90, 0.0, 1.0, 0.0);
        curTransformMatrix.rotate(90, 0.0, 1.0, 0.0);
        break;
    case 4: //Right View
        glRotated(-90, 0.0, 1.0, 0.0);
        curTransformMatrix.rotate(-90, 0.0, 1.0, 0.0);
        break;
    case 5: //Up View
        glRotated(90, 1.0, 0.0, 0.0);
        curTransformMatrix.rotate(90, 1.0, 0.0, 0.0);
        break;
    case 6: //Bottom View
        glRotated(-90, 1.0, 0.0, 0.0);
        curTransformMatrix.rotate(-90, 1.0, 0.0, 0.0);
        break;
    }

    update();
}

void MyOpenGLWidget3D::mouseMoveEvent(QMouseEvent *mme)
{
    if(mme->buttons() & Qt::LeftButton)//Translate
    {
        m_transep=mme->pos();
        QVector3D transdir;
        FindTranslateInformation(m_transbp, m_transep, transdir);
        makeCurrent();
        glTranslated(transdir.x(), transdir.y(), transdir.z());
        curTransformMatrix.translate(transdir.x(), transdir.y(), transdir.z());
        m_transbp=m_transep;
        update();
    }
    else if(mme->buttons() & Qt::RightButton)//Rotation
    {
        m_rotep=mme->pos();
        double rotangle;
        QVector3D rotaxis;
        rotangle=FindRotationInformation(m_rotbp, m_rotep, rotaxis);
        makeCurrent();
        glRotated(rotangle, rotaxis.x(), rotaxis.y(), rotaxis.z());
        curTransformMatrix.rotate(rotangle, rotaxis.x(), rotaxis.y(), rotaxis.z());
        update();
        m_rotbp=m_rotep;
    }
}

void MyOpenGLWidget3D::wheelEvent(QWheelEvent *event)
{
    int numdegrees=event->delta();
    double m_scaleratio;
    if(numdegrees==0)
        m_scaleratio=1.0;
    else if(numdegrees>0)
        m_scaleratio=1.0*numdegrees/600+1;
    else m_scaleratio=1.0/(1-1.0*numdegrees/600);
    m_scalevalue*=m_scaleratio;

    makeCurrent();
    glScaled(m_scaleratio, m_scaleratio, m_scaleratio);
    curTransformMatrix.scale(m_scaleratio);
    update();
}

void MyOpenGLWidget3D::mousePressEvent(QMouseEvent *mpe)
{
    if(mpe->button() == Qt::LeftButton)
    {
        m_transbp=mpe->pos();
    }
    else if(mpe->button() == Qt::RightButton)
    {
        m_rotbp=mpe->pos();
    }
}

void MyOpenGLWidget3D::FindTranslateInformation(QPoint &p0, QPoint &p1, QVector3D &m_transdir)
{
    QVector3D bp, ep;
    bp.setX(range_x*(2.0*p0.x()-m_width)/m_width);
    bp.setY(range_y*(m_height-2.0*p0.y())/m_height);
    bp.setZ(0.0);

    ep.setX(range_x*(2.0*p1.x()-m_width)/m_width);
    ep.setY(range_y*(m_height-2.0*p1.y())/m_height);
    ep.setZ(0.0);

    m_transdir=curTransformMatrix.inverted().mapVector(ep-bp);

}

double MyOpenGLWidget3D::FindRotationInformation(QPoint &p0, QPoint &p1, QVector3D &m_rotaxis)
{
    double radius;
    radius=(m_width < m_height ? m_width : m_height);
    QVector3D bp, ep;

    bp.setX((2.0*p0.x()-m_width)/m_width);
    bp.setY((m_height-2.0*p0.y())/m_height);
    if(bp.x()<-1)bp.setX(-1.0);
    else if(bp.x()>1)bp.setX(1.0);
    if(bp.y()<-1)bp.setY(-1.0);
    else if(bp.y()>1)bp.setY(1.0);
    bp.setZ(sqrt(2-bp.x()*bp.x()-bp.y()*bp.y()));

    ep.setX((2.0*p1.x()-m_width)/m_width);
    ep.setY((m_height-2.0*p1.y())/m_height);
    if(ep.x()<-1)ep.setX(-1.0);
    else if(ep.x()>1)ep.setX(1.0);
    if(ep.y()<-1)ep.setY(-1.0);
    else if(ep.y()>1)ep.setY(1.0);
    ep.setZ(sqrt(2-ep.x()*ep.x()-ep.y()*ep.y()));

    bp.normalize();
    ep.normalize();
    m_rotaxis=QVector3D::crossProduct(bp, ep);
    double angle(0);
    angle=acos(QVector3D::dotProduct(bp, ep))*180/pi;
    if(m_rotaxis.length()<0.01)
        angle=0;
    m_rotaxis.normalize();

    m_rotaxis=curTransformMatrix.inverted().mapVector(m_rotaxis);

    return angle;

}

