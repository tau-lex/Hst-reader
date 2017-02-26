#ifndef CSVWRITER_H
#define CSVWRITER_H

#include <QObject>

class CsvWriter : public QObject
{
    Q_OBJECT
public:
    explicit CsvWriter(QObject *parent = 0);
    CsvWriter(QString fName);
    ~CsvWriter();

protected:
    QString                     fileName;
private:
    QList<std::vector<double> > *data;
    bool                        zeroColumnIsTime = false;
    qint32                      precision = 6;
    std::vector<qint32>         precisionVector;

public:
    void setFileName(const QString fName);
    QString getFileName(void) const;
    qint32 getSize(void) const;
    void setZeroColumnIsTime(const bool isTime);
    bool getZeroColumnIsTime(void) const;
    void setPrecision(const qint32 prec);
    void setPrecision(const std::vector<qint32> precVec);
    qint32 getPrecision(void) const;
    QList<std::vector<double> > *getDataPtr(void);
    void writeFile(void);
    void writeFile(const QString fName);
};

#endif // CSVWRITER_H
