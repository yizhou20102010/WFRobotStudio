#ifndef MYOPENGLWIDGET2D_H
#define MYOPENGLWIDGET2D_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QVector3D>
#include <QMoveEvent>
#include <QWheelEvent>
#include <QMatrix4x4>
class MyOpenGLWidget2D : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit MyOpenGLWidget2D(QWidget *parent = 0);

signals:

public slots:
    void LoadDefaultView(void);
    void ResizeView(double rmax_x, double rmax_y);//缩放窗口
protected:
    void initializeGL();
    void resizeGL(GLint w, GLint h);
    void paintGL();

    void bSetupPixelFormat();

    //绘制圆弧p0起点，p1中点，p2终点
    void DrawArc(double p0[3], double p1[3], double p2[3]);
    void mouseMoveEvent(QMouseEvent *mme);//鼠标移动操作
    void wheelEvent(QWheelEvent *event); //鼠标滚轮操作
    void mousePressEvent(QMouseEvent *mpe);//鼠标键按下操作
    void FindTranslateInformation(QPoint &p0, QPoint &p1, QVector3D &m_transdir);
private:
    double range_x, range_y;
    GLint m_width, m_height;
    QPoint m_beginpoint, m_endpoint;
    double m_scalevalue;//缩放倍率
    QMatrix4x4 curTransformMatrix;//当前物体坐标系相对于基坐标的变换矩阵

};

#endif // MYOPENGLWIDGET2D_H
