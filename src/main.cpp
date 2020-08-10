#include <QCoreApplication>
#include "server.h"
#include "src/examplannerserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    JsonRpcServer<ExamPlannerServer> examPlannerServer(56730);
    examPlannerServer.startListening();

    return a.exec();
}
