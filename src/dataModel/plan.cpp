#include "plan.h"

Plan::Plan(QObject *parent) : SerializableDataObject(parent)
{

}
QString Plan::name()
{
    return planName;
}

void Plan::setName(const QString &name)
{
    if (name == planName)
        return;

    planName = name;
    emit nameChanged();
}

QList<Group*> Plan::getConstraints(){
    return constraints;
}

QList<Group*> Plan::getGroups() const
{
    return groups;
}

void Plan::setConstraints(QList<Group*> constraints)
{
    if (this->constraints == constraints)
        return;

    this->constraints = constraints;
    emit constraintsChanged(this->constraints);
}
void Plan::setGroups(QList<Group*> groups)
{
    if (this->groups == groups)
        return;

    this->groups = groups;
    emit groupsChanged(this->groups);
}


QList<Module*> Plan::getModules(){
    return modules;
}

QList<Week*> Plan::getWeeks() const
{
    return this->weeks;
}

void Plan::setWeeks(QList<Week*> weeks)
{
    if (this->weeks == weeks)
        return;

    this->weeks = weeks;
    emit weeksChanged(this->weeks);
}

void Plan::addNewGroup(QString name){

      Group* gp = new Group(this);
      gp->setName("testname");

      qDebug() << weeks.first()->getDays().first()->getTimeslots().first()->getActiveGroups().size();
      QJsonObject a = weeks.first()->getDays().first()->getTimeslots().first()->toJsonObject();
      Timeslot* ts = new Timeslot(weeks.first()->getDays().first());
      ts->fromJsonObject(a);
      QJsonObject b = ts->toJsonObject();
      qDebug() << a;
      qDebug() << b;

//    Group* gp = new Group(this);
//    gp->setName(name);
//    groups.append(gp);
//    emit groupsChanged(this->groups);
}

void Plan::removeGroup(Group* gp){
    if(groups.removeAll(gp) > 0){
        for(Week* week : weeks){
            for(Day* day : week->getDays()){
                for(Timeslot* slot : day->getTimeslots()){
                    slot->removeActiveGroup(gp);
                }
            }
        }
    }
    emit groupsChanged(this->groups);
}

void Plan::addNewConstraint(QString name){
    Group* gp = new Group(this);
    gp->setName(name);
    constraints.append(gp);
    emit constraintsChanged(this->groups);
}

void Plan::removeConstraint(Group* gp){
    if(constraints.removeAll(gp) > 0){
        for(Week* week : weeks){
            for(Day* day : week->getDays()){
                for(Timeslot* slot : day->getTimeslots()){
                    slot->removeActiveGroup(gp);
                }
            }
        }
    }
    emit constraintsChanged(this->groups);
}


void Plan::fromJsonObject(const QJsonObject &content)
{
    simpleValuesFromJsonObject(content);

    QJsonArray groupsJsonArray = content.value("groups").toArray();
    groups = fromObjectJsonArray<Group>(groupsJsonArray);

    QJsonArray constraintsJsonArray = content.value("constraints").toArray();
    constraints = fromObjectJsonArray<Group>(constraintsJsonArray);

    QJsonArray modulesJsonArray = content.value("modules").toArray();
    modules = fromObjectJsonArray<Module>(modulesJsonArray);

    QJsonArray weeksJsonArray = content.value("weeks").toArray();
    weeks = fromObjectJsonArray<Week>(weeksJsonArray);
}

QJsonObject Plan::toJsonObject() const
{
    return recursiveToJsonObject();
}
