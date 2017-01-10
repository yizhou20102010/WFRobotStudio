#include "pathposturesetdialog.h"
#include "ui_pathposturesetdialog.h"
#include "QDebug"
PathPostureSetDialog::PathPostureSetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PathPostureSetDialog)
{
    ui->setupUi(this);
    for(int i=0;i<3;i++)
        Euler[i]=0.0;

//    QSurfaceFormat format;
//    format.setDepthBufferSize(24);
//    format.setStencilBufferSize(8);
//    format.setVersion(3, 2);
//    format.setProfile(QSurfaceFormat::CoreProfile);
//    ui->openGLWidget->setFormat(format);
//    ui->openGLWidget->makeCurrent();
}

PathPostureSetDialog::~PathPostureSetDialog()
{
    delete ui;
}

void PathPostureSetDialog::SetPosture(double euler[])
{
    for(int i=0;i<3;i++)
        Euler[i]=euler[i];
    ui->Value_A->setValue(Euler[0]);
    ui->Value_B->setValue(Euler[1]);
    ui->Value_C->setValue(Euler[2]);

//    ui->openGLWidget->SetPosture(euler);
//    ui->openGLWidget->update();
}

void PathPostureSetDialog::Getposture(double euler[])
{
    for(int i=0;i<3;i++)
        euler[i]=Euler[i];
}


void PathPostureSetDialog::on_Value_A_valueChanged(double arg1)
{
    Euler[0]=arg1;
    ui->openGLWidget->SetPosture(Euler);
}

void PathPostureSetDialog::on_Value_B_valueChanged(double arg1)
{
    Euler[1]=arg1;
    ui->openGLWidget->SetPosture(Euler);
}

void PathPostureSetDialog::on_Value_C_valueChanged(double arg1)
{
    Euler[2]=arg1;
    ui->openGLWidget->SetPosture(Euler);
}

void PathPostureSetDialog::on_Confirm_clicked()
{
    QDialog::accept();
}

void PathPostureSetDialog::on_Cancle_clicked()
{
    QDialog::reject();
}
