#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QtVariantPropertyManager>
#include <QtButtonPropertyBrowser>
#include <QtEnumPropertyManager>
#include <QtEnumEditorFactory>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QStandardItemModel>
#include <QStringListModel>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum ShowMode {TwoDimension, ThreeDimension};
    Q_ENUMS(ShowMode)

signals:
    void loaddefaultview2D(void); //信号：显示2D的默认视图
    void ResizeView2D(double rmax_x, double rmax_y);
    //信号：显示3D的预设视图，默认视图(0)，前视图(1)，后视图(2)，左视图(3)
    //右视图(4)，上视图(5)，下视图(6)；
    void loadsetupview3D(int viewindex);
private:

protected:
    void keyPressEvent(QKeyEvent *kevent);//键盘事件
private slots:
    void on_ShowDimension_triggered();//槽：2D/3D视图切换
    void on_DefaultView_triggered();
    void on_FrontView_triggered();
    void on_BackView_triggered();
    void on_LeftView_triggered();
    void on_RightView_triggered();
    void on_UpView_triggered();
    void on_BottomView_triggered();

    //槽：属性栏的变量及Combo的信息修改响应函数
    void PathsPropertyStringValueChanged(QtProperty* property, const QString &value);
    void PathsPropertyEnumValueChanged(QtProperty* property, const int value);
    void PathsPropertyBoolValueChanged(QtProperty *property, const bool value);
    void PointsPropertyStringValueChanged(QtProperty* property, const QString &value);
    void PointsPropertyDoubleValueChanged(QtProperty* property, const double &value);
    void PointsPropertyEnumValueChanged(QtProperty* property, const int value);

    //轨迹合并时选择项更改
    void PathMergingInputDialogSelectItem(const int w_index);

    void on_action_Import_Dxf_triggered();//导入dxf格式文件并进行一笔画路径分析
    void on_OpenFile_2D_triggered();//打开2D项目
    void on_ProjectProperty_triggered();//显示项目属性
    //路径点位树结构属性时当前选择项
    void on_PathsProperty_currentItemChanged(QtBrowserItem *arg1);
    void on_PointsProperty_currentItemChanged(QtBrowserItem *arg1);

    void on_KEBAProject_triggered();
    ////////////////////////////////////////////////////////////////////////////////
    //轨迹操作
    void on_PathSplitting_triggered();//将一条轨迹拆分成两条独立的轨迹
    void on_PathsMerging_triggered();//两条首尾相接的轨迹合并
    void on_PathChangeDirection_triggered();//轨迹首尾换向
    void on_PathBPChange_triggered();//环状轨迹起始点更换
    void on_PathUniformPosture_triggered();//轨迹姿态统一设置

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //动态参数
    void on_MotionParaType_activated(int index); //动态参数类型：加减速、逼近
    void on_MotionInfoList_customContextMenuRequested(const QPoint &pos);//当前widget右键显示
    void on_MotionInfoList_itemClicked(QTreeWidgetItem *item, int column);//当前widget左键点击相应
    void on_action_MotionPara_triggered();//显示动态参数widget
    void on_action_NewDynPara_triggered();//新建加减速参数
    void on_action_NewOvlABSPara_triggered();//新建ABS类型逼近参数
    void on_action_NewOvlRELPara_triggered();//新建REL类型逼近参数
    void on_action_NewOvlSupposPara_triggered();//新建SUPPOS类型逼近参数
    void on_action_DeleteDynPara_triggered();//删除加减速参数
    void on_action_DeleteOvlPara_triggered();//删除逼近参数

    void on_PathsInfo_visibilityChanged(bool visible);

    void on_MotionPara_visibilityChanged(bool visible);
    //动态参数属性当前选择信息项
    void on_MotionInfoTreeProperty_currentItemChanged(QtBrowserItem *arg1);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    //辅助点（进入点、脱离点）操作
    void on_action_AddInPoint_triggered();//为轨迹增加进入点
    void on_action_AddOutPoint_triggered();//为轨迹增加脱离点
    void on_action_DeleteCurInAP_triggered();//删除当前轨迹进入点
    void on_action_DeleteAllInAPs_triggered();//删除所有轨迹进入点
    void on_action_DeleteCurOutAP_triggered();//删除当前轨迹脱离点
    void on_action_DeleteAllOutAPs_triggered();//删除所有轨迹脱离点
    void ShowInorOutPointofPathsTreeView(int apmode, bool isall);//插
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //轨迹显示
    void on_PathsTreeView_clicked(const QModelIndex &index);//轨迹列表单击信息
    void on_PathsTreeView_customContextMenuRequested(const QPoint &pos);//轨迹列表右键信息

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //程序预览
    void on_action_KEBAProjectPreView_triggered();//生成程序预览
    void on_ProgramsNameList_currentIndexChanged(int index);//程序选择项
    void on_action_ProgramPreview_triggered();//显示程序预览widget
    void on_SaveProgramProject_clicked();//保存程序项目文件

    //关于
    void on_action_About_triggered();

private:
    Ui::MainWindow *ui;
    ShowMode cur_showmode; //当前的显示模式：2D or 3D
    QLabel *m_pstatebarshowmode;//状态栏中永久显示信息：当前模式：2D or 3D
    void ShowCurFrame(void);//根据二维或三维的选择显示界面

protected:
    //dxf文件读取
    bool ReadDXFData(QString &filepath);
    //根据树状结构选择信息显示属性内容
    void InitializePropertyWidget(void);
    void ShowPointsProperty(void);
    void ShowPathsProperty(void);
    //属性栏操作
    QtBrowserItem *curSelPathPropertyItem, *curSelPointPropertyItem;

    //动态信息
    void InitializeMotionParaWidget(void);
    void ShowMotionParaViewList(const int index);//显示当前选择的属性内容
    void ShowMotionPareTreeProperty(const int t_index, const int index);//显示当前编辑的item信息
    void MotionParaDynStringValueChanged(QtProperty* property, const QString &value);
    void MotionParaDynDoubleValueChanged(QtProperty* property, const double &value);
    void MotionParaOvlStringValueChanged(QtProperty* property, const QString &value);
    void MotionParaOvlDoubleValueChanged(QtProperty* property, const double &value);
    void MotionParaOvlBoolValueChanged(QtProperty* property, const bool value);

    QTreeWidgetItem *curSelTreeItemofDynInfoorOvlInfo;
    QtBrowserItem *curSelMotionParaPropertyItem;

private:
    QStandardItemModel *PathsTreeViewModel;
    QModelIndex curSelIndex;
    //路径数据初始化显示
    void InitializePathsTreeViewShow(void);
    void ShowTreeViewProperty(const QModelIndex &mindex);
    //删除轨迹信息树中的item，如果index为点位则删除点位，如为轨迹则删除整条轨迹
    bool RemoveItemsofPathsTreeView(const QModelIndex & index);
    //在Treeview的rowindex行插入一条轨迹，并选择当前行并展开
    bool InsertOnePathofPathsTreeView(const int rowindex);
    //在TreeView的rowindex行的轨迹上插入assistpoint
    bool InsertAssistPointofPahtsTreeVeiw(const int rowindex, const int apmode);
    void ShowPathsTreeViewSplitting(void);//显示拆分之后的轨迹
    QStandardItem * GetCurSelStandardItem(int columnindex=0);//根据当前curSelIndex返回相应的item

    /////////////////////////////////////////////////////////////////////////
    QStandardItemModel *DynOvlTreeViewModel;
    ////////////////////////////////////////////////////////////////////////////////////////
    QStringListModel *ProgramConsModel;
    //在程序预览中显示索引为index程序
    void ShowProgramPreview(const int index=0);
};

#endif // MAINWINDOW_H
