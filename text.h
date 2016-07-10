#pragma once
#include <vector>
#include <string>
#include <QObject>

class text {

private:
    int mass; // Not used ((
    std::string fileName;

public:

    struct myPair {
        std::wstring str;
        double weight;
        double multi;
    };

    std::vector<myPair> terms;
    text(std::string, std::string);
    text();
    ~text() {};
    void setMass(int);
    int getMass();

    void print();

    std::string getFileName() {
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

