#include "CmyDxfFilter.h"

CmyDxfFilter::CmyDxfFilter(void)
{
	is_datainblock=false;
	m_Lineslist.clear();
	m_Circleslist.clear();
	m_ArcsList.clear();
	m_PolylinesList.clear();
	m_VertexsList.clear();
	curpindex=-1;
}

CmyDxfFilter::~CmyDxfFilter(void)
{
	m_Circleslist.clear();
	m_Lineslist.clear();
	m_ArcsList.clear();
	m_PolylinesList.clear();
	m_VertexsList.clear();
}

//存储直线信息 index 1;
void CmyDxfFilter::addLine(const DL_LineData &data)
{
	if(!is_datainblock)
	{
		m_Lineslist.push_back(data);
		curpindex=-1;
	}

}
void CmyDxfFilter::addBlock(const DL_BlockData& data)
{
	is_datainblock=true;
}
void CmyDxfFilter::endBlock()
{
	is_datainblock=false;
}


//存储圆弧信息 index 2;
void CmyDxfFilter::addArc(const DL_ArcData&data)
{
	if (!is_datainblock)
	{
		m_ArcsList.push_back(data);
		curpindex=-1;
	}
}

//存储圆信息 index 3;
void CmyDxfFilter::addCircle(const DL_CircleData&data)
{
	if (!is_datainblock)
	{
		m_Circleslist.push_back(data);
		curpindex=-1;
	}
}

//存储多边形 index 4;
void CmyDxfFilter::addPolyline(const DL_PolylineData&data)
{
// 	if (!is_datainblock)
	{
		m_PolylinesList.push_back(data);
		if(data.number==0)
			curpindex=m_PolylinesList.size()-1;
		else 
			curpindex=-1;
	}
}



//存储多边形上的位置信息 index 5;
void CmyDxfFilter::addVertex(const DL_VertexData&data)
{
	if (!is_datainblock)
	{
		m_VertexsList.push_back(data);
		if(curpindex!=-1)
			m_PolylinesList[curpindex].number++;
	}
}
//读取椭圆曲线信息
void CmyDxfFilter::addEllipse( const DL_EllipseData& data )
{
	if(!is_datainblock)
	{
		m_EllipsesList.push_back(data);
		curpindex=-1;
	}
}


