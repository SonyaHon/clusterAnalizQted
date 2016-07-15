#pragma once
#include <vector>
#include <string>
#include <QObject>
#include <QString>

class text {

private:
    double mass; // Not used ((
    QString fileName;

public:

    struct myPair {
        std::wstring str;
        double weight;
        double multi;
    };


    std::vector<myPair> terms;
    text(std::string, QString);
    text();
    ~text() {};
    void setMass(double);
    double getMass();

    void print();

    QString getFileName() {
        return fileName;
    }

    text operator=(const text &t);
    text operator=(text &t);
    text operator+=(text &t) {

        for(size_t i = 0; i < t.terms.size(); ++i) {
            terms.push_back(t.terms[i]);
        }

        return *this;
    }
};

