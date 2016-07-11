#pragma once
#include <vector>
#include "text.h"
#include <QString>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QObject>

class clusterAnaliz : public  QObject {
    Q_OBJECT
private:
    std::vector<text> texts;
    void findTextMass(int);

    double textDist(text, text);

    QString dirName;
    QString resDirName;
    bool shouldBeCleaned;
    int clustersAmount;

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
    clusterAnaliz(QObject *parent, QString, QString, bool, int);
    ~clusterAnaliz();

public slots:
    void caDo();
    void stop();

signals:

    void finished();
    void statusChanged(QString);
    void unBlock();
};
