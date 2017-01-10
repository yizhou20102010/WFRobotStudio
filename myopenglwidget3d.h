#ifndef MYOPENGLWIDGET3D_H
#define MYOPENGLWIDGET3D_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QVector3D>
#include <QWheelEvent>
#include <QMatrix4x4>
class MyOpenGLWidget3D : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit MyOpenGLWidget3D(QWidget *parent = 0);

signals:

public slots:
    //槽函数：显示3D的预设视图，默认视图(0)，前视图(1)，后视图(2)，左视图(3)
    //右视图(4)，上视图(5)，下视图(6)；
    void LoadSetupView(int viewindex);
protected:
    void initializeGL();
    void resizeGL(GLint w, GLint h);
    void paintGL();

    void bSetupPixelFormat();//OpenGL初始化的参数设置
    void InitLighting(void);//初始化光照
    void DrawCubic(void);//绘制立方体

    void mouseMoveEvent(QMouseEvent *mme);//鼠标移动响应(键按下时响应)
    void wheelEvent(QWheelEvent *event);//鼠标中建滚轮响应
    void mousePressEvent(QMouseEvent *mpe);//鼠标键按下响应
    //根据屏幕上前后点的平移向量(p1-p0)计算当前物体坐标系下的平移向量m_transdir
    void FindTranslateInformation(QPoint &p0, QPoint &p1, QVector3D &m_transdir);
    //根据根据屏幕上前后点的位置结合虚拟球算法(外球)，计算当前物体坐标系下的旋转轴m_rotaxis和返回旋转角度
    double FindRotationInformation(QPoint &p0, QPoint &p1, QVector3D &m_rotaxis);//返回旋转角度和旋转轴
private:
    double range_x, range_y,range_z;
    GLint m_width, m_height;
    QPoint m_transbp, m_transep;//平移的前后点
    QPoint m_rotbp, m_rotep;//旋转的前后点
    double m_scalevalue; //当前缩放倍率
    double eyepos[3], centerpos[3], updir[3];
    QMatrix4x4 curTransformMatrix;//当前物体坐标系相对于基坐标的变换矩阵
};

#endif // MYOPENGLWIDGET3D_H
