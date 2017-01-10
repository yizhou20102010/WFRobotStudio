#include "programpreivwsetupdlg.h"
#include "ui_programpreivwsetupdlg.h"
#include "graphtopathdatastruct.h"
#include "globalvariables.h"

ProgramPreivwSetupDlg::ProgramPreivwSetupDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgramPreivwSetupDlg)
{
    ui->setupUi(this);
    //添加系统
    ui->ContronSystemList->addItem(tr("KEBA"));
    ui->ContronSystemList->setCurrentIndex(0);

    ui->ReadyInPoint->setChecked(true);
    ui->ReadyOutPoint->setChecked(true);
    ui->BeginInPlace->setChecked(true);
    ui->EndInPlace->setChecked(true);
}

ProgramPreivwSetupDlg::~ProgramPreivwSetupDlg()
{
    delete ui;
}

void ProgramPreivwSetupDlg::on_GenerateProgramPreview_clicked()
{
    GenerateProgramPara m_gppara;
    PosInfo m_Inpoint, m_Outpoint;
    m_gppara.bpWaitFinished=ui->BeginInPlace->isChecked();
    m_gppara.epWaitFinished=ui->EndInPlace->isChecked();
    m_gppara.InpointEnabled=ui->ReadyInPoint->isChecked();
    m_gppara.OutpointEnabled=ui->ReadyOutPoint->isChecked();

    //Inpoint
    m_Inpoint.name=string("InPoint");
    m_Inpoint.dpname=string("");
    m_Inpoint.opname=string("");
    m_Inpoint.mode=0;
    m_Inpoint.x=0.0;
    m_Inpoint.y=0.0;
    m_Inpoint.z=30.0;
    m_Inpoint.a=0.0;
    m_Inpoint.b=0.0;
    m_Inpoint.c=0.0;
    m_gppara.Inpoint=m_Inpoint;

    //Outpoint
    m_Outpoint.name=string("OutPoint");
    m_Outpoint.dpname=string("");
    m_Outpoint.opname=string("");
    m_Outpoint.mode=0;
    m_Outpoint.x=0.0;
    m_Outpoint.y=0.0;
    m_Outpoint.z=30.0;
    m_Outpoint.a=0.0;
    m_Outpoint.b=0.0;
    m_Outpoint.c=0.0;
    m_gppara.Outpoint=m_Outpoint;

    g_CADtoPath.SetGenerateParaofAllPaths(m_gppara);

    QDialog::accept();
}
