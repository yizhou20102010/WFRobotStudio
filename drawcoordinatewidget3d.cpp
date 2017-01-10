#include "drawcoordinatewidget3d.h"
#include <QtOpenGL/glut.h>
#include <QDebug>
DrawCoordinateWidget3D::DrawCoordinateWidget3D(QWidget *parent) : QOpenGLWidget(parent)
{
    for(int i=0;i<3;i++)
        m_Euler[i]=0.0;
}

void DrawCoordinateWidget3D::initializeGL()
{
    initializeOpenGLFunctions();
//    bSetupPixelFormat();
    glClearColor(0.0, 0.0, 0,0);
}

void DrawCoordinateWidget3D::resizeGL(GLint w, GLint h)
{
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    if(h==0)h=1;

    InitLighting();
    glMatrixMode(GL_PROJECTION);//进入投影变换状态
    glLoadIdentity();          //重新进行投影变换
    glOrtho(-5, 5, -5, 15, -5, 5);//  (20.0f, (GLdouble)w/(GLdouble)h, 1.0, 40.0f);
//    gluPerspective(90.0f, (GLdouble)w/(GLdouble)h, 5.0, 15.0f);
//    glFrustum(-range_x, range_x, -range_y, range_y, eyepos[2]-range_z,eyepos[2]+range_z);
    glMatrixMode(GL_MODELVIEW);//结束投影变换
    glLoadIdentity();
    glViewport(0, 0, w, h);  //设置视口
    gluLookAt(2,2,3,0,0,0,0,1,0);

//    m_width=w;
//    m_height=h;
}

void DrawCoordinateWidget3D::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(1.0, 0.0, 0.0);

    double pos[3]={0,0,0};
    double euler[3]={0,0,0};
    DrawCoordinate(pos, euler);

    double pos1[3]={0,10,0};
    DrawCoordinate(pos1, m_Euler);

    glFlush();
}

//void DrawCoordinateWidget3D::paintEvent(QPaintEvent *event)
//{

//}

void DrawCoordinateWidget3D::bSetupPixelFormat()
{
//    QSurfaceFormat format;
//    format.setDepthBufferSize(24);
//    format.setStencilBufferSize(8);
//    format.setVersion(3, 2);
//    format.setProfile(QSurfaceFormat::CoreProfile);
//    QSurfaceFormat::setDefaultFormat(format);
}

void DrawCoordinateWidget3D::InitLighting()
{
    glClearColor(1.0, 1.0, 1.0, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    GLfloat ambientLight[]  = {0.2f,  0.2f,  0.2f,  1.0f};//环境光
    GLfloat diffuseLight[]  = {0.9f,  0.9f,  0.9f,  1.0f};//漫反射
    GLfloat specularLight[] = {1.0f,  1.0f,  1.0f,  1.0f};//镜面光
    GLfloat lightPos[]      = {50.0f, 80.0f, 60.0f, 1.0f};//光源位置

    //启用光照
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);     //设置环境光源
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);     //设置漫反射光源
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);   //设置镜面光源
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);        //设置灯光位置
    glEnable(GL_LIGHT0);                                //打开第一个灯光

    glEnable(GL_COLOR_MATERIAL);                        //启用材质的颜色跟踪
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);  //指定材料着色的面
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularLight); //指定材料对镜面光的反应
    glMateriali(GL_FRONT, GL_SHININESS, 100);           //指定反射系数

    glEnable(GL_NORMALIZE);//启用法向正规化，用于glscale之后坐标系缩放后的法向异常。

    glEnable(GL_POINT_SMOOTH);
     glEnable(GL_LINE_SMOOTH);
     glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); // Make round points, not square points
     glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);  // Antialias the lines
     glEnable(GL_BLEND);
}

void DrawCoordinateWidget3D::DrawCoordinate(double origin[], double euler[])
{
    GLUquadricObj *quadratic;
    //X轴
    glPushMatrix();
    glTranslated(origin[0], origin[1], origin[2]);
    glRotated(euler[0], 0,0,1);
    glRotated(euler[1], 0,1,0);
    glRotated(euler[2], 1,0,0);
    glColor3d(1.0, 0.0, 0.0);
    glPushMatrix();
    quadratic = gluNewQuadric();
    gluCylinder(quadratic,0.1,0.1,3,10,10);
    glTranslated(0,0,3);
    glutSolidCone(0.2,0.4,5,5);
    glPopMatrix();

    //Y轴
    glColor3d(0.0, 1.0, 0.0);
    glPushMatrix();
    glRotated(90,0,1,0);
    quadratic = gluNewQuadric();
    gluCylinder(quadratic,0.1,0.1,3,10,10);
    glTranslated(0,0,3);
    glutSolidCone(0.2,0.4,5,5);
    glPopMatrix();

    //Z轴
    glColor3d(0.0, 0.0, 1.0);
    glPushMatrix();
    glRotated(-90,1,0,0);
    quadratic = gluNewQuadric();
    gluCylinder(quadratic,0.1,0.1,3,10,10);
    glTranslated(0,0,3);
    glutSolidCone(0.2,0.4,5,5);
    glPopMatrix();

    glPopMatrix();
}

void DrawCoordinateWidget3D::SetPosture(double euler[])
{
    for(int i=0;i<3;i++)
        m_Euler[i]=euler[i];
    update();
}

