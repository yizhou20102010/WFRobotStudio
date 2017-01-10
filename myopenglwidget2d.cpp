#include "myopenglwidget2d.h"
#include <QtOpenGL/glut.h>
#include <QDebug>
#include <globalvariables.h>
#include <QMatrix2x2>
MyOpenGLWidget2D::MyOpenGLWidget2D(QWidget *parent) : QOpenGLWidget(parent)
{
    range_x=20;
    range_y=20;
    m_width=0;
    m_height=0;
    m_scalevalue=1.0;
}

void MyOpenGLWidget2D::initializeGL()
{
    initializeOpenGLFunctions();
//    bSetupPixelFormat();
    glClearColor(0.0, 0.0, 0,0);
}

void MyOpenGLWidget2D::resizeGL(GLint w, GLint h)
{
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    if(h==0)h=1;
    double wh_ratio;
    wh_ratio=1.0*w/h;
    if(m_height!=0)
        range_y*=1.0*h/m_height;
    range_x=range_y*wh_ratio;

    glMatrixMode(GL_PROJECTION);//进入投影变换状态
    glLoadIdentity();          //重新进行投影变换
    gluOrtho2D(-range_x, range_x, -range_y, range_y);//  (20.0f, (GLdouble)w/(GLdouble)h, 1.0, 40.0f);
    glMatrixMode(GL_MODELVIEW);//结束投影变换
    glLoadIdentity();
    glViewport(0, 0, w, h);  //设置视口
    m_width=w;
    m_height=h;
    m_scalevalue=1.0;
    curTransformMatrix.setToIdentity();
}

void MyOpenGLWidget2D::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int curpathindex, curpathindex_another,curpointindex;
    curpathindex=g_CADtoPath.GetCurIndexofSelOneWholePath();
    curpathindex_another=g_CADtoPath.GetCurIndexofSelOneWholePath_Another();
    curpointindex=g_CADtoPath.GetCurIndexofSelPointinOneWholePath();

    for(size_t i=0;i<g_CADtoPath.m_MultiPathInfoList.size();i++)
    {
        if(i==curpathindex)
        {
            glLineWidth(2.0);
            glColor3f(0.0f, 0.0f, 1.0f);
        }
        else if(i==curpathindex_another)
        {
            glLineWidth(2.0);
            glColor3f(0.5f, 0.0f, 1.0f);
        }
        else
        {
            glLineWidth(1.0);
            glColor3f(0.0f, 0.0f, 0.0f);
        }
        glBegin(GL_LINE_STRIP);
        int j=0;
        while(j<g_CADtoPath.m_MultiPathInfoList[i].onepath.size())
        {
            if(j>0&&j<g_CADtoPath.m_MultiPathInfoList[i].onepath.size()-1&&g_CADtoPath.m_MultiPathInfoList[i].onepath[j].mode==0x21)//圆弧
            {
                double p0[3], p1[3], p2[3];
                p0[0]=g_CADtoPath.m_MultiPathInfoList[i].onepath[j-1].x;
                p0[1]=g_CADtoPath.m_MultiPathInfoList[i].onepath[j-1].y;
                p0[2]=g_CADtoPath.m_MultiPathInfoList[i].onepath[j-1].z;

                p1[0]=g_CADtoPath.m_MultiPathInfoList[i].onepath[j].x;
                p1[1]=g_CADtoPath.m_MultiPathInfoList[i].onepath[j].y;
                p1[2]=g_CADtoPath.m_MultiPathInfoList[i].onepath[j].z;

                p2[0]=g_CADtoPath.m_MultiPathInfoList[i].onepath[j+1].x;
                p2[1]=g_CADtoPath.m_MultiPathInfoList[i].onepath[j+1].y;
                p2[2]=g_CADtoPath.m_MultiPathInfoList[i].onepath[j+1].z;
                DrawArc(p0, p1, p2);
                j+=2;
            }
            else
            {
                glVertex2d(g_CADtoPath.m_MultiPathInfoList[i].onepath[j].x, g_CADtoPath.m_MultiPathInfoList[i].onepath[j].y);
                j++;
            }
        }
        glEnd();
    }
    //绘制当前路径上的选择点
    if(curpathindex!=-1&&curpointindex!=-1)
    {
        glPointSize(5);
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_POINTS);
        glVertex2d(g_CADtoPath.m_MultiPathInfoList[curpathindex].onepath[curpointindex].x, g_CADtoPath.m_MultiPathInfoList[curpathindex].onepath[curpointindex].y);
        glEnd();
        glPointSize(1.0);
    }

    glFlush();
}

void MyOpenGLWidget2D::bSetupPixelFormat()
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);
}

void MyOpenGLWidget2D::DrawArc(double p0[], double p1[], double p2[])
{
    QVector3D cp0(p0[0], p0[1], 0.0),cp1(p1[0], p1[1], 0.0),cp2(p2[0], p2[1], 0.0);
    QVector3D dir1(cp1.y()-cp0.y(), -cp1.x()+cp0.x(), 0.0), dir2(cp2.y()-cp1.y(), -cp2.x()+cp1.x(), 0.0);
    QVector3D mp01,mp12,bm;
    mp01=(cp0+cp1)*0.5;
    mp12=(cp1+cp2)*0.5;
    bm=mp12-mp01;
    double det=-dir1.x()*dir2.y()+dir1.y()*dir2.x();
    if(fabs(det)>0.000001)
    {
        double s=(-bm.x()*dir2.y()+bm.y()*dir2.x())/det;
        QVector3D center;
        center=mp01+s*dir1;
        double radius=(center-cp0).length();
        if(radius>0)
        {
            double begina, enda;
            if((cp0.x()-center.x())/radius>1)
                begina=0;
            else if((cp0.x()-center.x())/radius<-1)
                begina=Pi;
            else begina=acos((cp0.x()-center.x())/radius);
            if(cp0.y()-center.y()<0)begina=-begina;

            if((cp2.x()-center.x())/radius>1)
                enda=0;
            else if((cp2.x()-center.x())/radius<-1)
                enda=Pi;
            else enda=acos((cp2.x()-center.x())/radius);
            if(cp2.y()-center.y()<0)enda=-enda;

            //绘制圆弧
            double istep,rada;
            istep=3.0/180*Pi;//m_scalevalue;
            QVector3D normal=QVector3D::crossProduct(cp1-cp0, cp2-cp0);
            if(normal.z()<=0)
            {
                if(enda>begina)enda-=2*Pi;
                rada=begina-istep;
//                glBegin(GL_LINE_STRIP);
//                glVertex2d(p0[0], p0[1]);
                while(rada>enda)
                {
                    glVertex2d(center.x()+radius*cos(rada), center.y()+radius*sin(rada));
                    rada-=istep;
                }
                glVertex2d(p2[0], p2[1]);
//                glEnd();
            }
            else
            {
                if(enda<begina)enda+=2*Pi;
                rada=begina+istep;
//                glBegin(GL_LINE_STRIP);
//                glVertex2d(p0[0], p0[1]);
                while(rada<enda)
                {
//                    qDebug()<<"rada: "<<center.x()+radius*cos(rada)<<" "<<center.y()+radius*sin(rada);
                    glVertex2d(center.x()+radius*cos(rada), center.y()+radius*sin(rada));
                    rada+=istep;
                }
                glVertex2d(p2[0], p2[1]);
//                glEnd();
            }
        }

    }
}

void MyOpenGLWidget2D::LoadDefaultView()
{
    m_scalevalue=1.0;
    curTransformMatrix.setToIdentity();
    makeCurrent();
    glLoadIdentity();
    update();
}

void MyOpenGLWidget2D::ResizeView(double rmax_x, double rmax_y)
{
    range_x=(int)rmax_x/0.618;
    range_y=(int)rmax_y/0.618;
    makeCurrent();
    resizeGL(m_width, m_height);
    update();
}


void MyOpenGLWidget2D::mouseMoveEvent(QMouseEvent *mme)
{
    if(mme->buttons() & Qt::LeftButton)
    {
        m_endpoint=mme->pos();
        QVector3D transdir;
        FindTranslateInformation(m_beginpoint, m_endpoint, transdir);
        makeCurrent();
        glTranslated(transdir.x(), transdir.y(), transdir.z());
        curTransformMatrix.translate(transdir.x(), transdir.y(), transdir.z());
        m_beginpoint=m_endpoint;
        update();
    }
}

void MyOpenGLWidget2D::wheelEvent(QWheelEvent *event)
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
    glScaled(m_scaleratio, m_scaleratio, 0);
    curTransformMatrix.scale(m_scaleratio);
    update();
}


void MyOpenGLWidget2D::mousePressEvent(QMouseEvent *mpe)
{
    if(mpe->button() == Qt::LeftButton)
    {
        m_beginpoint=mpe->pos();
    }
}

void MyOpenGLWidget2D::FindTranslateInformation(QPoint &p0, QPoint &p1, QVector3D &m_transdir)
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

