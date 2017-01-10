#ifndef DRAWCOORDINATEWIDGET3D_H
#define DRAWCOORDINATEWIDGET3D_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class DrawCoordinateWidget3D : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit DrawCoordinateWidget3D(QWidget *parent = 0);

signals:

public slots:

protected:
    void initializeGL();
    void resizeGL(GLint w, GLint h);
    void paintGL();
//    void paintEvent(QPaintEvent *event);

    void bSetupPixelFormat();//OpenGL初始化的参数设置
    void InitLighting(void);//初始化光照
    void DrawCoordinate(double origin[3], double euler[3]);//绘制立方体

private:
    double m_Euler[3];
public:
    void SetPosture(double euler[3]);
};

#endif // DRAWCOORDINATEWIDGET3D_H
