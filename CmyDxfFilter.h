#pragma once
#include "dl_creationadapter.h"

#include <vector>
using namespace  std;
class CmyDxfFilter :
	public DL_CreationAdapter
{
public:
	vector<DL_LineData>m_Lineslist;//ֱ�������˵�����
	vector<DL_CircleData>m_Circleslist;//��Բ����
	vector<DL_ArcData>m_ArcsList;//Բ������
	vector<DL_PolylineData>m_PolylinesList;//���������
	vector<DL_VertexData>m_VertexsList;//������ϵĵ�λ����
	vector<DL_EllipseData>m_EllipsesList; //��ԲԲ������

private:
	//����ζ��������ȡ�ɹ���LWPOLYLINE���Զ�ȡ���������POLYLINE���������������
	int curpindex;//��ǰ���������,-1Ϊ��ȡ�ɹ�������Ϊû�ж�ȡ�ɹ�
	bool is_datainblock;//��ǵ�ǰ�����Ƿ��ڿ��У����ڿ��в���ȡ
public:

	CmyDxfFilter(void);
	~CmyDxfFilter(void);
	virtual void addLine(const DL_LineData& data); //��ȡֱ����Ϣ
	virtual void addCircle(const DL_CircleData& data); //��ȡ��Բ��Ϣ
	virtual void addArc(const DL_ArcData& data);//��ȡԲ����Ϣ
	virtual void addPolyline(const DL_PolylineData& data);//��ȡ���߶���Ϣ
	virtual void addVertex(const DL_VertexData& data);//��ȡ�˵���Ϣ
	virtual void addEllipse(const DL_EllipseData& data);//��ȡ��Բ������Ϣ
	virtual void addBlock(const DL_BlockData& data);
	virtual void endBlock();

};
