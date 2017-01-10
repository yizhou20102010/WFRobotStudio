#pragma once
#include "CmyDxfFilter.h"
#include "graphtopathdatastruct.h"

class CGraphtoPath
{
private:
    string savePath; //当前程序保存路径
    vector<singlePathinfo>m_ChaosPathList;//dxf格式读取时保存的容器
public:
    vector<oneWholePathinfo >m_MultiPathInfoList;//一笔画路径保存容器（dxf读取分析后的数据）
    vector<DynInfo>m_DynVarList;//项目中Dyn动态参数保存容器
    vector<OvlInfo>m_OvlVarList;//项目中Ovl动态参数保存容器
    vector<ProgramInfo>m_ProjectProgram;//项目中预览程序内容保存容器
private:
    int curindex_SeloneWholePath; //当前选择的轨迹，对应m_MultiPathInfoList的索引号
    int curindex_SeloneWholePath_another;//标记融合轨迹的另一条轨迹，对应m_MultiPathInfoList的索引号
    int curindex_SelPointinoneWholePath;//当前选择的点位，对应m_MultiPathInfoList中Onepath的索引号
public:
	CGraphtoPath(void);
	~CGraphtoPath(void);
private:
    double xmax,xmin,ymax,ymin,zmax,zmin;//当前读入数据的xyz的范围
    //dxf导入时的设置默认的路径姿态
    double posure[3];
    bool isdefaultposure;
    ///根据圆弧圆心circle、半径radius以及起止角度angle，计算该段圆弧的起点pos[0]、中点pos[1]、终点pos[2]
	int ExtractArcInfo(const double circle[3], const double radius, const double angle[2], PosInfo pos[3]);
    ///根据读取的dxf数据m_ChaosPathList，分析提取为单笔画格式
	int InitAnalysisPathInfo(const int index=0);
    //根据pos信息在m_ChaosPathList中寻找点位相同的点，并返回相关信息result[0]为m_ChaosPathList的索引，
    //result[1]为该段路径为正向合并(1)还是逆向合并(-1)
	bool FindNextPos(const PosInfo pos, int result[2]);
    //比较点位p1,p2在误差允许范围内是否为同一点
    bool IsEqualPos(const PosInfo &p1, const PosInfo &p2);
    //将一笔画m_onepath信息生成并保存为KEBA格式的点位数据文件，保存路径由savePath确定（不使用）
    bool GenerateKEBAPathData(const oneWholePathinfo &m_onepath);
    //将一笔画m_onepath信息生成并保存为KEBA格式的程序指令文件，保存路径由savePath确定（不使用）
    bool GenerateKEBAPathOrder(const oneWholePathinfo &m_onepath);
    //生成KEBA程序项目的主文件
    bool GenerateKEBAMainProgram(void);
public:
    //获取一笔画路径的总条数
    int GetPahtsCount(void);
    //获取当前索引为w_index轨迹的名称，w_index不在m_MultiPathInfoList索引范围时，返回string("")
    string GetPathName(const int w_index);
    //设置dxf文件导入时轨迹的默认姿态
	void SetDefaultPosure(double data[3], bool istake=true);
    //返回当前程序项目保存的路径
    string GetSaveFilePath(void){return savePath;};
    //设置当前程序项目保存的路径
    void SetSaveFilePath(const string &filepath);
    //读取绝对路径为filename的dxf文件，支持直线、圆弧，同时完成一笔画分析
    int ReadDxfFile(const string &filename);
    //获得当前数据的xyz范围
	void GetRangeCuboid(double xyzrange[6]);
    //空缺
    bool SavePathInfoList(const string filename);
    //将m_MultiPathInfoList中信息生成KEBA项目，包含轨迹程序文件，轨迹点位文件，主程序文件（不使用）
    bool GenerateKEBAPathFiles(void);
	///////////////////////////////////////////////////////////////////////////////////////
    //修改m_MultiPathInfoList中索引为w_index的轨迹的信息
    bool ModifyoneWholePath(const int w_index, const string &name, const int beginindex, const double posture[3], const double dyn[12], const double ovl[5] );
    //修改m_MultiPathInfoList中索引为w_index的轨迹名称为name
    bool ModifyoneWholePathName(const int w_index, const string &name);
    //修改m_MultiPathInfoList中当前选择项（索引为curindex_SeloneWholePath）的轨迹名称为name
    bool ModifyoneWholePathName(const string &name);
    //修改m_MultiPathInfoList中索引为w_index的轨迹的起始点为onepath的中索引为beginindex的点
	bool ModifyoneWholePathBeginpos(const int w_index, const int beginindex);
    //修改m_MultiPathInfoList中索引为w_index的轨迹上所有的点位的姿态为postrue
	bool ModifyoneWholePathPosture(const int w_index, const double posture[3]);
    //修改m_MultiPathInfoList中索引为w_index的轨迹的全局动态参数为dyn和ovl
	bool ModifyoneWholePathDynamic(const int w_index, const double dyn[12], const double ovl[5]);
    //获取m_MultiPathInfoList当前选择项的索引
    int GetCurIndexofSelOneWholePath(void);
    int GetCurIndexofSelOneWholePath_Another(void);
    //获取m_MultiPathInfoList当前选择项的轨迹名称
    string GetCurNameofSelOneWholePath(void);
    //获取在m_MultiPathInfoList当前选择轨迹中的onepath上的当前选择点索引
    int GetCurIndexofSelPointinOneWholePath(void);
    //获取在m_MultiPathInfoList当前选择轨迹中的onepath上的当前选择点名称
    string GetCurNameofSelPointinOneWholePath(void);
    //根据点位名称name返回该店的位置及点位点位类型
    void GetCurInformationofSelPointinOneWholePath(string &pname, double pos[6], string &ptype);
    //获取在m_MultiPathInfoList上索引为w_index的轨迹中额onepath上索引为spindex的点位数据
    void GetCurPostureofSelPointinOneWholePath(const int w_index, const int spindex, double pos[6]);
    //获取在m_MultiPathInfoList上索引为w_index的轨迹中的点位个数
    int GetPointsCountofOneWholePath(const int w_index);
    //设置m_MultiPathInfoList上索引为curindex为当前选择项
    bool SetCurIndexofSelOneWholePath(int curindex);
    bool SetCurIndexofSelOneWholePath_Another(int curindex);
    //设置m_MultiPathInfoList上轨迹名称为pathname为当前选择项
    bool SetCurIndexofSelOneWholePath(string &pathname);
    bool SetCurIndexofSelOneWholePath_Another(string &pathname);
    //设置m_MultiPathInfoList上当前选择轨迹中onepath上索引为curindex的点位当前选择点
    bool SetCurIndexofSelPointinOneWholePath(int curindex);
    //设置m_MultiPathInfoList上当前选择轨迹中onepath上名称为pointname的点位当前选择点
    bool SetCurIndexofSelPointinOneWholePath(string &pointname);
    //修改m_MultiPathInfoList上索引为w_index的轨迹中onepath上索引为curindex的点位信息：名称，位姿，加减速信息，逼近信息（未使用）
    bool ModifyonePosition(const int w_index, const int p_index, const string &name, const double pos[6], const double dyn[12], const double ovl[5]);
    //修改m_MultiPathInfoList上索引为w_index的轨迹中onepath上索引为curindex的点位名称（未使用）
    bool ModifyonePositionName(const int w_index, const int p_index, const string &name);
    //修改m_MultiPathInfoList上当前选择轨迹中onepath上当前选择点的点位名称
    bool ModifyonePositionName(const string &name);
    //修改m_MultiPathInfoList上索引为w_index轨迹中onepath上索引为p_index点的点位数据（未使用）
	bool ModifyonePosition(const int w_index, const int p_index, const double pos[6]);
    //修改m_MultiPathInfoList上当前选择轨迹中onepath上当前选择点的点位数据
    //index x:0 y:1 z:2 a:3 b:4 c:5
    bool ModifyonePosition(const double posvalue, const int index);
    //修改m_MultiPathInfoList上索引为w_index轨迹中onepath上索引为p_index点的动态参数（未使用）
    bool ModifyonePositionDynamic(const int w_index, const int p_index, const double dyn[12], const double ovl[5]);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //将m_MultiPathInfoList上索引为w_index的轨迹以onepath上索引为spindex的点位为分割点，分成两段独立的轨迹
    int PathPathSplittingoneWhole(const int w_index, const int spindex);
    //将m_MultiPathInfoList上索引为w_index的轨迹首尾顺序互反
    bool PathChangeDirection(const int w_index);
    //将m_MultiPathInfoList上索引为w_index的轨迹（首尾相接轨迹）以onepath上索引为spindex的点作为新的起点
    int PathBeginPointChange(const int w_index, const int  spindex);
    //将m_MultiPathInfoList上索引为w_index0的轨迹和m_MultiPathInfoList上索引为w_index1的轨迹，
    //两条首尾相接的轨迹合并成为一条新的轨迹
    int PathMergingoneWhole(const int w_index0, const int w_index1);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Motion Parameter operator
    //从文件filename中读取Dyn参数
    bool ReadMotionParaofDynInfo(const string filename);
    //从文件filename中读取Ovl参数
    bool ReadMotionParaofOvlInfo(const string filename);
    int GetCurIndexofDynInfo(const string dynname);
    int GetIndexofDynInfobyCurSelPath(void);//
    int GetIndexofDynInfobyCurSelPoint(void);//
    void GetCurSelDynInfo(const int index, double dyn[12]);
    string GetCurSelDynInfoName(const int index);
    int GetCurIndexofOvlInfo(const string ovlname);
    int GetIndexofOvlInfobyCurSelPath(void);//
    int GetIndexofOvlInfobyCurSelPoint(void);//
    void GetCurSelOvlInfo(const int index, double ovl[5]);
    string GetCurSelOvlInfoName(const int index);
    int GetCurSelOvlInfoMode(const int index);
    //
    void SetDynParaofSelOneWholePath(const string dpname);
    void SetOvlParaofSelOneWholePath(const string opname);
    void SetDynParaofSelPointinOneWholePath(const string dpname);
    void SetOvlParaofSelPointinOneWholePath(const string opname);
    //编辑轨迹动态参数：新建、删除
    bool AddNewDynParaInfo(const string dynname);
    bool AddNewOvlParaInfo(const string ovlname, const int ovlmode=0);
    bool DeleteDynParaInfo(const string dynname);
    bool DeleteOvlParaInfo(const string ovlname);
    //更改轨迹动态参数的引用次数，flag=1 increase flag=-1 discrease
    void ChangeQuoteNumofDynPara(const string dynname, const int flag=1);
    void ChangeQuoteNumofOvlPara(const string ovlname, const int flag=1);
    //修改轨迹动态参数的信息
    bool ModifyDynParaName(const string oldname, const string newname);
    bool ModifyOvlParaName(const string oldname, const string newname);
    bool ModifyDynParaInfo(const string dynname, const double value, const int index);
    bool ModifyOvlParaInfo(const string ovlname, const double value, const int index);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //轨迹生成的辅助信息：辅助点为信息及是否添加，首末点到位信息是否添加（默认添加）
    //设置所有轨迹的生成参数
    bool SetGenerateParaofAllPaths(const GenerateProgramPara &m_gppara);
    //add new gppara, if exist, modify information
    bool SetAssistPointforCurrentPath(const string name, const double dpos[6], const int apflag=0);
    bool SetAssistPointforAllPaths(const string name, const double dpos[6], const int apflag=0);
    bool SetAPWaitFinishedofCurrentPath(const bool val, const int apflag);
    bool DeleteAssistPointofCurrentPath(const int apflag=0);
    bool DeleteAssisPointofAllPaths(const int apflag=0);

    void GetPointInformationofcurSelPathAssistPoint(string &pname, double pos[6], string &ptype, const int apflag=0 );
    void GetAssistInformationofcurSelPathAssistPoint(bool &InPEnabled, bool &OutPEnabled, bool &bpWF, bool &epWF);

    bool ModifyAssistPointforCurSelPath(const double val, const int index, const int apflag=0);
    bool ModifyAPNameofCurSelPath(const string name, const int apflag=0);
    bool SetAPDynParaofCurSelPath(const string name, const int apflag=0);
    bool SetAPOvlParaofCurSelPath(const string name, const int apflag=0);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    /// 生成KEBA程序项目预览
    bool GeneratePathProgramPreview_KEBA(void);
    //生成KEBA指令的关节运动或直线运动
    string GenerateOrderJointorLinear(PosInfo &pos);//according to information of pos, generate order;
    //生成KEBA指令的圆弧运动
    string GenerateOrderJArc(PosInfo &mpos, PosInfo &epos);
    //保存KEBA程序项目文件：程序文件、点位数据文件、项目全局变量文件
    bool SaveProgramProject_KEBA(const string savepath);
private:
    bool SaveProgramPointData_KEBA(const oneWholePathinfo &m_onepath);
    bool SaveProgramOrder_KEBA(const ProgramInfo &m_onepathorder);
    bool SaveProgramGlobalData_KEBA(void);

};
