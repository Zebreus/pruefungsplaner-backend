#include <QCoreApplication>
#include "server.h"
#include "src/examplannerserver.h"
#include "security-provider/client.h"

jsonrpc::Server<ExamPlannerServer> server(9093);
QString publicKey;

void startServerWithKey(const QString& key){
    publicKey = key;
    qDebug() << "Got Public key from securityProvider";
    qDebug() << "Starting server";
    server.setConstructorArguments(publicKey);
    server.startListening();
}


void providerError(){
    qDebug() << "Error retrieving public key from security provider";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    securityprovider::Client client;
    QObject::connect(&client, &securityprovider::Client::gotPublicKey, startServerWithKey);
    QObject::connect(&client, &securityprovider::Client::error, providerError);
    QObject::connect(&client, &securityprovider::Client::socketError, providerError);
    client.open(QUrl("ws://localhost:9092"));
    client.getPublicKey();
    return a.exec();
}
