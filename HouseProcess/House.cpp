#pragma once
#include "House.h"

#define MIN_ERR 0.00001 // ������С��������ȼ���

namespace HouseProcess {

    /* ������������� */
    double computeTriArea(YFPoint a, YFPoint b, YFPoint c) {
        const double l1 = YFSegment(a, b).distance;
        const double l2 = YFSegment(a, c).distance;
        const double l3 = YFSegment(c, b).distance;
        const double p = (l1 + l2 + l3) / 2; // ���ܳ�
        return sqrt(p * (p - l1) * (p - l2) * (p - l3));
    };

    YFHouse::YFHouse() {
        this->isNULL = true;
    }
    YFHouse::YFHouse(vector<YFSegment> lines) {
        this->isNULL = false;
        this->lines = lines;
        this->regions = this->findRegions();
        this->outLines = this->findOutLines();
    }


    /* Ѱ�����еıպ����� */
    vector<YFRegion> YFHouse::findRegions() {
        vector<YFRegion> regions; // ���ڴ�����е�����
        vector<YFSegment> tmpLines; // ���ڴ�����е���

                                    // ���߶ε���β�ߵ�
        auto reverseSeg = [](YFSegment s) {
            YFPoint tmp = s.startPoint;
            s.startPoint = s.endPoint;
            s.endPoint = tmp;
            s.startPoint.bulge = -s.startPoint.bulge;
            s.endPoint.bulge = -s.endPoint.bulge;
            return s;
        };

        /* ɾ��������ĳ��ǽ�� */
        auto delWall = [](vector<YFSegment> lines, YFSegment seg) {
            for (auto i = lines.begin(); i != lines.end(); i++) {
                if ((i->startPoint.isEqualTo(seg.startPoint) && i->endPoint.isEqualTo(seg.endPoint))
                    || (i->endPoint.isEqualTo(seg.startPoint) && i->startPoint.isEqualTo(seg.endPoint))) {
                    lines.erase(i);
                    break;
                }
                //i->startPoint.isEqualTo(seg.startPoint)
                //    && i->endPoint.isEqualTo(seg.endPoint)
            }
            return lines; // ����ɾ����ļ���
        };

        /* �ڼ�����Ѱ����ĳ�߶����ӵ��߶� */
        auto findNextWall = [](vector<YFSegment> lines, YFSegment seg) {
            for (YFSegment s : lines) {
                if (s.startPoint.isEqualTo(seg.endPoint)
                    || s.endPoint.isEqualTo(seg.endPoint)
                    || s.startPoint.isEqualTo(seg.startPoint)
                    || s.endPoint.isEqualTo(seg.startPoint)) {
                    return s;
                }
            }
            return YFSegment();
        };
        //��ʼ��tmpLines�������й���ǽ���޳�
        for (YFSegment seg : this->lines) {
            int sFlag = 0;
            int eFlag = 0;
            for (YFSegment s : this->lines) {
                if (s.startPoint.isEqualTo(seg.startPoint)
                    || s.endPoint.isEqualTo(seg.startPoint)) {
                    sFlag++;
                } else if (s.startPoint.isEqualTo(seg.endPoint)
                    || s.endPoint.isEqualTo(seg.endPoint)) {
                    eFlag++;
                }
            }
            // ����ǽ�ڱ���һ�����������㶼���ص�
            if (sFlag > 0 && eFlag > 0) tmpLines.push_back(seg);
        }

        while (tmpLines.size() > 0) {
            YFSegment curWall = tmpLines.at(0);
            vector<YFSegment> borders; // �������ǽ��
            borders.push_back(curWall);
            tmpLines = delWall(tmpLines, curWall);
            while (tmpLines.size() > 0) { // ���ϵش�����ǽ�����ҳ���β������ǽ
                YFSegment nextWall = findNextWall(tmpLines, curWall);
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
                    break;
                }
                borders.push_back(nextWall); // ����һ��ǽ������ջ
                int lastSize = tmpLines.size();
                tmpLines = delWall(tmpLines, nextWall); // ��������ջ��ǽ��ɾ��
                if (tmpLines.size() == lastSize) break; // ɾ��ʧ�ܣ�����
                curWall = nextWall;
                if (nextWall.endPoint.isEqualTo(borders.at(0).startPoint)) {
                    // �γɱջ�������
                    //break;
                    // Tips: ���ﲻ�������������и��õ�Ч�����ر��ǵ�����������ʱ��
                }
            }
            if (borders.size() == 0) {
                break; // û���ռ�������ֹͣѭ��
            } else {
                regions.push_back(YFRegion(borders)); // ���ռ�����������ջ
                borders.clear();
            }
        }
        return regions;
    }

    /* ��ȡ���������� */
    vector<YFSegment> YFHouse::findOutLines() {
        vector<YFSegment> outLines;
        return outLines;
    }

    YFRegion::YFRegion() {
        this->isNULL = true;
    }

    YFRegion::YFRegion(vector<YFSegment> s) {
        this->borders = s;
        this->center = this->findCenter(); // ��Ҫ���ģ����ܽ�˽�����Ա�¶����
        this->area = this->computeArea();
        this->perimeter = this->computePerimeter();
        this->isNULL = false;
    }

    /* �����Ӿ�����λ */
    /* ���Ҳ��ԣ�Ѱ����з��ߣ��з��߲�����߽��н��㣬�����е���������
    * ȡ�з����е���Ϊ�Ӿ�����λ
    */
    YFPoint YFRegion::findCenter() {
        vector<YFSegment> inLines;
        int borderNum = this->borders.size();
        for (int i = 0; i < int(borderNum / 2); i++) { // ��ʼ���������з���
            for (int j = i + 1; j < borderNum; j++) {
                YFSegment s = YFSegment(
                    this->borders.at(i).startPoint,
                    this->borders.at(j).startPoint
                    );
                // �ж������з����Ƿ��ڱ�����
                // TIPs: ����ʹ�㷨���Ӷ���������o(n! * n)
                bool isInBorder = false;
                for (YFSegment seg : this->borders) {
                    if (seg.isParalWith(s)) {
                        isInBorder = true;
                        break;
                    };
                }
                if (isInBorder) continue;

                // �ж��������Ƿ�������ཻ
                vector<YFPoint> corPoints = s.getCorWithRegion(*this);
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
            for (auto s : this->borders) {
                double x = s.startPoint.x;
                double y = s.startPoint.y;
                min_cx = x < min_cx ? x : min_cx;
                max_cx = x > max_cx ? x : max_cx;
                min_cy = y < min_cy ? y : min_cy;
                max_cy = y > max_cy ? y : max_cy;
            }
            return YFPoint((min_cx + max_cx) / 2, (min_cy + max_cy) / 2); // û�з�����������
        }

        // ��ʼѰ������зֵ�
        YFPoint bestPoint;
        double maxRatio = 0;
        for (YFSegment seg : inLines) {
            // �����߶κ����ε����
            double l = abs(seg.xRange.max - seg.xRange.min); // ��
            double w = abs(seg.yRange.max - seg.yRange.min); // ��
            double ratio = l * w;
            YFPoint center = seg.center; // ѡȡ�зֵ���е���Ϊ����Ӿ����ĵ�
            bool isInRegion = center.isInRegion(*(this));
            if (ratio > maxRatio && isInRegion) {
                maxRatio = ratio;
                bestPoint = center;
            }
        }
        return bestPoint;
    }

    /* ����������� */
    double YFRegion::computeArea() {
        vector<YFPoint> points; // ��������нǵ�
        double area = 0;

        /* �ӵ㼯��ɾ���� */
        auto delPointFromPoints = [](YFPoint p, vector<YFPoint> points) {
            for (auto i = points.begin(); i != points.end(); i++) {
                if (p.isEqualTo(*i)) {
                    points.erase(i);
                    break;
                }
            }
            return points;
        };

        double arcArea = 0; // �ȼ�����л��ߵ����

        for (YFSegment s : this->borders) {
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
                YFPoint sp = points.at(i);
                YFPoint cp = points.at((i + 1) % lastsize);
                YFPoint ep = points.at((i + 2) % lastsize);
                YFSegment triLine = YFSegment(sp, ep); // б��
                vector<YFPoint> corPoints = triLine.getCorWithRegion(*this);
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
    double YFRegion::computePerimeter() {
        double perimeter = 0;
        for (auto l : this->borders) {
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

    YFPoint::YFPoint() {
        this->isNULL = true;
    }

    YFPoint::YFPoint(double x_val, double y_val, double bulge_val, string id_val) {
        this->x = x_val;
        this->y = y_val;
        this->z = 0;
        this->bulge = bulge_val;
        this->id = id_val;
        this->isNULL = false;
    }

    YFPoint::YFPoint(double x_val, double y_val) {
        this->x = x_val;
        this->y = y_val;
        this->z = 0;
        this->bulge = 0;
        this->id = "No ID";
        this->isNULL = false;
    }

    /* �жϸõ�������һ�����Ƿ������� */
    bool YFPoint::isEqualTo(YFPoint p) {
        return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2)) < MIN_ERR;
    }

    /* �ж�ĳ���Ƿ���ĳ������ */
    bool YFPoint::isInRegion(YFRegion r) {
        YFPoint zeroPoint(-1, -1);
        YFSegment line = YFSegment(zeroPoint, *this); // ��һ�����������������
        vector<YFPoint> corPoints = line.getCorWithRegion(r); // ȡ������������Ľ���
        return corPoints.size() % 2 == 1; // ����������Ϊ�����������ж��õ���������
    }


    YFSegment::YFSegment(YFPoint sp, YFPoint ep, string id_val) {
        this->startPoint = sp;
        this->endPoint = ep;
        this->id = id_val;
        this->a = sp.y - ep.y;
        this->b = sp.x - ep.x;
        this->c = sp.x * ep.y - sp.y * ep.x;
        this->isNULL = false;
        this->distance = sqrt(pow(sp.x - ep.x, 2) + pow(sp.y - ep.y, 2)); // ���㳤��
        this->xRange.min = sp.x < ep.x ? sp.x : ep.x;
        this->xRange.max = sp.x > ep.x ? sp.x : ep.x;
        this->yRange.min = sp.y < ep.y ? sp.y : ep.y;
        this->yRange.max = sp.y > ep.y ? sp.y : ep.y;
        this->center = YFPoint(
            (this->startPoint.x + this->endPoint.x) / 2,
            (this->startPoint.y + this->endPoint.y) / 2
            );
    }

    YFSegment::YFSegment(YFPoint sp, YFPoint ep) {
        this->startPoint = sp;
        this->endPoint = ep;
        this->id = "NO ID";
        this->a = sp.y - ep.y;
        this->b = sp.x - ep.x;
        this->c = sp.x * ep.y - sp.y * ep.x;
        this->isNULL = false;
        this->distance = sqrt(pow(sp.x - ep.x, 2) + pow(sp.y - ep.y, 2)); // ���㳤��
        this->xRange.min = sp.x < ep.x ? sp.x : ep.x;
        this->xRange.max = sp.x > ep.x ? sp.x : ep.x;
        this->yRange.min = sp.y < ep.y ? sp.y : ep.y;
        this->yRange.max = sp.y > ep.y ? sp.y : ep.y;
        this->center = YFPoint(
            (this->startPoint.x + this->endPoint.x) / 2,
            (this->startPoint.y + this->endPoint.y) / 2
            );
    }

    YFSegment::YFSegment() {
        this->isNULL = true;
    }


    /* �ж��Ƿ�����һ���߶�ƽ�� */
    bool YFSegment::isParalWith(YFSegment s) {
        return abs(a * s.b - b * s.a) < MIN_ERR;
    }

    /* ��������һ���߶εĽ��� */
    YFPoint YFSegment::getCorWith(YFSegment s) {
        auto isInRange = [](double n, Range range) { // �����ж�ĳ��ֵ�Ƿ��ڷ�Χ��
            return n >= range.min - MIN_ERR && n <= range.max + MIN_ERR;
        };
        if (this->isParalWith(s)) {
            return YFPoint(); // �����ƽ�еģ��Ͳ����ڽ���
        }
        double der = a * s.b - b * s.a;
        double x = (b * s.c - c * s.b) / der;
        double y = (a * s.c - c * s.a) / der; // �������������ֵ
        if (isInRange(x, xRange)
            && isInRange(y, yRange)
            && isInRange(x, s.xRange)
            && isInRange(y, s.yRange)) {
            YFPoint p = YFPoint(x, y);
            return p;
        } else {
            return YFPoint(); // ������㲻���߶η�Χ�ڣ�Ҳ������
        }
    }

    /* �����߶�������Ľ��� */
    vector<YFPoint> YFSegment::getCorWithRegion(YFRegion r) {
        vector<YFSegment> borders = r.borders;
        vector<YFPoint> corPoints; // ���㼯��
        auto hasInSet = [](YFPoint p, vector<YFPoint> pset) {
            bool flag = false;
            for (YFPoint pi : pset) {
                flag = flag || p.isEqualTo(pi);
                if (flag) break;
            }
            return flag;
        };
        for (YFSegment s : borders) {
            YFPoint corPoint = this->getCorWith(s);
            YFPoint(1, 1);
            if (!corPoint.isNULL // �ǿ�
                && !corPoint.isEqualTo(this->startPoint) // �����߶εĶ˵�
                && !corPoint.isEqualTo(this->endPoint)
                && !hasInSet(corPoint, corPoints)) { // �����ظ����
                corPoints.push_back(corPoint); // �����㱣���뼯��
            }
        }
        return corPoints;
    }
}

