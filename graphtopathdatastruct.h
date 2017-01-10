#ifndef GRAPHTOPATHDATASTRUCT_H
#define GRAPHTOPATHDATASTRUCT_H
#include <string>
#include <vector>
using namespace std;
//动态参数
struct DynInfo{
    string name;
    double dyn[12];//动态参数（速度、加速度……）
    int quotenum;//0 is not quoted
};
struct OvlInfo{
    string name;
    double ovl[5];//结束方式
    int quotenum; //0 is not quoted
    int mode; //0 ABS, 1 REL, 2 SUPPOS
};

//点位信息
struct PosInfo{
    int mode;//运动到该点方式：0, Joint; 1, Linear; 0x21,0x22 Arc
    double x,y,z,a,b,c;
    string name;
    string dpname;//运动到该点的动态参数（速度、加速度……）
    string opname;//结束方式
};

//Program Generate Parameters
struct GenerateProgramPara{
    string paraname;
    int systemflag;//0:KEBA
    bool InpointEnabled, OutpointEnabled;
    PosInfo Inpoint, Outpoint;
    bool bpWaitFinished, epWaitFinished;
};

//单段轨迹信息
struct singlePathinfo{
    int mode; //Joint 0; Linear 1; Arc 2;
    PosInfo pos[3]; //位置
};
//单条一笔画拓扑
struct oneWholePathinfo{
    vector<PosInfo> onepath; //一条一笔画拓扑中包含的点位信息
    string name;//一条一笔画拓扑的名称，用于生成程序名称
    string dpname, opname;
    GenerateProgramPara gppara;
};
struct ProgramInfo
{
    string name;
    vector<string>orderslist;
};

#endif // GRAPHTOPATHDATASTRUCT_H
