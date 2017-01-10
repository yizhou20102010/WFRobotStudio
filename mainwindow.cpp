#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QtSpinBoxFactory>
#include <QtAbstractEditorFactoryBase>
#include <QtDoubleSpinBoxFactory>
#include <QtStringPropertyManager>
#include <QtLineEditFactory>
#include <QDate>
#include "globalvariables.h"
#include "myinputdialog.h"
#include "pathposturesetdialog.h"
#include "programpreivwsetupdlg.h"
#include "pathinofoutpointsetdlg.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //设置状态栏永久显示信息：当前显示模式
    m_pstatebarshowmode=new QLabel(this);
    m_pstatebarshowmode->setText("hello word!");
    ui->statusBar->addPermanentWidget(m_pstatebarshowmode);
    //初始化显示模式
    cur_showmode=TwoDimension;
    ShowCurFrame();

    //显示默认视图的信号槽
    connect(this, SIGNAL(loaddefaultview2D()), ui->OpenGL2D, SLOT(LoadDefaultView()));
    connect(this, SIGNAL(ResizeView2D(double,double)), ui->OpenGL2D, SLOT(ResizeView(double,double)));
    connect(this, SIGNAL(loadsetupview3D(int )), ui->OpenGL3D, SLOT(LoadSetupView(int)));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //初始化轨迹窗口
    PathsTreeViewModel= new QStandardItemModel(ui->PathsTreeView);
    ui->PathsTreeView->setModel(PathsTreeViewModel);

    QStringList treeheader;
    treeheader<<QStringLiteral("轨迹信息")<<QStringLiteral("描述");
    PathsTreeViewModel->setHorizontalHeaderLabels(treeheader);
    PathsTreeViewModel->setColumnCount(2);
    ui->PathsTreeView->setColumnWidth(0, 180);
    ui->PathsTreeView->setColumnWidth(1, 50);
     //初始化属性窗口
    InitializePropertyWidget();

    ////////////////////////////////////////////////////////////////////////////////////////
    ProgramConsModel = new QStringListModel(ui->OneProgramContents);
    ui->OneProgramContents->setModel(ProgramConsModel);
    ui->OneProgramContents->setColumnWidth(0,ui->OneProgramContents->width());
    //浮动窗口显示
    tabifyDockWidget(ui->PathsInfo, ui->MotionPara);
    tabifyDockWidget(ui->MotionPara, ui->ProgramsPreview);
    ui->PathsInfo->raise();
    InitializeMotionParaWidget();
    curSelTreeItemofDynInfoorOvlInfo=NULL;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *kevent)
{

}
//2D/3D显示切换
void MainWindow::on_ShowDimension_triggered()
{
    if(cur_showmode == TwoDimension)
        cur_showmode=ThreeDimension;
    else if(cur_showmode==ThreeDimension)
        cur_showmode=TwoDimension;
    //显示当前的OpengGL Widget
    ShowCurFrame();
}
//////////////////////////////////////////////////////////////////////
//显示当前视图
void MainWindow::ShowCurFrame()
{
    if(cur_showmode==TwoDimension)
    {
        ui->OpenGL2D->show();
        ui->OpenGL3D->hide();
        m_pstatebarshowmode->setText("ShowMode: 2D");
        qDebug()<<"cur Show Mode: 2D";
        ui->View3D->setEnabled(false);
    }
    else if(cur_showmode==ThreeDimension)
    {
        ui->OpenGL2D->hide();
        ui->OpenGL3D->show();
        m_pstatebarshowmode->setText("ShowMode: 3D");
        ui->View3D->setEnabled(true);
        qDebug()<<"cur Show Mode: 3D";
    }
}
//读取dxf文件，
bool MainWindow::ReadDXFData(QString &filepath)
{
    //读取DXF文件
    int onepathnum;
    onepathnum=g_CADtoPath.ReadDxfFile(filepath.toStdString());

    qDebug()<<onepathnum;

    return true;
}

void MainWindow::InitializePropertyWidget()
{
    ui->PathsProperty->hide();
    ui->PointsProperty->hide();
    curSelPathPropertyItem = NULL;
    curSelPointPropertyItem = NULL;
}
//显示轨迹的树结构属性
void MainWindow::ShowPathsProperty()
{
    ui->PathsProperty->clear();
    //轨迹
    QtStringPropertyManager *m_pPathsStringManager;
    QtLineEditFactory *m_pPathsLineEditFactory;

    QtVariantPropertyManager *m_pPathsVariantManager;

    QtEnumPropertyManager *m_pPathsEnumManager;
    QtEnumEditorFactory* m_pPathsEnumFactory;

    QtBoolPropertyManager *m_pPathsBoolManagerReadOnly,*m_pPathsBoolManager;
    QtCheckBoxFactory *m_pPathsBoolFactory;
    //新建属性栏中variable的manager和factory变量
    m_pPathsStringManager=new QtStringPropertyManager(ui->PathsProperty);
    m_pPathsLineEditFactory=new QtLineEditFactory(ui->PathsProperty);

    m_pPathsVariantManager = new QtVariantPropertyManager(ui->PathsProperty);

    m_pPathsEnumManager = new QtEnumPropertyManager(ui->PathsProperty);
    m_pPathsEnumFactory = new QtEnumEditorFactory(ui->PathsProperty);

    m_pPathsBoolManagerReadOnly = new QtBoolPropertyManager(ui->PathsProperty);
    m_pPathsBoolManager = new QtBoolPropertyManager(ui->PathsProperty);
    m_pPathsBoolFactory = new QtCheckBoxFactory(ui->PathsProperty);
    //连接variable项数据更改的信号和槽函数
    connect(m_pPathsStringManager, &QtStringPropertyManager::valueChanged, this, &MainWindow::PathsPropertyStringValueChanged);
    connect(m_pPathsEnumManager, &QtEnumPropertyManager::valueChanged, this, &MainWindow::PathsPropertyEnumValueChanged);
    //添加string项：“轨迹名”
    QtProperty *stringitem;
    stringitem=m_pPathsStringManager->addProperty(QStringLiteral("轨迹名: "));
    string pathname=g_CADtoPath.GetCurNameofSelOneWholePath();
    m_pPathsStringManager->setValue(stringitem, QString::fromStdString(pathname));
    ui->PathsProperty->addProperty(stringitem);

    QtProperty *groupitem;
    QtProperty *enumitem;
    //显示项目的动态参数，采用enum格式
    QStringList dynparaslist, ovlparaslist;//项目的动态参数列表
    dynparaslist.clear();
    ovlparaslist.clear(); 
    groupitem=m_pPathsVariantManager->addProperty(QtVariantPropertyManager::groupTypeId(), QStringLiteral("Dyn参数"));
    enumitem=m_pPathsEnumManager->addProperty(QStringLiteral("Dyn参数"));
    dynparaslist<<"None";
    for(size_t i=0;i<g_CADtoPath.m_DynVarList.size();i++)
        dynparaslist<<QString::fromStdString(g_CADtoPath.m_DynVarList[i].name);
    int dynindex=g_CADtoPath.GetIndexofDynInfobyCurSelPath()+1;
    m_pPathsEnumManager->setEnumNames(enumitem, dynparaslist);
    m_pPathsEnumManager->setValue(enumitem, dynindex);
    groupitem->addSubProperty(enumitem);
    ui->PathsProperty->addProperty(groupitem);
    groupitem=m_pPathsVariantManager->addProperty(QtVariantPropertyManager::groupTypeId(), QStringLiteral("Ovl参数"));
    enumitem=m_pPathsEnumManager->addProperty(QStringLiteral("Ovl参数"));
    ovlparaslist<<"None";
    for(size_t i=0;i<g_CADtoPath.m_OvlVarList.size();i++)
        ovlparaslist<<QString::fromStdString(g_CADtoPath.m_OvlVarList[i].name);
    int ovlindex=g_CADtoPath.GetIndexofOvlInfobyCurSelPath()+1;
    m_pPathsEnumManager->setEnumNames(enumitem, ovlparaslist);
    m_pPathsEnumManager->setValue(enumitem, ovlindex);
    groupitem->addSubProperty(enumitem);
    ui->PathsProperty->addProperty(groupitem);
    //显示轨迹是否采用辅助点及点到位，采用bool格式
    bool InPEnabled, OutPEnabled, bpWF, epWF;
    g_CADtoPath.GetAssistInformationofcurSelPathAssistPoint(InPEnabled, OutPEnabled, bpWF, epWF);
    QtProperty *boolitem;
    groupitem=m_pPathsVariantManager->addProperty(QtVariantPropertyManager::groupTypeId(), QStringLiteral("辅助点"));
    boolitem=m_pPathsBoolManagerReadOnly->addProperty(QStringLiteral("进入点"));
    m_pPathsBoolManagerReadOnly->setValue(boolitem, InPEnabled);
    groupitem->addSubProperty(boolitem);
    boolitem=m_pPathsBoolManagerReadOnly->addProperty(QStringLiteral("脱离点"));
    m_pPathsBoolManagerReadOnly->setValue(boolitem, OutPEnabled);
    groupitem->addSubProperty(boolitem);
    ui->PathsProperty->addProperty(groupitem);

    groupitem=m_pPathsVariantManager->addProperty(QtVariantPropertyManager::groupTypeId(), QStringLiteral("点到位"));
    boolitem=m_pPathsBoolManager->addProperty(QStringLiteral("启动点"));
    m_pPathsBoolManager->setValue(boolitem, bpWF);
    groupitem->addSubProperty(boolitem);
    boolitem=m_pPathsBoolManager->addProperty(QStringLiteral("结束点"));
    m_pPathsBoolManager->setValue(boolitem, epWF);
    groupitem->addSubProperty(boolitem);
    ui->PathsProperty->addProperty(groupitem);

    //将部分item设置成为可编辑项目
    ui->PathsProperty->setFactoryForManager(m_pPathsStringManager, m_pPathsLineEditFactory);
    ui->PathsProperty->setFactoryForManager(m_pPathsEnumManager, m_pPathsEnumFactory);
    ui->PathsProperty->setFactoryForManager(m_pPathsBoolManager, m_pPathsBoolFactory);

}

void MainWindow::InitializeMotionParaWidget()
{
    ui->MotionParaType->clear();
    ui->MotionParaType->addItem(QStringLiteral("加减速参数信息"));
    ui->MotionParaType->addItem(QStringLiteral("轨迹逼近参数信息"));
    ui->MotionParaType->setCurrentIndex(0);

    ShowMotionParaViewList(0);
    curSelMotionParaPropertyItem=NULL;
}
//显示动态参数
void MainWindow::ShowMotionParaViewList(const int index)
{
    //初始化动态参数显示列表
    ui->MotionInfoList->setColumnCount(2);
    ui->MotionInfoList->setColumnWidth(0, 180);
    ui->MotionInfoList->setColumnWidth(1, 50);

    ui->MotionInfoList->clear();
    curSelTreeItemofDynInfoorOvlInfo=NULL;
    QTreeWidgetItem *root;
    QList<QTreeWidgetItem *>rootlist;
    rootlist.clear();
    QStringList rootname;
    if(index==0)//加减速信息
    {
        //添加行首信息
        QStringList treeheader;
        treeheader<<QStringLiteral("加减速参数")<<QStringLiteral("状态");
        ui->MotionInfoList->setHeaderLabels(treeheader);

        for(size_t i=0;i<g_CADtoPath.m_DynVarList.size();i++)
        {
            rootname.clear();
            rootname<<QString::fromStdString(g_CADtoPath.m_DynVarList[i].name);
            if(g_CADtoPath.m_DynVarList[i].quotenum>0)
                   rootname<<QStringLiteral("已使用");
            else rootname<<QStringLiteral("未使用");
            root = new QTreeWidgetItem(ui->MotionInfoList, rootname);
            rootlist<<root;
        }
        ui->MotionInfoList->insertTopLevelItems(0, rootlist);
    }
    else if(index==1)//Blend信息
    {
        //添加行首信息
        QStringList treeheader;
        treeheader<<QStringLiteral("逼近参数")<<QStringLiteral("状态");
        ui->MotionInfoList->setHeaderLabels(treeheader);

        for(size_t i=0;i<g_CADtoPath.m_OvlVarList.size();i++)
        {
            rootname.clear();
            rootname<<QString::fromStdString(g_CADtoPath.m_OvlVarList[i].name);
            if(g_CADtoPath.m_OvlVarList[i].quotenum>0)
                   rootname<<QStringLiteral("已使用");
            else rootname<<QStringLiteral("未使用");
            root = new QTreeWidgetItem(ui->MotionInfoList, rootname);
            rootlist<<root;
        }
        ui->MotionInfoList->insertTopLevelItems(0, rootlist);
    }
    ShowMotionPareTreeProperty(-1,-1);
}
//显示动态参数的树结构属性
void MainWindow::ShowMotionPareTreeProperty(const int t_index, const int index)
{
    ui->MotionInfoTreeProperty->clear();
    curSelMotionParaPropertyItem=NULL;
    if(t_index==-1||index==-1)return;
    QtStringPropertyManager *m_pMotionParaStringManager;
    QtLineEditFactory *m_pMotionParaLineEditFactory;

    QtVariantPropertyManager *m_pMotionParaVarManager;

    QtDoublePropertyManager *m_pMotionParaDoubleManager;
    QtDoubleSpinBoxFactory *m_pMotionParaDoubleSpinFactory;

    //新建属性栏中variable的manager和factory变量
    m_pMotionParaStringManager = new QtStringPropertyManager(ui->MotionInfoTreeProperty);
    m_pMotionParaLineEditFactory = new QtLineEditFactory(ui->MotionInfoTreeProperty);
    m_pMotionParaVarManager = new QtVariantPropertyManager(ui->MotionInfoTreeProperty);
    m_pMotionParaDoubleManager = new QtDoublePropertyManager(ui->MotionInfoTreeProperty);
    m_pMotionParaDoubleSpinFactory = new QtDoubleSpinBoxFactory(ui->MotionInfoTreeProperty);
    if(t_index==0)//motion type,加减速信息
    {
        //连接variable项数据更改的信号和槽函数
        connect(m_pMotionParaStringManager, &QtStringPropertyManager::valueChanged, this, &MainWindow::MotionParaDynStringValueChanged);
        connect(m_pMotionParaDoubleManager, &QtDoublePropertyManager::valueChanged, this, &MainWindow::MotionParaDynDoubleValueChanged);

        double dyn[12];
        string dynname=g_CADtoPath.GetCurSelDynInfoName(index);
        g_CADtoPath.GetCurSelDynInfo(index, dyn);
        //点位名称
        QtProperty *stringitem;
        stringitem=m_pMotionParaStringManager->addProperty(QStringLiteral("Dyn参数名: "));
        m_pMotionParaStringManager->setValue(stringitem, QString::fromStdString(dynname));
        ui->MotionInfoTreeProperty->addProperty(stringitem);

        QtProperty *groupitem;
        QtProperty * doubleitem;
        groupitem=m_pMotionParaVarManager->addProperty(QtVariantPropertyManager::groupTypeId(), QStringLiteral("关节运动参数"));
        doubleitem=m_pMotionParaDoubleManager->addProperty(QStringLiteral("velAxis: PERCENT "));
        m_pMotionParaDoubleManager->setDecimals(doubleitem, 0);
        m_pMotionParaDoubleManager->setValue(doubleitem,dyn[0]);
        groupitem->addSubProperty(doubleitem);
        doubleitem=m_pMotionParaDoubleManager->addProperty(QStringLiteral("accAxis: PERCENT "));
        m_pMotionParaDoubleManager->setDecimals(doubleitem, 0);
        m_pMotionParaDoubleManager->setValue(doubleitem,dyn[1]);
        groupitem->addSubProperty(doubleitem);
        doubleitem=m_pMotionParaDoubleManager->addProperty(QStringLiteral("decAxis: PERCENT "));
        m_pMotionParaDoubleManager->setDecimals(doubleitem, 0);
        m_pMotionParaDoubleManager->setValue(doubleitem,dyn[2]);
        groupitem->addSubProperty(doubleitem);
        doubleitem=m_pMotionParaDoubleManager->addProperty(QStringLiteral("jerkAxis: PERCENT "));
        m_pMotionParaDoubleManager->setDecimals(doubleitem, 0);
        m_pMotionParaDoubleManager->setValue(doubleitem,dyn[3]);
        groupitem->addSubProperty(doubleitem);
        ui->MotionInfoTreeProperty->addProperty(groupitem);

        groupitem=m_pMotionParaVarManager->addProperty(QtVariantPropertyManager::groupTypeId(), QStringLiteral("直线运动参数"));
        doubleitem=m_pMotionParaDoubleManager->addProperty(QStringLiteral("vel: REAL "));
        m_pMotionParaDoubleManager->setDecimals(doubleitem, 2);
        m_pMotionParaDoubleManager->setValue(doubleitem,dyn[4]);
        groupitem->addSubProperty(doubleitem);
        doubleitem=m_pMotionParaDoubleManager->addProperty(QStringLiteral("acc: REAL "));
        m_pMotionParaDoubleManager->setDecimals(doubleitem, 2);
        m_pMotionParaDoubleManager->setValue(doubleitem,dyn[5]);
        groupitem->addSubProperty(doubleitem);
        doubleitem=m_pMotionParaDoubleManager->addProperty(QStringLiteral("dec: REAL "));
        m_pMotionParaDoubleManager->setDecimals(doubleitem, 2);
        m_pMotionParaDoubleManager->setValue(doubleitem,dyn[6]);
        groupitem->addSubProperty(doubleitem);
        doubleitem=m_pMotionParaDoubleManager->addProperty(QStringLiteral("jerk: REAL "));
        m_pMotionParaDoubleManager->setDecimals(doubleitem, 2);
        m_pMotionParaDoubleManager->setValue(doubleitem,dyn[7]);
        groupitem->addSubProperty(doubleitem);
        ui->MotionInfoTreeProperty->addProperty(groupitem);

        groupitem=m_pMotionParaVarManager->addProperty(QtVariantPropertyManager::groupTypeId(), QStringLiteral("姿态变换参数"));
        doubleitem=m_pMotionParaDoubleManager->addProperty(QStringLiteral("velOri: REAL "));
        m_pMotionParaDoubleManager->setDecimals(doubleitem, 2);
        m_pMotionParaDoubleManager->setValue(doubleitem,dyn[8]);
        groupitem->addSubProperty(doubleitem);
        doubleitem=m_pMotionParaDoubleManager->addProperty(QStringLiteral("accOri: REAL "));
        m_pMotionParaDoubleManager->setDecimals(doubleitem, 2);
        m_pMotionParaDoubleManager->setValue(doubleitem,dyn[9]);
        groupitem->addSubProperty(doubleitem);
        doubleitem=m_pMotionParaDoubleManager->addProperty(QStringLiteral("decOri: REAL "));
        m_pMotionParaDoubleManager->setDecimals(doubleitem, 2);
        m_pMotionParaDoubleManager->setValue(doubleitem,dyn[10]);
        groupitem->addSubProperty(doubleitem);
        doubleitem=m_pMotionParaDoubleManager->addProperty(QStringLiteral("jerkOri: REAL "));
        m_pMotionParaDoubleManager->setDecimals(doubleitem, 2);
        m_pMotionParaDoubleManager->setValue(doubleitem,dyn[11]);
        groupitem->addSubProperty(doubleitem);
        ui->MotionInfoTreeProperty->addProperty(groupitem);

        ui->MotionInfoTreeProperty->setFactoryForManager(m_pMotionParaStringManager, m_pMotionParaLineEditFactory);
        ui->MotionInfoTreeProperty->setFactoryForManager(m_pMotionParaDoubleManager, m_pMotionParaDoubleSpinFactory);
    }
    else if(t_index==1)//Blend information
    {
        QtBoolPropertyManager *m_pMotionParaBoolManager;
        QtCheckBoxFactory *m_pMotionParaCheckBoxFactory;

        QtStringPropertyManager *m_pMotionParaStringManagerReadOnly;

        m_pMotionParaBoolManager = new QtBoolPropertyManager(ui->MotionInfoTreeProperty);
        m_pMotionParaCheckBoxFactory = new QtCheckBoxFactory(ui->MotionInfoTreeProperty);

        m_pMotionParaStringManagerReadOnly = new QtStringPropertyManager(ui->MotionInfoTreeProperty);
        //连接variable项数据更改的信号和槽函数
        connect(m_pMotionParaStringManager, &QtStringPropertyManager::valueChanged, this, &MainWindow::MotionParaOvlStringValueChanged);
        connect(m_pMotionParaDoubleManager, &QtDoublePropertyManager::valueChanged, this, &MainWindow::MotionParaOvlDoubleValueChanged);
        connect(m_pMotionParaBoolManager, &QtBoolPropertyManager::valueChanged, this, &MainWindow::MotionParaOvlBoolValueChanged);

        double ovl[5];
        string ovlname=g_CADtoPath.GetCurSelOvlInfoName(index);
        g_CADtoPath.GetCurSelOvlInfo(index, ovl);
        int ovlmode=g_CADtoPath.GetCurSelOvlInfoMode(index);
        //ovl参数名称
        QtProperty *stringitem;
        stringitem=m_pMotionParaStringManager->addProperty(QStringLiteral("Ovl参数名: "));
        m_pMotionParaStringManager->setValue(stringitem, QString::fromStdString(ovlname));
        ui->MotionInfoTreeProperty->addProperty(stringitem);

        //ovl参数名称
        stringitem=m_pMotionParaStringManagerReadOnly->addProperty(QStringLiteral("Ovl参数类型: "));
        QString ovlparatype("");
        if(ovlmode==0)ovlparatype=QString("OVLABS");
        else if(ovlmode==1)ovlparatype=QString("OVLREL");
        else if(ovlmode==2)ovlparatype=QString("OVLSUPPOS");
        m_pMotionParaStringManagerReadOnly->setValue(stringitem, ovlparatype);
        ui->MotionInfoTreeProperty->addProperty(stringitem);

        QtProperty *groupitem;
        QtProperty * doubleitem;
        if(ovlmode==0)//OVLABS
        {
            groupitem=m_pMotionParaVarManager->addProperty(QtVariantPropertyManager::groupTypeId(), QStringLiteral("逼近绝对参数"));
            doubleitem=m_pMotionParaDoubleManager->addProperty(QStringLiteral("posDist: REAL "));
            m_pMotionParaDoubleManager->setDecimals(doubleitem, 0);
            m_pMotionParaDoubleManager->setValue(doubleitem,ovl[0]);
            groupitem->addSubProperty(doubleitem);
            doubleitem=m_pMotionParaDoubleManager->addProperty(QStringLiteral("OriDist: REAL "));
            m_pMotionParaDoubleManager->setDecimals(doubleitem, 0);
            m_pMotionParaDoubleManager->setValue(doubleitem,ovl[1]);
            groupitem->addSubProperty(doubleitem);
            doubleitem=m_pMotionParaDoubleManager->addProperty(QStringLiteral("linAxDist: REAL "));
            m_pMotionParaDoubleManager->setDecimals(doubleitem, 0);
            m_pMotionParaDoubleManager->setValue(doubleitem,ovl[2]);
            groupitem->addSubProperty(doubleitem);
            doubleitem=m_pMotionParaDoubleManager->addProperty(QStringLiteral("rotAxDist: REAL  "));
            m_pMotionParaDoubleManager->setDecimals(doubleitem, 0);
            m_pMotionParaDoubleManager->setValue(doubleitem,ovl[3]);
            groupitem->addSubProperty(doubleitem);
            QtProperty * boolitem;
            boolitem=m_pMotionParaBoolManager->addProperty(QStringLiteral("vConst: BOOL  "));
            if(ovl[4]==1)
                m_pMotionParaBoolManager->setValue(boolitem, true);
            else m_pMotionParaBoolManager->setValue(boolitem, false);
            groupitem->addSubProperty(boolitem);
            ui->MotionInfoTreeProperty->addProperty(groupitem);
            ui->MotionInfoTreeProperty->setFactoryForManager(m_pMotionParaBoolManager, m_pMotionParaCheckBoxFactory);
        }
        else if(ovlmode==1||ovlmode==2)//OVLREL
        {
            groupitem=m_pMotionParaVarManager->addProperty(QtVariantPropertyManager::groupTypeId(), QStringLiteral("逼近相对参数"));
            doubleitem=m_pMotionParaDoubleManager->addProperty(QStringLiteral("ovl: PERC200 "));
            m_pMotionParaDoubleManager->setDecimals(doubleitem, 0);
            m_pMotionParaDoubleManager->setValue(doubleitem,ovl[0]);
            groupitem->addSubProperty(doubleitem);
            ui->MotionInfoTreeProperty->addProperty(groupitem);
        }

        ui->MotionInfoTreeProperty->setFactoryForManager(m_pMotionParaStringManager, m_pMotionParaLineEditFactory);
        ui->MotionInfoTreeProperty->setFactoryForManager(m_pMotionParaDoubleManager, m_pMotionParaDoubleSpinFactory);

    }
}
//动态Dyn参数string值（参数名）变更响应
void MainWindow::MotionParaDynStringValueChanged(QtProperty *property, const QString &value)
{
    QString paraname=curSelTreeItemofDynInfoorOvlInfo->text(0);
    QString propertyname=property->propertyName();
    if(propertyname.compare(QStringLiteral("Dyn参数名: "))==0)
    {
        if(g_CADtoPath.ModifyDynParaName(paraname.toStdString(), value.toStdString()))
        {
            curSelTreeItemofDynInfoorOvlInfo->setText(0, value);
            curSelTreeItemofDynInfoorOvlInfo->setBackgroundColor(0, QColor(255, 255, 255));
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 255, 255));
            ui->statusBar->showMessage(QStringLiteral(""));
        }
        else
        {
            curSelTreeItemofDynInfoorOvlInfo->setBackgroundColor(0, QColor(255, 0, 0));
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("Dyn参数名更改失败，或已使用或存在重命名！"));
        }
    }
}
//动态Dyn参数double值（12项参数）变更响应
void MainWindow::MotionParaDynDoubleValueChanged(QtProperty *property, const double &value)
{
    QString paraname=curSelTreeItemofDynInfoorOvlInfo->text(0);
    QString propertyname=property->propertyName();
    if(propertyname.compare(QStringLiteral("velAxis: PERCENT "))==0)//velAxis
    {
        if(g_CADtoPath.ModifyDynParaInfo(paraname.toStdString(),value, 0))
            ui->statusBar->showMessage(QStringLiteral("参数数据修改！"));
        else
        {
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
        }
    }
    else if(propertyname.compare(QStringLiteral("accAxis: PERCENT "))==0)//accAxis
    {
        if(g_CADtoPath.ModifyDynParaInfo(paraname.toStdString(),value, 1))
            ui->statusBar->showMessage(QStringLiteral("参数数据修改！"));
        else
        {
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
        }
    }
    else if(propertyname.compare(QStringLiteral("decAxis: PERCENT "))==0)//decAxis: PERCENT
    {
        if(g_CADtoPath.ModifyDynParaInfo(paraname.toStdString(),value, 2))
            ui->statusBar->showMessage(QStringLiteral("参数数据修改！"));
        else
        {
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
        }
    }
    else if(propertyname.compare(QStringLiteral("jerkAxis: PERCENT "))==0)//jerkAxis: PERCENT
    {
        if(g_CADtoPath.ModifyDynParaInfo(paraname.toStdString(),value, 3))
            ui->statusBar->showMessage(QStringLiteral("参数数据修改！"));
        else
        {
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
        }
    }
    else if(propertyname.compare(QStringLiteral("vel: REAL "))==0)//vel: REAL
    {
        if(g_CADtoPath.ModifyDynParaInfo(paraname.toStdString(),value, 4))
            ui->statusBar->showMessage(QStringLiteral("参数数据修改！"));
        else
        {
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
        }
    }
    else if(propertyname.compare(QStringLiteral("acc: REAL "))==0)//acc: REAL
    {
        if(g_CADtoPath.ModifyDynParaInfo(paraname.toStdString(),value, 5))
            ui->statusBar->showMessage(QStringLiteral("参数数据修改！"));
        else
        {
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
        }
    }
    else if(propertyname.compare(QStringLiteral("dec: REAL "))==0)//dec: REAL
    {
        if(g_CADtoPath.ModifyDynParaInfo(paraname.toStdString(),value, 6))
            ui->statusBar->showMessage(QStringLiteral("参数数据修改！"));
        else
        {
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
        }
    }
    else if(propertyname.compare(QStringLiteral("jerk: REAL "))==0)//jerk: REAL
    {
        if(g_CADtoPath.ModifyDynParaInfo(paraname.toStdString(),value, 7))
            ui->statusBar->showMessage(QStringLiteral("参数数据修改！"));
        else
        {
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
        }
    }
    else if(propertyname.compare(QStringLiteral("velOri: REAL "))==0)//velOri: REAL
    {
        if(g_CADtoPath.ModifyDynParaInfo(paraname.toStdString(),value, 8))
            ui->statusBar->showMessage(QStringLiteral("参数数据修改！"));
        else
        {
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
        }
    }
    else if(propertyname.compare(QStringLiteral("accOri: REAL "))==0)//accOri: REAL
    {
        if(g_CADtoPath.ModifyDynParaInfo(paraname.toStdString(),value, 9))
            ui->statusBar->showMessage(QStringLiteral("参数数据修改！"));
        else
        {
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
        }
    }
    else if(propertyname.compare(QStringLiteral("decOri: REAL "))==0)//decOri: REAL
    {
        if(g_CADtoPath.ModifyDynParaInfo(paraname.toStdString(),value, 10))
            ui->statusBar->showMessage(QStringLiteral("参数数据修改！"));
        else
        {
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
        }
    }
    else if(propertyname.compare(QStringLiteral("jerkOri: REAL "))==0)//jerkOri: REAL
    {
        if(g_CADtoPath.ModifyDynParaInfo(paraname.toStdString(),value, 11))
            ui->statusBar->showMessage(QStringLiteral("参数数据修改！"));
        else
        {
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
        }
    }
}
//动态Ovl参数string值（参数名）变更响应
void MainWindow::MotionParaOvlStringValueChanged(QtProperty *property, const QString &value)
{
    QString paraname=curSelTreeItemofDynInfoorOvlInfo->text(0);
    QString propertyname=property->propertyName();
    if(propertyname.compare(QStringLiteral("Ovl参数名: "))==0)
    {
        if(g_CADtoPath.ModifyOvlParaName(paraname.toStdString(), value.toStdString()))
        {
            curSelTreeItemofDynInfoorOvlInfo->setText(0, value);
            curSelTreeItemofDynInfoorOvlInfo->setBackgroundColor(0, QColor(255, 255, 255));
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 255, 255));
            ui->statusBar->showMessage(QStringLiteral(""));
        }
        else
        {
            curSelTreeItemofDynInfoorOvlInfo->setBackgroundColor(0, QColor(255, 0, 0));
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("Ovl参数名更改失败，或已使用或存在重命名！"));
        }
    }
}
//动态Ovl参数double值（）变更响应
void MainWindow::MotionParaOvlDoubleValueChanged(QtProperty *property, const double &value)
{
    QString paraname=curSelTreeItemofDynInfoorOvlInfo->text(0);
    QString propertyname=property->propertyName();
    if(propertyname.compare(QStringLiteral("ovl: PERC200 "))==0)//OVLREL, OVLSUPPOS
    {
        if(g_CADtoPath.ModifyOvlParaInfo(paraname.toStdString(),value, 0))
            ui->statusBar->showMessage(QStringLiteral("参数数据修改！"));
        else
        {
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
        }
    }
    if(propertyname.compare(QStringLiteral("posDist: REAL "))==0)//posDist:
    {
        if(g_CADtoPath.ModifyOvlParaInfo(paraname.toStdString(),value, 0))
            ui->statusBar->showMessage(QStringLiteral("参数数据修改！"));
        else
        {
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
        }
    }
    else if(propertyname.compare(QStringLiteral("OriDist: REAL "))==0)//OriDist:
    {
        if(g_CADtoPath.ModifyOvlParaInfo(paraname.toStdString(),value, 1))
            ui->statusBar->showMessage(QStringLiteral("参数数据修改！"));
        else
        {
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
        }
    }
    else if(propertyname.compare(QStringLiteral("linAxDist: REAL "))==0)//linAxDist: REAL
    {
        if(g_CADtoPath.ModifyOvlParaInfo(paraname.toStdString(),value, 2))
            ui->statusBar->showMessage(QStringLiteral("参数数据修改！"));
        else
        {
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
        }
    }
    else if(propertyname.compare(QStringLiteral("rotAxDist: REAL  "))==0)//rotAxDist: REAL
    {
        if(g_CADtoPath.ModifyOvlParaInfo(paraname.toStdString(),value, 3))
            ui->statusBar->showMessage(QStringLiteral("参数数据修改！"));
        else
        {
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
        }
    }

}
//动态Ovl参数Bool值（vConst）变更响应
void MainWindow::MotionParaOvlBoolValueChanged(QtProperty *property, const bool value)
{
    double num=(value) ? 1:0;
    QString paraname=curSelTreeItemofDynInfoorOvlInfo->text(0);
    QString propertyname=property->propertyName();
    if(propertyname.compare(QStringLiteral("vConst: BOOL  "))==0)//vConst: BOOL
    {
        if(g_CADtoPath.ModifyOvlParaInfo(paraname.toStdString(),num, 4))
            ui->statusBar->showMessage(QStringLiteral("参数数据修改！"));
        else
        {
            ui->MotionInfoTreeProperty->setBackgroundColor(curSelMotionParaPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
        }
    }
}
//轨迹列表的初始化显示，文件读取时执行
void MainWindow::InitializePathsTreeViewShow()
{
    //初始化程序预览及轨迹列表
    ui->ProgramsNameList->clear();
    ProgramConsModel->removeRows(0, ProgramConsModel->rowCount());
    int rowcount=PathsTreeViewModel->rowCount();
    PathsTreeViewModel->removeRows(0, rowcount);

    QStandardItem *root, *leaf;
    QString rootname, leafname;
    //显示轨迹信息
    for(size_t i=0;i<g_CADtoPath.m_MultiPathInfoList.size();i++)
    {
        rootname=QString::fromStdString(g_CADtoPath.m_MultiPathInfoList[i].name);
        root = new QStandardItem(rootname);
        PathsTreeViewModel->setItem(i, 0, root);
        PathsTreeViewModel->setItem(i,1,  new QStandardItem(QStringLiteral("轨迹")));
        //轨迹进入点
        if(g_CADtoPath.m_MultiPathInfoList[i].gppara.InpointEnabled)
        {
            leafname=QString::fromStdString(g_CADtoPath.m_MultiPathInfoList[i].gppara.Inpoint.name);
            leaf = new QStandardItem(leafname);
            root->setChild(0, 0, leaf);
            root->setChild(0, 1, new QStandardItem(QStringLiteral("进入点")));
        }
        //轨迹
        for(size_t j=0;j<g_CADtoPath.m_MultiPathInfoList[i].onepath.size();j++)
        {
            leafname=QString::fromStdString(g_CADtoPath.m_MultiPathInfoList[i].onepath[j].name);
            leaf = new QStandardItem(leafname);
            root->appendRow(leaf);
            root->setChild(leaf->index().row(), 1, new QStandardItem(QStringLiteral("点位")));
        }
        //轨迹脱离点
        if(g_CADtoPath.m_MultiPathInfoList[i].gppara.OutpointEnabled)
        {
            leafname=QString::fromStdString(g_CADtoPath.m_MultiPathInfoList[i].gppara.Outpoint.name);
            leaf = new QStandardItem(leafname);
            root->appendRow(leaf);
            root->setChild(leaf->index().row(), 1, new QStandardItem(QStringLiteral("脱离点")));
        }
    }
}

void MainWindow::ShowTreeViewProperty(const QModelIndex &mindex)
{
    if(! mindex.isValid()) return;
    if(mindex.parent().isValid())//子节点 点位
    {
        ui->PathsProperty->hide();
        ui->PointsProperty->show();
        ShowPointsProperty();
    }
    else //根节点 轨迹
    {
        ui->PathsProperty->show();
        ui->PointsProperty->hide();
        ShowPathsProperty();
    }
}
//在轨迹列表的rowindex位置插入一条新的轨迹
//数据及排序与数据m_MultiPathInfoList中保持一致
bool MainWindow::InsertOnePathofPathsTreeView(const int rowindex)
{
    if(rowindex<0||rowindex>g_CADtoPath.m_MultiPathInfoList.size()-1)
        return false;

    QStandardItem *root, *leaf;
    QString rootname, leafname;
    rootname=QString::fromStdString(g_CADtoPath.m_MultiPathInfoList[rowindex].name);
    root = new QStandardItem(rootname);
    PathsTreeViewModel->insertRow(rowindex, root);
    PathsTreeViewModel->setItem(rowindex,1,  new QStandardItem(QStringLiteral("轨迹")));

    if(g_CADtoPath.m_MultiPathInfoList[rowindex].gppara.InpointEnabled)
    {
        leafname=QString::fromStdString(g_CADtoPath.m_MultiPathInfoList[rowindex].gppara.Inpoint.name);
        leaf = new QStandardItem(leafname);
        root->setChild(0, 0, leaf);
        root->setChild(0, 1, new QStandardItem(QStringLiteral("进入点")));
    }
    for(size_t j=0;j<g_CADtoPath.m_MultiPathInfoList[rowindex].onepath.size();j++)
    {
        leafname=QString::fromStdString(g_CADtoPath.m_MultiPathInfoList[rowindex].onepath[j].name);
        leaf = new QStandardItem(leafname);
        root->appendRow(leaf);
        root->setChild(leaf->index().row(), 1, new QStandardItem(QStringLiteral("点位")));
    }
    if(g_CADtoPath.m_MultiPathInfoList[rowindex].gppara.OutpointEnabled)
    {
        leafname=QString::fromStdString(g_CADtoPath.m_MultiPathInfoList[rowindex].gppara.Outpoint.name);
        leaf = new QStandardItem(leafname);
        root->appendRow(leaf);
        root->setChild(leaf->index().row(), 1, new QStandardItem(QStringLiteral("脱离点")));
    }
    return true;
}
//在轨迹显示列表的rowindex条轨迹上增加辅助点显示
bool MainWindow::InsertAssistPointofPahtsTreeVeiw(const int rowindex, const int apmode)
{
    if(rowindex<0||rowindex>g_CADtoPath.m_MultiPathInfoList.size()-1)return false;
    QModelIndex cpindex=PathsTreeViewModel->index(rowindex,0);
    QStandardItem *m_curselitem=PathsTreeViewModel->item(rowindex);
    QStandardItem *m_insertitem;
    QString pname,curptype;
    if(apmode==0)//Inpoint
    {
        curptype=m_curselitem->child(0,1)->text();
        pname=QString::fromStdString(g_CADtoPath.m_MultiPathInfoList[rowindex].gppara.Inpoint.name);

        if(curptype == QStringLiteral("进入点"))//如已经存在进入点，则修改该进入点的name
        {
            m_curselitem->child(0,0)->setText(pname);
        }
        else//不存在则插入进入点
        {
            m_insertitem = new QStandardItem(pname);
            m_curselitem->insertRow(0, m_insertitem);
            m_curselitem->setChild(0,1,new QStandardItem(QStringLiteral("进入点")));
        }
    }
    else if(apmode==1)//OutPoint
    {
        int childrownum=m_curselitem->rowCount();

        curptype=m_curselitem->child(childrownum-1, 1)->text();
        pname=QString::fromStdString(g_CADtoPath.m_MultiPathInfoList[rowindex].gppara.Outpoint.name);
        if(curptype == QStringLiteral("脱离点"))
        {
            m_curselitem->child(childrownum-1, 0)->setText(pname);
        }
        else
        {
            m_insertitem = new QStandardItem(pname);
            m_curselitem->appendRow(m_insertitem);
            m_curselitem->setChild(m_insertitem->index().row(),1,new QStandardItem(QStringLiteral("脱离点")));
        }
    }
    else return false;
}
//在轨迹显示列表中移除一个item
bool MainWindow::RemoveItemsofPathsTreeView(const QModelIndex &index)
{
    if(index.isValid())
    {
        if(index.parent().isValid())//child
            PathsTreeViewModel->removeRow(index.row(), index.parent());
        else
        {
            int num=PathsTreeViewModel->rowCount(index);
            for(int i=0;i<num;i++)
                PathsTreeViewModel->removeRow(0, index);
            PathsTreeViewModel->removeRow(index.row());
        }
        return true;
    }
    return false;
}
//显示轨迹分裂后的轨迹列表
void MainWindow::ShowPathsTreeViewSplitting()
{
    if(curSelIndex.isValid()&&curSelIndex.parent().isValid())
    {
        //移除
        int num=PathsTreeViewModel->rowCount(curSelIndex.parent());
        for(int i=curSelIndex.row()+1;i<num;i++)
            PathsTreeViewModel->removeRow(curSelIndex.row()+1, curSelIndex.parent());

        InsertOnePathofPathsTreeView(curSelIndex.parent().row()+1);
    }
}
//获取轨迹列表视图上当前鼠标点击的item
QStandardItem *MainWindow::GetCurSelStandardItem(int columnindex)
{
    if(columnindex==0||columnindex==1)
    {
        if(curSelIndex.isValid())
        {
            if(curSelIndex.parent().isValid())//child
                return  PathsTreeViewModel->itemFromIndex(curSelIndex.parent())->child(curSelIndex.row(),columnindex);
            else return PathsTreeViewModel->item(curSelIndex.row(), columnindex);
        }
        else return NULL;
    }
    else return NULL;

}
//将m_ProjectProgram内容显示在程序预览widget中
void MainWindow::ShowProgramPreview(const int index)
{
    if(index<0||index>g_CADtoPath.m_ProjectProgram.size()-1)
        return;

    ProgramConsModel->removeRows(0, ProgramConsModel->rowCount());
    QStringList orderlist;
    orderlist.clear();
    for(size_t i=0;i<g_CADtoPath.m_ProjectProgram[index].orderslist.size();i++)
    {
        orderlist<<QString::fromStdString(g_CADtoPath.m_ProjectProgram[index].orderslist[i]);
    }
    ProgramConsModel->setStringList(orderlist);

}
//显示点位属性：包括点名称、点类型、位置、动态参数（Dyn、Ovl）
void MainWindow::ShowPointsProperty()
{
    ui->PointsProperty->clear();
    string pointname,ptype;
    double pos[6];
    QString commenttype=GetCurSelStandardItem(1)->text();
    if(commenttype==QStringLiteral("点位"))
        g_CADtoPath.GetCurInformationofSelPointinOneWholePath(pointname, pos, ptype);
    else if(commenttype==QStringLiteral("进入点"))
        g_CADtoPath.GetPointInformationofcurSelPathAssistPoint(pointname, pos, ptype, 0);
    else if(commenttype==QStringLiteral("脱离点"))
        g_CADtoPath.GetPointInformationofcurSelPathAssistPoint(pointname, pos, ptype, 1);
    else
        return;

    //属性栏操作
    QtStringPropertyManager *m_pPointsStringManager, *m_pPointsStringManagerReadOnly;
    QtLineEditFactory *m_pPointsLineEditFactory;

    QtVariantPropertyManager *m_pPointsVarManager;

    QtDoublePropertyManager *m_pPointsDoubleManager;
    QtDoubleSpinBoxFactory *m_pPointsDoubleSpinFactory;

    QtEnumPropertyManager *m_pPointsEnumManager;
    QtEnumEditorFactory *m_pPointsEnumFactory;
    //新建属性栏中variable的manager和factory变量
    m_pPointsStringManager = new QtStringPropertyManager(ui->PointsProperty);
    m_pPointsStringManagerReadOnly = new QtStringPropertyManager(ui->PointsProperty);
    m_pPointsLineEditFactory = new QtLineEditFactory(ui->PointsProperty);

    m_pPointsVarManager = new QtVariantPropertyManager(ui->PointsProperty);

    m_pPointsDoubleManager = new QtDoublePropertyManager(ui->PointsProperty);
    m_pPointsDoubleSpinFactory = new QtDoubleSpinBoxFactory(ui->PointsProperty);

    m_pPointsEnumManager = new QtEnumPropertyManager(ui->PointsProperty);
    m_pPointsEnumFactory = new QtEnumEditorFactory(ui->PointsProperty);
    //连接variable项数据更改的信号和槽函数
    connect(m_pPointsStringManager, &QtStringPropertyManager::valueChanged, this, &MainWindow::PointsPropertyStringValueChanged);
    connect(m_pPointsDoubleManager, &QtDoublePropertyManager::valueChanged, this, &MainWindow::PointsPropertyDoubleValueChanged);
    connect(m_pPointsEnumManager, &QtEnumPropertyManager::valueChanged, this, &MainWindow::PointsPropertyEnumValueChanged);

    //点位名称
    QtProperty *stringitem;
    stringitem=m_pPointsStringManager->addProperty(QStringLiteral("点位名: "));
    m_pPointsStringManager->setValue(stringitem, QString::fromStdString(pointname));
    ui->PointsProperty->addProperty(stringitem);

    stringitem = m_pPointsStringManagerReadOnly->addProperty(QStringLiteral("点类型: "));
    m_pPointsStringManagerReadOnly->setValue(stringitem, QString::fromStdString(ptype));
    ui->PointsProperty->addProperty(stringitem);
    //添加属性表中位置信息
    QtProperty *groupitem=m_pPointsVarManager->addProperty(QtVariantPropertyManager::groupTypeId(), QStringLiteral("位姿"));
    QtProperty * doubleitem;
    doubleitem=m_pPointsDoubleManager->addProperty(QStringLiteral("X: "));
    m_pPointsDoubleManager->setDecimals(doubleitem, 3);
    m_pPointsDoubleManager->setValue(doubleitem,pos[0]);
    groupitem->addSubProperty(doubleitem);

    doubleitem=m_pPointsDoubleManager->addProperty(QStringLiteral("Y: "));
    m_pPointsDoubleManager->setDecimals(doubleitem, 3);
    m_pPointsDoubleManager->setValue(doubleitem,pos[1]);
    groupitem->addSubProperty(doubleitem);

    doubleitem=m_pPointsDoubleManager->addProperty(QStringLiteral("Z: "));
    m_pPointsDoubleManager->setDecimals(doubleitem, 3);
    m_pPointsDoubleManager->setValue(doubleitem,pos[2]);
    groupitem->addSubProperty(doubleitem);

    doubleitem=m_pPointsDoubleManager->addProperty(QStringLiteral("A: "));
    m_pPointsDoubleManager->setDecimals(doubleitem, 3);
    m_pPointsDoubleManager->setValue(doubleitem,pos[3]);
    groupitem->addSubProperty(doubleitem);

    doubleitem=m_pPointsDoubleManager->addProperty(QStringLiteral("B: "));
    m_pPointsDoubleManager->setValue(doubleitem,pos[4]);
    groupitem->addSubProperty(doubleitem);

    doubleitem=m_pPointsDoubleManager->addProperty(QStringLiteral("C: "));
    m_pPointsDoubleManager->setDecimals(doubleitem, 3);
    m_pPointsDoubleManager->setValue(doubleitem,pos[5]);
    groupitem->addSubProperty(doubleitem);

    ui->PointsProperty->addProperty(groupitem);

    if(ptype!=string("Arc_MPoint"))
    {
        QtProperty *enumitem;
        QStringList dynparaslist, ovlparaslist;
        dynparaslist.clear();
        ovlparaslist.clear();
        groupitem=m_pPointsVarManager->addProperty(QtVariantPropertyManager::groupTypeId(), QStringLiteral("Dyn参数"));
        enumitem=m_pPointsEnumManager->addProperty(QStringLiteral("Dyn参数"));
        dynparaslist<<"None";
        for(size_t i=0;i<g_CADtoPath.m_DynVarList.size();i++)
            dynparaslist<<QString::fromStdString(g_CADtoPath.m_DynVarList[i].name);
        int dynindex=g_CADtoPath.GetIndexofDynInfobyCurSelPoint()+1;
        m_pPointsEnumManager->setEnumNames(enumitem, dynparaslist);
        m_pPointsEnumManager->setValue(enumitem, dynindex);
        groupitem->addSubProperty(enumitem);
        ui->PointsProperty->addProperty(groupitem);

        groupitem=m_pPointsVarManager->addProperty(QtVariantPropertyManager::groupTypeId(), QStringLiteral("Ovl参数"));
        enumitem=m_pPointsEnumManager->addProperty(QStringLiteral("Ovl参数"));
        ovlparaslist<<"None";
        for(size_t i=0;i<g_CADtoPath.m_OvlVarList.size();i++)
            ovlparaslist<<QString::fromStdString(g_CADtoPath.m_OvlVarList[i].name);
        int ovlindex=g_CADtoPath.GetIndexofOvlInfobyCurSelPoint()+1;
        m_pPointsEnumManager->setEnumNames(enumitem, ovlparaslist);
        m_pPointsEnumManager->setValue(enumitem, ovlindex);
        groupitem->addSubProperty(enumitem);
        ui->PointsProperty->addProperty(groupitem);

        ui->PointsProperty->setFactoryForManager(m_pPointsEnumManager, m_pPointsEnumFactory);

    }
    ui->PointsProperty->setFactoryForManager(m_pPointsStringManager, m_pPointsLineEditFactory);
    ui->PointsProperty->setFactoryForManager(m_pPointsDoubleManager, m_pPointsDoubleSpinFactory);
}

void MainWindow::on_DefaultView_triggered()
{
    if(cur_showmode==TwoDimension)//2D
    {
        emit loaddefaultview2D();
    }
    else if(cur_showmode==ThreeDimension)//3D
    {
        emit loadsetupview3D(0);
    }
}

void MainWindow::on_FrontView_triggered()
{
    emit loadsetupview3D(1);
}

void MainWindow::on_BackView_triggered()
{
    emit loadsetupview3D(2);
}

void MainWindow::on_LeftView_triggered()
{
    emit loadsetupview3D(3);
}

void MainWindow::on_RightView_triggered()
{
    emit loadsetupview3D(4);
}

void MainWindow::on_UpView_triggered()
{
    emit loadsetupview3D(5);
}

void MainWindow::on_BottomView_triggered()
{
    emit loadsetupview3D(6);
}
//轨迹string参数修改响应（轨迹名），包含重命名判断显示
void MainWindow::PathsPropertyStringValueChanged(QtProperty *property, const QString &value)
{
    QStandardItem *curselItem=GetCurSelStandardItem();
    QString propertyname=property->propertyName();
    if(propertyname.compare(QStringLiteral("轨迹名: "))==0)
    {
        if(g_CADtoPath.ModifyoneWholePathName(value.toStdString()))
        {
            if(curselItem!=NULL)
            {
                curselItem->setText(value);
                curselItem->setBackground(QColor(255, 255, 255));
            }
            ui->PathsProperty->setBackgroundColor(curSelPathPropertyItem, QColor(255, 255, 255));
            ui->statusBar->showMessage(QStringLiteral(""));
        }
        else
        {  
            if(curselItem!=NULL)
                curselItem->setBackground(QColor(255, 0, 0));
            ui->PathsProperty->setBackgroundColor(curSelPathPropertyItem, QColor(255, 0, 0));
            ui->statusBar->showMessage(QStringLiteral("轨迹路径存在重命名！"));
        }
    }
}
//轨迹动态参数（Dyn、Ovl）修改响应
void MainWindow::PathsPropertyEnumValueChanged(QtProperty *property, const int value)
{
    if(property->propertyName()==QStringLiteral("Dyn参数"))
    {
        if(property->valueText()!=QString("None"))
            g_CADtoPath.SetDynParaofSelOneWholePath(property->valueText().toStdString());
    }
    else if(property->propertyName()==QStringLiteral("Ovl参数"))
    {
        if(property->valueText()!=QString("None"))
            g_CADtoPath.SetOvlParaofSelOneWholePath(property->valueText().toStdString());
    }
}
//轨迹bool参数（点到位情况）修改响应
void MainWindow::PathsPropertyBoolValueChanged(QtProperty *property, const bool value)
{
    if(property->propertyName()==QStringLiteral("启动点"))
        g_CADtoPath.SetAPWaitFinishedofCurrentPath(value, 0);
    else if(property->propertyName()==QStringLiteral("结束点"))
        g_CADtoPath.SetAPWaitFinishedofCurrentPath(value, 1);
}
//位置点double参数修改响应
void MainWindow::PointsPropertyDoubleValueChanged(QtProperty *property, const double &value)
{
    QString pnamemode=GetCurSelStandardItem(1)->text();
    QString propertyname=property->propertyName();
    if(pnamemode==QStringLiteral("点位"))
    {
        if(propertyname.compare(QStringLiteral("X: "))==0)//X
        {
            if(g_CADtoPath.ModifyonePosition(value, 0))
                ui->statusBar->showMessage(QStringLiteral("点位数据修改！"));
            else
            {
                ui->PointsProperty->setBackgroundColor(curSelPointPropertyItem, QColor(255, 0, 0));
                ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
            }
            ui->OpenGL2D->update();//窗口重绘
        }
        else if(propertyname.compare(QStringLiteral("Y: "))==0)//Y
        {
            if(g_CADtoPath.ModifyonePosition(value, 1))
                ui->statusBar->showMessage(QStringLiteral("点位数据修改！"));
            else
            {
                ui->PointsProperty->setBackgroundColor(curSelPointPropertyItem, QColor(255, 0, 0));
                ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
            }
            ui->OpenGL2D->update();//窗口重绘
        }
        else if(propertyname.compare(QStringLiteral("Z: "))==0)//Z
        {
            if(g_CADtoPath.ModifyonePosition(value, 2))
                ui->statusBar->showMessage(QStringLiteral("点位数据修改！"));
            else
            {
                ui->PointsProperty->setBackgroundColor(curSelPointPropertyItem, QColor(255, 0, 0));
                ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
            }
            ui->OpenGL2D->update();//窗口重绘
        }
        else if(propertyname.compare(QStringLiteral("A: "))==0)//A
        {
            if(g_CADtoPath.ModifyonePosition(value, 3))
                ui->statusBar->showMessage(QStringLiteral("点位数据修改！"));
            else
            {
                ui->PointsProperty->setBackgroundColor(curSelPointPropertyItem, QColor(255, 0, 0));
                ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
            }
            ui->OpenGL2D->update();//窗口重绘
        }
        else if(propertyname.compare(QStringLiteral("B: "))==0)//B
        {
            if(g_CADtoPath.ModifyonePosition(value, 4))
                ui->statusBar->showMessage(QStringLiteral("点位数据修改！"));
            else
            {
                ui->PointsProperty->setBackgroundColor(curSelPointPropertyItem, QColor(255, 0, 0));
                ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
            }
            ui->OpenGL2D->update();//窗口重绘
        }
        else if(propertyname.compare(QStringLiteral("C: "))==0)//C
        {
            if(g_CADtoPath.ModifyonePosition(value, 5))
                ui->statusBar->showMessage(QStringLiteral("点位数据修改！"));
            else
            {
                ui->PointsProperty->setBackgroundColor(curSelPointPropertyItem, QColor(255, 0, 0));
                ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
            }
            ui->OpenGL2D->update();//窗口重绘
        }
    }
    else if(pnamemode==QStringLiteral("进入点") || pnamemode==QStringLiteral("脱离点"))
    {
        int pmodeindex;
        if(pnamemode==QStringLiteral("进入点"))pmodeindex=0;
        else pmodeindex=1;
        if(propertyname.compare(QStringLiteral("X: "))==0)//X
        {
            if(g_CADtoPath.ModifyAssistPointforCurSelPath(value, 0, pmodeindex))
                ui->statusBar->showMessage(QStringLiteral("点位数据修改！"));
            else
            {
                ui->PointsProperty->setBackgroundColor(curSelPointPropertyItem, QColor(255, 0, 0));
                ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
            }
        }
        else if(propertyname.compare(QStringLiteral("Y: "))==0)//Y
        {
            if(g_CADtoPath.ModifyAssistPointforCurSelPath(value, 1, pmodeindex))
                ui->statusBar->showMessage(QStringLiteral("点位数据修改！"));
            else
            {
                ui->PointsProperty->setBackgroundColor(curSelPointPropertyItem, QColor(255, 0, 0));
                ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
            }
        }
        else if(propertyname.compare(QStringLiteral("Z: "))==0)//Z
        {
            if(g_CADtoPath.ModifyAssistPointforCurSelPath(value, 2, pmodeindex))
                ui->statusBar->showMessage(QStringLiteral("点位数据修改！"));
            else
            {
                ui->PointsProperty->setBackgroundColor(curSelPointPropertyItem, QColor(255, 0, 0));
                ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
            }
        }
        else if(propertyname.compare(QStringLiteral("A: "))==0)//A
        {
            if(g_CADtoPath.ModifyAssistPointforCurSelPath(value, 3, pmodeindex))
                ui->statusBar->showMessage(QStringLiteral("点位数据修改！"));
            else
            {
                ui->PointsProperty->setBackgroundColor(curSelPointPropertyItem, QColor(255, 0, 0));
                ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
            }
        }
        else if(propertyname.compare(QStringLiteral("B: "))==0)//B
        {
            if(g_CADtoPath.ModifyAssistPointforCurSelPath(value, 4, pmodeindex))
                ui->statusBar->showMessage(QStringLiteral("点位数据修改！"));
            else
            {
                ui->PointsProperty->setBackgroundColor(curSelPointPropertyItem, QColor(255, 0, 0));
                ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
            }
        }
        else if(propertyname.compare(QStringLiteral("C: "))==0)//C
        {
            if(g_CADtoPath.ModifyAssistPointforCurSelPath(value, 5, pmodeindex))
                ui->statusBar->showMessage(QStringLiteral("点位数据修改！"));
            else
            {
                ui->PointsProperty->setBackgroundColor(curSelPointPropertyItem, QColor(255, 0, 0));
                ui->statusBar->showMessage(QStringLiteral("数据转换不正确！"));
            }
        }
    }
}
//位置点动态参数（Dyn、Ovl）修改响应
void MainWindow::PointsPropertyEnumValueChanged(QtProperty *property, const int value)
{
    QString pnamemode=GetCurSelStandardItem(1)->text();
    if(pnamemode==QStringLiteral("点位"))
    {
        if(property->propertyName()==QStringLiteral("Dyn参数")&&property->valueText()!=QString("None"))
            g_CADtoPath.SetDynParaofSelPointinOneWholePath(property->valueText().toStdString());
        else if(property->propertyName()==QStringLiteral("Ovl参数")&&property->valueText()!=QString("None"))
            g_CADtoPath.SetOvlParaofSelPointinOneWholePath(property->valueText().toStdString());
    }
    else if(pnamemode==QStringLiteral("进入点")||pnamemode==QStringLiteral("脱离点"))
    {
        int pmodeindex;
        if(pnamemode==QStringLiteral("进入点"))pmodeindex=0;
        else pmodeindex=1;

        if(property->propertyName()==QStringLiteral("Dyn参数")&&property->valueText()!=QString("None"))
            g_CADtoPath.SetAPDynParaofCurSelPath(property->valueText().toStdString(), pmodeindex);
        else if(property->propertyName()==QStringLiteral("Ovl参数")&&property->valueText()!=QString("None"))
            g_CADtoPath.SetAPOvlParaofCurSelPath(property->valueText().toStdString(), pmodeindex);
    }

}
//位置点string参数（点位名称）修改响应
void MainWindow::PointsPropertyStringValueChanged(QtProperty *property, const QString &value)
{
    QStandardItem *curselItem = GetCurSelStandardItem();
    QString pnamemode=GetCurSelStandardItem(1)->text();
    QString propertyname=property->propertyName();
    if(pnamemode==QStringLiteral("点位"))
    {
        if(propertyname.compare(QStringLiteral("点位名: "))==0)//修改点名称
        {
            if(g_CADtoPath.ModifyonePositionName(value.toStdString()))
            {
                if(curselItem != NULL)
                {
                    curselItem->setText(value);
                    curselItem->setBackground(QColor(255, 255, 255));
                }
                ui->PointsProperty->setBackgroundColor(curSelPointPropertyItem, QColor(255, 255, 255));
                ui->statusBar->showMessage(QStringLiteral(""));
            }
            else
            {
                if(curselItem!=NULL)
                    curselItem->setBackground(QColor(255, 0, 0));
                ui->PointsProperty->setBackgroundColor(curSelPointPropertyItem, QColor(255, 0, 0));
                ui->statusBar->showMessage(QStringLiteral("轨迹点存在重命名！"));
            }
        }
    }
    else if(pnamemode==QStringLiteral("进入点")||pnamemode==QStringLiteral("脱离点"))
    {
        int pmodeindex;
        if(pnamemode==QStringLiteral("进入点"))pmodeindex=0;
        else pmodeindex=1;

        if(propertyname.compare(QStringLiteral("点位名: "))==0)//修改点名称
        {
            if(g_CADtoPath.ModifyAPNameofCurSelPath(value.toStdString(), pmodeindex))
            {
                if(curselItem != NULL)
                {
                    curselItem->setText(value);
                    curselItem->setBackground(QColor(255, 255, 255));
                }
                ui->PointsProperty->setBackgroundColor(curSelPointPropertyItem, QColor(255, 255, 255));
                ui->statusBar->showMessage(QStringLiteral(""));
            }
            else
            {
                if(curselItem!=NULL)
                    curselItem->setBackground(QColor(255, 0, 0));
                ui->PointsProperty->setBackgroundColor(curSelPointPropertyItem, QColor(255, 0, 0));
                ui->statusBar->showMessage(QStringLiteral("轨迹点存在重命名！"));
            }
        }
    }
}

//显示与当前轨迹合并的另一条轨迹
void MainWindow::PathMergingInputDialogSelectItem(const int w_index)
{
    g_CADtoPath.SetCurIndexofSelOneWholePath_Another(w_index);
    ui->OpenGL2D->update();
}
//2D dxf文件打开
void MainWindow::on_OpenFile_2D_triggered()
{

}

void MainWindow::on_action_Import_Dxf_triggered()
{
    //如项目已打开，提示是否保存项目
    //TODO:
    QString filepath("");
    filepath = QFileDialog::getOpenFileName(this, tr("Data Files"), ".", tr("DXF Files(*.dxf)"));
    if(!filepath.isEmpty())
    {
        if(ReadDXFData(filepath))
        {
            //根据读取的数据重新绘制窗口
            double xyzrange_2d[6], rmax_x,rmax_y;
            g_CADtoPath.GetRangeCuboid(xyzrange_2d);
            rmax_x = fabs(xyzrange_2d[0]) > fabs(xyzrange_2d[1]) ? fabs(xyzrange_2d[0]) : fabs(xyzrange_2d[1]);
            rmax_y = fabs(xyzrange_2d[2]) > fabs(xyzrange_2d[3]) ? fabs(xyzrange_2d[2]) : fabs(xyzrange_2d[3]);
//            qDebug()<<xyzrange_2d[0]<<" "<<xyzrange_2d[1]<<" "<<xyzrange_2d[2]<<" "<<xyzrange_2d[3];
            emit ResizeView2D(rmax_x, rmax_y);

            InitializePathsTreeViewShow();
        }
    }
}
//凸显轨迹信息widget
void MainWindow::on_ProjectProperty_triggered()
{
    ui->PathsInfo->show();
    if(ui->MotionPara->isVisible()||ui->ProgramsPreview->isVisible())
    {
        tabifyDockWidget(ui->PathsInfo, ui->MotionPara);
        tabifyDockWidget(ui->MotionPara, ui->ProgramsPreview);
        ui->PathsInfo->raise();
    }
}
//凸显动态参数信息widget
void MainWindow::on_action_MotionPara_triggered()
{
    ui->MotionPara->show();
    if(ui->PathsInfo->isVisible()||ui->ProgramsPreview->isVisible())
    {
        tabifyDockWidget(ui->PathsInfo, ui->MotionPara);
        tabifyDockWidget(ui->MotionPara, ui->ProgramsPreview);
        ui->MotionPara->raise();
    }
}
//凸显程序预览信息widget
void MainWindow::on_action_ProgramPreview_triggered()
{
    ui->ProgramsPreview->show();
    if(ui->PathsInfo->isVisible()||ui->MotionPara->isVisible())
    {
        tabifyDockWidget(ui->PathsInfo, ui->MotionPara);
        tabifyDockWidget(ui->MotionPara, ui->ProgramsPreview);
        ui->ProgramsPreview->raise();
    }
}

void MainWindow::on_PathsProperty_currentItemChanged(QtBrowserItem *arg1)
{
    curSelPathPropertyItem = arg1;
}

void MainWindow::on_PointsProperty_currentItemChanged(QtBrowserItem *arg1)
{
    curSelPointPropertyItem = arg1;
}
//轨迹拆分
void MainWindow::on_PathSplitting_triggered()
{
    string pathname, pointname;
    pathname=g_CADtoPath.GetCurNameofSelOneWholePath();
    if(pathname=="")
    {
        ui->statusBar->showMessage(QStringLiteral("未选择有效轨迹！"));
        return;
    }
    pointname=g_CADtoPath.GetCurNameofSelPointinOneWholePath();
    if(pointname=="")
    {
        ui->statusBar->showMessage(QStringLiteral("未选择有效拆分点！"));
        return;
    }
    QString content;
    content= QStringLiteral("是否以点: ")+QString::fromStdString(pointname)+QStringLiteral(" 为断点拆分轨迹: ")
            +QString::fromStdString(pathname)+QStringLiteral(" ? ");;
    QMessageBox::StandardButton rb = QMessageBox::information(this, QStringLiteral("轨迹拆分"),content, QMessageBox::Yes | QMessageBox::No,
                                                              QMessageBox::Yes);

    if(rb == QMessageBox::Yes)
    {
        int w_index, spindex;
        w_index=g_CADtoPath.GetCurIndexofSelOneWholePath();
        spindex=g_CADtoPath.GetCurIndexofSelPointinOneWholePath();
        int remark=g_CADtoPath.PathPathSplittingoneWhole(w_index, spindex);
        switch (remark)
        {
        case 0:
            ShowPathsTreeViewSplitting();
            ui->statusBar->showMessage(QStringLiteral("轨迹拆分成功！"));
            break;
        case 1:
            ui->statusBar->showMessage(QStringLiteral("轨迹拆分失败！"));
            break;
        case 2:
            ui->statusBar->showMessage(QStringLiteral("首末点不能作为拆分点！"));
            break;
        case 3:
            ui->statusBar->showMessage(QStringLiteral("圆弧中点不能作为拆分点！"));
            break;
        default:
            break;
        }
    }
}
//轨迹合并
void MainWindow::on_PathsMerging_triggered()
{
    QStringList conslist;
    conslist.clear();
    for(int i=0;i<g_CADtoPath.GetPahtsCount();i++)
        conslist<<QString::fromStdString(g_CADtoPath.GetPathName(i));

    MyInputDialog *m_InputDialog = new MyInputDialog(this);
    connect(m_InputDialog, SIGNAL(PathsNameListSelChanged(int)), this, SLOT(PathMergingInputDialogSelectItem(int)));

    if(m_InputDialog->exec() == QDialog::Accepted)
    {
        int w_index0, w_index1;
        w_index0=g_CADtoPath.GetCurIndexofSelOneWholePath();
        w_index1=g_CADtoPath.GetCurIndexofSelOneWholePath_Another();
        int remark=g_CADtoPath.PathMergingoneWhole(w_index0, w_index1);
        switch (remark) {
        case 0:
        {
            QModelIndex cdindex0,cdindex1,insertindex;
            cdindex0=PathsTreeViewModel->index(w_index0, 0);
            RemoveItemsofPathsTreeView(cdindex0);
            if(w_index0<w_index1)
            {
                cdindex1=PathsTreeViewModel->index(w_index1-1, 0);
                insertindex=cdindex0;
            }
            else
            {
                cdindex1=PathsTreeViewModel->index(w_index1, 0);
                insertindex=PathsTreeViewModel->index(w_index0-1, 0);
            }
            RemoveItemsofPathsTreeView(cdindex1);

            InsertOnePathofPathsTreeView(insertindex.row());
            curSelIndex=PathsTreeViewModel->index(insertindex.row(),0);
            on_PathsTreeView_clicked(curSelIndex);
            ui->PathsTreeView->setCurrentIndex(curSelIndex);

            ui->statusBar->showMessage(QStringLiteral("轨迹合并成功！"));
        }
            break;
        case 1:
            ui->statusBar->showMessage(QStringLiteral("轨迹合并失败！"));
            break;
        case 2:
            ui->statusBar->showMessage(QStringLiteral("相同轨迹不能合并！"));
            break;
        case 3:
            ui->statusBar->showMessage(QStringLiteral("轨迹合并过程中反转失败！"));
            break;
        case 4:
            ui->statusBar->showMessage(QStringLiteral("两条轨迹首、尾都不存在相交，无法合并！"));
            break;
        default:
            break;
        }
    }
    g_CADtoPath.SetCurIndexofSelOneWholePath_Another(-1);
    ui->OpenGL2D->update();
}
//轨迹反向
void MainWindow::on_PathChangeDirection_triggered()
{
    string pathname;
    pathname=g_CADtoPath.GetCurNameofSelOneWholePath();
    if(pathname=="")
    {
        ui->statusBar->showMessage(QStringLiteral("未选择有效轨迹！"));
        return;
    }
    QString content;
    content= QStringLiteral("是否将轨迹 ")+QString::fromStdString(pathname)+QStringLiteral(" 进行反向？");
    QMessageBox::StandardButton rb = QMessageBox::information(this, QStringLiteral("轨迹方向"),content, QMessageBox::Yes | QMessageBox::No,
                                                              QMessageBox::Yes);

    if(rb == QMessageBox::Yes)
    {
        int w_index;
        w_index=g_CADtoPath.GetCurIndexofSelOneWholePath();
        if(g_CADtoPath.PathChangeDirection(w_index))
        {
            QModelIndex cdindex;
            cdindex=PathsTreeViewModel->index(w_index,0);

            RemoveItemsofPathsTreeView(cdindex);
            InsertOnePathofPathsTreeView(cdindex.row());
            curSelIndex=PathsTreeViewModel->index(cdindex.row(),0);
            on_PathsTreeView_clicked(curSelIndex);
            ui->PathsTreeView->setCurrentIndex(curSelIndex);
            ui->statusBar->showMessage(QStringLiteral("轨迹反向成功！"));
        }
        else ui->statusBar->showMessage(QStringLiteral("轨迹方向失败！"));
    }
}
//起点选择
void MainWindow::on_PathBPChange_triggered()
{
    string pathname, pointname;
    pathname=g_CADtoPath.GetCurNameofSelOneWholePath();
    if(pathname=="")
    {
        ui->statusBar->showMessage(QStringLiteral("未选择有效轨迹！"));
        return;
    }
    pointname=g_CADtoPath.GetCurNameofSelPointinOneWholePath();
    if(pointname=="")
    {
        ui->statusBar->showMessage(QStringLiteral("未选择有效拆分点！"));
        return;
    }
    QString content;
    content= QStringLiteral("是否以点: ")+QString::fromStdString(pointname)+QStringLiteral(" 为轨迹: ")
            +QString::fromStdString(pathname)+QStringLiteral(" 的新起始点？ ");
    QMessageBox::StandardButton rb = QMessageBox::information(this, QStringLiteral("起点选择"),content, QMessageBox::Yes | QMessageBox::No,
                                                              QMessageBox::Yes);

    if(rb == QMessageBox::Yes)
    {
        int w_index, spindex;
        w_index=g_CADtoPath.GetCurIndexofSelOneWholePath();
        spindex=g_CADtoPath.GetCurIndexofSelPointinOneWholePath();
        int remark=g_CADtoPath.PathBeginPointChange(w_index, spindex);
        switch (remark)
        {
        case 0:
        {
            QModelIndex cdindex;
            cdindex=PathsTreeViewModel->index(w_index,0);
            RemoveItemsofPathsTreeView(cdindex);
            InsertOnePathofPathsTreeView(cdindex.row());
            curSelIndex=PathsTreeViewModel->index(cdindex.row(),0);
            on_PathsTreeView_clicked(curSelIndex);
            ui->PathsTreeView->setCurrentIndex(curSelIndex);
            ui->statusBar->showMessage(QStringLiteral("新的起点设置成功！同时辅助信息需要重新设置！"));
        }
            break;
        case 1:
            ui->statusBar->showMessage(QStringLiteral("新起点设置失败！"));
            break;
        case 2:
            ui->statusBar->showMessage(QStringLiteral("非环状轨迹不能更换新的起始点！"));
            break;
        case 3:
            ui->statusBar->showMessage(QStringLiteral("圆弧中点不能作为新的起始点！"));
            break;
        default:
            break;
        }
    }
}
//统一设置路径的姿态
void MainWindow::on_PathUniformPosture_triggered()
{
    int w_index=g_CADtoPath.GetCurIndexofSelOneWholePath();
    int spindex=g_CADtoPath.GetPointsCountofOneWholePath(w_index);
    if(w_index<0 || spindex<0)
    {
        ui->statusBar->showMessage(QStringLiteral("为选择有效轨迹！"));
        return;
    }
    double pos[6], euler[3];
    g_CADtoPath.GetCurPostureofSelPointinOneWholePath(w_index, 0, pos);
    for(int i=0;i<3;i++)euler[i]=pos[3+i];
    PathPostureSetDialog *m_pPostureSetDlg = new PathPostureSetDialog(this);
    m_pPostureSetDlg->SetPosture(euler);
    if(m_pPostureSetDlg->exec() == QDialog::Accepted)
    {
        m_pPostureSetDlg->Getposture(euler);
        if(g_CADtoPath.ModifyoneWholePathPosture(w_index,euler ))
            ui->statusBar->showMessage(QStringLiteral("轨迹姿态修改成功!"));
        else
            ui->statusBar->showMessage(QStringLiteral("轨迹姿态修改失败！"));
    }
}

void MainWindow::on_MotionParaType_activated(int index)
{
    ShowMotionParaViewList(index);
}

void MainWindow::on_MotionInfoList_itemClicked(QTreeWidgetItem *item, int column)
{
    curSelTreeItemofDynInfoorOvlInfo=item;
    int t_index=ui->MotionParaType->currentIndex();
    int index;
    if(t_index==0)//加减速
    {
        index=g_CADtoPath.GetCurIndexofDynInfo(item->text(0).toStdString());
        if(index!=-1)ShowMotionPareTreeProperty(t_index, index);
    }
    else if(t_index==1)//Blend
    {
        index=g_CADtoPath.GetCurIndexofOvlInfo(item->text(0).toStdString());
        if(index!=-1)ShowMotionPareTreeProperty(t_index, index);
    }
}
//为MotionInfoList添加右键菜单
void MainWindow::on_MotionInfoList_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *curRightSelItem=ui->MotionInfoList->itemAt(pos);
    curSelTreeItemofDynInfoorOvlInfo=curRightSelItem;
    int t_index=ui->MotionParaType->currentIndex();

    QMenu *treepopmenu = new QMenu(this);
    if(t_index==0)
    {
        treepopmenu->addAction(ui->action_NewDynPara);
        if(curRightSelItem!=NULL)
            treepopmenu->addAction(ui->action_DeleteDynPara);
    }
    else if(t_index==1)
    {
        treepopmenu->addAction(ui->action_NewOvlABSPara);
        treepopmenu->addAction(ui->action_NewOvlRELPara);
        treepopmenu->addAction(ui->action_NewOvlSupposPara);
        if(curRightSelItem!=NULL)
            treepopmenu->addAction(ui->action_DeleteOvlPara);
    }
    treepopmenu->exec(QCursor::pos());

}

//新建Dyn参数
void MainWindow::on_action_NewDynPara_triggered()
{
    bool isok;
    QString dynname=QInputDialog::getText(this, QStringLiteral("Dyn参数名"), QStringLiteral("输入新建Dyn参数名："), QLineEdit::Normal, tr("DynPara"), &isok);
    if(isok && !dynname.isEmpty())
    {
        if(g_CADtoPath.AddNewDynParaInfo(dynname.toStdString()))
        {
            ShowMotionParaViewList(0);

            int index=ui->MotionInfoList->topLevelItemCount()-1;
            ui->MotionInfoList->setCurrentItem(ui->MotionInfoList->topLevelItem(index));
            curSelTreeItemofDynInfoorOvlInfo=ui->MotionInfoList->topLevelItem(index);
            ShowMotionPareTreeProperty(0,index);

            ui->statusBar->showMessage(QStringLiteral("新建Dyn参数成功！"));
        }
        else ui->statusBar->showMessage(QStringLiteral("新建Dyn参数失败，或存在重命名！"));
    }
}

void MainWindow::on_action_DeleteDynPara_triggered()
{
    QString deldynname=curSelTreeItemofDynInfoorOvlInfo->text(0);
    if(QMessageBox::question(this, QStringLiteral("删除Dyn参数"),QStringLiteral("是否删除Dyn参数：")+deldynname+tr("?")) == QMessageBox::Yes)
    {
        if(g_CADtoPath.DeleteDynParaInfo(deldynname.toStdString()))
        {
            ShowMotionParaViewList(0);
            ShowMotionPareTreeProperty(0,-1);
        }
        else
            ui->statusBar->showMessage(QStringLiteral("删除Dyn参数失败，该参数或已使用或不存在！"));
    }
}

void MainWindow::on_action_DeleteOvlPara_triggered()
{
    QString delovlname=curSelTreeItemofDynInfoorOvlInfo->text(0);
    if(QMessageBox::question(this, QStringLiteral("删除Ovl参数"),QStringLiteral("是否删除Ovl参数：")+delovlname+tr("?")) == QMessageBox::Yes)
    {
        if(g_CADtoPath.DeleteOvlParaInfo(delovlname.toStdString()))
        {
            ShowMotionParaViewList(1);
            ShowMotionPareTreeProperty(1,-1);
        }
        else
            ui->statusBar->showMessage(QStringLiteral("删除Ovl参数失败，该参数或已使用或不存在！"));
    }
}

void MainWindow::on_PathsInfo_visibilityChanged(bool visible)
{

}

void MainWindow::on_MotionPara_visibilityChanged(bool visible)
{
    if(visible)
    {
        int index=ui->MotionParaType->currentIndex();
        if(index==-1)return;
        ShowMotionParaViewList(index);
    }
}



void MainWindow::on_MotionInfoTreeProperty_currentItemChanged(QtBrowserItem *arg1)
{
    curSelMotionParaPropertyItem=arg1;
}

void MainWindow::on_action_AddInPoint_triggered()
{
    PathInofOutPointSetDlg *m_ppsetDlg=new PathInofOutPointSetDlg(this);
    connect(m_ppsetDlg, SIGNAL(acceptstate(int,bool)), this, SLOT(ShowInorOutPointofPathsTreeView(int,bool)));
    m_ppsetDlg->SetDefaultName(0);
    m_ppsetDlg->exec();
}

void MainWindow::on_action_AddOutPoint_triggered()
{
    PathInofOutPointSetDlg *m_ppsetDlg=new PathInofOutPointSetDlg(this);
    connect(m_ppsetDlg, SIGNAL(acceptstate(int,bool)), this, SLOT(ShowInorOutPointofPathsTreeView(int,bool)));
    m_ppsetDlg->SetDefaultName(1);
    m_ppsetDlg->exec();
}

void MainWindow::on_action_DeleteCurInAP_triggered()
{
    QString content;
    content= QStringLiteral("是否删除当前轨迹的进入点？");
    QMessageBox::StandardButton rb = QMessageBox::information(this, QStringLiteral("轨迹拆分"),content, QMessageBox::Yes | QMessageBox::No,
                                                              QMessageBox::Yes);

    if(rb == QMessageBox::No)return;

    if(g_CADtoPath.DeleteAssistPointofCurrentPath(0))
    {
        int w_index=g_CADtoPath.GetCurIndexofSelOneWholePath();
        QModelIndex pathrowindex=PathsTreeViewModel->index(w_index, 0);
        PathsTreeViewModel->removeRow(0, pathrowindex);
        ui->PathsTreeView->setCurrentIndex(pathrowindex.child(0,0));
        curSelIndex=QModelIndex::QModelIndex();
        on_PathsTreeView_clicked(pathrowindex.child(0, 0));
        ui->statusBar->showMessage(QStringLiteral("删除当前轨迹进入点成功"));
    }
    else ui->statusBar->showMessage(QStringLiteral("删除当前轨迹进入点失败"));
}

void MainWindow::on_action_DeleteAllInAPs_triggered()
{
    QString content;
    content= QStringLiteral("是否删除所有轨迹的进入点？");
    QMessageBox::StandardButton rb = QMessageBox::information(this, QStringLiteral("轨迹拆分"),content, QMessageBox::Yes | QMessageBox::No,
                                                              QMessageBox::Yes);

    if(rb == QMessageBox::No)return;

    if(g_CADtoPath.DeleteAssisPointofAllPaths(0))
    {
        QModelIndex pathrowindex,selindex;
        QString ptype;
        selindex=curSelIndex.parent();
        for(int i=0;i<PathsTreeViewModel->rowCount();i++)
        {
            pathrowindex=PathsTreeViewModel->index(i, 0);
            ptype=PathsTreeViewModel->itemFromIndex(pathrowindex)->child(0,1)->text();
            if(ptype==QStringLiteral("进入点"))
                PathsTreeViewModel->removeRow(0, pathrowindex);
        }
        ui->PathsTreeView->setCurrentIndex(selindex.child(0,0));
        curSelIndex=QModelIndex::QModelIndex();
        on_PathsTreeView_clicked(selindex.child(0,0));

        ui->statusBar->showMessage(QStringLiteral("删除所有轨迹进入点成功"));
    }
    else ui->statusBar->showMessage(QStringLiteral("删除所有轨迹进入点失败"));
}
//删除当前轨迹脱离点
void MainWindow::on_action_DeleteCurOutAP_triggered()
{
    QString content;
    content= QStringLiteral("是否删除当前轨迹的脱离点？");
    QMessageBox::StandardButton rb = QMessageBox::information(this, QStringLiteral("轨迹拆分"),content, QMessageBox::Yes | QMessageBox::No,
                                                              QMessageBox::Yes);

    if(rb == QMessageBox::No)return;

    if(g_CADtoPath.DeleteAssistPointofCurrentPath(1))
    {
        int w_index=g_CADtoPath.GetCurIndexofSelOneWholePath();
        QModelIndex pathrowindex=PathsTreeViewModel->index(w_index, 0);
        int num=PathsTreeViewModel->itemFromIndex(pathrowindex)->rowCount();
        PathsTreeViewModel->removeRow(num-1, pathrowindex);
        curSelIndex=QModelIndex::QModelIndex();
        ui->PathsTreeView->setCurrentIndex(pathrowindex.child(num-2,0));
        on_PathsTreeView_clicked(pathrowindex.child(num-2, 0));

        ui->statusBar->showMessage(QStringLiteral("删除当前轨迹进脱离点成功"));
    }
    else ui->statusBar->showMessage(QStringLiteral("删除当前轨迹脱离点失败"));
}
//删除所有轨迹的路径点
void MainWindow::on_action_DeleteAllOutAPs_triggered()
{
    QString content;
    content= QStringLiteral("是否删除所有轨迹的脱离点？");
    QMessageBox::StandardButton rb = QMessageBox::information(this, QStringLiteral("轨迹拆分"),content, QMessageBox::Yes | QMessageBox::No,
                                                              QMessageBox::Yes);

    if(rb == QMessageBox::No)return;

    if(g_CADtoPath.DeleteAssisPointofAllPaths(1))
    {
        QModelIndex pathrowindex,selindex;
        QString ptype;
        int num;
        selindex=curSelIndex.parent();
        for(int i=0;i<PathsTreeViewModel->rowCount();i++)
        {
            pathrowindex=PathsTreeViewModel->index(i, 0);
            num=PathsTreeViewModel->itemFromIndex(pathrowindex)->rowCount();
            ptype=PathsTreeViewModel->itemFromIndex(pathrowindex)->child(num-1,1)->text();
            if(ptype==QStringLiteral("脱离点"))
                PathsTreeViewModel->removeRow(num-1, pathrowindex);
        }
        num=PathsTreeViewModel->itemFromIndex(selindex)->rowCount();
        ui->PathsTreeView->setCurrentIndex(selindex.child(num-1,0));
        curSelIndex=QModelIndex::QModelIndex();
        on_PathsTreeView_clicked(selindex.child(num-1,0));

        ui->statusBar->showMessage(QStringLiteral("删除所有轨迹脱离点成功"));
    }
    else ui->statusBar->showMessage(QStringLiteral("删除所有轨迹脱离点失败"));
}
void MainWindow::on_PathsTreeView_clicked(const QModelIndex &index)
{
    QString pathname, pointname;
    if(index.parent().isValid())//Child
    {
        pathname = PathsTreeViewModel->item(index.parent().row())->text();
        pointname = PathsTreeViewModel->item(index.parent().row())->child(index.row())->text();

        g_CADtoPath.SetCurIndexofSelOneWholePath(pathname.toStdString());
        g_CADtoPath.SetCurIndexofSelPointinOneWholePath(pointname.toStdString());

        if(ui->ProgramsNameList->count()>0)
            ui->ProgramsNameList->setCurrentIndex(index.parent().row()+1);
    }
    else //Parent
    {
        ui->PathsTreeView->collapseAll();
        ui->PathsTreeView->expand(index);
        pathname = PathsTreeViewModel->item(index.row())->text();
        g_CADtoPath.SetCurIndexofSelOneWholePath(pathname.toStdString());

        if(ui->ProgramsNameList->count()>0)
            ui->ProgramsNameList->setCurrentIndex(index.row()+1);
    }
    if(curSelIndex.isValid())
        GetCurSelStandardItem()->setBackground(QColor(255, 255, 255));
    curSelIndex=index;
    ShowTreeViewProperty(index);
    ui->OpenGL2D->update();
}

void MainWindow::on_PathsTreeView_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex curRightIndex=ui->PathsTreeView->indexAt(pos);
    QMenu *treepopmenu = new QMenu(this);

    if(curRightIndex.isValid())
    {
        on_PathsTreeView_clicked(curRightIndex);
        QString ptip=GetCurSelStandardItem(1)->text();

        if(ptip==QStringLiteral("进入点"))
        {
            treepopmenu->addAction(ui->action_DeleteCurInAP);
            treepopmenu->addAction(ui->action_DeleteAllInAPs);
        }
        else if(ptip==QStringLiteral("脱离点"))
        {
            treepopmenu->addAction(ui->action_DeleteCurOutAP);
            treepopmenu->addAction(ui->action_DeleteAllOutAPs);
        }
        else
        {
            treepopmenu->addAction(ui->PathSplitting);
            treepopmenu->addAction(ui->PathsMerging);
            treepopmenu->addAction(ui->PathChangeDirection);

            QAction *menuseparator1 = new QAction(this);
            menuseparator1->setSeparator(true);
            treepopmenu->addAction(menuseparator1);

            treepopmenu->addAction(ui->PathBPChange);
            treepopmenu->addAction(ui->PathUniformPosture);

            QAction *menuseparator2 = new QAction(this);
            menuseparator2->setSeparator(true);
            treepopmenu->addAction(menuseparator2);

            QMenu *childpopmenu0 = new QMenu(this);
            childpopmenu0->addAction(ui->action_AddInPoint);
            childpopmenu0->addAction(ui->action_AddOutPoint);
            childpopmenu0->setTitle(QStringLiteral("添加辅助点"));
            treepopmenu->addMenu(childpopmenu0);

            QAction *menuseparator3 = new QAction(this);
            menuseparator3->setSeparator(true);
            treepopmenu->addAction(menuseparator3);

            QMenu *childpopmenu1 = new QMenu(this);
            childpopmenu1->addAction(ui->action_KEBAProjectPreView);
            childpopmenu1->setTitle(QStringLiteral("程序预览"));
            treepopmenu->addMenu(childpopmenu1);
        }
        treepopmenu->exec(QCursor::pos());
    }
    else
    {
        QMenu *childpopmenu1 = new QMenu(this);
        childpopmenu1->addAction(ui->action_KEBAProjectPreView);
        childpopmenu1->setTitle(QStringLiteral("程序预览"));
        treepopmenu->addMenu(childpopmenu1);
        treepopmenu->exec(QCursor::pos());
    }
}

void MainWindow::ShowInorOutPointofPathsTreeView(int apmode, bool isall)
{
    if(isall)//apply all paths
    {
        int num=g_CADtoPath.m_MultiPathInfoList.size();
        for(int i=0;i<g_CADtoPath.m_MultiPathInfoList.size();i++)
            InsertAssistPointofPahtsTreeVeiw(i, apmode);
    }
    else//curpath
    {
        int w_index=g_CADtoPath.GetCurIndexofSelOneWholePath();
        if(w_index!=-1)
        {
            InsertAssistPointofPahtsTreeVeiw(w_index, apmode);
            ui->PathsTreeView->setExpanded(PathsTreeViewModel->index(w_index,0), true);

            int childcuritemindex(0);
            if(apmode==1)
                childcuritemindex=PathsTreeViewModel->item(w_index)->rowCount()-1;
            QModelIndex curindex=PathsTreeViewModel->item(w_index,0)->child(childcuritemindex,0)->index();
            on_PathsTreeView_clicked(curindex);
            ui->PathsTreeView->setCurrentIndex(curindex);
        }
    }

}

//根据当前轨迹生成KEBA项目
void MainWindow::on_KEBAProject_triggered()
{
    QString filepath(""), filename("");
    filepath = QFileDialog::getSaveFileName(this, tr("Keba Project"), ".", tr("Keba Project(*.tt)"));
    if(!filepath.isEmpty())
    {
        int index=filepath.lastIndexOf('/');
        filename=filepath.mid(index+1);
        filename.remove(".tt");
        QDir *projectdir=new QDir;
        if(!projectdir->exists(filepath))//文件夹不存在则新建
        {
            if(projectdir->mkdir(filepath))
            {
                g_CADtoPath.SetSaveFilePath(filepath.toStdString());
                if(g_CADtoPath.GenerateKEBAPathFiles())
                    ui->statusBar->showMessage(QStringLiteral("KEBA项目生成成功！"));
                else ui->statusBar->showMessage(QStringLiteral("KEBA项目生成失败！"));
            }
        }
    }
}

void MainWindow::on_action_KEBAProjectPreView_triggered()
{

    if(g_CADtoPath.GeneratePathProgramPreview_KEBA())
    {
        ui->ProgramsNameList->clear();
        for(size_t i=0;i<g_CADtoPath.m_ProjectProgram.size();i++)
        {
            ui->ProgramsNameList->addItem(QString::fromStdString(g_CADtoPath.m_ProjectProgram[i].name));
        }
        if(g_CADtoPath.m_ProjectProgram.size()>0)
        {
            ui->ProgramsNameList->setCurrentIndex(0);
            ShowProgramPreview(0);
        }
        if(ui->ProgramsPreview->isHidden())
            ui->ProgramsPreview->show();
        if(!ui->ProgramsPreview->isFloating())
            ui->ProgramsPreview->raise();

        ui->ProgramNameTip->setText(QStringLiteral("KEBA项目程序"));
        ui->SaveProgramProject->setText(QStringLiteral("保存KEBA程序项目"));
    }
    else
        ui->statusBar->showMessage(QStringLiteral("程序预览失败，或不存在轨迹！"));
}

void MainWindow::on_ProgramsNameList_currentIndexChanged(int index)
{
    ShowProgramPreview(index);
}


void MainWindow::on_SaveProgramProject_clicked()
{
    if(g_CADtoPath.m_MultiPathInfoList.empty())
    {
        ui->statusBar->showMessage(QStringLiteral("没有有效轨迹"));
        return;
    }
    if(g_CADtoPath.m_ProjectProgram.empty())
    {
        ui->statusBar->showMessage(QStringLiteral("未生成有效程序预览"));
        return;
    }
    QString filepath(""), filename("");
    filepath = QFileDialog::getSaveFileName(this, tr("Keba Project"), ".", tr("Keba Project(*.tt)"));
    if(!filepath.isEmpty())
    {
        int index=filepath.lastIndexOf('/');
        filename=filepath.mid(index+1);
        filename.remove(".tt");
        QDir *projectdir=new QDir;
        if(!projectdir->exists(filepath))//文件夹不存在则新建
        {
            if(projectdir->mkdir(filepath))
            {
                if(g_CADtoPath.SaveProgramProject_KEBA(filepath.toStdString()))
                    ui->statusBar->showMessage(QStringLiteral("KEBA项目程序保存生成成功！"));
                else ui->statusBar->showMessage(QStringLiteral("KEBA项目程序保存生成失败！"));
            }
        }
    }
}

void MainWindow::on_action_NewOvlABSPara_triggered()
{
    bool isok;
    QString ovlname=QInputDialog::getText(this, QStringLiteral("OvlABS参数名"), QStringLiteral("输入新建OvlABS参数名："), QLineEdit::Normal, tr("OvlABSPara"), &isok);
    if(isok && !ovlname.isEmpty())
    {
        if(g_CADtoPath.AddNewOvlParaInfo(ovlname.toStdString(),0))
        {
            ShowMotionParaViewList(1);

            int index=ui->MotionInfoList->topLevelItemCount()-1;
            ui->MotionInfoList->setCurrentItem(ui->MotionInfoList->topLevelItem(index));
            curSelTreeItemofDynInfoorOvlInfo=ui->MotionInfoList->topLevelItem(index);
            ShowMotionPareTreeProperty(1,index);

            ui->statusBar->showMessage(QStringLiteral("新建OvlABS参数成功！"));
        }
        else ui->statusBar->showMessage(QStringLiteral("新建OvlABS参数失败，或存在重命名！"));
    }
}

void MainWindow::on_action_NewOvlRELPara_triggered()
{
    bool isok;
    QString ovlname=QInputDialog::getText(this, QStringLiteral("OvlREL参数名"), QStringLiteral("输入新建OvlREL参数名："), QLineEdit::Normal, tr("OvlRELPara"), &isok);
    if(isok && !ovlname.isEmpty())
    {
        if(g_CADtoPath.AddNewOvlParaInfo(ovlname.toStdString(),1))
        {
            ShowMotionParaViewList(1);

            int index=ui->MotionInfoList->topLevelItemCount()-1;
            ui->MotionInfoList->setCurrentItem(ui->MotionInfoList->topLevelItem(index));
            curSelTreeItemofDynInfoorOvlInfo=ui->MotionInfoList->topLevelItem(index);
            ShowMotionPareTreeProperty(1,index);

            ui->statusBar->showMessage(QStringLiteral("新建OvlREL参数成功！"));
        }
        else ui->statusBar->showMessage(QStringLiteral("新建OvlREL参数失败，或存在重命名！"));
    }
}

void MainWindow::on_action_NewOvlSupposPara_triggered()
{
    bool isok;
    QString ovlname=QInputDialog::getText(this, QStringLiteral("OvlSuppos参数名"), QStringLiteral("输入新建OvlSuppos参数名："), QLineEdit::Normal, tr("OvlSupposPara"), &isok);
    if(isok && !ovlname.isEmpty())
    {
        if(g_CADtoPath.AddNewOvlParaInfo(ovlname.toStdString(),2))
        {
            ShowMotionParaViewList(1);

            int index=ui->MotionInfoList->topLevelItemCount()-1;
            ui->MotionInfoList->setCurrentItem(ui->MotionInfoList->topLevelItem(index));
            curSelTreeItemofDynInfoorOvlInfo=ui->MotionInfoList->topLevelItem(index);
            ShowMotionPareTreeProperty(1,index);

            ui->statusBar->showMessage(QStringLiteral("新建OvlSuppos参数成功！"));
        }
        else ui->statusBar->showMessage(QStringLiteral("新建OvlSuppos参数失败，或存在重命名！"));
    }
}

void MainWindow::on_action_About_triggered()
{
    QString curDate=QDate::currentDate().toString("yyyy-MM-dd");
    QString tipstr;
    tipstr=QStringLiteral("软件版本：Version 1.0\r\n 时间：")+curDate;
    QMessageBox::information(this, QStringLiteral("关于软件…"), tipstr);
}
