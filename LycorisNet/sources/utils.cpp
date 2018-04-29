#include "utils.h"

unsigned sort1(Individual &in, float *ret1, unsigned *ret2) {
    std::vector<bool> temp1(in.innovationNum, false);
    float temp2[in.innovationNum];
    unsigned temp3[in.innovationNum];

    unsigned length = 0;
    for (auto iter = in.nodeMap->begin(); iter != in.nodeMap->end(); ++iter) {
        auto genomeMap = iter->second->genomeMap;
        length += genomeMap->size();
        for (auto it = genomeMap->begin(); it != genomeMap->end(); ++it) {
            auto num = it->second.innovationNum;
            temp1[num] = true;
            temp2[num] = it->second.weight;
            temp3[num] = num;
        }
    }

    if (length != 0) {
        ret1 = new float[length];
        ret2 = new unsigned[length];
        unsigned count = 0;
        for (int i = 0; i < temp1.size(); ++i) {
            if (temp1[i]) {
                ret1[count] = temp2[i];
                ret2[count] = temp3[i];
                count++;
            }
        }
    }

    return length;
}

unsigned sort2(Individual &in, Gen *ret1, Ome *ret2) {
    std::vector<bool> temp1(in.innovationNum, false);
    Gen temp2[in.innovationNum];
    Ome temp3[in.innovationNum];

    unsigned length = 0;
    for (auto iter = in.nodeMap->begin(); iter != in.nodeMap->end(); ++iter) {
        auto genomeMap = iter->second->genomeMap;
        length += genomeMap->size();
        for (auto it = genomeMap->begin(); it != genomeMap->end(); ++it) {
            auto num = it->second.innovationNum;
            temp1[num] = true;
            temp2[num] = it->first;
            temp3[num] = it->second;
        }
    }

    if (length != 0) {
        ret1 = new Gen[length];
        ret2 = new Ome[length];
        unsigned count = 0;
        for (int i = 0; i < temp1.size(); ++i) {
            if (temp1[i]) {
                ret1[count] = temp2[i];
                ret2[count] = temp3[i];
                count++;
            }
        }
    }

    return length;
}

float distance(Individual &in1, Individual &in2) {
    float d;
    unsigned DE = 0;
    float *w1 = nullptr;
    unsigned *i1 = nullptr;
    float *w2 = nullptr;
    unsigned *i2 = nullptr;
    auto len1 = sort1(in1, w1, i1);
    auto len2 = sort1(in2, w2, i2);
    unsigned N = 0;
    if (len1 > len2) {
        N = len1;
    } else {
        N = len2;
    }
    if (N == 0) {
        return 0;
    }
    float W = 0;
    unsigned countW = 0;
    unsigned point1 = 0;
    unsigned point2 = 0;

    while (true) {
        if (point1 == len1 || point2 == len2) {
            break;
        }

        if (i1[point1] == i2[point2]) {
            auto temp = (w1[point1] - w2[point2]);
            W += temp > 0 ? temp : (0 - temp);
            countW++;
            point1++;
            point2++;
        } else if (i1[point1] > i2[point2]) {
            DE++;
            point2++;
        } else {
            DE++;
            point1++;
        }
    }

    DE += len1 + len2 - point1 - point2;
    d = (in1.args->c1 * DE) / N + (in1.args->c2 * W) / countW;

    if (len1 != 0) {
        delete[] w1;
        delete[] i1;
    }
    if (len2 != 0) {
        delete[] w2;
        delete[] i2;
    }

    return d;
}


