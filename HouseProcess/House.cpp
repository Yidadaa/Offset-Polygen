#pragma once
#include "House.h"
//#include "stdafx.h"

#define MIN_ERR 0.00001 // ������С��������ȼ���
/* ������������� */
double computeTriArea(Point a, Point b, Point c) {
    const double l1 = Segment(a, b).distance;
    const double l2 = Segment(a, c).distance;
    const double l3 = Segment(c, b).distance;
    const double p = (l1 + l2 + l3) / 2; // ���ܳ�
    return sqrt(p * (p - l1) * (p - l2) * (p - l3));
};

House::House() {
    isNULL = true;
}
House::House(vector<Segment> lines) {
    isNULL = false;
    this->lines = lines;
    regions = this->findRegions();
}

/* Ѱ�����еıպ����� */
vector<Region> House::findRegions() {
    vector<Region> regions; // ���ڴ�����е�����
    vector<Segment> tmpLines = this->lines; // ���ڴ�����е���

                                            // ���߶ε���β�ߵ�
    auto reverseSeg = [](Segment s) {
        Point tmp = s.startPoint;
        s.startPoint = s.endPoint;
        s.endPoint = tmp;
        s.startPoint.bulge = -s.startPoint.bulge;
        s.endPoint.bulge = -s.endPoint.bulge;
        return s;
    };

    /* ɾ��������ĳ��ǽ�� */
    auto delWall = [](vector<Segment> lines, Segment seg) {
        for (auto i = lines.begin(); i != lines.end(); i++) {
            if ((i->startPoint.isEqualTo(seg.startPoint)
                && i->endPoint.isEqualTo(seg.endPoint)) || i->id == seg.id) {
                lines.erase(i); // ɾ����seg��ͬid��Ԫ��
                break;
            }
            //i->startPoint.isEqualTo(seg.startPoint)
            //    && i->endPoint.isEqualTo(seg.endPoint)
        }
        return lines; // ����ɾ����ļ���
    };

    /* �ڼ�����Ѱ����ĳ�߶����ӵ��߶� */
    auto findNextWall = [](vector<Segment> lines, Segment seg) {
        for each (Segment s in lines) {
            if (s.startPoint.isEqualTo(seg.endPoint)
                || s.endPoint.isEqualTo(seg.endPoint)
                || s.startPoint.isEqualTo(seg.startPoint)
                || s.endPoint.isEqualTo(seg.startPoint)) {
                return s;
            }
        }
        return Segment();
    };

    while (tmpLines.size() > 0) {
        Segment curWall = tmpLines.at(0);
        vector<Segment> borders; // �������ǽ��
        borders.push_back(curWall);
        tmpLines = delWall(tmpLines, curWall);
        while (tmpLines.size() > 0) { // ���ϵش�����ǽ�����ҳ���β������ǽ
            Segment nextWall = findNextWall(tmpLines, curWall);
            if (!nextWall.isNULL) {
                // �ҵ����߶ο��������ڵ��߶������ֲ�ͬ���������
                // Ҫ�����ǵ�������β������״̬
                if (curWall.endPoint.isEqualTo(nextWall.endPoint)) {
                    // curWall:  ---->
                    // nextWall:     <----
                    nextWall = reverseSeg(nextWall);
                } else if (curWall.startPoint.isEqualTo(nextWall.startPoint)) {
                    // curWall:  ---->
                    // nextWall: ---->
                    curWall = reverseSeg(curWall);
                    borders.pop_back();
                    borders.push_back(curWall);
                    // �ѵ�ǰǽ�ڷ�ת��������ջ��һ����˵��ֻ���ڵ������ڶ���ʱִ�е�����
                } else if (curWall.startPoint.isEqualTo(nextWall.endPoint)) {
                    // curWall:  ---->
                    // nextWall: <----
                    curWall = reverseSeg(curWall);
                    borders.pop_back();
                    borders.push_back(curWall);
                    nextWall = reverseSeg(nextWall);
                }
            } else {
                // �Ҳ�����һ������ǽ���ˣ�Ҳ����
                // TIPs: ���Ż���������ǽ��ɾ�������з��գ���������Ǳջ��Ļ���
                //       �����޷����ҵ���
                break;
            }
            borders.push_back(nextWall); // ����һ��ǽ������ջ
            int lastSize = tmpLines.size();
            tmpLines = delWall(tmpLines, nextWall); // ��������ջ��ǽ��ɾ��
            if (tmpLines.size() == lastSize) break; // ɾ��ʧ�ܣ�����
            curWall = nextWall;
            if (nextWall.endPoint.isEqualTo(borders.at(0).startPoint)) {
                // �γɱջ�������
                // break;
                // Tips: ���ﲻ�������������и��õ�Ч�����ر��ǵ�����������ʱ��
            }
        }
        if (borders.size() == 0) {
            break; // û���ռ�������ֹͣѭ��
        } else {
            regions.push_back(Region(borders)); // ���ռ�����������ջ
            borders.clear();
        }
    }
    return regions;
}

Region::Region() {
    isNULL = true;
}

Region::Region(vector<Segment> s) {
    borders = s;
    center = this->findCenter(); // ��Ҫ���ģ����ܽ�˽�����Ա�¶����
    area = this->computeArea();
    perimeter = this->computePerimeter();
    isNULL = false;
}

/* �����Ӿ�����λ */
/* ���Ҳ��ԣ�Ѱ����з��ߣ��з��߲�����߽��н��㣬�����е���������
* ȡ�з����е���Ϊ�Ӿ�����λ
*/
Point Region::findCenter() {
    vector<Segment> inLines;
    int borderNum = this->borders.size();
    for (int i = 0; i < int(borderNum / 2); i++) { // ��ʼ���������з���
        for (int j = i + 1; j < borderNum; j++) {
            Segment s = Segment(
                this->borders.at(i).startPoint,
                this->borders.at(j).startPoint
                );
            // �ж������з����Ƿ��ڱ�����
            // TIPs: ����ʹ�㷨���Ӷ���������o(n! * n)
            bool isInBorder = false;
            for each (Segment seg in this->borders) {
                if (seg.isParalWith(s)) {
                    isInBorder = true;
                    break;
                };
            }
            if (isInBorder) continue;

            // �ж��������Ƿ�������ཻ
            vector<Point> corPoints = s.getCorWithRegion(*this);
            if (corPoints.size() > 0) continue;

            inLines.push_back(s); // �������
        }
    }
    if (inLines.size() == 0) {
        // TODO: ʵ���Ҳ��������������߶εļ�Ȩ���ĵ�
        double min_cx = 10000;
        double max_cx = 0;
        double min_cy = 10000;
        double max_cy = 0;
        for each (auto s in this->borders) {
            double x = s.startPoint.x;
            double y = s.startPoint.y;
            min_cx = x < min_cx ? x : min_cx;
            max_cx = x > max_cx ? x : max_cx;
            min_cy = y < min_cy ? y : min_cy;
            max_cy = y > max_cy ? y : max_cy;
        }
        return Point((min_cx + max_cx) / 2, (min_cy + max_cy) / 2); // û�з�����������
    }

    // ��ʼѰ������зֵ�
    Point bestPoint;
    double maxRatio = 0;
    for each (Segment seg in inLines) {
        // �����߶κ����ε����
        double l = abs(seg.xRange.max - seg.xRange.min); // ��
        double w = abs(seg.yRange.max - seg.yRange.min); // ��
        double ratio = l * w;
        Point center = seg.center; // ѡȡ�зֵ���е���Ϊ����Ӿ����ĵ�
        bool isInRegion = center.isInRegion(*(this));
        if (ratio > maxRatio && isInRegion) {
            maxRatio = ratio;
            bestPoint = center;
        }
    }
    return bestPoint;
}

/* ����������� */
double Region::computeArea() {
    vector<Point> points; // ��������нǵ�
    double area = 0;

    /* �ӵ㼯��ɾ���� */
    auto delPointFromPoints = [](Point p, vector<Point> points) {
        for (auto i = points.begin(); i != points.end(); i++) {
            if (p.isEqualTo(*i)) {
                points.erase(i);
                break;
            }
        }
        return points;
    };

    double arcArea = 0; // �ȼ�����л��ߵ����

    for each (Segment s in this->borders) {
        points.push_back(s.startPoint);
        double b = abs(s.startPoint.bulge);
        double p = s.startPoint.bulge > 0 ? 1 : -1; // ����͹�������ǰ���ȥ
        if (b > MIN_ERR) {
            double alpha = 2 * atan(b); // ����֮һ�Ƕ�
            double a = s.distance / 2;
            double R = a / sin(alpha);
            double b = a / tan(alpha);
            double s = 0.5 * a * b; // �������������
            double arc = 0.5 * alpha * pow(R, 2); // �����������
            arcArea += p * 2 * (arc - s); // ���㻡���б����
        }
    } // ��ȡ���нǵ�

    while (points.size() > 0) { // ���ϵشӶ������ѡȡ�㣬�зֳ������ν�������
        int lastsize = points.size();
        for (int i = 0; i < lastsize; i++) {
            Point sp = points.at(i);
            Point cp = points.at((i + 1) % lastsize);
            Point ep = points.at((i + 2) % lastsize);
            Segment triLine = Segment(sp, ep); // б��
            vector<Point> corPoints = triLine.getCorWithRegion(*this);
            bool centerIsInRegion = triLine.center.isInRegion(*this);
            if (corPoints.size() == 0 && centerIsInRegion) {
                area += computeTriArea(sp, cp, ep); // ���������ε����
                points = delPointFromPoints(cp, points); // ���е�ɾȥ
                break;
            }
        }
        if (points.size() == lastsize) break; // ���û�п���ѡȡ�ĵ��ˣ���ô����
    }
    return area + arcArea;
}

/* �����ܳ� */
double Region::computePerimeter() {
    double perimeter = 0;
    for each (auto l in this->borders) {
        double b = abs(l.startPoint.bulge);
        if (b > MIN_ERR) {
            // ���㻡���ܳ�
            double alpha = 2 * atan(b); // �õ�����֮һ�Ƕ�
            double a = l.distance / 2;
            double R = a / sin(alpha);
            double arc = R * alpha; // ����뻡��
            perimeter += 2 * arc;
        } else {
            perimeter += l.distance;
        }
    }
    return perimeter;
};

Point::Point() {
    isNULL = true;
}

Point::Point(double x_val, double y_val, double bulge_val, string id_val) {
    x = x_val;
    y = y_val;
    z = 0;
    bulge = bulge_val;
    id = id_val;
    isNULL = false;
}

Point::Point(double x_val, double y_val) {
    x = x_val;
    y = y_val;
    z = 0;
    bulge = 0;
    id = "No ID";
    isNULL = false;
}

/* �жϸõ�������һ�����Ƿ������� */
bool Point::isEqualTo(Point p) {
    return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2)) < MIN_ERR;
}

/* �ж�ĳ���Ƿ���ĳ������ */
bool Point::isInRegion(Region r) {
    Point zeroPoint(-1, -1);
    Segment line = Segment(zeroPoint, *this); // ��һ�����������������
    vector<Point> corPoints = line.getCorWithRegion(r); // ȡ������������Ľ���
    return corPoints.size() % 2 == 1; // ����������Ϊ�����������ж��õ���������
}


Segment::Segment(Point sp, Point ep, string id_val) {
    startPoint = sp;
    endPoint = ep;
    id = id_val;
    a = sp.y - ep.y;
    b = sp.x - ep.x;
    c = sp.x * ep.y - sp.y * ep.x;
    isNULL = false;
    distance = sqrt(pow(sp.x - ep.x, 2) + pow(sp.y - ep.y, 2)); // ���㳤��
    xRange.min = min(sp.x, ep.x);
    xRange.max = max(sp.x, ep.x);
    yRange.min = min(sp.y, ep.y);
    yRange.max = max(sp.y, ep.y);
    center = Point(
        (this->startPoint.x + this->endPoint.x) / 2,
        (this->startPoint.y + this->endPoint.y) / 2
        );
}

Segment::Segment(Point sp, Point ep) {
    startPoint = sp;
    endPoint = ep;
    id = "No ID";
    a = sp.y - ep.y;
    b = sp.x - ep.x;
    c = sp.x * ep.y - sp.y * ep.x;
    isNULL = false;
    distance = sqrt(pow(sp.x - ep.x, 2) + pow(sp.y - ep.y, 2)); // ���㳤��
    xRange.min = min(sp.x, ep.x);
    xRange.max = max(sp.x, ep.x);
    yRange.min = min(sp.y, ep.y);
    yRange.max = max(sp.y, ep.y);
    center = Point(
        (this->startPoint.x + this->endPoint.x) / 2,
        (this->startPoint.y + this->endPoint.y) / 2
        );
}

Segment::Segment() {
isNULL: true;
}


/* �ж��Ƿ�����һ���߶�ƽ�� */
bool Segment::isParalWith(Segment s) {
    return abs(a * s.b - b * s.a) < MIN_ERR;
}

/* ��������һ���߶εĽ��� */
Point Segment::getCorWith(Segment s) {
    auto isInRange = [](double n, Range range) { // �����ж�ĳ��ֵ�Ƿ��ڷ�Χ��
        return n >= range.min - MIN_ERR && n <= range.max + MIN_ERR;
    };
    if (this->isParalWith(s)) {
        return Point(); // �����ƽ�еģ��Ͳ����ڽ���
    }
    double der = a * s.b - b * s.a;
    double x = (b * s.c - c * s.b) / der;
    double y = (a * s.c - c * s.a) / der; // �������������ֵ
    if (isInRange(x, xRange)
        && isInRange(y, yRange)
        && isInRange(x, s.xRange)
        && isInRange(y, s.yRange)) {
        Point p = Point(x, y);
        return p;
    } else {
        return Point(); // ������㲻���߶η�Χ�ڣ�Ҳ������
    }
}

/* �����߶�������Ľ��� */
vector<Point> Segment::getCorWithRegion(Region r) {
    vector<Segment> borders = r.borders;
    vector<Point> corPoints; // ���㼯��
    auto hasInSet = [](Point p, vector<Point> pset) {
        bool flag = false;
        for each (Point pi in pset) {
            flag = flag || p.isEqualTo(pi);
            if (flag) break;
        }
        return flag;
    };
    for each (Segment s in borders) {
        Point corPoint = this->getCorWith(s);
        Point(1, 1);
        if (!corPoint.isNULL // �ǿ�
            && !corPoint.isEqualTo(this->startPoint) // �����߶εĶ˵�
            && !corPoint.isEqualTo(this->endPoint)
            && !hasInSet(corPoint, corPoints)) { // �����ظ����
            corPoints.push_back(corPoint); // �����㱣���뼯��
        }
    }
    return corPoints;
}

