#ifndef CSVLOADER_H
#define CSVLOADER_H

#include <QObject>
#include <QFile>
#include <QDebug>
#include <QString>
#include <QList>
#include "day.h"
#include "group.h"
#include "module.h"
#include "plan.h"
#include "semester.h"
#include "timeslot.h"
#include "week.h"

class CsvLoader : public QObject
{
    Q_OBJECT
public:
    explicit CsvLoader(QObject *parent = nullptr);
    Plan* loadPlan();

private:
    void addBase(Plan* plan);
    void addConstraints(Plan* plan);
    void addGroups(Plan* plan);
    void addModules(Plan* plan);

signals:

};

#endif // CSVLOADER_H
