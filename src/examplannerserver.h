#ifndef EXAMPLANNERSERVER_H
#define EXAMPLANNERSERVER_H

#include <QObject>

class ExamPlannerServer : public QObject
{
    Q_OBJECT
public:
    explicit ExamPlannerServer(QObject *parent = nullptr);

signals:

public slots:
    QString getPlans();
    void setPlans(QString newplans);
};

#endif // EXAMPLANNERSERVER_H
