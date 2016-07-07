#pragma once
#include <vector>
#include "text.h"
#include <QString>

class clusterAnaliz {
private:
    std::vector<text> texts;
    void findTextMass(int);

    double textDist(text, text);

    struct centroid {

        double mass;

        int t_idx;

        text c_text;

        std::vector<text> c_texts;

        void countMass() {
            text temp = c_text;
            for(size_t i = 1; i < c_texts.size(); ++i) {
                temp+=c_texts[i];
            }
            c_text = temp;
            c_texts.clear();
        }
    };

public:
    clusterAnaliz(QString);
    void caDo(int, QString, QString, bool shouldBeCleaned);
};
