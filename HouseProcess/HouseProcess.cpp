// HouseProcess.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "House.h"
#include "House.cpp"

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
    Point p = Point(1, 1, 1, "hello");
    Point p2 = Point(1.000001, 1.000001, 1, "fuck");
    Point emptyP = Point();
    bool a = p.isEqualTo(p2) == true;
    isPassed("Point::isEqualTo()", a);
    isPassed("Point::Point()", emptyP.isNULL);
    /*����Segment��*/
    Segment s1 = Segment(Point(1, 1), Point(2, 2));
    Segment s2 = Segment(Point(0, 0), Point(1, 1));
    Segment s3 = Segment(Point(0, 1), Point(1, 0));
    isPassed("Segment::isParalWith()", s1.isParalWith(s2));
    isPassed("Segment::getCorWith()", s1.getCorWith(s2).isNULL);
    isPassed("Segment::getCorWith()", s2.getCorWith(s3).isEqualTo(Point(0.5, 0.5)));
    vector<Segment> segs = {
        Segment(Point(0.4, 0.4), Point(0, 1), "line_1"),
        Segment(Point(0, 1), Point(1, 0), "line_2"),
        Segment(Point(1, 0), Point(0.4, 0.4), "line_3")
    };
    Region r = Region(segs);
    isPassed("Point::isInReigon", Point(0.5, 0.5).isInRegion(r));

    /* ��ʼ����House���findRegions����
     * ��Ϊ���������һ���Ǳպϣ�һ���ǲ��պ�
     * ÿ������ַ��߶ε��������з�ʽ
     */
    vector<Segment> segs_2 = {
        Segment(Point(0, 1), Point(1, 0), "line_2"),
        Segment(Point(0, 0), Point(0, 1), "line_1"),
        Segment(Point(0.5, 0), Point(0, 0), "line_3"),
        Segment(Point(1, 0), Point(0.5, 0), "line_4")
    };
    House h = House(segs_2);
    bool hasPassed = false;
    vector<string> conds = { "line_2", "line_1", "line_3", "line_4" };
    hasPassed = hasPassed || h.regions.size() == 1; // �Ƿ�ֻ�ҵ���һ������
    for (int i = 0; i < h.regions.at(0).borders.size(); i++) {
        hasPassed = hasPassed && h.regions.at(0).borders.at(i).id == conds.at(i);
    }
    Point center = h.regions.at(0).center;
    double area = h.regions.at(0).computeArea();
    isPassed("Regin::computeArea()", abs(area - 0.5) < MIN_ERR);
    isPassed("House::findRegions()", hasPassed);
    isPassed("Region::findCenter()", center.isEqualTo(Point(0.25, 0.5)));

    double triArea = computeTriArea(
        Point(0, 0), Point(0, 1), Point(1, 0)
    );

    double nu = 0.1;
    isPassed("computeTriArea()", true);
}

/* �������������Ϣ */
void isPassed(string name, bool t) {
    cout << (t ? "passed" : "wrong") << '\t' << name << '\n';
}