#pragma once
#include "dl_creationadapter.h"

#include <vector>
using namespace  std;
class CmyDxfFilter :
	public DL_CreationAdapter
{
public:
	vector<DL_LineData>m_Lineslist;//直线两个端点坐标
	vector<DL_CircleData>m_Circleslist;//整圆数据
	vector<DL_ArcData>m_ArcsList;//圆弧数据
	vector<DL_PolylineData>m_PolylinesList;//多边形数据
	vector<DL_VertexData>m_VertexsList;//多边形上的点位数据
	vector<DL_EllipseData>m_EllipsesList; //椭圆圆弧数据

private:
	//多边形顶点个数读取成功（LWPOLYLINE可以读取顶点个数；POLYLINE读不到顶点个数）
	int curpindex;//当前多边形索引,-1为读取成功，其他为没有读取成功
	bool is_datainblock;//标记当前数据是否在块中，如在块中不读取
public:

	CmyDxfFilter(void);
	~CmyDxfFilter(void);
	virtual void addLine(const DL_LineData& data); //读取直线信息
	virtual void addCircle(const DL_CircleData& data); //读取整圆信息
	virtual void addArc(const DL_ArcData& data);//读取圆弧信息
	virtual void addPolyline(const DL_PolylineData& data);//读取多线段信息
	virtual void addVertex(const DL_VertexData& data);//读取端点信息
	virtual void addEllipse(const DL_EllipseData& data);//读取椭圆曲线信息
	virtual void addBlock(const DL_BlockData& data);
	virtual void endBlock();

};
