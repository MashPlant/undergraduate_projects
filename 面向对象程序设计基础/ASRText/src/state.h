#ifndef State_H
#define State_H
#include <QString>
#include <QObject>

class State:public QObject
{
    Q_OBJECT
public:
    enum Status { SAVED, MODIFIED, NEW, EMPTY };
    QString getfilePath() { return filePath; }
    void setfilePath(QString filePath)
    {
        this->filePath = filePath;
        emit statusChanged();
    }
    int getstatus() { return status; }
    void setstatus(int status)
    {
        this->status = status;
        emit statusChanged();
    }
    QString getfileName() { return fileName; }
    void setfileName(QString fileName)
    {
        this->fileName=fileName;
        emit statusChanged();
    }
private:
    QString filePath;
    QString fileName;
    int status = EMPTY;
signals:
    void statusChanged();
};

#endif // State_H
