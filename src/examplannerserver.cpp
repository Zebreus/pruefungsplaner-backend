#include "examplannerserver.h"

void addTimeslots(Day* day){
    QList<Timeslot*> timeslots;

    timeslots.append(new Timeslot(day));
    timeslots.last()->setName("1");
    timeslots.append(new Timeslot(day));
    timeslots.last()->setName("2");
    timeslots.append(new Timeslot(day));
    timeslots.last()->setName("3");
    timeslots.append(new Timeslot(day));
    timeslots.last()->setName("4");
    timeslots.append(new Timeslot(day));
    timeslots.last()->setName("5");
    timeslots.append(new Timeslot(day));
    timeslots.last()->setName("6");

    day->setTimeslots(timeslots);
}

void addDays(Week* week){
    QList<Day*> days;

    days.append(new Day(week));
    days.last()->setName("Montag");
    addTimeslots(days.last());
    days.append(new Day(week));
    days.last()->setName("Dienstag");
    addTimeslots(days.last());
    days.append(new Day(week));
    days.last()->setName("Mittwoch");
    addTimeslots(days.last());
    days.append(new Day(week));
    days.last()->setName("Donnerstag");
    addTimeslots(days.last());
    days.append(new Day(week));
    days.last()->setName("Freitag");
    addTimeslots(days.last());
    days.append(new Day(week));
    days.last()->setName("Samstag");
    addTimeslots(days.last());

    week->setDays(days);
}

Plan* createPlan(QObject* parent){
    Plan* m_plan;
    m_plan = new Plan(parent);
    m_plan->setName("Plan b");

    Module* module_a = new Module(m_plan);
    module_a->setName("Module a");
    module_a->setNumber("30.1234");
    module_a->setOrigin("FBI");
    module_a->setActive(true);
    Module* module_b = new Module(m_plan);
    module_b->setName("Module b");
    module_b->setNumber("30.1235");
    module_b->setOrigin("FBI");
    module_b->setActive(true);

    m_plan->modules.append(module_a);
    m_plan->modules.append(module_b);
    //
    m_plan->modules.append(module_b);

    Group* constraint_a = new Group(m_plan);
    constraint_a->setName("Constraint a");
    Group* constraint_b = new Group(m_plan);
    constraint_b->setName("Constraint b");
    Group* constraint_c = new Group(m_plan);
    constraint_c->setName("Constraint c");
    Group* constraint_d = new Group(m_plan);
    constraint_d->setName("Constraint d");

    m_plan->constraints.append(constraint_a);
    m_plan->constraints.append(constraint_b);
    m_plan->constraints.append(constraint_c);
    m_plan->constraints.append(constraint_d);
    module_a->constraints.append(constraint_a);
    module_a->constraints.append(constraint_b);
    module_a->constraints.append(constraint_b);
    module_a->constraints.append(constraint_d);
    module_b->constraints.append(constraint_b);
    module_b->constraints.append(constraint_c);

    Group* group_a = new Group(m_plan);
    group_a->setName("group a");
    Group* group_b = new Group(m_plan);
    group_b->setName("group b");

    m_plan->groups.append(group_a);
    m_plan->groups.append(group_b);
    module_a->groups.append(group_a);
    module_a->groups.append(group_b);
    module_b->groups.append(group_a);

    Week* week_a = new Week(m_plan);
    week_a->setName("Week a");
    Week* week_b = new Week(m_plan);
    week_b->setName("Week b");
    Week* week_c = new Week(m_plan);
    week_c->setName("Week c");

    addDays(week_a);
    addDays(week_b);
    addDays(week_c);
    m_plan->weeks.append(week_a);
    m_plan->weeks.append(week_b);
    m_plan->weeks.append(week_c);

    QList<Group*> activeGroups;
    activeGroups.append(constraint_a);
    activeGroups.append(constraint_c);
    activeGroups.append(group_b);
    week_a->getDays().first()->getTimeslots().first()->setActiveGroups(activeGroups);
    week_a->getDays().last()->getTimeslots().last()->setActiveGroups(activeGroups);
    week_b->getDays().last()->getTimeslots().first()->setActiveGroups(activeGroups);
    week_c->getDays().first()->getTimeslots().last()->setActiveGroups(activeGroups);

//    QList<Module*> modules_a, modules_b, modules_c;
//    modules_a.append(module_a);
//    modules_b.append(module_b);
//    modules_c.append(module_a);
//    modules_c.append(module_b);
//    week_a->getDays().first()->getTimeslots().at(0)->setModules(modules_a);
//    week_a->getDays().first()->getTimeslots().at(1)->setModules(modules_b);
//    week_a->getDays().first()->getTimeslots().at(2)->setModules(modules_c);
//    qDebug() << modules_b.size();

    QJsonObject plan = m_plan->toJsonObject();

    Plan* plan_b = new Plan(parent);
    plan_b->fromJsonObject(plan);
    QJsonObject planb = plan_b->toJsonObject();

    return plan_b;
}

QJsonValue getSemesters(){
    Semester* semester_a = new Semester(nullptr);
    Semester* semester_b = new Semester(nullptr);
    semester_a->setName("Semester a");
    semester_b->setName("Semester b");

    QList<Plan*> plans_a;
    plans_a.append(createPlan(semester_a));
    plans_a.first()->setName("plan a");
    QList<Plan*> plans_b;
    plans_b.append(createPlan(semester_b));
    plans_b.append(createPlan(semester_b));
    plans_b.first()->setName("plan b");
    plans_b.last()->setName("plan c");

    semester_a->setPlans(plans_a);
    semester_b->setPlans(plans_b);

    QJsonValue semestera = semester_a->toJsonObject();
    QJsonValue semesterb = semester_b->toJsonObject();

    QJsonArray semesters;
    semesters.append(semestera);
    semesters.append(semesterb);

    return QJsonValue(semesters);
}

ExamPlannerServer::ExamPlannerServer(const QString& publicKey, QObject *parent) :
    QObject(parent), publicKey(publicKey), authorized(false)
{
    plans = getSemesters();
}

void ExamPlannerServer::finishedPlanning(Plan *finishedPlan)
{
    QJsonObject finPlan = finishedPlan->toJsonObject();
    plannerPlan = finPlan;
}

void ExamPlannerServer::progressChanged(int progress)
{
    plannerProgress = progress;
}

QJsonValue ExamPlannerServer::getPlans()
{
    return plans;
}

void ExamPlannerServer::setPlans(QJsonValue newplans)
{
    plans = newplans;
}

void ExamPlannerServer::startPlanning(QJsonValue plan)
{
    Plan* p = new Plan();
    p->fromJsonObject(plan.toObject());

    ExamPlanner *worker = new ExamPlanner(nullptr, p);
    worker->moveToThread(&examPlannerThread);
    connect(&examPlannerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &ExamPlanner::finishedPlanning, this, &ExamPlannerServer::finishedPlanning);
    connect(worker, &ExamPlanner::progressChanged, this, &ExamPlannerServer::progressChanged);
    QMetaObject::invokeMethod( worker, "startPlanning", Qt::QueuedConnection );
    examPlannerThread.start();

}

void ExamPlannerServer::startPlanningTest()
{
    QJsonValue plan = plans.toArray().at(0).toObject().value("plans").toArray().at(0);
    startPlanning(plan);
}

int ExamPlannerServer::getPlanningProgress()
{
    return plannerProgress;
}

QJsonValue ExamPlannerServer::getPlannedPlan()
{
    return plannerPlan;
}
