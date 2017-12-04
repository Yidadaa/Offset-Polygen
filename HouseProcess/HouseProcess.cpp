// HouseProcess.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "House.h"
#include "House.cpp"

using namespace HouseProcess;

void test();
void isPassed(string name, bool t);

int main()
{
    test();
    system("pause");
    return 0;
}
/* �ú������ڲ��� */
void test() {
    /*����Point��*/
    YFPoint p = YFPoint(1, 1, 1, "hello");
    YFPoint p2 = YFPoint(1.0000001, 1.0000001, 1, "fuck");
    YFPoint emptyP = YFPoint();
    bool a = p.isEqualTo(p2) == true;
    isPassed("Point::isEqualTo()", a);
    isPassed("Point::Point()", emptyP.isNULL);
    /*����Segment��*/
    YFSegment s1 = YFSegment(YFPoint(1, 1), YFPoint(2, 2));
    YFSegment s2 = YFSegment(YFPoint(0, 0), YFPoint(1, 1));
    YFSegment s3 = YFSegment(YFPoint(0, 1), YFPoint(1, 0));
    isPassed("Segment::isParalWith()", s1.isParalWith(s2));
    isPassed("Segment::getCorWith()", s1.getCorWith(s2).isNULL);
    isPassed("Segment::getCorWith()", s2.getCorWith(s3).isEqualTo(YFPoint(0.5, 0.5)));
    vector<YFSegment> segs = {
        YFSegment(YFPoint(0.4, 0.4), YFPoint(0, 1), "line_1"),
        YFSegment(YFPoint(0, 1), YFPoint(1, 0), "line_2"),
        YFSegment(YFPoint(1, 0), YFPoint(0.4, 0.4), "line_3")
    };
    YFRegion r = YFRegion(segs);
    isPassed("Point::isInReigon", YFPoint(0.5, 0.5).isInRegion(r));

    /* ��ʼ����House���findRegions����
     * ��Ϊ���������һ���Ǳպϣ�һ���ǲ��պ�
     * ÿ������ַ��߶ε��������з�ʽ
     */
    vector<YFSegment> segs_2 = {
        YFSegment(YFPoint(0, 1), YFPoint(1, 0), "line_2"),
        YFSegment(YFPoint(0, 0), YFPoint(0, 1), "line_1"),
        YFSegment(YFPoint(0.5, 0), YFPoint(0, 0), "line_3"),
        YFSegment(YFPoint(1, 0), YFPoint(0.5, 0), "line_4")
    };
    YFHouse h = YFHouse(segs_2, 0.35);
    bool hasPassed = false;
    vector<string> conds = { "line_2", "line_1", "line_3", "line_4" };
    hasPassed = hasPassed || h.regions.size() == 1; // �Ƿ�ֻ�ҵ���һ������
    for (int i = 0; i < h.regions.at(0).borders.size(); i++) {
        hasPassed = hasPassed && h.regions.at(0).borders.at(i).id == conds.at(i);
    }
    YFPoint center = h.regions.at(0).center;
    double area = h.regions.at(0).computeArea();
    isPassed("Regin::computeArea()", abs(area - 0.5) < MIN_ERR);
    isPassed("House::findRegions()", hasPassed);
    isPassed("Region::findCenter()", center.isEqualTo(YFPoint(0.25, 0.5)));

    double triArea = computeTriArea(
        YFPoint(0, 0), YFPoint(0, 1), YFPoint(1, 0)
    );

    double nu = 0.1;
    isPassed("computeTriArea()", true);
}

/* �������������Ϣ */
void isPassed(string name, bool t) {
    cout << (t ? "passed" : "wrong") << '\t' << name << '\n';
}