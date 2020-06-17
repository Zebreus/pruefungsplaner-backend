#include "examplanner.h"

ExamPlanner::ExamPlanner(QObject *parent, Plan *plan) : QObject(parent), plan(plan), progress(0)
{

}

int ExamPlanner::getProgress()
{
    return progress;
}

void ExamPlanner::startPlanning()
{
    if(plan != nullptr){
        progress = 0;
        for(int x = 0; x<=100; x++){
            QThread::msleep(100);
            progress = x;
            if(progress%10 == 0){
                qDebug() << "Planning :" << progress << "%";
            }
            emit progressChanged(progress);
        }

        Module* module_a = plan->getModules().at(0);
        Module* module_b = plan->getModules().at(1);

        plan->getWeeks().at(0)->getDays().at(0)->getTimeslots().at(0)->addModule(module_a);
        plan->getWeeks().at(0)->getDays().at(0)->getTimeslots().at(2)->addModule(module_b);

        emit finishedPlanning(plan);
    }
}
