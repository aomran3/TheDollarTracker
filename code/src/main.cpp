#include <QApplication>
#include <QStringList>
#include "ui/LoginWindow.h"
#include "ui/EmployeeWindow.h"
#include "ui/ManagerWindow.h"
#include "core/Paths.h"
#include "core/FileGateway.h"

static bool isManagerId(const QString &id) {
    auto text = FileGateway::instance().readAll(Paths::file("mg_ids.txt"));
    if (!text) return false;

    const auto lines = text->split('\n', Qt::KeepEmptyParts);
    for (QString line : lines) {
        line = line.trimmed();
        if (line.isEmpty()) continue;
        if (line == id) return true;
    }
    return false;
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // main.cpp (pattern)
    while (true) {
        LoginWindow login;
        if (login.exec() != QDialog::Accepted) return 0;
        const QString id = login.userId();

        // check manager list...
        const bool isManager = [&]{
            auto t = FileGateway::instance().readAll(Paths::file("mg_ids.txt"));
            if (!t) return false;
            for (auto line : t->split('\n')) {
                if (line.trimmed() == id) return true;
            }
            return false;
        }();

        if (isManager) {
            ManagerWindow w;
            w.exec();       // clicking "Sign Out" calls accept() -> loop restarts
        } else {
            EmployeeWindow w(id);
            w.exec();       // clicking "Sign Out" calls accept() -> loop restarts
        }
    }
    return 0;
}
