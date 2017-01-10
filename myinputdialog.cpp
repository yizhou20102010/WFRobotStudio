#include "myinputdialog.h"
#include "ui_myinputdialog.h"
#include "globalvariables.h"
#include "mainwindow.h"
MyInputDialog::MyInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MyInputDialog)
{
    ui->setupUi(this);
    QStringList conslist;
    conslist.clear();
    for(int i=0;i<g_CADtoPath.GetPahtsCount();i++)
        conslist<<QString::fromStdString(g_CADtoPath.GetPathName(i));
    ui->PathsNameList->addItems(conslist);
    ui->PathsNameList->setCurrentIndex(-1);
}

MyInputDialog::~MyInputDialog()
{
    delete ui;
}



void MyInputDialog::on_PathsNameList_currentIndexChanged(int index)
{
    emit PathsNameListSelChanged(index);
}
