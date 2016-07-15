#include "clusteranaliz.h"
#include <ctime>
#include <sys/types.h>
#include <algorithm>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <iostream>
#include <math.h>
#include <QDir>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>
#include <map>

clusterAnaliz::~clusterAnaliz() {

}

void clearDir( const QString path )
{
    QDir dir( path );

    dir.setFilter( QDir::NoDotAndDotDot | QDir::Files );
    foreach( QString dirItem, dir.entryList() )
        dir.remove( dirItem );

    dir.setFilter( QDir::NoDotAndDotDot | QDir::Dirs );
    foreach( QString dirItem, dir.entryList() )
    {
        QDir subDir( dir.absoluteFilePath( dirItem ) );
        subDir.removeRecursively();
    }
}

std::vector <QString> readDir(QString dir) {
    std::vector <QString> result;
    QDir folder(dir);
    QStringList list = folder.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden
                                        | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
    for(size_t i = 0; i < list.size(); ++i) {
        if(QFileInfo(dir + list[i]).isFile()) {
            result.push_back(list[i]);
        }
    }


    return result;
}

void clusterAnaliz::stop() {

}

clusterAnaliz::clusterAnaliz(QObject *parent, QString directory, QString rDirectory, bool sbcl, int clAm, bool _customFunc, QString csv) {

    std::vector<QString> fileNames = readDir(directory  + "/");

    for (size_t i = 0; i < fileNames.size(); ++i) {
        std::string name = directory.toStdString() + "/" + fileNames[i].toStdString();
        texts.push_back(text(name, fileNames[i]));
    }

    for (unsigned int i = 0; i < texts.size(); ++i) {
        findTextMass(i);
    }

    dirName = directory;
    resDirName = rDirectory;
    shouldBeCleaned = sbcl;
    clustersAmount = clAm;
    customFunc = _customFunc;
    if(customFunc)
        csvAddr = csv;
    else
        csvAddr = "";
}

void clusterAnaliz::caDo() {
    if (!customFunc) {
        srand(time(NULL));
        std::vector<clusterAnaliz::centroid> centroids;
        std::vector<int> usedIdxs;

        emit statusChanged("Cluster 1 ...");
        int fIdx = rand() % texts.size();

        clusterAnaliz::centroid c;
        c.mass = 0;
        c.t_idx = fIdx;
        c.c_text = texts[fIdx];
        usedIdxs.push_back(fIdx);
        centroids.push_back(c);

        text mainT = c.c_text;

        emit statusChanged("Cluster 2 ...");

        double dist = textDist(mainT, texts[fIdx]);
        int idx = fIdx;
        for (size_t i = 0; i < texts.size(); ++i) {
            if (dist > textDist(mainT, texts[i])) {
                dist = textDist(mainT, texts[i]);
                idx = i;
            }
        }

        clusterAnaliz::centroid temp;
        temp.mass = 0;
        temp.t_idx = idx;
        temp.c_text = texts[idx];
        usedIdxs.push_back(idx);
        centroids.push_back(temp);

        int m = 1;
        int textId = 0;
        for (size_t cl = 0; cl < clustersAmount - 2; ++cl) {
            emit statusChanged("Cluster " + QString::number(cl + 2) + " ...");
            for (size_t clT = m; clT < centroids.size(); ++clT) { // Прибавление текста
                mainT += texts[centroids[clT].t_idx];
            }

            m++;

            for (size_t t = 0 ; t < texts.size(); ++t) { // Поиск не использованого текста
                int count = 0;
                for (size_t uI = 0; uI < usedIdxs.size(); ++uI) {
                    if (t != usedIdxs[uI]) {
                        count++;
                    }
                }
                if (count == usedIdxs.size()) {
                    textId = t;
                    break;
                }
            }

            dist = textDist(mainT, texts[textId]);
            idx = textId;

            // Поиск максимально не похожего текста
            for (size_t t = 0; t < texts.size(); ++t) {
                emit statusChanged("Setting text " + QString::number(t + 1) + " ...");
                int count = 0;
                for (size_t uI = 0; uI < usedIdxs.size(); ++uI) {
                    if (t != usedIdxs[uI]) {
                        count++;
                    }
                }
                if (count == usedIdxs.size()) {
                    // Текс не был использован ранее, проверяем его на схожеть с mainT
                    if (dist > textDist(mainT, texts[t])) {
                        dist = textDist(mainT, texts[t]);
                        idx = t;
                    }
                }
            }

            clusterAnaliz::centroid temp1;
            temp1.c_text = texts[idx];
            temp1.t_idx = idx;
            temp1.mass = 0;
            usedIdxs.push_back(idx);
            centroids.push_back(temp1);
        }

        for (size_t t = 0; t < texts.size(); ++t) {
            double di = textDist(texts[t], centroids[0].c_text);
            int whereIdx = 0;
            for (size_t cl = 0; cl < centroids.size(); ++cl) {
                if (di < textDist(texts[t], centroids[cl].c_text)) {
                    di = textDist(texts[t], centroids[cl].c_text);
                    whereIdx = cl;
                }
            }
            centroids[whereIdx].c_texts.push_back(texts[t]);
        }

        if (shouldBeCleaned) {
            clearDir(resDirName);
        }

        for (size_t i = 0; i < centroids.size(); ++i) {
            QDir().mkdir(resDirName + "/group" + QString::number(i));
            for (size_t j = 0; j < centroids[i].c_texts.size(); ++j) {
                QFile::copy(dirName + "/" + centroids[i].c_texts[j].getFileName().toStdString().c_str(),
                            resDirName + "/group" + QString::number(i) + "/" + centroids[i].c_texts[j].getFileName().toStdString().c_str());
            }
        }

        emit statusChanged("Done.");
        emit unBlock();
        emit finished();
    }
    else { // USING CSV TABLES _______________________________________ !__!__! ___A_SD_A_S_!___S_AD_AS_D_
        //Only god knows whats going on after this line
        QStringList vls;
        emit statusChanged("Reading csv file...");
        QFile file(csvAddr);
        if(file.open(QIODevice::ReadOnly)) {
            QString data;
            data = QString::fromUtf8( file.readAll());
            QRegExp rx(",|\n");
            vls = data.split(rx);
            file.close();
        }

        size_t f2 = 4;
        size_t d = 5;

        for(size_t f1 = 3; f1 < vls.size() - 1; f1 += 3) {
            std::map<QString, double> tempM;
            if(weights.find(vls[f1]) == weights.end()) {
                //weights[vls[f1]] = tempM;
                weights.insert(std::pair<QString, std::map<QString, double> >(vls[f1], tempM) );
            }

        }

        for(size_t f1 = 3; f1 < vls.size() - 1, f2 < vls.size() - 1, d < vls.size() - 1; f1 += 3, f2 += 3, d += 3) {
           // weights[vls[f1]][vls[f2]] = vls[d].toDouble();
            weights[vls[f1]].insert(std::pair<QString, double>(vls[f2], vls[d].toDouble()));
        }

        srand(time(NULL));
        std::vector<clusterAnaliz::centroid> centroids;
        std::vector<int> usedIdxs;

          int zerosAmount;

       // do {

              zerosAmount = 0;
              centroids.clear();

        emit statusChanged("Cluster 1 ...");
        int fIdx = rand() % texts.size();

        clusterAnaliz::centroid c;
        c.mass = 0;
        c.t_idx = fIdx;
        c.c_text = texts[fIdx];
        usedIdxs.push_back(fIdx);
        centroids.push_back(c);




        for(size_t c = 0; c < clustersAmount - 1; ++c) {


            for (size_t t = 0 ; t < texts.size(); ++t) { // Поиск не использованого текста
                double dist = 0;
                int count = 0;
                for (size_t uI = 0; uI < usedIdxs.size(); ++uI) {
                    if (t != usedIdxs[uI]) {
                        count++;
                    }
                }
                if (count == usedIdxs.size()) {
                    for(size_t cl = 0; cl < centroids.size(); ++cl) {
                        dist += textDistS(texts[centroids[cl].t_idx].getFileName(), texts[t].getFileName());

                    }
                  //  std::cout << dist << std::endl;
                    if(dist == 0) zerosAmount += 1;
                    texts[t].setMass((dist/centroids.size())*(dist/centroids.size()));
                }
            }

            double mass;
            int idx;

            for (size_t t = 0 ; t < texts.size(); ++t) { // Поиск не использованого текста
                int count = 0;
                for (size_t uI = 0; uI < usedIdxs.size(); ++uI) {
                    if (t != usedIdxs[uI]) {
                        count++;
                    }
                }
                if (count == usedIdxs.size()) {
                    mass = texts[t].getMass();
                    idx = t;
                    break;
                }
        }

            for (size_t t = 0 ; t < texts.size(); ++t) { // Поиск не использованого текста
                int count = 0;
                for (size_t uI = 0; uI < usedIdxs.size(); ++uI) {
                    if (t != usedIdxs[uI]) {
                        count++;
                    }
                }
                if (count == usedIdxs.size()) {
                    if(mass > texts[t].getMass()) {
                        mass = texts[t].getMass();
                        idx = t;
                    }
                }
            }

            clusterAnaliz::centroid temp;
            temp.mass =0;
            temp.t_idx = idx;
            temp.c_text = texts[idx];
            usedIdxs.push_back(idx);
            centroids.push_back(temp);
        }
        //}while(zerosAmount > 0);

        for(size_t i = 0; i < centroids.size(); ++i) {
            std::cout << texts[centroids[i].t_idx].getFileName().toStdString() << std::endl;
        }

        for (size_t t = 0; t < texts.size(); ++t) {
            double di = textDistS(texts[t].getFileName(), texts[centroids[0].t_idx].getFileName());
           // std::cout << di << std::endl;
            int whereIdx = 0;
            for (size_t cl = 0; cl < centroids.size(); ++cl) {
                double d;
                if(textDistS(texts[t].getFileName(), texts[centroids[cl].t_idx].getFileName()) != 0) {
                    d = textDistS(texts[t].getFileName(), texts[centroids[cl].t_idx].getFileName());
                }
                else {
                    d = (double)(rand()/RAND_MAX)/2;
                    }
                if (di < d) {
                    di = d;
                    whereIdx = cl;
                }
            }
            centroids[whereIdx].c_texts.push_back(texts[t]);
        }

        if (shouldBeCleaned) {
            clearDir(resDirName);
        }

        for (size_t i = 0; i < centroids.size(); ++i) {
            QDir().mkdir(resDirName + "/group" + QString::number(i));
            for (size_t j = 0; j < centroids[i].c_texts.size(); ++j) {
                QFile::copy(dirName + "/" + centroids[i].c_texts[j].getFileName().toStdString().c_str(),
                            resDirName + "/group" + QString::number(i) + "/" + centroids[i].c_texts[j].getFileName().toStdString().c_str());
            }
        }

        std::cout << "-----------------------------------------------------" << std::endl;

        emit statusChanged("Done.");
        emit unBlock();
        emit finished();
    }
}

double clusterAnaliz::textDistS(QString f1, QString f2) {

    if(weights.count(f1) != 0) {
        if(weights[f1].count(f2) != 0) {
          return weights[f1][f2] * 100;
        }
    }
    else {
        return weights[f2][f1] * 100;
    }

}

void clusterAnaliz::findTextMass(int idx) {

    for (size_t i = 0; i < texts[idx].terms.size(); ++i) {
        for (size_t j = 0; j < texts[idx].terms.size(); ++j) {
            if (texts[idx].terms[i].str == texts[idx].terms[j].str) {
                texts[idx].terms[i].weight += 1 * texts[idx].terms[i].multi;
            }
        }
    }

    for (size_t i = 0; i < texts[idx].terms.size(); ++i) {
        if (texts[idx].terms[i].weight <= 2) {
            texts[idx].terms.erase(texts[idx].terms.begin() + i);
            std::vector<text::myPair>(texts[idx].terms).swap(texts[idx].terms);
            --i;
        }
    }
}

double clusterAnaliz::textDist(text t1, text t2) {

    double summ = 0;

    for (size_t i = 0; i < t1.terms.size(); ++i) {
        for (size_t j = 0; j < t2.terms.size(); ++j) {
            if (t1.terms[i].str == t2.terms[j].str) {
                summ += t1.terms[i].weight + t2.terms[j].weight;

            }
        }
    }
    return summ / (t1.terms.size() + t2.terms.size());
}
