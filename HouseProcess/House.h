#include <string>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

#ifndef HOUSE_H
#define HOUSE_H

namespace HouseProcess {
    class YFSegment;
    class YFPoint;
    class YFRegion;
    class YFHouse;

    double computeTriArea(YFPoint a, YFPoint b, YFPoint c);

    /* ���� */
    class YFPoint {
    public:
        double x;
        double y;
        double z;
        string id;
        double bulge;
        bool isNULL;
        YFPoint();
        YFPoint(double x, double y, double bulge, string id);
        YFPoint(double x_val, double y_val);
        bool isEqualTo(YFPoint p);
        bool isInRegion(YFRegion r);
    };

    /* ����һ���߶� */
    class YFSegment {
    public:
        // a, b, c Ϊֱ�ߵ�һ����ʽ����������
        struct Range {
            double min;
            double max;
        };
        double a, b, c;
        string id;
        Range xRange;
        Range yRange;
        YFPoint startPoint;
        YFPoint endPoint;
        YFPoint center;
        bool isNULL;
        double distance; // ֱ�߾��룬�������߾���
        YFSegment();
        YFSegment(YFPoint sp, YFPoint ep, string id);
        YFSegment(YFPoint sp, YFPoint ep);
        YFPoint getCorWith(YFSegment s);
        vector<YFPoint> getCorWithRegion(YFRegion r);
        bool isParalWith(YFSegment s);
    };

    class YFRegion {
    public:
        YFRegion();
        YFRegion(vector<YFSegment> s);
        vector<YFSegment> borders; // �߽缯��
        bool isNULL;
        double area;
        double perimeter;
        YFPoint center;
        YFPoint findCenter();
        double computeArea();
        double computePerimeter();
    };

    class YFHouse {
    public:
        vector<YFRegion> regions;
        YFHouse();
        YFHouse(vector<YFSegment> lines);
        vector<YFRegion> findRegions();
        vector<YFSegment> findOutLines();
        vector<YFSegment> lines;
        vector<YFSegment> outLines; // ����������
        bool isNULL;
    };
}
#endif