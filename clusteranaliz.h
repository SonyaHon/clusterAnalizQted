#pragma once
#include <vector>
#include "text.h"
#include <QString>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QObject>
#include <map>

class clusterAnaliz : public  QObject {
    Q_OBJECT
private:
    std::vector<text> texts;
    void findTextMass(int);

    std::map <QString, std::map <QString, double> > weights;


    double textDist(text, text);
    double textDistS(QString, QString);

    QString dirName;
    QString resDirName;
    bool shouldBeCleaned;
    int clustersAmount;
    bool customFunc;
    QString csvAddr;

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
    clusterAnaliz(QObject *parent, QString, QString, bool, int, bool, QString);
    ~clusterAnaliz();

public slots:
    void caDo();
    void stop();

signals:

    void finished();
    void statusChanged(QString);
    void unBlock();
};
