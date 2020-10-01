#include "csvloader.h"



CsvLoader::CsvLoader(QObject *parent) : QObject(parent)
{
}

Plan *CsvLoader::loadPlan()
{
    Plan* plan = new Plan(nullptr);
    addBase(plan);
    addConstraints(plan);
    addGroups(plan);
    addModules(plan);
    return plan;
}

void CsvLoader::addConstraints(Plan *plan)
{
    QFile constraintsFile("../pruefungsplaner-backend/res/pruef-intervalle.csv");
    if (!constraintsFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Error opening constraints file";
    }

    QTextStream stream(&constraintsFile);
    QString content = stream.readAll();

    QList<QStringList> lines;
    for(QString part : content.split("\n")){
        lines.append(part.split(";"));
    }

    for(int i = 1; i<lines[0].size()-1; i++){
        Group* group = new Group(plan);
        group->setName(lines[0][i]);
        plan->constraints.append(group);
    }

    int line = 2; //2 is the first content line
    for(Week* week : plan->weeks){
        for(Day* day : week->getDays()){
            for(Timeslot* timeslot : day->getTimeslots()){
                for(int i = 1; i<lines[line].size()-1; i++){
                    if(lines[line][i] == "FREI"){
                        timeslot->addActiveGroup(plan->constraints[i-1]);
                    }
                }
                line++;
            }
        }
    }
}

void CsvLoader::addGroups(Plan *plan)
{
    QFile groupsFile("../pruefungsplaner-backend/res/zuege-pruef.csv");
    if (!groupsFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Error opening constraints file";
    }

    QTextStream stream(&groupsFile);
    QString content = stream.readAll();

    QList<QStringList> lines;
    for(QString part : content.split("\n")){
        lines.append(part.split(";"));
    }

    for(int i = 1; i<lines[0].size()-1; i++){
        Group* group = new Group(plan);
        group->setName(lines[0][i]);
        plan->groups.append(group);
    }

    int line = 2; //2 is the first content line
    for(Week* week : plan->weeks){
        for(Day* day : week->getDays()){
            for(Timeslot* timeslot : day->getTimeslots()){
                for(int i = 1; i<lines[line].size()-1; i++){
                    if(lines[line][i] == "FREI"){
                        timeslot->addActiveGroup(plan->groups[i-1]);
                    }
                }
                line++;
            }
        }
    }
}

void CsvLoader::addModules(Plan *plan)
{
    QFile groupsFile("../pruefungsplaner-backend/res/pruefungen.csv");
    if (!groupsFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Error opening exams file";
    }

    QTextStream stream(&groupsFile);
    QString content = stream.readAll();

    QList<QStringList> lines;
    for(QString part : content.split("\n")){
        lines.append(part.split(";"));
    }

    int line = 2; //2 is the first content line
    while(!lines[line][0].startsWith("-ENDE-")){
        Module* module = new Module(plan);
        module->setName(lines[line][2]);
        module->setOrigin(lines[line][4]);
        module->setNumber(lines[line][3]);
        for(QString groupName : lines[line][1].split(",")){
            for(Group* group : plan->groups){
                if(group->name() == groupName){
                    module->groups.append(group);
                    break;
                }
            }
        }
        for(Group* constraint : plan->constraints){
            if(constraint->name() == lines[line][0]){
                module->constraints.append(constraint);
                break;
            }
        }

        plan->modules.append(module);
        line++;
    }
}

void CsvLoader::addBase(Plan* plan)
{
    plan->weeks.append(new Week(plan));
    plan->weeks.append(new Week(plan));
    plan->weeks.append(new Week(plan));
    plan->weeks[0]->setName("Woche 1");
    plan->weeks[1]->setName("Woche 2");
    plan->weeks[2]->setName("Woche 3");
    for(Week* week : plan->weeks){
        QList<Day*> days;
        for(int x = 0; x<6; x++){
            days.append(new Day(week));
        }
        days[0]->setName("Montag");
        days[1]->setName("Dienstag");
        days[2]->setName("Mittwoch");
        days[3]->setName("Donnerstag");
        days[4]->setName("Freitag");
        days[5]->setName("Samstag");
        for(Day* day : days){
            QList<Timeslot*> timeslots;
            for(int x = 0; x<6; x++){
                timeslots.append(new Timeslot(day));
            }
            timeslots[0]->setName("Block 1");
            timeslots[1]->setName("Block 2");
            timeslots[2]->setName("Block 3");
            timeslots[3]->setName("Block 4");
            timeslots[4]->setName("Block 5");
            timeslots[5]->setName("Block 6");
            day->setTimeslots(timeslots);
        }
        week->setDays(days);
    }
}
