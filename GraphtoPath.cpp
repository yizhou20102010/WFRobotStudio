#include "GraphtoPath.h"
#include <fstream>
#include "globalvariables.h"
#include "dl_dxf.h"
#include <boost/lexical_cast.hpp>
using namespace std;

CGraphtoPath::CGraphtoPath(void)
{
	posure[0]=-180;
	posure[1]=180;
	posure[2]=0.0;
	isdefaultposure=true;

	xmax=xmin=0.0;
	ymax=ymin=0.0;
	zmax=zmin=0.0;

    curindex_SeloneWholePath=-1;
    curindex_SeloneWholePath_another=-1;
    curindex_SelPointinoneWholePath=-1;
	//??????????·??
// 	GetModuleFileName(NULL, savePath.GetBufferSetLength(MAX_PATH+1), MAX_PATH);
// 	savePath.ReleaseBuffer();
// 	int m_index=savePath.ReverseFind('\\');
// 	savePath=savePath.Left(m_index);
}

CGraphtoPath::~CGraphtoPath(void)
{
}
//设置dxf文件导入时轨迹的默认姿态
void CGraphtoPath::SetDefaultPosure(double data[3], bool istake)
{
	for(int i=0;i<3;i++)
		posure[i]=data[i];
	isdefaultposure=istake;
}
//???????λ??
void CGraphtoPath::SetSaveFilePath(const string &filepath)
{
	savePath=filepath;
}
//读取绝对路径为filename的dxf文件，支持直线、圆弧，同时完成一笔画分析
int CGraphtoPath::ReadDxfFile(const string &filename)
{
	DL_Dxf dxf;
	CmyDxfFilter m_dxffilter;
	//???Dxf???
    if(!dxf.in(filename, &m_dxffilter))
		return -1;

	//???????????

	m_MultiPathInfoList.clear();
	m_ChaosPathList.clear();
    m_ProjectProgram.clear();
	//??*.dxf????ж???????λ?????????????????????m_PathInfoList?С?
	singlePathinfo m_path;
	//?????????
	m_path.mode=1;
	for (size_t i=0;i<m_dxffilter.m_Lineslist.size();i++)
	{
		m_path.pos[0].x=m_dxffilter.m_Lineslist[i].x1;
		m_path.pos[0].y=m_dxffilter.m_Lineslist[i].y1;
		m_path.pos[0].z=m_dxffilter.m_Lineslist[i].z1;
		m_path.pos[1].x=m_dxffilter.m_Lineslist[i].x2;
		m_path.pos[1].y=m_dxffilter.m_Lineslist[i].y2;
		m_path.pos[1].z=m_dxffilter.m_Lineslist[i].z2;
		m_path.pos[0].mode=1;
		m_path.pos[1].mode=1;
		if(isdefaultposure)
		{
			m_path.pos[0].a=m_path.pos[1].a=posure[0];
			m_path.pos[0].b=m_path.pos[1].b=posure[1];
			m_path.pos[0].c=m_path.pos[1].c=posure[2];
		}
		m_ChaosPathList.push_back(m_path);
	}
	//?????????
	m_path.mode=2;
	for(size_t i=0;i<m_dxffilter.m_ArcsList.size();i++)
	{
		double circle[3],radius,angle[2];
		PosInfo m_pos[3];
		circle[0]=m_dxffilter.m_ArcsList[i].cx;
		circle[1]=m_dxffilter.m_ArcsList[i].cy;
		circle[2]=m_dxffilter.m_ArcsList[i].cz;
		radius=m_dxffilter.m_ArcsList[i].radius;
		angle[0]=m_dxffilter.m_ArcsList[i].angle1;
		angle[1]=m_dxffilter.m_ArcsList[i].angle2;
		ExtractArcInfo(circle,radius,angle,m_pos);//?????????????????????????????е??????
		for(int j=0;j<3;j++)
			m_path.pos[j]=m_pos[j];
		m_path.pos[0].mode=2;
		m_path.pos[1].mode=2;
		m_path.pos[2].mode=2;
		if(isdefaultposure)
		{
			m_path.pos[0].a=m_path.pos[1].a=m_path.pos[2].a=posure[0];
			m_path.pos[0].b=m_path.pos[1].b=m_path.pos[2].b=posure[1];
			m_path.pos[0].c=m_path.pos[1].c=m_path.pos[2].c=posure[2];
		}
		m_ChaosPathList.push_back(m_path);
	}
	//?????????
	m_path.mode=3;
	for(size_t i=0;i<m_dxffilter.m_Circleslist.size();i++)
	{

	}

	//????????????
	m_path.mode=4;
	for(size_t i=0;i<m_dxffilter.m_PolylinesList.size();i++)
	{

	}


	double m_posure[]={-180,180,0};
	SetDefaultPosure(m_posure);
	//???????????????????
	int count;
	count=InitAnalysisPathInfo(0);

    curindex_SeloneWholePath=-1;
    curindex_SeloneWholePath_another=-1;
    curindex_SelPointinoneWholePath=-1;
	return count;
}
//获得当前数据的xyz范围
void CGraphtoPath::GetRangeCuboid( double xyzrange[6] )
{
    xyzrange[0]=xmin;
    xyzrange[1]=xmax;
    xyzrange[2]=ymin;
    xyzrange[3]=ymax;
    xyzrange[4]=zmin;
    xyzrange[5]=zmax;
}

//根据圆弧圆心circle、半径radius以及起止角度angle，计算该段圆弧的起点pos[0]、中点pos[1]、终点pos[2]
int CGraphtoPath::ExtractArcInfo(const double circle[3], const double radius, const double angle[2], PosInfo pos[3])
{
	pos[0].x=circle[0]+radius*cos(angle[0]/180*Pi);
	pos[0].y=circle[1]+radius*sin(angle[0]/180*Pi);
	pos[0].z=circle[2];

	double midangle;
	if(angle[1]-angle[0]>=0)
		midangle=1.0*(angle[0]+angle[1])/2.0;
	else 
		midangle=1.0*(angle[0]+angle[1]+360.0)/2.0;
	pos[1].x=circle[0]+radius*cos(midangle/180*Pi);
	pos[1].y=circle[1]+radius*sin(midangle/180*Pi);
	pos[1].z=circle[2];

	pos[2].x=circle[0]+radius*cos(angle[1]/180*Pi);
	pos[2].y=circle[1]+radius*sin(angle[1]/180*Pi);
	pos[2].z=circle[2];

	return 0;
}
//??M_PathInfoList?е?????????????
bool CGraphtoPath::SavePathInfoList(const string filename)
{
	return true;
}
///根据读取的dxf数据m_ChaosPathList，分析提取为单笔画格式
int CGraphtoPath::InitAnalysisPathInfo(const int index)
{
	//?ж?????????????????index????????????????????????
	if(index<0||index>=m_ChaosPathList.size()||m_ChaosPathList.size()==0)
		return 0;
	//????????????????????????
	vector<singlePathinfo>m_midvarlist;
	m_midvarlist.clear();

	//???????????m_PathInfoList?У???????????е?????
	m_midvarlist.push_back(m_ChaosPathList[index]);
	m_ChaosPathList.erase(m_ChaosPathList.begin()+index);

	//???????
	PosInfo m_endpos, m_prevpos;	//???????β????
	bool sdnext(true), sdprev(true);
	size_t len_path;
	int m_resultinfo[2];
	int partitionindex(0);
	while (!m_ChaosPathList.empty())//?ж???????????????????
	{
		if(sdnext)//???????
		{
			len_path=m_midvarlist.size()-1;
			if(m_midvarlist[len_path].mode==1)//?????????
				m_endpos=m_midvarlist[len_path].pos[1];
			else if(m_midvarlist[len_path].mode==2)//?????????
				m_endpos=m_midvarlist[len_path].pos[2];
			if(FindNextPos(m_endpos, m_resultinfo))//??????????λ
			{
				if(m_resultinfo[1]==1)//?????÷??
				{
					m_midvarlist.push_back(m_ChaosPathList[m_resultinfo[0]]);
					m_ChaosPathList.erase(m_ChaosPathList.begin()+m_resultinfo[0]);
				}
				else if(m_resultinfo[1]==-1)//????????
				{
					singlePathinfo m_pathinfo;
					m_pathinfo.mode=m_ChaosPathList[m_resultinfo[0]].mode;
					if(m_pathinfo.mode==1)//???
					{
						m_pathinfo.pos[0]=m_ChaosPathList[m_resultinfo[0]].pos[1];
						m_pathinfo.pos[1]=m_ChaosPathList[m_resultinfo[0]].pos[0];
					}
					else if(m_pathinfo.mode==2)//Arc
					{
						m_pathinfo.pos[0]=m_ChaosPathList[m_resultinfo[0]].pos[2];
						m_pathinfo.pos[1]=m_ChaosPathList[m_resultinfo[0]].pos[1];
						m_pathinfo.pos[2]=m_ChaosPathList[m_resultinfo[0]].pos[0];
					}
					m_midvarlist.push_back(m_pathinfo);
					m_ChaosPathList.erase(m_ChaosPathList.begin()+m_resultinfo[0]);
				}
			}
			else sdnext=false;
		}
		else if(sdprev)//??????
		{
			m_prevpos=m_midvarlist[partitionindex].pos[0];//????λ??
			if(FindNextPos(m_prevpos, m_resultinfo))//???????ú???β?????
			{
				if(m_resultinfo[1]==-1)//?????÷??
				{
					m_midvarlist.insert(m_midvarlist.begin()+partitionindex,m_ChaosPathList[m_resultinfo[0]]);
					m_ChaosPathList.erase(m_ChaosPathList.begin()+m_resultinfo[0]);
				}
				else if(m_resultinfo[1]==1)//????????
				{
					singlePathinfo m_pathinfo;
					m_pathinfo.mode=m_ChaosPathList[m_resultinfo[0]].mode;
					if(m_pathinfo.mode==1)//???
					{
						m_pathinfo.pos[0]=m_ChaosPathList[m_resultinfo[0]].pos[1];
						m_pathinfo.pos[1]=m_ChaosPathList[m_resultinfo[0]].pos[0];
					}
					else if(m_pathinfo.mode==2)//Arc
					{
						m_pathinfo.pos[0]=m_ChaosPathList[m_resultinfo[0]].pos[2];
						m_pathinfo.pos[1]=m_ChaosPathList[m_resultinfo[0]].pos[1];
						m_pathinfo.pos[2]=m_ChaosPathList[m_resultinfo[0]].pos[0];
					}
					m_midvarlist.insert(m_midvarlist.begin()+partitionindex, m_pathinfo);
					m_ChaosPathList.erase(m_ChaosPathList.begin()+m_resultinfo[0]);
				}
			}
			else sdprev=false;
		}
		else //?????????????????????????????
		{
			
			singlePathinfo m_pathinfo;
			m_pathinfo.mode=-1;
			m_midvarlist.push_back(m_pathinfo);

			partitionindex=m_midvarlist.size();
			m_midvarlist.push_back(m_ChaosPathList[0]);
			m_ChaosPathList.erase(m_ChaosPathList.begin());
			sdnext=sdprev=true;
		}
	}

	//???????????????m_PathInfoList??
	oneWholePathinfo m_onepath;
	PosInfo m_pos;
	bool isbeginpos(true);
    string m_posname(""),m_pathname("");
	int m_pointindex(0),m_pathindex(0);
	m_onepath.onepath.clear();
	m_ChaosPathList.clear();

	//???????????λ????????m_ChaosPathList?У????????????????????????????
	for(size_t i=0;i<m_midvarlist.size();i++)
	{
		m_ChaosPathList.push_back(m_midvarlist[i]);
		//???????????λ???????m_MultiPahtInfoList;
		if(m_midvarlist[i].mode==-1)
		{
			m_pathindex++;
            m_pathname = string("Path")+boost::lexical_cast<std::string>(m_pathindex);
            m_onepath.name=m_pathname;
			m_MultiPathInfoList.push_back(m_onepath);
			m_onepath.onepath.clear();
			isbeginpos=true;
		}
		else
		{
			if(isbeginpos)
			{
                m_midvarlist[i].pos[0].mode=1;
				m_onepath.onepath.push_back(m_midvarlist[i].pos[0]);
				isbeginpos=false;
			}

			if(m_midvarlist[i].mode==1)//line
				m_onepath.onepath.push_back(m_midvarlist[i].pos[1]);
			else if(m_midvarlist[i].mode==2)//arc
			{
                m_midvarlist[i].pos[1].mode=0x21;
				m_onepath.onepath.push_back(m_midvarlist[i].pos[1]);
                m_midvarlist[i].pos[2].mode=0x22;
				m_onepath.onepath.push_back(m_midvarlist[i].pos[2]);
			}
		}
	}
	m_pathindex++;
    m_pathname= string("Path")+boost::lexical_cast<string>(m_pathindex);
	m_onepath.name=m_pathname;
	m_MultiPathInfoList.push_back(m_onepath);
	m_onepath.onepath.clear();

	m_midvarlist.clear();

    string pathname("");
	//?????λ????·??????
	for (size_t i=0;i<m_MultiPathInfoList.size();i++)
	{
        m_MultiPathInfoList[i].dpname=string("");
        m_MultiPathInfoList[i].opname=string("");
        m_MultiPathInfoList[i].gppara.InpointEnabled=false;
        m_MultiPathInfoList[i].gppara.OutpointEnabled=false;
        m_MultiPathInfoList[i].gppara.bpWaitFinished=true;
        m_MultiPathInfoList[i].gppara.epWaitFinished=true;
		pathname=m_MultiPathInfoList[i].name;
		m_pointindex=0;
		for(size_t j=0;j<m_MultiPathInfoList[i].onepath.size();j++)
		{
            m_MultiPathInfoList[i].onepath[j].dpname=string("");
            m_MultiPathInfoList[i].onepath[j].opname=string("");
            m_posname=string("Point")+boost::lexical_cast<string>(j);
			m_MultiPathInfoList[i].onepath[j].name=m_posname;

			if(xmax<m_MultiPathInfoList[i].onepath[j].x)xmax=m_MultiPathInfoList[i].onepath[j].x;
			else if(xmin>m_MultiPathInfoList[i].onepath[j].x)xmin=m_MultiPathInfoList[i].onepath[j].x;

			if(ymax<m_MultiPathInfoList[i].onepath[j].y)ymax=m_MultiPathInfoList[i].onepath[j].y;
			else if(ymin>m_MultiPathInfoList[i].onepath[j].y)ymin=m_MultiPathInfoList[i].onepath[j].y;

			if(zmax<m_MultiPathInfoList[i].onepath[j].z)zmax=m_MultiPathInfoList[i].onepath[j].z;
			else if(zmin>m_MultiPathInfoList[i].onepath[j].z)zmin=m_MultiPathInfoList[i].onepath[j].z;
		}					
	}

//	AddPathsTransitionPos();
	return m_MultiPathInfoList.size();//???????????????

}

//根据pos信息在m_ChaosPathList中寻找点位相同的点，并返回相关信息result[0]为m_ChaosPathList的索引，
//result[1]为该段路径为正向合并(1)还是逆向合并(-1)
bool CGraphtoPath::FindNextPos(const PosInfo pos, int result[2])
{
	for(size_t i=0;i<m_ChaosPathList.size();i++)
	{
		if(IsEqualPos(pos, m_ChaosPathList[i].pos[0]))
		{
			result[0]=i;
			result[1]=1;
			return true;
		}
		if(m_ChaosPathList[i].mode==1||m_ChaosPathList[i].mode==2)//Line or Arc
		{
			if(IsEqualPos(pos, m_ChaosPathList[i].pos[m_ChaosPathList[i].mode]))
			{
				result[0]=i;
				result[1]=-1;
				return true;
			}
		}
	}
	return false;
}

//比较点位p1,p2在误差允许范围内是否为同一点
bool CGraphtoPath::IsEqualPos(const PosInfo &p1, const PosInfo &p2)
{
	double vec[3];
	vec[0]=p1.x-p2.x;
	vec[1]=p1.y-p2.y;
	vec[2]=p1.z-p2.z;
	if(fabs(vec[0])<0.001&&fabs(vec[1])<0.001&&fabs(vec[2])<0.001)
		return true;
	else return false;
}

//将m_MultiPathInfoList中信息生成KEBA项目，包含轨迹程序文件，轨迹点位文件，主程序文件
bool CGraphtoPath::GenerateKEBAPathFiles(void)
{
    if(m_MultiPathInfoList.size()<=0)return false;

    bool isok(true);
	for(size_t i=0;i<m_MultiPathInfoList.size();i++)
	{
        isok &=GenerateKEBAPathData(m_MultiPathInfoList[i]);
        isok &=GenerateKEBAPathOrder(m_MultiPathInfoList[i]);
	}
	//??????????????
    isok &=GenerateKEBAMainProgram();
    return isok;
}
//生成KEBA程序项目的主文件
bool CGraphtoPath::GenerateKEBAMainProgram(void)
{
	if(m_MultiPathInfoList.size()<=0)
		return false;

    string filename,extname("");
    filename=savePath+string("//")+extname+string("main.tip");
    string str(""), strcons("");
	ofstream fp;
	fp.open(filename);//,ios_base::out | ios_base::trunc  );

	size_t index(0);
	while(index<m_MultiPathInfoList.size())
	{
        str=("");
        str= string("CALL ")+extname+m_MultiPathInfoList[index].name+string("()");
		index++;
        strcons+=str+string("\n");
	}
    string savecons(strcons);
	fp.write(savecons.c_str(), savecons.size());
	fp.flush();
	fp.close();
    return true;
}
//获取一笔画路径的总条数
int CGraphtoPath::GetPahtsCount(void)
{
    return m_MultiPathInfoList.size();
}
//获取当前索引为w_index轨迹的名称，w_index不在m_MultiPathInfoList索引范围时，返回string("")
string CGraphtoPath::GetPathName(const int w_index)
{
    if(w_index<0||w_index>m_MultiPathInfoList.size()-1)
        return string("");
    else
        return m_MultiPathInfoList[w_index].name;
}

//将一笔画m_onepath信息生成并保存为KEBA格式的程序指令文件，保存路径由savePath确定
bool CGraphtoPath::GenerateKEBAPathOrder(const oneWholePathinfo &m_onepath)
{
	if(m_onepath.onepath.size()<=0)
		return false;

    string filename,extname("");
    filename=savePath+string("//")+extname+m_onepath.name+string(".tip");
    string str(""), strcons("");
	ofstream fp;
	fp.open(filename);//,ios_base::out | ios_base::trunc  );

	size_t index(0);
	while(index<m_onepath.onepath.size())
	{
        str=("");
		if(m_onepath.onepath[index].mode==0)//Joint
		{
            str=string("PTP(")+m_onepath.name+string("_")+m_onepath.onepath[index].name+string(")");
			index++;
		}
		else if(m_onepath.onepath[index].mode==1)//Linear
		{
            str=string("Lin(")+m_onepath.name+string("_")+m_onepath.onepath[index].name+string(")");
			index++;
		}
        else if(m_onepath.onepath[index].mode==0x21)//Arc
		{
			if(index+1<m_onepath.onepath.size())
			{
                if(m_onepath.onepath[index+1].mode==0x22)
				{
                    str=string("Circ(")+m_onepath.name+string("_")+m_onepath.onepath[index].name+string(",")+m_onepath.name+string("_")+m_onepath.onepath[index+1].name+string(")");
					index+=2;
				}
				else return false;
			}
			else return false;
		}
        strcons+=str+string("\n");
//		if(index==2)
//            strcons+=string("WaitIsFinished()\n");
//		else if(index==m_onepath.onepath.size()-1)
//            strcons+=string("WaitIsFinished()\n");
	}
    string savecons(strcons);

	fp.write(savecons.c_str(), savecons.size());
	fp.flush();
	fp.close();
	return true;
}
//将一笔画m_onepath信息生成并保存为KEBA格式的点位数据文件，保存路径由savePath确定
bool CGraphtoPath::GenerateKEBAPathData(const oneWholePathinfo &m_onepath)
{
	if(m_onepath.onepath.size()<=0)
		return false;

    string filename,extname("");
    filename=savePath+string("//")+extname+m_onepath.name+string(".tid");
    string str(""), strcons("");
	ofstream fp;
	fp.open(filename);//,ios_base::out | ios_base::trunc  );
	for(size_t i=0;i<m_onepath.onepath.size();i++)
	{
        str=("");
        str=m_onepath.name+string("_")+m_onepath.onepath[i].name+string(" : CARTPOS := (x := ")+boost::lexical_cast<string>(m_onepath.onepath[i].x)
                +string(", y := ")+boost::lexical_cast<string>(m_onepath.onepath[i].y)
                +string(", z := ")+boost::lexical_cast<string>(m_onepath.onepath[i].z)
                +string(", a := ")+boost::lexical_cast<string>(m_onepath.onepath[i].a)
                +string(", b := ")+boost::lexical_cast<string>(m_onepath.onepath[i].b)
                +string(", c := ")+boost::lexical_cast<string>(m_onepath.onepath[i].c)
                +string(", mode := 1)");
        strcons+=str+string("\r\n");
	}
    //添加进入点位置
    if(m_onepath.gppara.InpointEnabled)
    {
        str=("");
        str=m_onepath.name+string("_")+m_onepath.gppara.Inpoint.name
                +string(" : CARTPOS := (x := ")+boost::lexical_cast<string>(m_onepath.gppara.Inpoint.x)
                +string(", y := ")+boost::lexical_cast<string>(m_onepath.gppara.Inpoint.y)
                +string(", z := ")+boost::lexical_cast<string>(m_onepath.gppara.Inpoint.z)
                +string(", a := ")+boost::lexical_cast<string>(m_onepath.gppara.Inpoint.a)
                +string(", b := ")+boost::lexical_cast<string>(m_onepath.gppara.Inpoint.b)
                +string(", c := ")+boost::lexical_cast<string>(m_onepath.gppara.Inpoint.c)
                +string(", mode := 1)");
        strcons+=str+string("\r\n");
    }
    if(m_onepath.gppara.OutpointEnabled)
    {
        str=("");
        str=m_onepath.name+string("_")+m_onepath.gppara.Outpoint.name
                +string(" : CARTPOS := (x := ")+boost::lexical_cast<string>(m_onepath.gppara.Outpoint.x)
                +string(", y := ")+boost::lexical_cast<string>(m_onepath.gppara.Outpoint.y)
                +string(", z := ")+boost::lexical_cast<string>(m_onepath.gppara.Outpoint.z)
                +string(", a := ")+boost::lexical_cast<string>(m_onepath.gppara.Outpoint.a)
                +string(", b := ")+boost::lexical_cast<string>(m_onepath.gppara.Outpoint.b)
                +string(", c := ")+boost::lexical_cast<string>(m_onepath.gppara.Outpoint.c)
                +string(", mode := 1)");
        strcons+=str+string("\r\n");
    }
    string savecons(strcons);
	fp.write(savecons.c_str(), savecons.size());
	fp.flush();
	fp.close();

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////
//修改m_MultiPathInfoList中索引为w_index的轨迹的信息
bool CGraphtoPath::ModifyoneWholePath(const int w_index, const string &name, const int beginindex, const double posture[3], const double dyn[12], const double ovl[5] )
{
	bool resultbool(true), midbool(false);
	midbool=ModifyoneWholePathName(w_index, name);
	resultbool&=midbool;
	midbool=ModifyoneWholePathBeginpos(w_index, beginindex);
	resultbool&=midbool;
	midbool=ModifyoneWholePathPosture(w_index, posture);
	resultbool&=midbool;
	midbool=ModifyoneWholePathDynamic(w_index, dyn, ovl);
	resultbool&=midbool;
	return resultbool;
}
//修改m_MultiPathInfoList中索引为w_index的轨迹名称为name
bool CGraphtoPath::ModifyoneWholePathName(const int w_index, const string &name)
{
	int num=m_MultiPathInfoList.size();
	if(w_index>=0&&w_index<num)
	{
        for(int i=0;i<num;i++)
            if(m_MultiPathInfoList[i].name==name)//rename
                return false;
		m_MultiPathInfoList[w_index].name=name;
		return true;
	}
    else return false;
}
//修改修改m_MultiPathInfoList中当前选择项（索引为curindex_SeloneWholePath）的轨迹名称为name
bool CGraphtoPath::ModifyoneWholePathName(const string &name)
{
    int num=m_MultiPathInfoList.size();
    if(curindex_SeloneWholePath>=0&&curindex_SeloneWholePath<num)
    {
        for(int i=0;i<num;i++)
            if(m_MultiPathInfoList[i].name==name&&i!=curindex_SeloneWholePath)//rename
                return false;
        m_MultiPathInfoList[curindex_SeloneWholePath].name=name;
        return true;
    }
    else return false;
}
//修改m_MultiPathInfoList中索引为w_index的轨迹的起始点为onepath的中索引为beginindex的点
bool CGraphtoPath::ModifyoneWholePathBeginpos(const int w_index, const int beginindex)
{
	int num=m_MultiPathInfoList.size();
	if(w_index>=0&&w_index<num)
	{
		return true;
	}
	else return false;
}
//修改m_MultiPathInfoList中索引为w_index的轨迹上所有的点位的姿态为postrue
bool CGraphtoPath::ModifyoneWholePathPosture(const int w_index, const double posture[3])
{
    if(w_index<0||w_index>m_MultiPathInfoList.size()-1)
        return false;
    for(int i=0;i<m_MultiPathInfoList[w_index].onepath.size();i++)
    {
        m_MultiPathInfoList[w_index].onepath[i].a=posture[0];
        m_MultiPathInfoList[w_index].onepath[i].b=posture[1];
        m_MultiPathInfoList[w_index].onepath[i].c=posture[2];
    }
	return true;
}
//修改m_MultiPathInfoList中索引为w_index的轨迹的全局动态参数为dyn和ovl
bool CGraphtoPath::ModifyoneWholePathDynamic(const int w_index, const double dyn[12], const double ovl[5])
{
	return true;
}
//获取m_MultiPathInfoList当前选择项的索引
int CGraphtoPath::GetCurIndexofSelOneWholePath( void )
{
    if(curindex_SeloneWholePath>=m_MultiPathInfoList.size()||curindex_SeloneWholePath<0)
		return -1;
    else	return curindex_SeloneWholePath;
}

int CGraphtoPath::GetCurIndexofSelOneWholePath_Another()
{
    if(curindex_SeloneWholePath_another>=m_MultiPathInfoList.size()||curindex_SeloneWholePath_another<0)
        return -1;
    else	return curindex_SeloneWholePath_another;
}
//获取m_MultiPathInfoList当前选择项的轨迹名称
string CGraphtoPath::GetCurNameofSelOneWholePath()
{
    if(curindex_SeloneWholePath==-1||curindex_SeloneWholePath>m_MultiPathInfoList.size()-1)
        return string("");
    else
        return m_MultiPathInfoList[curindex_SeloneWholePath].name;
}
//获取在m_MultiPathInfoList当前选择轨迹中的onepath上的当前选择点索引
int CGraphtoPath::GetCurIndexofSelPointinOneWholePath( void )
{
    if(curindex_SeloneWholePath>=m_MultiPathInfoList.size()||curindex_SeloneWholePath<0)
		return -1;
    else	if(curindex_SelPointinoneWholePath>=m_MultiPathInfoList[curindex_SeloneWholePath].onepath.size()||curindex_SelPointinoneWholePath<0)
		return -1;
	else
        return curindex_SelPointinoneWholePath;
}
//获取在m_MultiPathInfoList当前选择轨迹中的onepath上的当前选择点名称
string CGraphtoPath::GetCurNameofSelPointinOneWholePath()
{
    if(curindex_SeloneWholePath>=m_MultiPathInfoList.size()||curindex_SeloneWholePath<0)
        return string("");
    else	if(curindex_SelPointinoneWholePath>=m_MultiPathInfoList[curindex_SeloneWholePath].onepath.size()||curindex_SelPointinoneWholePath<0)
        return string("");
    else
        return m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].name;
}
//根据点位名称name返回该店的位置及点位点位类型
void CGraphtoPath::GetCurInformationofSelPointinOneWholePath(string &pname, double pos[], string &ptype)
{
    if(curindex_SeloneWholePath>=m_MultiPathInfoList.size()||curindex_SeloneWholePath<0)
    {
        pname=string("");
        ptype=string("");
        for(int i=0;i<6;i++)pos[i]=0.0;
    }
    else	if(curindex_SelPointinoneWholePath>=m_MultiPathInfoList[curindex_SeloneWholePath].onepath.size()||curindex_SelPointinoneWholePath<0)
    {
        pname=string("");
        ptype=string("");
        for(int i=0;i<6;i++)pos[i]=0.0;
    }
    else
    {
        pname=m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].name;
        pos[0]=m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].x;
        pos[1]=m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].y;
        pos[2]=m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].z;
        pos[3]=m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].a;
        pos[4]=m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].b;
        pos[5]=m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].c;

        if(m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].mode==1)//
        {
            ptype = string("Line_Point");
        }
        else if(m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].mode==0x21)
        {
            ptype =  string("Arc_MPoint");
        }
        else if(m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].mode==0x22)
        {
            ptype =  string("Arc_EPoint");
        }
    }

}
//获取在m_MultiPathInfoList上索引为w_index的轨迹中额onepath上索引为spindex的点位数据
void CGraphtoPath::GetCurPostureofSelPointinOneWholePath(const int w_index, const int spindex, double pos[])
{
    if(w_index>=m_MultiPathInfoList.size()||w_index<0)
    {
        for(int i=0;i<6;i++)pos[i]=0.0;
    }
    else	if(spindex>=m_MultiPathInfoList[w_index].onepath.size()||spindex<0)
    {
        for(int i=0;i<6;i++)pos[i]=0.0;
    }
    else
    {
        pos[0]=m_MultiPathInfoList[w_index].onepath[spindex].x;
        pos[1]=m_MultiPathInfoList[w_index].onepath[spindex].y;
        pos[2]=m_MultiPathInfoList[w_index].onepath[spindex].z;
        pos[3]=m_MultiPathInfoList[w_index].onepath[spindex].a;
        pos[4]=m_MultiPathInfoList[w_index].onepath[spindex].b;
        pos[5]=m_MultiPathInfoList[w_index].onepath[spindex].c;
    }
}
//获取在m_MultiPathInfoList上索引为w_index的轨迹中的点位个数
int CGraphtoPath::GetPointsCountofOneWholePath(const int w_index)
{
    if(w_index<0 || w_index>m_MultiPathInfoList.size()-1)
        return -1;
    else
        return m_MultiPathInfoList[w_index].onepath.size();
}

//设置m_MultiPathInfoList上索引为curindex为当前选择项
bool CGraphtoPath::SetCurIndexofSelOneWholePath( int curindex )
{
    if(curindex>=m_MultiPathInfoList.size()||curindex<0)
	{
        curindex_SeloneWholePath=-1;
		return false;
	}
	else
	{
        curindex_SeloneWholePath=curindex;
		return true;
    }
}

bool CGraphtoPath::SetCurIndexofSelOneWholePath_Another(int curindex)
{
    if(curindex>=m_MultiPathInfoList.size()||curindex<0)
    {
        curindex_SeloneWholePath_another=-1;
        return false;
    }
    else
    {
        curindex_SeloneWholePath_another=curindex;
        return true;
    }
}
//设置m_MultiPathInfoList上轨迹名称为pathname为当前选择项
bool CGraphtoPath::SetCurIndexofSelOneWholePath(string &pathname)
{
    curindex_SelPointinoneWholePath=-1;
    for(int i=0;i<m_MultiPathInfoList.size();i++)
        if(pathname==m_MultiPathInfoList[i].name)
        {
            curindex_SeloneWholePath=i;
            return true;
        }
    curindex_SeloneWholePath=-1;
    return false;
}

bool CGraphtoPath::SetCurIndexofSelOneWholePath_Another(string &pathname)
{
    for(int i=0;i<m_MultiPathInfoList.size();i++)
        if(pathname==m_MultiPathInfoList[i].name)
        {
            curindex_SeloneWholePath_another=i;
            return true;
        }
    curindex_SeloneWholePath_another=-1;
    return false;
}
//设置m_MultiPathInfoList上当前选择轨迹中onepath上索引为curindex的点位当前选择点
bool CGraphtoPath::SetCurIndexofSelPointinOneWholePath( int curindex )
{
    if(curindex_SeloneWholePath>=m_MultiPathInfoList.size()||curindex_SeloneWholePath<0)
	{
        curindex_SelPointinoneWholePath=-1;
		return false;
	}
    if(curindex>=m_MultiPathInfoList[curindex_SeloneWholePath].onepath.size()||curindex<0)
	{
        curindex_SelPointinoneWholePath=-1;
		return false;
	}
	else
	{
        curindex_SelPointinoneWholePath=curindex;
		return true;
    }
}
//设置m_MultiPathInfoList上当前选择轨迹中onepath上名称为pointname的点位当前选择点
bool CGraphtoPath::SetCurIndexofSelPointinOneWholePath(string &pointname)
{
    if(curindex_SeloneWholePath>=m_MultiPathInfoList.size()||curindex_SeloneWholePath<0)
    {
        curindex_SelPointinoneWholePath=-1;
        return false;
    }
    for(int i=0;i<m_MultiPathInfoList[curindex_SeloneWholePath].onepath.size();i++)
    {
        if(pointname==m_MultiPathInfoList[curindex_SeloneWholePath].onepath[i].name)
        {
            curindex_SelPointinoneWholePath=i;
            return true;
        }
    }
    curindex_SelPointinoneWholePath=-1;
    return false;
}

///////////////////////////////////////////////////////
//修改m_MultiPathInfoList上索引为w_index的轨迹中onepath上索引为curindex的点位信息：名称，位姿，加减速信息，逼近信息
bool CGraphtoPath::ModifyonePosition(const int w_index, const int p_index, const string &name, const double pos[6], const double dyn[12], const double ovl[5])
{
	return true;
}
//修改m_MultiPathInfoList上索引为w_index的轨迹中onepath上索引为curindex的点位名称（未使用）
bool CGraphtoPath::ModifyonePositionName(const int w_index, const int p_index, const string &name)
{
    return true;
}
//修改m_MultiPathInfoList上当前选择轨迹中onepath上当前选择点的点位名称
bool CGraphtoPath::ModifyonePositionName(const string &name)
{
    if(curindex_SeloneWholePath<0||curindex_SeloneWholePath>m_MultiPathInfoList.size()-1)
        return false;
    int posnum=m_MultiPathInfoList[curindex_SeloneWholePath].onepath.size();
    if(curindex_SelPointinoneWholePath<0||curindex_SelPointinoneWholePath>posnum-1)
        return false;
    for(int i=0;i<posnum;i++)
        if(i!=curindex_SelPointinoneWholePath&&name == m_MultiPathInfoList[curindex_SeloneWholePath].onepath[i].name)
            return false;
    if(m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.name==name ||
            m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.name == name)
        return false;
    m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].name=name;
    return true;
}
//修改m_MultiPathInfoList上索引为w_index轨迹中onepath上索引为p_index点的点位数据
bool CGraphtoPath::ModifyonePosition(const int w_index, const int p_index, const double pos[6])
{
    return true;
}
//修改m_MultiPathInfoList上当前选择轨迹中onepath上当前选择点的点位数据
//index x:0 y:1 z:2 a:3 b:4 c:5
bool CGraphtoPath::ModifyonePosition(const double posvalue, const int index)
{
    if(curindex_SeloneWholePath<0||curindex_SeloneWholePath>m_MultiPathInfoList.size()-1)
        return false;
    int posnum=m_MultiPathInfoList[curindex_SeloneWholePath].onepath.size();
    if(curindex_SelPointinoneWholePath<0||curindex_SelPointinoneWholePath>posnum-1)
        return false;

    switch (index) {
    case 0: //X
        m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].x=posvalue;
        break;
    case 1: //Y
        m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].y=posvalue;
        break;
    case 2: //Z
        m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].z=posvalue;
        break;
    case 3: //A
        m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].a=posvalue;
        break;
    case 4: //B
        m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].b=posvalue;
        break;
    case 5: //C
        m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].c=posvalue;
        break;
    default:
        break;
    }
    return true;
}
//修改m_MultiPathInfoList上索引为w_index轨迹中onepath上索引为p_index点的动态参数（未使用）
bool CGraphtoPath::ModifyonePositionDynamic(const int w_index, const int p_index, const double dyn[12], const double ovl[5])
{
	return true;
}

//将m_MultiPathInfoList上索引为w_index的轨迹以onepath上索引为spindex的点位为分割点，分成两段独立的轨迹
int CGraphtoPath::PathPathSplittingoneWhole(const int w_index, const int spindex)
{
    if(w_index<0||w_index>=m_MultiPathInfoList.size())
        return 1;
    if(spindex<0||spindex>=m_MultiPathInfoList[w_index].onepath.size())
        return 1;
    if(spindex==0 || spindex == m_MultiPathInfoList[w_index].onepath.size()-1)
        return 2;
    if(m_MultiPathInfoList[w_index].onepath[spindex].mode==0x21)
        return 3;

    oneWholePathinfo m_onepath;
    string newname;
    int pnum=m_MultiPathInfoList[w_index].onepath.size();
    newname=boost::lexical_cast<string>(pnum);
     m_onepath.name=m_MultiPathInfoList[w_index].name+string("_split")+newname;
    for(int i=spindex;i<pnum;i++)
        m_onepath.onepath.push_back(m_MultiPathInfoList[w_index].onepath[i]);
    if(m_onepath.onepath.size()>0)
        m_onepath.onepath[0].mode=1;
    for(int i=spindex+1;i<pnum;i++)
        m_MultiPathInfoList[w_index].onepath.erase(m_MultiPathInfoList[w_index].onepath.begin()+spindex+1);

    m_onepath.gppara.bpWaitFinished=false;
    m_onepath.gppara.InpointEnabled=false;
    m_onepath.gppara.epWaitFinished=m_MultiPathInfoList[w_index].gppara.epWaitFinished;
    m_onepath.gppara.OutpointEnabled=m_MultiPathInfoList[w_index].gppara.OutpointEnabled;
    m_onepath.gppara.Outpoint=m_MultiPathInfoList[w_index].gppara.Outpoint;

    m_MultiPathInfoList[w_index].gppara.epWaitFinished=false;
    m_MultiPathInfoList[w_index].gppara.OutpointEnabled=false;
    m_MultiPathInfoList.insert(m_MultiPathInfoList.begin()+w_index+1, m_onepath);
    return 0;
}

//将m_MultiPathInfoList上索引为w_index的轨迹首尾顺序互反
bool CGraphtoPath::PathChangeDirection(const int w_index)
{
    if(w_index<0||w_index>=m_MultiPathInfoList.size())
        return false;
    oneWholePathinfo m_oneWholePath;

    m_oneWholePath=m_MultiPathInfoList[w_index];
    int pnum=m_oneWholePath.onepath.size();

    for(int i=0;i<pnum;i++)
    {
        if(m_oneWholePath.onepath[i].mode==0x21)//????м???
        {
            if(i==0||i==pnum-1)return false;

            m_oneWholePath.onepath[i-1].mode=0x22;
            m_oneWholePath.onepath[i+1].mode=1;
        }
    }

    m_MultiPathInfoList[w_index].onepath.clear();
    for(int i=0;i<pnum;i++)
        m_MultiPathInfoList[w_index].onepath.push_back(m_oneWholePath.onepath[pnum-1-i]);
    //change in and out assist point
    PosInfo m_pos;
    bool m_flag;
    m_pos=m_MultiPathInfoList[w_index].gppara.Inpoint;
    m_MultiPathInfoList[w_index].gppara.Inpoint = m_MultiPathInfoList[w_index].gppara.Outpoint;
    m_MultiPathInfoList[w_index].gppara.Outpoint=m_pos;
    m_flag=m_MultiPathInfoList[w_index].gppara.InpointEnabled;
    m_MultiPathInfoList[w_index].gppara.InpointEnabled=m_MultiPathInfoList[w_index].gppara.OutpointEnabled;
    m_MultiPathInfoList[w_index].gppara.OutpointEnabled=m_flag;

    m_flag=m_MultiPathInfoList[w_index].gppara.bpWaitFinished;
    m_MultiPathInfoList[w_index].gppara.bpWaitFinished=m_MultiPathInfoList[w_index].gppara.epWaitFinished;
    m_MultiPathInfoList[w_index].gppara.epWaitFinished=m_flag;
    return true;
}
//将m_MultiPathInfoList上索引为w_index的轨迹（首尾相接轨迹）以onepath上索引为spindex的点作为新的起点
int CGraphtoPath::PathBeginPointChange(const int w_index, const int spindex)
{
    if(w_index<0||w_index>=m_MultiPathInfoList.size())
        return 1;
    if(spindex<0||spindex>=m_MultiPathInfoList[w_index].onepath.size())
        return 1;

    int pnum=m_MultiPathInfoList[w_index].onepath.size();
    if(!IsEqualPos(m_MultiPathInfoList[w_index].onepath[0], m_MultiPathInfoList[w_index].onepath[pnum-1]))
        return 2;
    if(m_MultiPathInfoList[w_index].onepath[spindex].mode==0x21)
        return 3;

    if(spindex!=0)
    {
        oneWholePathinfo m_onewholepath;
        m_onewholepath.onepath.clear();
        for(int i=0;i<pnum;i++)
        {
            if((i+spindex)%pnum!=0)
                m_onewholepath.onepath.push_back(m_MultiPathInfoList[w_index].onepath[(i+spindex)%pnum]);
        }
        m_onewholepath.onepath.push_back(m_MultiPathInfoList[w_index].onepath[spindex]);

        m_onewholepath.onepath[0].mode=1;
        m_onewholepath.onepath[0].name=m_MultiPathInfoList[w_index].onepath[spindex].name+string("_bp");

        m_MultiPathInfoList[w_index].onepath.clear();
        //    for(int i=0;i<pnum;i++)
        m_MultiPathInfoList[w_index].onepath=m_onewholepath.onepath;
        //辅助点信息取消
        m_MultiPathInfoList[w_index].gppara.InpointEnabled=false;
        m_MultiPathInfoList[w_index].gppara.OutpointEnabled=false;
        m_MultiPathInfoList[w_index].gppara.bpWaitFinished=false;
        m_MultiPathInfoList[w_index].gppara.epWaitFinished=false;
        curindex_SelPointinoneWholePath=0;
    }
    return 0;
}
//将m_MultiPathInfoList上索引为w_index0的轨迹和m_MultiPathInfoList上索引为w_index1的轨迹，
//两条首尾相接的轨迹合并成为一条新的轨迹
int CGraphtoPath::PathMergingoneWhole(const int w_index0, const int w_index1)
{
    if(w_index0<0||w_index0>=m_MultiPathInfoList.size())
        return 1;
    if(w_index1<0||w_index1>=m_MultiPathInfoList.size())
        return 1;
    if(w_index0==w_index1)return 2;//???????????

    string pathname;
    int pnum0,pnum1;
    pnum0=m_MultiPathInfoList[w_index0].onepath.size();
    pnum1=m_MultiPathInfoList[w_index1].onepath.size();
    if(pnum0>0&&pnum1>0)
    {
        PosInfo bp0, bp1,ep0,ep1;
        bp0=m_MultiPathInfoList[w_index0].onepath[0];
        ep0=m_MultiPathInfoList[w_index0].onepath[pnum0-1];
        bp1=m_MultiPathInfoList[w_index1].onepath[0];
        ep1=m_MultiPathInfoList[w_index1].onepath[pnum1-1];
         string pathname0, pathname1;
         pathname0=m_MultiPathInfoList[w_index0].name;
         pathname1=m_MultiPathInfoList[w_index1].name;
         if(IsEqualPos(ep0, bp1))//??β????
         {
             for(int i=0;i<pnum0;i++)
                 m_MultiPathInfoList[w_index0].onepath[i].name=pathname0+string("_")+m_MultiPathInfoList[w_index0].onepath[i].name;

             for(int i=1;i<pnum1;i++)
             {
                 m_MultiPathInfoList[w_index1].onepath[i].name=pathname1+string("_")+m_MultiPathInfoList[w_index1].onepath[i].name;
                 m_MultiPathInfoList[w_index0].onepath.push_back(m_MultiPathInfoList[w_index1].onepath[i]);
             }
              pathname=m_MultiPathInfoList[w_index0].name;
              m_MultiPathInfoList[w_index0].gppara.OutpointEnabled=m_MultiPathInfoList[w_index1].gppara.OutpointEnabled;
              m_MultiPathInfoList[w_index0].gppara.Outpoint=m_MultiPathInfoList[w_index1].gppara.Outpoint;
              m_MultiPathInfoList[w_index0].gppara.epWaitFinished=m_MultiPathInfoList[w_index1].gppara.epWaitFinished;
              m_MultiPathInfoList.erase(m_MultiPathInfoList.begin()+w_index1);

         }
         else if(IsEqualPos(ep1, bp0))//??β????
         {
             if(PathChangeDirection(w_index0)&&PathChangeDirection(w_index1))
             {
                 for(int i=0;i<pnum0;i++)
                     m_MultiPathInfoList[w_index0].onepath[i].name=pathname0+string("_")+m_MultiPathInfoList[w_index0].onepath[i].name;

                 for(int i=1;i<pnum1;i++)
                 {
                     m_MultiPathInfoList[w_index1].onepath[i].name=pathname1+string("_")+m_MultiPathInfoList[w_index1].onepath[i].name;
                     m_MultiPathInfoList[w_index0].onepath.push_back(m_MultiPathInfoList[w_index1].onepath[i]);
                 }
                 pathname=m_MultiPathInfoList[w_index0].name;
                 m_MultiPathInfoList[w_index0].gppara.OutpointEnabled=m_MultiPathInfoList[w_index1].gppara.OutpointEnabled;
                 m_MultiPathInfoList[w_index0].gppara.Outpoint=m_MultiPathInfoList[w_index1].gppara.Outpoint;
                 m_MultiPathInfoList[w_index0].gppara.epWaitFinished=m_MultiPathInfoList[w_index1].gppara.epWaitFinished;
                 m_MultiPathInfoList.erase(m_MultiPathInfoList.begin()+w_index1);
             }
             else return 3;
         }
         else if(IsEqualPos(bp0, bp1))//????
         {
             if(PathChangeDirection(w_index0))//???
             {
                 for(int i=0;i<pnum0;i++)
                     m_MultiPathInfoList[w_index0].onepath[i].name=pathname0+string("_")+m_MultiPathInfoList[w_index0].onepath[i].name;

                 for(int i=1;i<pnum1;i++)
                 {
                     m_MultiPathInfoList[w_index1].onepath[i].name=pathname1+string("_")+m_MultiPathInfoList[w_index1].onepath[i].name;
                     m_MultiPathInfoList[w_index0].onepath.push_back(m_MultiPathInfoList[w_index1].onepath[i]);
                 }
                 pathname=m_MultiPathInfoList[w_index0].name;
                 m_MultiPathInfoList[w_index0].gppara.OutpointEnabled=m_MultiPathInfoList[w_index1].gppara.OutpointEnabled;
                 m_MultiPathInfoList[w_index0].gppara.Outpoint=m_MultiPathInfoList[w_index1].gppara.Outpoint;
                 m_MultiPathInfoList[w_index0].gppara.epWaitFinished=m_MultiPathInfoList[w_index1].gppara.epWaitFinished;
                 m_MultiPathInfoList.erase(m_MultiPathInfoList.begin()+w_index1);
             }
             return 3;//??????
         }
         else if(IsEqualPos(ep0, ep1))//β??β
         {
             if(PathChangeDirection(w_index1))//???
             {
                 for(int i=0;i<pnum0;i++)
                     m_MultiPathInfoList[w_index0].onepath[i].name=pathname0+string("_")+m_MultiPathInfoList[w_index0].onepath[i].name;

                 for(int i=1;i<pnum1;i++)
                 {
                     m_MultiPathInfoList[w_index1].onepath[i].name=pathname1+string("_")+m_MultiPathInfoList[w_index1].onepath[i].name;
                     m_MultiPathInfoList[w_index0].onepath.push_back(m_MultiPathInfoList[w_index1].onepath[i]);
                 }
                 pathname=m_MultiPathInfoList[w_index0].name;
                 m_MultiPathInfoList[w_index0].gppara.OutpointEnabled=m_MultiPathInfoList[w_index1].gppara.OutpointEnabled;
                 m_MultiPathInfoList[w_index0].gppara.Outpoint=m_MultiPathInfoList[w_index1].gppara.Outpoint;
                 m_MultiPathInfoList[w_index0].gppara.epWaitFinished=m_MultiPathInfoList[w_index1].gppara.epWaitFinished;
                 m_MultiPathInfoList.erase(m_MultiPathInfoList.begin()+w_index1);
             }
             return 3;//??????
         }
         else return 4;//????????β????
    }
    else return 1;

    SetCurIndexofSelOneWholePath(pathname);
    curindex_SelPointinoneWholePath=-1;
    return 0;
}
//从文件filename中读取Dyn参数
bool CGraphtoPath::ReadMotionParaofDynInfo(const string filename)
{
    m_DynVarList.clear();
    DynInfo m_dyninfo;
    for(int i=0;i<12;i++)
        m_dyninfo.dyn[i]=0.0;
    for(int i=0;i<5;i++)
    {
        string name=boost::lexical_cast<string>(i);
        name=string("DynName")+name;
        m_dyninfo.name=name;
        m_dyninfo.quotenum=0;
        m_DynVarList.push_back(m_dyninfo);
    }
    return true;
}
//从文件filename中读取Ovl参数
bool CGraphtoPath::ReadMotionParaofOvlInfo(const string filename)
{
    m_OvlVarList.clear();
    OvlInfo m_ovlinfo;
    for(int i=0;i<5;i++)
        m_ovlinfo.ovl[i]=0.0;
    for(int i=0;i<5;i++)
    {
        string name=boost::lexical_cast<string>(i);
        name=string("OvlName")+name;
        m_ovlinfo.name=name;
        m_ovlinfo.quotenum=0;
        m_OvlVarList.push_back(m_ovlinfo);
    }
    return true;
}

int CGraphtoPath::GetCurIndexofDynInfo(const string dynname)
{
    for(size_t i=0;i<m_DynVarList.size();i++)
        if(dynname==m_DynVarList[i].name)
            return i;
    return -1;
}

int CGraphtoPath::GetIndexofDynInfobyCurSelPath()
{
    if(curindex_SeloneWholePath<0 || curindex_SeloneWholePath>m_MultiPathInfoList.size()-1)
        return -1;
    for(int i=0;i<m_DynVarList.size();i++)
        if(m_DynVarList[i].name == m_MultiPathInfoList[curindex_SeloneWholePath].dpname)
            return i;
    return -1;
}

int CGraphtoPath::GetIndexofDynInfobyCurSelPoint()
{
    if(curindex_SeloneWholePath<0 || curindex_SeloneWholePath>m_MultiPathInfoList.size()-1)
        return -1;
    if(curindex_SelPointinoneWholePath<0 || curindex_SelPointinoneWholePath>m_MultiPathInfoList[curindex_SeloneWholePath].onepath.size())
        return -1;
    for(int i=0;i<m_DynVarList.size();i++)
        if(m_DynVarList[i].name == m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].dpname)
            return i;
    return -1;
}

void CGraphtoPath::GetCurSelDynInfo(const int index, double dyn[])
{
    for(int i=0;i<12;i++)
        dyn[i]=0.0;
    if(index<0||index>m_DynVarList.size()-1)
        return;
    for(int i=0;i<12;i++)
        dyn[i]=m_DynVarList[index].dyn[i];

}

string CGraphtoPath::GetCurSelDynInfoName(const int index)
{
    if(index<0||index>m_DynVarList.size()-1)
        return string("");
    return m_DynVarList[index].name;
}

int CGraphtoPath::GetCurIndexofOvlInfo(const string ovlname)
{
    for(int i=0;i<m_OvlVarList.size();i++)
        if(ovlname==m_OvlVarList[i].name)
            return i;
    return -1;
}

int CGraphtoPath::GetIndexofOvlInfobyCurSelPath()
{
    if(curindex_SeloneWholePath<0 || curindex_SeloneWholePath>m_MultiPathInfoList.size()-1)
        return -1;
    for(int i=0;i<m_OvlVarList.size();i++)
        if(m_OvlVarList[i].name == m_MultiPathInfoList[curindex_SeloneWholePath].opname)
            return i;
    return -1;
}

int CGraphtoPath::GetIndexofOvlInfobyCurSelPoint()
{
    if(curindex_SeloneWholePath<0 || curindex_SeloneWholePath>m_MultiPathInfoList.size()-1)
        return -1;
    if(curindex_SelPointinoneWholePath<0 || curindex_SelPointinoneWholePath>m_MultiPathInfoList[curindex_SeloneWholePath].onepath.size())
        return -1;
    for(int i=0;i<m_OvlVarList.size();i++)
        if(m_OvlVarList[i].name == m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].opname)
            return i;
    return -1;
}

void CGraphtoPath::GetCurSelOvlInfo(const int index, double ovl[])
{
    for(int i=0;i<5;i++)
        ovl[i]=0.0;
    if(index<0||index>m_OvlVarList.size()-1)
        return;
    for(int i=0;i<5;i++)
        ovl[i]=m_OvlVarList[index].ovl[i];
}

string CGraphtoPath::GetCurSelOvlInfoName(const int index)
{
    if(index<0||index>m_OvlVarList.size()-1)
        return string("");
    return m_OvlVarList[index].name;
}

int CGraphtoPath::GetCurSelOvlInfoMode(const int index)
{
    if(index<0||index>m_OvlVarList.size()-1)
        return -1;
    return m_OvlVarList[index].mode;
}

void CGraphtoPath::SetDynParaofSelOneWholePath(const string dpname)
{
    if(curindex_SeloneWholePath<0 || curindex_SeloneWholePath>m_MultiPathInfoList.size()-1)
        return;
    string oldname=m_MultiPathInfoList[curindex_SeloneWholePath].dpname;
    ChangeQuoteNumofDynPara(oldname,-1);
    m_MultiPathInfoList[curindex_SeloneWholePath].dpname=dpname;
    ChangeQuoteNumofDynPara(dpname, 1);
}

void CGraphtoPath::SetOvlParaofSelOneWholePath(const string opname)
{
    if(curindex_SeloneWholePath<0 || curindex_SeloneWholePath>m_MultiPathInfoList.size()-1)
        return;
    string oldname=m_MultiPathInfoList[curindex_SeloneWholePath].opname;
    ChangeQuoteNumofOvlPara(oldname, -1);
    m_MultiPathInfoList[curindex_SeloneWholePath].opname=opname;
    ChangeQuoteNumofOvlPara(opname, 1);
}

void CGraphtoPath::SetDynParaofSelPointinOneWholePath(const string dpname)
{
    if(curindex_SeloneWholePath<0 || curindex_SeloneWholePath>m_MultiPathInfoList.size()-1)
        return;
    if(curindex_SelPointinoneWholePath<0 || curindex_SelPointinoneWholePath>m_MultiPathInfoList[curindex_SeloneWholePath].onepath.size())
        return;
    m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].dpname=dpname;
}

void CGraphtoPath::SetOvlParaofSelPointinOneWholePath(const string opname)
{
    if(curindex_SeloneWholePath<0 || curindex_SeloneWholePath>m_MultiPathInfoList.size()-1)
        return;
    if(curindex_SelPointinoneWholePath<0 || curindex_SelPointinoneWholePath>m_MultiPathInfoList[curindex_SeloneWholePath].onepath.size())
        return;
    m_MultiPathInfoList[curindex_SeloneWholePath].onepath[curindex_SelPointinoneWholePath].opname=opname;
}

bool CGraphtoPath::AddNewDynParaInfo(const string dynname)
{
    for(size_t i=0;i<m_DynVarList.size();i++)
        if(dynname==m_DynVarList[i].name)
            return false;
    DynInfo m_dyninfo;
    m_dyninfo.quotenum=0;
    //Joint
    m_dyninfo.dyn[0]=93;
    m_dyninfo.dyn[1]=94;
    m_dyninfo.dyn[2]=94;
    m_dyninfo.dyn[3]=94;
    //Linear
    m_dyninfo.dyn[4]=1500.00;
    m_dyninfo.dyn[5]=6000.00;
    m_dyninfo.dyn[6]=6000.00;
    m_dyninfo.dyn[7]=1000000.00;
    //Ori
    m_dyninfo.dyn[8]=90.00;
    m_dyninfo.dyn[9]=180.00;
    m_dyninfo.dyn[10]=180.00;
    m_dyninfo.dyn[11]=1000000.00;

    m_dyninfo.name=dynname;

    m_DynVarList.push_back(m_dyninfo);

    return true;
}

bool CGraphtoPath::AddNewOvlParaInfo(const string ovlname, const int ovlmode)
{
    for(size_t i=0;i<m_OvlVarList.size();i++)
        if(ovlname==m_OvlVarList[i].name)
            return false;
    OvlInfo m_ovlinfo;
    m_ovlinfo.quotenum=0;
    if(ovlmode==0)
    {
        m_ovlinfo.mode=0;
        m_ovlinfo.ovl[0]=0.00;
        m_ovlinfo.ovl[1]=360.00;
        m_ovlinfo.ovl[2]=10000.00;
        m_ovlinfo.ovl[3]=360.00;
        m_ovlinfo.ovl[4]=0;
    }
    else if(ovlmode==1)
    {
        m_ovlinfo.mode=1;
        m_ovlinfo.ovl[0]=100;
    }
    else if(ovlmode==2)
    {
        m_ovlinfo.mode=2;
        m_ovlinfo.ovl[0]=200;
    }
    else return false;

    m_ovlinfo.name=ovlname;
    m_OvlVarList.push_back(m_ovlinfo);
    return true;
}

bool CGraphtoPath::DeleteDynParaInfo(const string dynname)
{
    for(size_t i=0;i<m_DynVarList.size();i++)
        if(dynname == m_DynVarList[i].name)
        {
            if(m_DynVarList[i].quotenum>0)return false;
            else
            {
                m_DynVarList.erase(m_DynVarList.begin()+i);
                return true;
            }
        }
    return false;
}

bool CGraphtoPath::DeleteOvlParaInfo(const string ovlname)
{
    for(int i=0;i<m_OvlVarList.size();i++)
        if(ovlname == m_OvlVarList[i].name)
        {
            if(m_OvlVarList[i].quotenum>0)return false;
            else
            {
                m_OvlVarList.erase(m_OvlVarList.begin()+i);
                return true;
            }
        }
    return true;
}

void CGraphtoPath::ChangeQuoteNumofDynPara(const string dynname, const int flag)
{
    if(dynname==string(""))return;
    if(flag==1||flag==-1)
    {
        for(size_t i=0;i<m_DynVarList.size();i++)
            if(dynname==m_DynVarList[i].name)
            {
                m_DynVarList[i].quotenum+=flag;
                if(m_DynVarList[i].quotenum<0)m_DynVarList[i].quotenum=0;
                return;
            }
    }
}

void CGraphtoPath::ChangeQuoteNumofOvlPara(const string ovlname, const int flag)
{
    if(ovlname==string(""))return;
    if(flag==1||flag==-1)
    {
        for(size_t i=0;i<m_OvlVarList.size();i++)
            if(ovlname==m_OvlVarList[i].name)
            {
                m_OvlVarList[i].quotenum+=flag;
                if(m_OvlVarList[i].quotenum<0)m_OvlVarList[i].quotenum=0;
                return;
            }
    }
}

bool CGraphtoPath::ModifyDynParaName(const string oldname, const string newname)
{
    size_t index;
    for(index=0;index<m_DynVarList.size();index++)
        if(m_DynVarList[index].name==oldname)
            break;
    if(index==m_DynVarList.size())return false;
    for(size_t i=0;i<m_DynVarList.size();i++)
        if(i!=index && m_DynVarList[i].name==newname)
            return false;
    m_DynVarList[index].name=newname;
}

bool CGraphtoPath::ModifyOvlParaName(const string oldname, const string newname)
{
    size_t index;
    for(index=0;index<m_OvlVarList.size();index++)
        if(m_OvlVarList[index].name==oldname)
            break;
    if(index==m_OvlVarList.size())return false;
    for(size_t i=0;i<m_OvlVarList.size();i++)
        if(i!=index && m_OvlVarList[i].name==newname)
            return false;
    m_OvlVarList[index].name=newname;
}

bool CGraphtoPath::ModifyDynParaInfo(const string dynname, const double value, const int index)
{
    if(index<0||index>11)return false;
    for(size_t i=0;i<m_DynVarList.size();i++)
        if(m_DynVarList[i].name == dynname)
        {
            m_DynVarList[i].dyn[index]=value;
            return true;
        }
    return false;
}

bool CGraphtoPath::ModifyOvlParaInfo(const string ovlname, const double value, const int index)
{
    if(index<0||index>4)return false;
    for(size_t i=0;i<m_OvlVarList.size();i++)
        if(m_OvlVarList[i].name == ovlname)
        {
                m_OvlVarList[i].ovl[index]=value;
            return true;
        }
    return false;
}

bool CGraphtoPath::SetGenerateParaofAllPaths(const GenerateProgramPara &m_gppara)
{
    for(size_t i=0;i<m_MultiPathInfoList.size();i++)
    {
        m_MultiPathInfoList[i].gppara=m_gppara;
    }
    return false;
}

bool CGraphtoPath::SetAssistPointforCurrentPath(const string name, const double dpos[], const int apflag)
{
    if(curindex_SeloneWholePath<0 || curindex_SeloneWholePath >m_MultiPathInfoList.size()-1)
        return false;
    if(m_MultiPathInfoList[curindex_SeloneWholePath].onepath.empty())return false;
    PosInfo m_basepoint;
    if(apflag==0)//InPoint
    {
        m_basepoint=m_MultiPathInfoList[curindex_SeloneWholePath].onepath[0];
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.x=m_basepoint.x+dpos[0];
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.y=m_basepoint.y+dpos[1];
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.z=m_basepoint.z+dpos[2];
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.a=m_basepoint.a+dpos[3];
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.b=m_basepoint.b+dpos[4];
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.c=m_basepoint.c+dpos[5];
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.name=name;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.mode=0;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.InpointEnabled=true;
    }
    else if(apflag==1)//OutPoint
    {
        int num=m_MultiPathInfoList[curindex_SeloneWholePath].onepath.size()-1;
        m_basepoint=m_MultiPathInfoList[curindex_SeloneWholePath].onepath[num];
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.x=m_basepoint.x+dpos[0];
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.y=m_basepoint.y+dpos[1];
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.z=m_basepoint.z+dpos[2];
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.a=m_basepoint.a+dpos[3];
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.b=m_basepoint.b+dpos[4];
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.c=m_basepoint.c+dpos[5];
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.name=name;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.mode=1;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.OutpointEnabled=true;
    }
    return true;
}

bool CGraphtoPath::SetAssistPointforAllPaths(const string name, const double dpos[], const int apflag)
{
    PosInfo m_basepoint;
    for(size_t i=0;i<m_MultiPathInfoList.size();i++)
    {
        if(m_MultiPathInfoList[i].onepath.empty())break;

        if(apflag==0)//InPoint
        {
            m_basepoint=m_MultiPathInfoList[i].onepath[0];
            m_MultiPathInfoList[i].gppara.Inpoint.x=m_basepoint.x+dpos[0];
            m_MultiPathInfoList[i].gppara.Inpoint.y=m_basepoint.y+dpos[1];
            m_MultiPathInfoList[i].gppara.Inpoint.z=m_basepoint.z+dpos[2];
            m_MultiPathInfoList[i].gppara.Inpoint.a=m_basepoint.a+dpos[3];
            m_MultiPathInfoList[i].gppara.Inpoint.b=m_basepoint.b+dpos[4];
            m_MultiPathInfoList[i].gppara.Inpoint.c=m_basepoint.c+dpos[5];
            m_MultiPathInfoList[i].gppara.Inpoint.name=name;
            m_MultiPathInfoList[i].gppara.Inpoint.mode=0;
            m_MultiPathInfoList[i].gppara.InpointEnabled=true;
        }
        else if(apflag==1)//OutPoint
        {
            int num=m_MultiPathInfoList[i].onepath.size()-1;
            m_basepoint=m_MultiPathInfoList[i].onepath[num];
            m_MultiPathInfoList[i].gppara.Outpoint.x=m_basepoint.x+dpos[0];
            m_MultiPathInfoList[i].gppara.Outpoint.y=m_basepoint.y+dpos[1];
            m_MultiPathInfoList[i].gppara.Outpoint.z=m_basepoint.z+dpos[2];
            m_MultiPathInfoList[i].gppara.Outpoint.a=m_basepoint.a+dpos[3];
            m_MultiPathInfoList[i].gppara.Outpoint.b=m_basepoint.b+dpos[4];
            m_MultiPathInfoList[i].gppara.Outpoint.c=m_basepoint.c+dpos[5];
            m_MultiPathInfoList[i].gppara.Outpoint.name=name;
            m_MultiPathInfoList[i].gppara.Outpoint.mode=1;
            m_MultiPathInfoList[i].gppara.OutpointEnabled=true;
        }
    }
    return true;
}

bool CGraphtoPath::SetAPWaitFinishedofCurrentPath(const bool val, const int apflag)
{
    if(curindex_SeloneWholePath<0 || curindex_SeloneWholePath >m_MultiPathInfoList.size()-1)
        return false;
    if(apflag==0)
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.bpWaitFinished=val;
    else if(apflag==1)
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.epWaitFinished=val;
    else return false;
    return true;
}

bool CGraphtoPath::DeleteAssistPointofCurrentPath(const int apflag)
{
    if(curindex_SeloneWholePath<0 || curindex_SeloneWholePath >m_MultiPathInfoList.size()-1)
        return false;
    if(apflag==0)
    {
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.InpointEnabled=false;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.name=string("");
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.x=0.0;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.y=0.0;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.z=0.0;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.a=0.0;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.b=0.0;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.c=0.0;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.dpname=string("");
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.opname=string("");
    }
    else if(apflag==1)
    {
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.OutpointEnabled=false;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.name=string("");
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.x=0.0;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.y=0.0;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.z=0.0;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.a=0.0;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.b=0.0;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.c=0.0;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.dpname=string("");
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.opname=string("");
    }
    else return false;
    return true;
}

bool CGraphtoPath::DeleteAssisPointofAllPaths(const int apflag)
{
    if(apflag==0)
    {
        for(size_t i=0;i<m_MultiPathInfoList.size();i++)
        {
            m_MultiPathInfoList[i].gppara.InpointEnabled=false;
            m_MultiPathInfoList[i].gppara.Inpoint.name=string("");
            m_MultiPathInfoList[i].gppara.Inpoint.x=0.0;
            m_MultiPathInfoList[i].gppara.Inpoint.y=0.0;
            m_MultiPathInfoList[i].gppara.Inpoint.z=0.0;
            m_MultiPathInfoList[i].gppara.Inpoint.a=0.0;
            m_MultiPathInfoList[i].gppara.Inpoint.b=0.0;
            m_MultiPathInfoList[i].gppara.Inpoint.c=0.0;
            m_MultiPathInfoList[i].gppara.Inpoint.dpname=string("");
            m_MultiPathInfoList[i].gppara.Inpoint.opname=string("");
        }
    }
    else if(apflag==1)
    {
        for(size_t i=0;i<m_MultiPathInfoList.size();i++)
        {
            m_MultiPathInfoList[i].gppara.OutpointEnabled=false;
            m_MultiPathInfoList[i].gppara.Outpoint.name=string("");
            m_MultiPathInfoList[i].gppara.Outpoint.x=0.0;
            m_MultiPathInfoList[i].gppara.Outpoint.y=0.0;
            m_MultiPathInfoList[i].gppara.Outpoint.z=0.0;
            m_MultiPathInfoList[i].gppara.Outpoint.a=0.0;
            m_MultiPathInfoList[i].gppara.Outpoint.b=0.0;
            m_MultiPathInfoList[i].gppara.Outpoint.c=0.0;
            m_MultiPathInfoList[i].gppara.Outpoint.dpname=string("");
            m_MultiPathInfoList[i].gppara.Outpoint.opname=string("");
        }
    }
    else return false;

    return true;
}

void CGraphtoPath::GetPointInformationofcurSelPathAssistPoint(string &pname, double pos[], string &ptype, const int apflag)
{
    pname=string("");
    ptype=string("");
    for(int i=0;i<6;i++)pos[i]=0.0;
    if(curindex_SeloneWholePath<0||curindex_SeloneWholePath>m_MultiPathInfoList.size()-1)
        return;
    if(apflag==0)//Inpoint
    {
        if(m_MultiPathInfoList[curindex_SeloneWholePath].gppara.InpointEnabled)
        {
            pname=m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.name;
            pos[0]=m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.x;
            pos[1]=m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.y;
            pos[2]=m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.z;
            pos[3]=m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.a;
            pos[4]=m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.b;
            pos[5]=m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.c;
            ptype=string("In_APoint");
        }
    }
    else if(apflag==1)//Outpoint
    {
        if(m_MultiPathInfoList[curindex_SeloneWholePath].gppara.OutpointEnabled)
        {
            pname=m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.name;
            pos[0]=m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.x;
            pos[1]=m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.y;
            pos[2]=m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.z;
            pos[3]=m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.a;
            pos[4]=m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.b;
            pos[5]=m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.c;
            ptype=string("Out_APoint");
        }
    }

}

void CGraphtoPath::GetAssistInformationofcurSelPathAssistPoint(bool &InPEnabled, bool &OutPEnabled, bool &bpWF, bool &epWF)
{
    InPEnabled=false;
    OutPEnabled=false;
    bpWF=false;
    epWF=false;
    if(curindex_SeloneWholePath<0||curindex_SeloneWholePath>m_MultiPathInfoList.size()-1)
        return;

    InPEnabled=m_MultiPathInfoList[curindex_SeloneWholePath].gppara.InpointEnabled;
    OutPEnabled=m_MultiPathInfoList[curindex_SeloneWholePath].gppara.OutpointEnabled;
    bpWF=m_MultiPathInfoList[curindex_SeloneWholePath].gppara.bpWaitFinished;
    epWF=m_MultiPathInfoList[curindex_SeloneWholePath].gppara.epWaitFinished;
}

bool CGraphtoPath::ModifyAssistPointforCurSelPath(const double val, const int index, const int apflag)
{
    if(curindex_SeloneWholePath<0 ||curindex_SeloneWholePath>m_MultiPathInfoList.size()-1)
        return false;
    if(apflag==0)//Inpoint
    {
        switch(index)
        {
        case 0:
            m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.x=val;
            break;
        case 1:
            m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.y=val;
            break;
        case 2:
            m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.z=val;
            break;
        case 3:
            m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.a=val;
            break;
        case 4:
            m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.b=val;
            break;
        case 5:
            m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.c=val;
            break;
        default:
            break;
        }
        return true;
    }
    else if(apflag==1)//Outpoint
    {
        switch(index)
        {
        case 0:
            m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.x=val;
            break;
        case 1:
            m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.y=val;
            break;
        case 2:
            m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.z=val;
            break;
        case 3:
            m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.a=val;
            break;
        case 4:
            m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.b=val;
            break;
        case 5:
            m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.c=val;
            break;
        default:
            break;
        }
        return true;
    }
    else return false;
}

bool CGraphtoPath::ModifyAPNameofCurSelPath(const string name, const int apflag)
{
    if(curindex_SeloneWholePath<0 ||curindex_SeloneWholePath>m_MultiPathInfoList.size()-1)
        return false;
    for(size_t i=0;i<m_MultiPathInfoList[curindex_SeloneWholePath].onepath.size();i++)
        if(name==m_MultiPathInfoList[curindex_SeloneWholePath].onepath[i].name)
            return false;
    if(apflag==0)
    {
        if(m_MultiPathInfoList[curindex_SeloneWholePath].gppara.OutpointEnabled)
            if(name == m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.name)
                return false;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.name=name;
    }
    else if(apflag==1)
    {
        if(m_MultiPathInfoList[curindex_SeloneWholePath].gppara.InpointEnabled)
            if(name == m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.name)
                return false;
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.name=name;
    }
    else  return false;
}

bool CGraphtoPath::SetAPDynParaofCurSelPath(const string name, const int apflag)
{
    if(curindex_SeloneWholePath<0 ||curindex_SeloneWholePath>m_MultiPathInfoList.size()-1)
        return false;
    if(apflag==0)
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.dpname=name;
    else if(apflag==1)
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.dpname=name;
    else return false;

    return true;
}

bool CGraphtoPath::SetAPOvlParaofCurSelPath(const string name, const int apflag)
{
    if(curindex_SeloneWholePath<0 ||curindex_SeloneWholePath>m_MultiPathInfoList.size()-1)
        return false;
    if(apflag==0)
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Inpoint.opname=name;
    else if(apflag==1)
        m_MultiPathInfoList[curindex_SeloneWholePath].gppara.Outpoint.opname=name;
    else return false;

    return true;
}
/// 生成KEBA程序项目预览
bool CGraphtoPath::GeneratePathProgramPreview_KEBA()
{
    if(m_MultiPathInfoList.empty())return false;
    m_ProjectProgram.clear();
    ProgramInfo m_onepathprogram;
    PosInfo m_pos;
    string strline;
    size_t index(0);
    //Main Program
    m_onepathprogram.orderslist.clear();
    m_onepathprogram.name=string("Main");
    for(size_t k=0;k<m_MultiPathInfoList.size();k++)
    {
        strline=string("CALL ")+m_MultiPathInfoList[k].name+string("()");
        m_onepathprogram.orderslist.push_back(strline);
    }
    m_ProjectProgram.push_back(m_onepathprogram);
    //子程序
    for(size_t i=0;i<m_MultiPathInfoList.size();i++)
    {
        m_onepathprogram.orderslist.clear();
        m_onepathprogram.name=m_MultiPathInfoList[i].name;
        if(m_MultiPathInfoList[i].dpname!=string(""))
        {
            strline=string("Dyn(")+m_MultiPathInfoList[i].dpname+string(")");
            m_onepathprogram.orderslist.push_back(strline);
        }
        if(m_MultiPathInfoList[i].opname!=string(""))
        {
            strline=string("Ovl(")+m_MultiPathInfoList[i].opname+string(")");
            m_onepathprogram.orderslist.push_back(strline);
        }
        if(m_MultiPathInfoList[i].gppara.InpointEnabled)//Inpoint
        {
            m_pos=m_MultiPathInfoList[i].gppara.Inpoint;
            strline=GenerateOrderJointorLinear(m_pos);
            m_onepathprogram.orderslist.push_back(strline);
        }
        index=0;
        while(index<m_MultiPathInfoList[i].onepath.size())
        {
            m_pos=m_MultiPathInfoList[i].onepath[index];
            if(m_pos.mode==0||m_pos.mode==1)//Joint or Linear
            {
                strline=GenerateOrderJointorLinear(m_pos);
                m_onepathprogram.orderslist.push_back(strline);
                if(index==0&&m_MultiPathInfoList[i].gppara.bpWaitFinished)
                {
                    strline=string("WaitIsFinished()");
                    m_onepathprogram.orderslist.push_back(strline);
                }
                index++;
            }
            else if(m_pos.mode==0x21)//arc
            {
                strline=GenerateOrderJArc(m_pos, m_MultiPathInfoList[i].onepath[index+1]);
                m_onepathprogram.orderslist.push_back(strline);
                if(index==0&&m_MultiPathInfoList[i].gppara.bpWaitFinished)
                {
                    strline=string("WaitIsFinished()");
                    m_onepathprogram.orderslist.push_back(strline);
                }
                index+=2;
            }
        }
        if(m_MultiPathInfoList[i].gppara.epWaitFinished)
        {
            strline=string("WaitIsFinished()");
            m_onepathprogram.orderslist.push_back(strline);
        }
        if(m_MultiPathInfoList[i].gppara.OutpointEnabled)
        {
            m_pos=m_MultiPathInfoList[i].gppara.Outpoint;
            strline=GenerateOrderJointorLinear(m_pos);
            m_onepathprogram.orderslist.push_back(strline);
        }
        m_ProjectProgram.push_back(m_onepathprogram);
    }

    return true;
}

string CGraphtoPath::GenerateOrderJointorLinear(PosInfo &pos)
{
    string order("");
    if(pos.mode==0)//Joint
    {
        if(pos.dpname!=string("")&&pos.opname!=string(""))
            order=string("PTP( ")+pos.name+string(", ")+pos.dpname+string(", ")+pos.opname+string(" )");
        else if(pos.dpname!=string("")&&pos.opname==string(""))
            order=string("PTP( ")+pos.name+string(", ")+pos.dpname+string(" )");
        else if(pos.dpname==string("")&&pos.opname!=string(""))
            order=string("PTP( ")+pos.name+string(", ")+pos.opname+string(" )");
        else order=string("PTP( ")+pos.name+string(" )");
    }
    else if(pos.mode==1)//Linear
    {
        if(pos.dpname!=string("")&&pos.opname!=string(""))
            order=string("Lin( ")+pos.name+string(", ")+pos.dpname+string(", ")+pos.opname+string(" )");
        else if(pos.dpname!=string("")&&pos.opname==string(""))
            order=string("Lin( ")+pos.name+string(", ")+pos.dpname+string(" )");
        else if(pos.dpname==string("")&&pos.opname!=string(""))
            order=string("Lin( ")+pos.name+string(", ")+pos.opname+string(" )");
        else order=string("Lin( ")+pos.name+string(" )");
    }
    return order;
}

string CGraphtoPath::GenerateOrderJArc(PosInfo &mpos, PosInfo &epos)
{
    string order("");
    if(mpos.mode==0x21&&epos.mode==0x22)
    {
        if(epos.dpname!=string("")&&epos.opname!=string(""))
            order=string("Circ( ")+mpos.name+string(", ")+epos.name+string(", ")+epos.dpname+string(", ")+epos.opname+string(" )");
        else if(epos.dpname!=string("")&&epos.opname==string(""))
            order=string("Circ( ")+mpos.name+string(", ")+epos.name+string(", ")+epos.dpname+string(" )");
        else if(epos.dpname==string("")&&epos.opname!=string(""))
            order=string("Circ( ")+mpos.name+string(", ")+epos.name+string(", ")+epos.opname+string(" )");
        else order=string("Circ( ")+mpos.name+string(", ")+epos.name+string(" )");
    }
    return order;
}
//保存KEBA程序项目文件：程序文件、点位数据文件、项目全局变量文件
bool CGraphtoPath::SaveProgramProject_KEBA(const string savepath)
{
    if(m_MultiPathInfoList.empty()||m_ProjectProgram.empty())return false;

    SetSaveFilePath(savepath);
    bool isok(true);
    //保存各轨迹的点位、进入点、脱离点的位置
    for(size_t i=0;i<m_MultiPathInfoList.size();i++)
        isok&=SaveProgramPointData_KEBA(m_MultiPathInfoList[i]);
    //保存程序
    for(size_t i=0;i<m_ProjectProgram.size();i++)
        isok&=SaveProgramOrder_KEBA(m_ProjectProgram[i]);
    //保存动态及Blend参数
    isok&=SaveProgramGlobalData_KEBA();

    return isok;
}

bool CGraphtoPath::SaveProgramPointData_KEBA(const oneWholePathinfo &m_onepath)
{
    if(m_onepath.onepath.size()<=0)
        return false;

    string filename,extname("");
    filename=savePath+string("//")+extname+m_onepath.name+string(".tid");
    string str(""), strcons("");
    ofstream fp;
    fp.open(filename);//,ios_base::out | ios_base::trunc  );
    for(size_t i=0;i<m_onepath.onepath.size();i++)
    {
        str=("");
        str=/*m_onepath.name+string("_")+*/m_onepath.onepath[i].name+string(" : CARTPOS := (x := ")+boost::lexical_cast<string>(m_onepath.onepath[i].x)
                +string(", y := ")+boost::lexical_cast<string>(m_onepath.onepath[i].y)
                +string(", z := ")+boost::lexical_cast<string>(m_onepath.onepath[i].z)
                +string(", a := ")+boost::lexical_cast<string>(m_onepath.onepath[i].a)
                +string(", b := ")+boost::lexical_cast<string>(m_onepath.onepath[i].b)
                +string(", c := ")+boost::lexical_cast<string>(m_onepath.onepath[i].c)
                +string(", mode := 1)");
        strcons+=str+string("\r\n");
    }
    //添加进入点位置
    if(m_onepath.gppara.InpointEnabled)
    {
        str=("");
        str=/*m_onepath.name+string("_")+*/m_onepath.gppara.Inpoint.name
                +string(" : CARTPOS := (x := ")+boost::lexical_cast<string>(m_onepath.gppara.Inpoint.x)
                +string(", y := ")+boost::lexical_cast<string>(m_onepath.gppara.Inpoint.y)
                +string(", z := ")+boost::lexical_cast<string>(m_onepath.gppara.Inpoint.z)
                +string(", a := ")+boost::lexical_cast<string>(m_onepath.gppara.Inpoint.a)
                +string(", b := ")+boost::lexical_cast<string>(m_onepath.gppara.Inpoint.b)
                +string(", c := ")+boost::lexical_cast<string>(m_onepath.gppara.Inpoint.c)
                +string(", mode := 1)");
        strcons+=str+string("\r\n");
    }
    if(m_onepath.gppara.OutpointEnabled)
    {
        str=("");
        str=/*m_onepath.name+string("_")+*/m_onepath.gppara.Outpoint.name
                +string(" : CARTPOS := (x := ")+boost::lexical_cast<string>(m_onepath.gppara.Outpoint.x)
                +string(", y := ")+boost::lexical_cast<string>(m_onepath.gppara.Outpoint.y)
                +string(", z := ")+boost::lexical_cast<string>(m_onepath.gppara.Outpoint.z)
                +string(", a := ")+boost::lexical_cast<string>(m_onepath.gppara.Outpoint.a)
                +string(", b := ")+boost::lexical_cast<string>(m_onepath.gppara.Outpoint.b)
                +string(", c := ")+boost::lexical_cast<string>(m_onepath.gppara.Outpoint.c)
                +string(", mode := 1)");
        strcons+=str+string("\r\n");
    }
    string savecons(strcons);
    fp.write(savecons.c_str(), savecons.size());
    fp.flush();
    fp.close();

    return true;
}
bool CGraphtoPath::SaveProgramOrder_KEBA(const ProgramInfo &m_onepathorder)
{
    if(m_onepathorder.orderslist.empty())return false;
    string filename,extname("");
    filename=savePath+string("//")+extname+m_onepathorder.name+string(".tip");
    string strcons("");
    ofstream fp;
    fp.open(filename);//,ios_base::out | ios_base::trunc  );

    for(size_t i=0;i<m_onepathorder.orderslist.size();i++)
        strcons+=m_onepathorder.orderslist[i]+string("\n");

    string savecons(strcons);

    fp.write(savecons.c_str(), savecons.size());
    fp.flush();
    fp.close();
    return true;
}
bool CGraphtoPath::SaveProgramGlobalData_KEBA()
{
    if(m_DynVarList.empty()&&m_OvlVarList.empty())return true;

    string filename,extname("");
    filename=savePath+string("//")+extname+string("_globalvars.tid");
    string strcons(""), str;
    ofstream fp;
    fp.open(filename);//,ios_base::out | ios_base::trunc  );

    for(size_t i=0;i<m_DynVarList.size();i++)
    {
        str=("");
        str=m_DynVarList[i].name+string(" : DYNAMIC := (velAxis := ")+boost::lexical_cast<string>(m_DynVarList[i].dyn[0])
                +string(", accAxis := ")+boost::lexical_cast<string>(m_DynVarList[i].dyn[1])
                +string(", decAxis := ")+boost::lexical_cast<string>(m_DynVarList[i].dyn[2])
                +string(", jerkAxis := ")+boost::lexical_cast<string>(m_DynVarList[i].dyn[3])
                +string(", vel := ")+boost::lexical_cast<string>(m_DynVarList[i].dyn[4])
                +string(", acc := ")+boost::lexical_cast<string>(m_DynVarList[i].dyn[5])
                +string(", dec := ")+boost::lexical_cast<string>(m_DynVarList[i].dyn[6])
                +string(", jerk := ")+boost::lexical_cast<string>(m_DynVarList[i].dyn[7])
                +string(", velOri := ")+boost::lexical_cast<string>(m_DynVarList[i].dyn[8])
                +string(", accOri := ")+boost::lexical_cast<string>(m_DynVarList[i].dyn[9])
                +string(", decOri := ")+boost::lexical_cast<string>(m_DynVarList[i].dyn[10])
                +string(", jerkOri := ")+boost::lexical_cast<string>(m_DynVarList[i].dyn[11])+string(")");
        strcons+=str+string("\r\n");
    }
    for(size_t i=0;i<m_OvlVarList.size();i++)
    {
        str=("");
        if(m_OvlVarList[i].mode==0)//ABS
        {
            str=m_OvlVarList[i].name+string(" : OVLABS := (posDist := ")+boost::lexical_cast<string>(m_OvlVarList[i].ovl[0])
                    +string(", oriDist := ")+boost::lexical_cast<string>(m_OvlVarList[i].ovl[1])
                    +string(", linAxDist := ")+boost::lexical_cast<string>(m_OvlVarList[i].ovl[2])
                    +string(", rotAxDist := ")+boost::lexical_cast<string>(m_OvlVarList[i].ovl[3])+string(")");
            strcons+=str+string("\r\n");
        }
        else if(m_OvlVarList[i].mode==1)//REL
        {
            str=m_OvlVarList[i].name+string(" : OVLREL := (ovl := ")+boost::lexical_cast<string>(m_OvlVarList[i].ovl[0])+string(")");
            strcons+=str+string("\r\n");
        }
        else if(m_OvlVarList[i].mode==2)//Suppos
        {
            str=m_OvlVarList[i].name+string(" : OVLSUPPOS := (ovl := ")+boost::lexical_cast<string>(m_OvlVarList[i].ovl[0])+string(")");
            strcons+=str+string("\r\n");
        }
    }

    string savecons(strcons);

    fp.write(savecons.c_str(), savecons.size());
    fp.flush();
    fp.close();
    return true;
}



