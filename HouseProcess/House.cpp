#pragma once
#include "House.h"
//#include "stdafx.h"

#define MIN_ERR 0.000001 // ������С��������ȼ���
#define PI 3.14159265358 // Ԥ�����ֵ

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
    YFHouse::YFHouse(vector<YFSegment> lines, double thickness) {
        this->isNULL = false;
        this->lines = lines;
        this->outWallThickness = thickness;
        this->regions = this->findRegions(lines);
        this->outLines = this->findOutLines();
        this->innerLines = this->findInnerLiners();
    }


    /* Ѱ�����еıպ����� */
    vector<YFRegion> YFHouse::findRegions(vector<YFSegment> lines) {
        vector<YFRegion> regions; // ���ڴ�����е�����
        vector<YFSegment> tmpLines; // ���ڴ�����е���

                                    // ���߶ε���β�ߵ�
        auto reverseSeg = [](YFSegment s) {
            auto negNum = [](double n) { // ȡ�෴��
                if (abs(n) < MIN_ERR) return n;
                else {
                    return -n;
                }
            };
            YFPoint tmp = s.startPoint;
            s.startPoint = s.endPoint;
            s.endPoint = tmp;
            double tmpBulge = s.startPoint.bulge;
            s.startPoint.bulge = negNum(s.endPoint.bulge); // ��β��ת�����Ƚ�������ȡ��
            s.endPoint.bulge = negNum(tmpBulge);
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
        for (YFSegment seg : lines) {
            int sFlag = 0;
            int eFlag = 0;
            for (YFSegment s : lines) {
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
            bool isClosure = false; // �Ƿ��Ǳպ�����
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
                    // Tips: ���ﲻ�������������и��õ�Ч�����ر��ǵ�����������ʱ��
                    isClosure = true;
                    break;
                }
            }
            vector<YFSegment> tmpBorders;
            for (auto l : borders) if (l.distance > MIN_ERR) tmpBorders.push_back(l); // �������߶�
            borders = tmpBorders;
            YFRegion region(borders);
            if (borders.size() == 0) {
                break; // û���ռ�������ֹͣѭ��
            } else if (isClosure && region.area > MIN_ERR) { // ���ռ��պ�����
                regions.push_back(region); // ���ռ�����������ջ
                borders.clear();
            }
        }
        return regions;
    }

    /* ��ȡ���������� */
    vector<YFSegment> YFHouse::findOutLines() {
        vector<YFSegment> outLines;
        vector<YFSegment> lines = this->lines;
        vector<YFRegion> regions = this->regions;
        const double distance = this->outWallThickness;
        const double D = 0.3; // ��С����

        vector<YFRegion> outRegions;
        vector<YFPoint> allPoints; // ���������������ߵĵ㶼��������

                                   /* ����ĳ��ļ������ʾ������ֱ�����꣬���ؼ������е�theta�� */
        auto computeTheta = [](double x, double y) {
            double theta = 0;
            if (abs(x) < MIN_ERR) {
                // x = 0
                if (abs(y) < MIN_ERR) {
                    // x = 0, y = 0
                    return 0.0;
                } else if (y > 0) {
                    // x = 0, y > 0
                    return PI / 2;
                } else {
                    // x = 0. y < 0
                    return -PI / 2;
                }
            } else if (x > 0) {
                // x > 0
                return atan(y / x);
            } else {
                // x < 0
                if (y < 0) {
                    // y < 0
                    return atan(y / x) - PI;
                } else {
                    // y >= 0
                    return atan(y / x) + PI;
                }
            }
        };

        /* ����������ڻ���������չһ�����룬��d����0ʱ���������ⲿ���ţ��������ڲ����� */
        auto zoomRegion = [computeTheta](YFRegion r, double distance) {
            double d = abs(distance);
            bool isZoomOut = distance > 0; // �Ƿ�����
            vector<YFPoint> outPoints; // �����н�ƽ�����ϵĵ㱣������
            int borderCount = r.borders.size();
            for (int i = 0; i < borderCount; i++) {
                YFSegment curSeg = r.borders.at(i);
                YFSegment nextSeg = r.borders.at((i + 1) % borderCount);
                YFPoint sp, mp, ep; // ��ʼ�㣬�м�㣬������
                if (curSeg.endPoint.isEqualTo(nextSeg.startPoint)) {
                    sp = curSeg.startPoint;
                    mp = nextSeg.startPoint;
                    ep = nextSeg.endPoint;
                } else if (curSeg.endPoint.isEqualTo(nextSeg.endPoint)) {
                    sp = curSeg.startPoint;
                    mp = curSeg.endPoint;
                    ep = nextSeg.startPoint;
                } else if (curSeg.startPoint.isEqualTo(nextSeg.startPoint)) {
                    sp = curSeg.endPoint;
                    mp = curSeg.startPoint;
                    ep = nextSeg.endPoint;
                } else if (curSeg.startPoint.isEqualTo(nextSeg.endPoint)) {
                    sp = curSeg.endPoint;
                    mp = curSeg.startPoint;
                    ep = nextSeg.startPoint;
                }
                // ��ʼ�����ƽ�����ϵĵ㣬��ʱ��mpΪ����ԭ����м���
                double oa[2] = { sp.x - mp.x, sp.y - mp.y };
                double ob[2] = { ep.x - mp.x, ep.y - mp.y };
                double thetaOA = computeTheta(oa[0], oa[1]); // �����a�ļ�����Ƕ�
                double thetaOB = computeTheta(ob[0], ob[1]); // �����b�ļ�����Ƕ�
                auto lenOfVector = [](double vector[]) { return sqrt(pow(vector[0], 2) + pow(vector[1], 2)); }; // ��������ģ��
                double alpha = acos((oa[0] * ob[0] + oa[1] * ob[1]) / (lenOfVector(oa) * lenOfVector(ob))); // oa��ob�ļн�
                                                                                                            // ����a��b�ļ����꣬���Լ�������߽�ƽ�����ϵ�����һ������
                double rho = d / sin(alpha / 2); // �����p�ļ������r
                double testRho = 0.01 / sin(alpha / 2); // testRho�����жϵ��Ƿ��������ڣ������d����ʱ���޷��ж�
                double theta1 = (thetaOA + thetaOB) / 2; // ��ƽ�����ϵĵ�
                double theta2 = theta1 + PI; // ��ƽ�����ӳ����ϵĵ�
                YFPoint p1(rho * cos(theta1) + mp.x, rho * sin(theta1) + mp.y); // ��Ҫ����mp��ֵ���и�ԭ����
                YFPoint p2(rho * cos(theta2) + mp.x, rho * sin(theta2) + mp.y);
                YFPoint testp1(testRho * cos(theta1) + mp.x, testRho * sin(theta1) + mp.y); // ���ڲ��Ե�������,testp1��testp2
                YFPoint testp2(testRho * cos(theta2) + mp.x, testRho * sin(theta2) + mp.y); // �������������p1��p2�������λ�ù�ϵ
                                                                                            // �ж�p1��p2˭�������ڣ��������������ô��ѡ������ĵ㣻�������������ô��ѡ�����ڵĵ�
                YFPoint p = testp1.isInRegion(r) == isZoomOut ? p2 : p1; // ʹ�������������жϣ���ѡp1����p2
                p.bulge = mp.bulge; // ���ֻ���һ��
                outPoints.push_back(p); // ����p�ռ�����
            }
            int pointCount = outPoints.size();
            vector<YFSegment> regionOutLines;
            double fullLength = 0.0; // ���������߶��ܳ�
            for (int i = 0; i < pointCount; i++) {
                auto ep = outPoints.at((i + 1) % pointCount);
                auto s = YFSegment(outPoints.at(i), YFPoint(ep.x, ep.y)); // ȥ��endPoint��bulge��Ϣ
                fullLength += s.distance;
                regionOutLines.push_back(s); // �����е�����������Ϊ�����
            }
            // ����Ƿ�������ཻ����������ཻ��ɾ�����ཻ�Ĳ���
            for (int i = 0; i < regionOutLines.size(); i++) {
                auto curLine = regionOutLines.at(i);
                for (int j = 0; j < regionOutLines.size(); j++) {
                    if ((i == 0 && j == regionOutLines.size() - 1) ||
                        (j == 0 && i == regionOutLines.size() - 1) ||
                        (abs(i - j) <= 1)) continue; // �������ڱ�
                    auto theJline = regionOutLines.at(j);
                    auto corPoint = theJline.getCorWith(curLine); // ���㽻��
                    if (corPoint.isNULL) continue; // ����Ϊ�գ���ʱ����
                                                   // ������ڽ��㣬�ͽ�������֮����߶�ɾ��
                                                   // ͨ�������ܳ����ж�ɾ���Ĳ����߶�
                    int minIndex = i > j ? j : i;
                    int maxIndex = i > j ? i : j;
                    double min2maxLength = 0.0; // ����min��max֮����߶γ���
                    for (int k = 0; k < regionOutLines.size(); k++) {
                        if (k >= minIndex && k <= maxIndex) min2maxLength += regionOutLines.at(k).distance;
                    }
                    int beforeDel = regionOutLines.size();
                    if (min2maxLength < fullLength / 2.0) {
                        // ��Ҫɾ��(min, max)֮����߶�
                        corPoint.bulge = regionOutLines.at(maxIndex).startPoint.bulge; // ���ֻ�����Ϣһ��
                        regionOutLines.at(minIndex) = YFSegment(regionOutLines.at(minIndex).startPoint, corPoint);
                        regionOutLines.at(maxIndex) = YFSegment(corPoint, regionOutLines.at(maxIndex).endPoint);
                        regionOutLines.erase(regionOutLines.begin() + minIndex + 1, regionOutLines.begin() + maxIndex); // ɾ��(minIndex, maxIndex)
                    } else {
                        // ��Ҫɾ��[0, min), (max, N)֮����߶�
                        corPoint.bulge = regionOutLines.at(minIndex).startPoint.bulge; // ���ֻ�����Ϣһ��
                        regionOutLines.at(minIndex) = YFSegment(corPoint, regionOutLines.at(minIndex).endPoint);
                        regionOutLines.at(maxIndex) = YFSegment(regionOutLines.at(maxIndex).startPoint, corPoint);
                        regionOutLines.erase(regionOutLines.begin() + maxIndex + 1, regionOutLines.end()); // ɾ��(maxIndex, end]
                        regionOutLines.erase(regionOutLines.begin(), regionOutLines.begin() + (minIndex - 1 <= 0 ? 1 : minIndex - 1)); // ɾ��[0, minIndex)
                    }
                    int afterDel = regionOutLines.size();
                    // ɾ���߶κ����¿�ʼѭ��
                    if (beforeDel != afterDel) {
                        // ������ж��Ƿ�ֹ��ѭ��
                        i = 0;
                        break;
                    }
                }
            }
            vector<YFSegment> tmpLines;
            for (auto l : regionOutLines) {
                // ɾ�����߶�
                if (l.distance > MIN_ERR) tmpLines.push_back(l);
            }
            regionOutLines = tmpLines;
            return YFRegion(regionOutLines);
        };

        for (auto r : regions) outRegions.push_back(zoomRegion(r, distance >= D ? distance : D));
        // ��������������distance����λ��Ϊ�˱�֤����֮�����ص���������СֵΪD��С��D�����д���

        /* ��n�㼯���������� */
        auto compare = [](YFPoint a, YFPoint b) {
            if (abs(a.x - b.x) < MIN_ERR) return a.y < b.y;
            else return a.x < b.x;
        };

        if (outRegions.size() > 1) {
            // ��ʼ������������֮��Ľ���
            vector<YFPoint> corPoints;
            for (int i = 0; i < outRegions.size(); i++) {
                auto curRegion = outRegions.at(i);
                for (auto l : curRegion.borders) { // ���ڵ�ǰ�����ÿһ���߽磬������������������Ľ���
                    vector<YFPoint> corPts; // ���ڴ�ŵ�ǰ�߽��������Ľ���
                    vector<YFRegion> corRegions; // ���ڴ���뵱ǰ�߽��н��������
                    for (int j = 0; j < outRegions.size(); j++) {
                        if (j == i) continue;
                        auto nextRegion = outRegions.at(j);
                        auto regionCorPts = l.getCorWithRegion(nextRegion);
                        if (regionCorPts.size() > 0) {
                            corPts.insert(corPts.end(), regionCorPts.begin(), regionCorPts.end()); // �����н����ռ�����
                            corRegions.push_back(nextRegion); // ���н��������Ҳ�ռ�����
                        }
                    }
                    if (corPts.size() > 0) { // ���ڽ���ʱ�����кϲ�����
                        for (int k = 0; k < corPts.size(); k++) {
                            corPts[k] = YFPoint(corPts.at(k).x, corPts.at(k).y, l.startPoint.bulge, ""); // ���ֻ�����Ϣ
                        }
                        corPts.push_back(l.startPoint);
                        corPts.push_back(l.endPoint);
                        corPoints.insert(corPoints.end(), corPts.begin(), corPts.end()); // ���潻��
                                                                                         // �����е������������֮��ȡ���߶Σ�����߶��е��������⣬�򱣴��߶�
                        sort(corPts.begin(), corPts.end(), compare);
                        for (int k = 0; k < corPts.size() - 1; k++) { // ���Ź���ĵ�˳��������
                            YFSegment tmpSeg(corPts.at(k), corPts.at(k + 1));
                            // �ж����������߶��Ƿ����ཻ������
                            bool isInRegion = false;
                            for (int index = 0; index < outRegions.size(); index++) {
                                // ���������ڵ���ȥ��
                                if (index == i) continue;
                                auto r = outRegions.at(index);
                                isInRegion = isInRegion || tmpSeg.center.isInRegionWithoutBorder(r);
                            }
                            for (auto r : this->regions) {
                                //continue;
                                // �������������Ҳȥ����
                                isInRegion = isInRegion || tmpSeg.center.isInRegion(r) || tmpSeg.startPoint.isInRegion(r) || tmpSeg.endPoint.isInRegion(r);
                            }
                            if (!isInRegion) { // ���������⣬���ж���Ϊ�߽�
                                outLines.push_back(tmpSeg);
                            }
                        }
                    } else {
                        // ���û�н��㣬֤�������߽粻��Ҫ�ϲ�
                        bool isInRegion = false;
                        for (int index = 0; index < outRegions.size(); index++) {
                            if (index == i) continue;
                            auto r = outRegions.at(index);
                            isInRegion = isInRegion || l.center.isInRegionWithoutBorder(r);
                        }
                        for (int index = 0; index < this->regions.size(); index++) {
                            //continue;
                            auto r = this->regions.at(index);
                            isInRegion = isInRegion || l.center.isInRegion(r) || l.startPoint.isInRegion(r) || l.endPoint.isInRegion(r);
                        }
                        if (!isInRegion) outLines.push_back(l);
                    }
                }
            }
        } else if (outRegions.size() == 1) {
            // ���ֻ��һ����������Ҫ���㽻�㣬ֱ�ӽ��ⲿ����ı��߷��ؼ���
            outLines = outRegions.begin()->borders;
        }

        if (outLines.size() > 0) {
            // ��outLines�е��߶ν���ȥ�ز���
            vector<YFSegment> tmpOutLines;
            for (auto l : outLines) {
                bool alreadyExist = false;
                for (auto s : tmpOutLines) {
                    // ����Ƿ�����ͬ�߶��Ѿ������ˣ����︴�Ӷ�Ϊo(n^2)�������ù�ϣ���Ż�
                    alreadyExist = abs(s.center.x - l.center.x) < MIN_ERR && abs(s.center.y - l.center.y) < MIN_ERR;
                    if (alreadyExist) break;
                }
                if (!alreadyExist) {
                    tmpOutLines.push_back(l);
                }
            }
            outLines = tmpOutLines;
        }
        auto rs = YFHouse().findRegions(outLines); // ����findRegions���������߽���������
        auto finalOutRegion = YFRegion();
        int maxSize = 0;
        if (rs.size() > 0) {
            // ����ҵ���������򣬷��������Ǹ�
            for (int i = 0; i < rs.size(); i++) {
                if (rs[i].borders.size() > maxSize) {
                    finalOutRegion = rs[i];
                    maxSize = rs[i].borders.size();
                }
            }
        }
        if (!finalOutRegion.isNULL && distance < D) {
            finalOutRegion = zoomRegion(finalOutRegion, distance - D);
        }
        if (!finalOutRegion.isNULL) outLines = finalOutRegion.borders;
        return outLines;
    }

    /* ������ǽ�� */
    vector<YFSegment> YFHouse::findInnerLiners() {
        struct LINE {
            YFSegment s;
            bool hasChanged;
        };
        map <string, LINE> inlinesMap; // �����������
        map <string, string> singleMap; // ����������Ӧ��ϵ��<��ǰ�߶Σ�lineMap��key>
                                        // ����һ����ϣ�����ڿ��ٲ���������֮�������
        vector<YFRegion> allLines;
        // ��������߶Σ������� ��������� - ����
        allLines = this->regions;
        allLines.push_back(YFRegion(this->outLines));
        // ÿһ������allLines�У���������[rIndex][lIndex]������
        // ������������ڲ��ҹ�ϣ��

        /* ��n�㼯���������� */
        auto compare = [](YFPoint a, YFPoint b) {
            if (abs(a.x - b.x) < MIN_ERR) return a.y < b.y;
            else return a.x < b.x;
        };

        /* ��С���󽫵㰴�������� */
        auto sortPoints = [compare](YFPoint a, YFPoint b) {
            vector<YFPoint> arr({ a, b });
            sort(arr.begin(), arr.end(), compare);
            return arr;
        };

        /* ����̽���߶� */
        auto computeSeekers = [sortPoints](YFSegment s, YFRegion r, bool isOutLine) {
            const double delta = 0.01; // ƫ����
            const double d = 0.5; // ̽���������
            auto sorted = sortPoints(s.startPoint, s.endPoint);
            auto sp = sorted[0];
            auto ep = sorted[1];
            auto cp = s.center;
            vector<YFSegment> seekers;
            double cosAlpha = abs(ep.y - sp.y) / s.distance;
            double sinAlpha = abs(ep.x - sp.x) / s.distance;
            YFPoint testPoint(cp.x + delta * cosAlpha, cp.y + delta * sinAlpha); // �����ж�̽�뷽��
            int direction = (testPoint.isInRegion(r) == isOutLine) ? 1 : -1;
            seekers.push_back(YFSegment(sp, YFPoint(sp.x + direction * d * cosAlpha, sp.y + direction * d * sinAlpha)));
            seekers.push_back(YFSegment(ep, YFPoint(ep.x + direction * d * cosAlpha, ep.y + direction * d * sinAlpha)));
            return seekers;
        };

        /* ���ɹ�ϣ��ļ�ֵ��rIndex - ���������������lIndex - �߶��������е����� */
        auto computeKeys = [](int rIndex1, int lIndex1, int rIndex2, int lIndex2) {
            vector<int> k1({ rIndex1, lIndex1 });
            vector<int> k2({ rIndex2, lIndex2 });
            vector<int> sk;
            string key;
            if (k1[0] > k2[0] || (k1[0] == k2[0] && k1[1] > k2[1])) {
                sk = k1;
                k1 = k2;
                k2 = sk;
            }
            key = to_string(k1[0]) + string("-") + to_string(k1[1]) + string("-")
                + to_string(k2[0]) + string("-") + to_string(k2[1]);
            return key;
        };

        /* ����singleMap��key */
        auto compute2Key = [](int i, int j) {
            return to_string(i) + string("-") + to_string(j);
        };

        /* ��������ֱ�ߵĽ��� */
        auto computeCorOfLines = [](YFSegment a, YFSegment b) {
            double der = a.a * b.b - a.b * b.a;
            if (a.isParalWith(b)) return YFPoint(); // a, b ƽ�в����ߣ������м���
            double x = (a.b * b.c - a.c * b.b) / der;
            double y = (a.a * b.c - a.c * b.a) / der; // �������������ֵ
            return YFPoint(x, y);
        };

        /* ���������߶ε����� */
        auto computeMidLine = [sortPoints](YFSegment s, YFSegment l) {
            auto a = sortPoints(s.startPoint, s.endPoint);
            auto b = sortPoints(l.startPoint, l.endPoint);
            auto msp = YFSegment(a[0], b[0]).center;
            auto mep = YFSegment(a[1], b[1]).center;

            return YFSegment(msp, mep);
        };

        /* ���߶����� */
        auto compareLine = [](YFSegment a, YFSegment b) {
            if (abs(a.center.x - b.center.x) < MIN_ERR) return a.center.y < b.center.y;
            else return a.center.x < b.center.x;
        };

        // �����㷨��o(n^2)��
        int count = allLines.size();
        for (int i = 0; i < count; i++) {
            bool isOutLine = i >= this->regions.size(); // �Ƿ�������߶�
            auto r = allLines[i]; // ��ǰ����
            for (int j = 0; j < r.borders.size(); j++) {
                auto s = r.borders[j]; // ��ǰ�߶�
                auto seekers = computeSeekers(s, r, isOutLine); // һ��������̽��

                struct TMP_LINE {
                    YFSegment nearestLine;
                    vector<int> keys;
                    double minDistance;
                };

                vector<TMP_LINE> opLines(seekers.size(), { YFSegment(), vector<int>(), 100000 });

                for (int k = 0; k < count; k++) { // ���������߶Σ��ҳ���Ӧ���������
                    auto rr = allLines[k];
                    for (int n = 0; n < rr.borders.size(); n++) {
                        for (int ptr = 0; ptr < seekers.size(); ptr++) {
                            // ����ÿһ��seeker���������������߶εĽ���
                            auto seeker = seekers[ptr];
                            auto curSeg = rr.borders[n];
                            double cosBeta = (s.a * curSeg.a + s.b * curSeg.b)
                                / sqrt(s.a * s.a + s.b * s.b)
                                / sqrt(curSeg.a * curSeg.a + curSeg.b * curSeg.b);
                            if (abs(cosBeta) < 0.9) continue; // ��ֱ�߼нǲ��ܴ���10��
                            auto p = seeker.getCorWith(curSeg);
                            if (!p.isNULL) {
                                YFSegment tmp(seeker.startPoint, p);
                                if (tmp.distance < opLines[ptr].minDistance && tmp.distance > 0.1) {
                                    opLines[ptr].minDistance = tmp.distance;
                                    opLines[ptr].nearestLine = curSeg;
                                    opLines[ptr].keys = { k, n };
                                }
                            }
                        }
                    }
                }
                // �ҵ���������Լ���Ӧ������֮�󣬸���inLinesMap
                struct TMP_MID_LINE {
                    YFSegment midLine;
                    vector<int> keys;
                };
                vector<TMP_MID_LINE> midLines;
                if (opLines[0].nearestLine.isNULL && opLines[1].nearestLine.isNULL) {
                    continue; // ���û�ҵ���Ӧ���ߣ��Ͳ����в���
                } else if (!opLines[0].nearestLine.isNULL && !opLines[1].nearestLine.isNULL) {
                    // ����seeker���ҵ��˶�Ӧ���ߣ���Ҫ�����߽����޼�
                    vector<YFPoint> tmpPts({
                        opLines[0].nearestLine.startPoint,
                        opLines[0].nearestLine.endPoint,
                        opLines[1].nearestLine.startPoint,
                        opLines[1].nearestLine.endPoint,
                    });
                    sort(tmpPts.begin(), tmpPts.end(), compare);

                    double midX = (tmpPts[1].x + tmpPts[2].x) / 2;
                    double midY = (tmpPts[1].y + tmpPts[2].y) / 2;

                    auto m1 = computeMidLine(s, opLines[0].nearestLine);
                    auto m2 = computeMidLine(s, opLines[1].nearestLine);
                    vector<YFSegment> m({ m1, m2 });
                    sort(m.begin(), m.end(), compareLine);
                    bool hasReverse = !m[0].center.isEqualTo(m1.center); // �Ƿ������������

                    auto computeAnotherPoint = [midX, midY](YFSegment s) {
                        double x;
                        double y;
                        if (abs(s.a) < MIN_ERR) {
                            x = midX;
                            y = (s.a * midX + s.c) / s.b;
                        } else {
                            x = (s.b * midY - s.c) / s.a;
                            y = midY;
                        }
                        return YFPoint(x, y);
                    };

                    auto m1sp = m[0].startPoint;
                    auto m1ep = computeAnotherPoint(m[0]);
                    auto m2sp = computeAnotherPoint(m[1]);
                    auto m2ep = m[1].endPoint;
                    midLines.push_back({ m1, opLines[hasReverse ? 1 : 0].keys });
                    midLines.push_back({ m2, opLines[hasReverse ? 0 : 1].keys });
                } else {
                    // ֻ��һ��seeker�ҵ��˶�Ӧ���ߣ�ֱ�ӷ��ض�Ӧ���߼���
                    auto theLINE = opLines[0].nearestLine.isNULL ?
                        opLines[1] : opLines[0];
                    auto nearestLine = theLINE.nearestLine;
                    auto midLine = computeMidLine(s, nearestLine);
                    midLine.thickness = theLINE.minDistance;
                    midLine.startPoint.bulge = s.startPoint.bulge; // ����Ļ�����Ϣ��Ҫ�ж�һ��

                    midLines.push_back({ midLine, theLINE.keys });
                }

                // ������õ����ߴ洢����Ӧλ��
                for (auto midLineWithKeys : midLines) {
                    auto keys = midLineWithKeys.keys;
                    auto midLine = midLineWithKeys.midLine;
                    if (keys.size() < 2) continue; // ��ֹԽ��
                    auto theKey = computeKeys(i, j, keys[0], keys[1]);

                    bool isInRegion = false; // �ж��������Ƿ���������
                    for (auto r : this->regions) {
                        isInRegion = isInRegion ||
                            midLine.startPoint.isInRegionWithoutBorder(r) ||
                            midLine.endPoint.isInRegionWithoutBorder(r) ||
                            midLine.center.isInRegionWithoutBorder(r);
                    }
                    if (isInRegion) continue;

                    LINE wmLine = { midLine, false };
                    auto curLineKey = compute2Key(i, j);
                    auto nearestLineKey = compute2Key(keys[0], keys[1]);

                    if (singleMap.find(curLineKey) == singleMap.end()) singleMap[curLineKey] = theKey; // ���浱ǰ����
                    if (singleMap.find(nearestLineKey) == singleMap.end()) singleMap[nearestLineKey] = theKey; // �����Ӧ����

                    if (inlinesMap.find(theKey) == inlinesMap.end()) {
                        inlinesMap[theKey] = wmLine; // �����߶�֮��ֻ��Ҫһ������
                    }
                }
            }
        }

        for (int i = 0; i < count; i++) {
            auto r = allLines[i];
            for (int j = 0; j < r.borders.size(); j++) {
                int nextIndex = (j + 1) % r.borders.size();
                auto curLineKey = compute2Key(i, j);
                auto nextLineKey = compute2Key(i, nextIndex);
                auto curLine = r.borders.at(j);
                auto nextLine = r.borders.at(nextIndex);

                YFPoint linePoint; // ������Ե�Ĺ�����
                if (curLine.endPoint.isEqualTo(nextLine.startPoint) || curLine.endPoint.isEqualTo(nextLine.endPoint)) {
                    linePoint = curLine.endPoint;
                } else linePoint = curLine.startPoint;

                if (singleMap.find(curLineKey) != singleMap.end() &&
                    singleMap.find(nextLineKey) != singleMap.end()) {
                    // �������߶ε����߶�����ʱ
                    auto curMidLineKey = singleMap[curLineKey];
                    auto nextMidLineKey = singleMap[nextLineKey];
                    auto curMidLine = inlinesMap[curMidLineKey];
                    auto nextMidLine = inlinesMap[nextMidLineKey];

                    // ��ʼ���������ӳ���Ӧ�ı�
                    if (!curMidLine.s.isParalWith(nextMidLine.s)) {
                        // ֻ���㲻ƽ�е�ʱ������
                        YFPoint corPoint = computeCorOfLines(curMidLine.s, nextMidLine.s); // �����������ߵĽ���
                        if (corPoint.isNULL) continue; // û�н��㣬ֱ������
                        vector<LINE> tmpArr = { curMidLine, nextMidLine };
                        string key[2] = { curMidLineKey, nextMidLineKey };
                        for (int k = 0; k < tmpArr.size(); k++) {
                            auto innerLine = tmpArr.at(k).s;
                            bool hasChanged = tmpArr.at(k).hasChanged; // �Ƿ��Ѿ�����������
                                                                       // ����߶��Ѿ�������������ôֻ�ӳ���������
                            if (((corPoint.x < innerLine.xRange.max && corPoint.x > innerLine.xRange.min) ||
                                (corPoint.y < innerLine.yRange.max && corPoint.y > innerLine.yRange.min)) && !hasChanged) {
                                // �������ֱ�ߵķ�Χ�ڣ������������
                                double v1[2] = { innerLine.endPoint.x - corPoint.x, innerLine.endPoint.y - corPoint.y };
                                double v2[2] = { innerLine.startPoint.x - corPoint.x, innerLine.startPoint.y - corPoint.y };
                                YFPoint otherPoint;
                                if (pow(v1[0], 2) + pow(v1[1], 2) < pow(v2[0], 2) + pow(v2[1], 2)) otherPoint = innerLine.startPoint;
                                else otherPoint = innerLine.endPoint;
                                inlinesMap.at(key[k]) = LINE({ YFSegment(corPoint, otherPoint), true });
                            } else {
                                // �������ֱ�߷�Χ�⣬��������ӳ�
                                vector<YFPoint> edgePoints = {
                                    innerLine.startPoint, innerLine.endPoint,
                                    corPoint
                                };
                                sort(edgePoints.begin(), edgePoints.end(), compare); // �������ߵĶ˵��������
                                YFSegment combineLine = YFSegment(edgePoints.at(0), edgePoints.at(2));
                                inlinesMap.at(key[k]) = LINE({ combineLine, true }); // ���¶�Ӧ������
                            }
                        }
                    }
                }
            }
        }
        vector<YFSegment> inlines;
        for (auto it = inlinesMap.begin(); it != inlinesMap.end(); it++) {
            inlines.push_back(it->second.s);
        }
        return inlines;
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

        /* �ж�ĳ������͹��ȥ���ǰ����� */
        auto arcDirection = [](YFSegment curSeg, YFSegment nextSeg) {
            double vector_1[2] = { curSeg.endPoint.x - curSeg.startPoint.x, curSeg.endPoint.y - curSeg.startPoint.y }; // �������߶�
            double vector_2[2] = { nextSeg.endPoint.x - nextSeg.startPoint.x, nextSeg.endPoint.y - nextSeg.startPoint.y };
            return vector_1[0] * vector_2[1] > vector_1[1] * vector_2[0]; // ���Ϊtrue����ô������ʱ�룻���Ϊfalse����ô������ʱ��
        };

        /* �ж����������˳�淽�� */
        int cw = 0; // ˳ʱ�뷽��Ľ� clockwise
        int anticw = 0; // ��ʱ�뷽��Ľ� anticlockwise
        for (int i = 0; i < this->borders.size() - 1; i++) {
            if (arcDirection(this->borders.at(i), this->borders.at(i + 1))) {
                anticw++;
            } else cw++;
        }

        int arcDirect = anticw > cw ? 1 : -1; // ���Ϊ1����ô����Ϊ��ʱ�룻���Ϊ������ô����Ϊ-1

        double arcArea = 0; // �ȼ�����л��ߵ����

        for (int i = 0; i < this->borders.size(); i++) {
            YFSegment s = this->borders.at(i);
            points.push_back(s.startPoint);
            double sb = abs(s.startPoint.bulge);
            double eb = abs(s.endPoint.bulge);
            double b = sb > eb ? sb : eb; // ���߷�ת֮�󣬱���һ�����͹�Ȳ�Ϊ0
            if (b > MIN_ERR) {
                int p = 1;
                if (sb > eb) {
                    p = arcDirect * (s.startPoint.bulge > 0 ? 1 : -1); // ������ķ��������巽��һ�£���ô��Ӧ�ü��Ϲ������
                } else {
                    p = arcDirect * (s.endPoint.bulge > 0 ? -1 : 1); // ����ͼ�ȥ�������
                }
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
            double sb = abs(l.startPoint.bulge);
            double eb = abs(l.endPoint.bulge);
            double b = sb > eb ? sb : eb; // ���߷�ת֮�󣬱���һ�����͹�Ȳ�Ϊ0
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

    /* ���㵱ǰ������ָ������Ľ��� */
    vector<YFPoint> YFRegion::getCorWithRegion(YFRegion r) {
        vector<YFPoint> regionCorPoints;
        for (YFSegment l : this->borders) {
            vector<YFPoint> lineCorPoints = l.getCorWithRegion(r);
            if (lineCorPoints.size() == 0) continue;
            regionCorPoints.insert(regionCorPoints.end(), lineCorPoints.begin(), lineCorPoints.end()); // �����㱣������
        }
        return regionCorPoints;
    }

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

    /* �жϵ��Ƿ��ڲ������߽�������� */
    bool YFPoint::isInRegionWithoutBorder(YFRegion r) {
        bool isOnBorder = false;
        for (auto l : r.borders) {
            bool hasPoint = l.hasPoint(*(this));
            isOnBorder = isOnBorder || hasPoint;
        }
        if (isOnBorder) return false; // ����ڱ߽��ϣ���ô��ֱ���ж�Ϊ����������
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
        this->b = sp.x - ep.x; // ֱ�ߵ�һ��ʽ��Ȼһֱ��д���ˣ�
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

    /* �ж�ĳ�߶��Ƿ����ĳ�� */
    bool YFSegment::hasPoint(YFPoint p) {
        auto l = *(this);
        bool isOnLine = abs(l.a * p.x - l.b * p.y + l.c) < MIN_ERR;
        bool isInRange;
        if (abs(l.a) < MIN_ERR) {
            // ���ֱ��ƽ����x�ᣬ���ж�x�����귶Χ
            isInRange = p.x <= l.xRange.max && p.x >= l.xRange.min;
        } else {
            // ���ֱ��ƽ����y�ᣬ���ж�y�����귶Χ
            isInRange = p.y <= l.yRange.max && p.y >= l.yRange.min;
        }
        return isInRange && isOnLine;
    }
}

