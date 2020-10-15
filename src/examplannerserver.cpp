#include "examplannerserver.h"

QJsonValue ExamPlannerServer::plans = QJsonValue();

ExamPlannerServer::ExamPlannerServer(const QString& publicKey, QObject *parent) :
    QObject(parent), publicKey(publicKey), authorized(false)
{
    if(plans.isUndefined() || plans.isNull()){

        PlanCsvHelper helper("../pruefungsplaner-backend/res/");
        QSharedPointer<Plan> plan = helper.readPlan();
        plan->setName("Plan A");

        QList<Plan*> newPlans;
        newPlans.append(plan.get());

        Semester* semester = new Semester();
        semester->setName("WS 2019");
        plan->setParent(semester);
        semester->setPlans(newPlans);

        QJsonArray arr;
        QJsonValue semesterValue = semester->toJsonObject();
        arr.append(semesterValue);

        plans = QJsonValue(arr);
    }
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

bool ExamPlannerServer::login(QString token)
{
    try{
        auto verifier = jwt::verifier<jwt::default_clock,QtJsonTraits>(jwt::default_clock{})
            .with_claim("pruefungsplanerRead",jwt::basic_claim<QtJsonTraits>(QString("true")))
            .with_claim("pruefungsplanerRead",jwt::basic_claim<QtJsonTraits>(QString("true")))
            .allow_algorithm(jwt::algorithm::rs256(publicKey.toUtf8().constData(),"","",""))
            .with_audience(QJsonArray{"pruefungsplaner-backend"})
            .with_issuer("securityprovider");

        auto decodedToken = jwt::decode<QtJsonTraits>(token);

        verifier.verify(decodedToken);

        authorized = true;
        qDebug() << "User " << decodedToken.get_subject() << " logged in with valid token";
        return true;
    }catch(std::runtime_error& e){
        authorized = false;
        qDebug() << "Invalid token";
        return false;
    }
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
    connect(&examPlannerThread, &QThread::finished, [](){qDebug("Thread finished");});
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
