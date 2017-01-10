#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>
#include "pathposturesetdialog.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    if (QCoreApplication::arguments().contains(QStringLiteral("--multisample")))
        format.setSamples(4);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    MainWindow w;
    w.show();


    return a.exec();
}
