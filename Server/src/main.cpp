#include "Server.hpp"
#include <QCoreApplication>
#include <QLockFile>
#include <QDir>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // For single instance app
    QLockFile lockFile(QDir::temp().absoluteFilePath("RyutpRPSServer.lock"));
    if (!lockFile.tryLock(100)) {
        qDebug() << "The RPSServer is already running";
        return -1;
    }

    RPS::Server server("127.0.0.1", 46789);
    qDebug() << "RPSServer started at" << "127.0.0.1" << 46789;
    return app.exec();
}
