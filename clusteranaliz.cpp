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

std::vector <std::string> readDir(QString dir) {
    std::vector <std::string> result;
    QDir folder(dir);
    QStringList list = folder.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden
                                        | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
    for(size_t i = 0; i < list.size(); ++i) {
        if(QFileInfo(dir + list[i]).isFile()) {
            result.push_back(list[i].toStdString());
        }
    }


    return result;
}

void clusterAnaliz::stop() {

}

clusterAnaliz::clusterAnaliz(QObject *parent, QString directory, QString rDirectory, bool sbcl, int clAm) {

    std::vector<std::string> fileNames = readDir(directory  + "/");

    for (size_t i = 0; i < fileNames.size(); ++i) {
        std::string name = directory.toStdString() + "/" + fileNames[i];
        texts.push_back(text(name, fileNames[i]));
    }

    for (unsigned int i = 0; i < texts.size(); ++i) {
        findTextMass(i);
    }

    dirName = directory;
    resDirName = rDirectory;
    shouldBeCleaned = sbcl;
    clustersAmount = clAm;
}

void clusterAnaliz::caDo() {
    srand(time(NULL));
    std::vector<clusterAnaliz::centroid> centroids;
    std::vector<int> usedIdxs;

    for (size_t i = 0; i < clustersAmount; ++i) {
        emit statusChanged("Cluster: " + QString::number(i));

        bool notValid = false;
        int idx = rand() % texts.size();

        for(size_t k = 0; k < centroids.size(); ++k) {
            if(idx == centroids[i].t_idx) {
                ++clustersAmount;
                notValid = true;
                break;
            }
        }
        if(!notValid) {
            clusterAnaliz::centroid c;
            c.mass = 0;
            c.t_idx = idx;
            text temp = texts[idx];
            c.c_text = temp;
            centroids.push_back(c);

        }
    }


/*
    int fIdx = rand() % texts.size();
    clusterAnaliz::centroid c;
    c.mass = 0;
    c.t_idx = fIdx;
    text temp = texts[fIdx];
    c.c_text = temp;

    usedIdxs.push_back(fIdx);

    centroids.push_back(c);

    //std::cout << temp.getFileName() << "HELLO" << std::endl;

    std::vector<text> ctexts;
    ctexts.push_back(temp);

    for(size_t i = 0; i < clustersAmount - 1; ++i) {

        std::vector<double> masses;

        for(size_t k = 0; k < texts.size(); ++k) {
            double mass = 0;
            for(size_t j = 0; j < ctexts.size(); ++j) {
                mass += textDist(texts[k], ctexts[j]);
            }
            masses.push_back(mass);
            std::cout << masses[k] << std::endl;
        }

        int idx = 0;
        int mass = masses[0];

        for(size_t m = 0; m < masses.size(); ++m) {
            if(mass > masses[m]) {
                puts("HELLO");
                mass = masses[m];
                idx = m;
            }
        }

        std::cout << idx << std::endl;

    }*/
//-------------------------------------------------------------------------------------------------
    for (size_t k = 0; k < 5; ++k) {
        emit statusChanged(QString::number(k) + " Step...");
        for (int i = 0; i < texts.size(); ++i) {
            double dist = 0;
            int whereIdx = 0;
            for ( int j = 0; j < centroids.size(); ++j) {

                if (dist < textDist(texts[i], centroids[j].c_text)) {
                    dist = textDist(texts[i], centroids[j].c_text);
                    whereIdx = j;
                }
            }

            centroids[whereIdx].c_texts.push_back(texts[i]);
        }

        for (size_t i = 0; i < centroids.size(); ++i) {
            centroids[i].countMass();
        }

    }

    for (int i = 0; i < texts.size(); ++i) {
        double dist = 0;
        int whereIdx = 0;
        for ( int j = 0; j < centroids.size(); ++j) {

            if (dist < textDist(texts[i], centroids[j].c_text)) {
                dist = textDist(texts[i], centroids[j].c_text);
                whereIdx = j;
            }
        }
        centroids[whereIdx].c_texts.push_back(texts[i]);
    }

    if(shouldBeCleaned) {
        clearDir(resDirName);
    }

    for(size_t i = 0; i < centroids.size(); ++i) {
        QDir().mkdir(resDirName + "/group" + QString::number(i));
        for(size_t j = 0; j < centroids[i].c_texts.size(); ++j) {
            //std::cout << centroids[i].c_texts[j].getFileName().c_str() << std::endl;
            QFile::copy(dirName + "/" + centroids[i].c_texts[j].getFileName().c_str(),
                        resDirName + "/group" + QString::number(i) + "/" + centroids[i].c_texts[j].getFileName().c_str());
        }
    }

    emit statusChanged("Done.");
    emit finished();

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
