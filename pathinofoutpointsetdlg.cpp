#include "pathinofoutpointsetdlg.h"
#include "ui_pathinofoutpointsetdlg.h"
#include "GraphtoPath.h"
#include "globalvariables.h"
#include "graphtopathdatastruct.h"
PathInofOutPointSetDlg::PathInofOutPointSetDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PathInofOutPointSetDlg)
{
    ui->setupUi(this);
    addpointmode=-1;
}

PathInofOutPointSetDlg::~PathInofOutPointSetDlg()
{
    delete ui;
}

void PathInofOutPointSetDlg::SetDefaultName(int mode)
{
    addpointmode=mode;
    if(mode==0)//InPoint
    {
        ui->AssistPointName->setText(tr("InPoint"));
        ui->AssistPointName->selectAll();
        ui->relpostip->setTitle(QStringLiteral("相对轨迹起点的偏置"));
    }
    else if(mode==1)//OutPoint
    {
        ui->AssistPointName->setText(tr("OutPoint"));
        ui->AssistPointName->selectAll();
        ui->relpostip->setTitle(QStringLiteral("相对轨迹终点的偏置"));
    }
}

void PathInofOutPointSetDlg::on_buttonBox_accepted()
{
    if(addpointmode==0||addpointmode==1)//
    {
        QString pname;
        double dpos[6];
        bool isApplyAll;
        isApplyAll=ui->ApplyAllPaths->isChecked();
        pname=ui->AssistPointName->text();
        dpos[0]=ui->dxpos->text().toDouble();
        dpos[1]=ui->dypos->text().toDouble();
        dpos[2]=ui->dzpos->text().toDouble();
        dpos[3]=ui->dapos->text().toDouble();
        dpos[4]=ui->dbpos->text().toDouble();
        dpos[5]=ui->dcpos->text().toDouble();
        if(isApplyAll)//应用到全局
            g_CADtoPath.SetAssistPointforAllPaths(pname.toStdString(), dpos, addpointmode);
        else g_CADtoPath.SetAssistPointforCurrentPath(pname.toStdString(), dpos, addpointmode);
        emit acceptstate(addpointmode, isApplyAll);
        QDialog::accept();
    }
    else QDialog::reject();

}

void PathInofOutPointSetDlg::on_buttonBox_rejected()
{
    QDialog::reject();
}
