#include "FileGateway.h"
#include <QFile>
#include <QTextStream>
FileGateway& FileGateway::instance(){ static FileGateway g; return g; }
std::optional<QString> FileGateway::readAll(const QString& path){
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return std::nullopt;
    QTextStream in(&f); return in.readAll();
}
bool FileGateway::writeAll(const QString& path, const QString& text){
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&f); out << text; return true;
}
bool FileGateway::appendLine(const QString& path, const QString& line){
    QFile f(path);
    if (!f.open(QIODevice::Append | QIODevice::Text)) return false;
    QTextStream out(&f); out << line << "\n"; return true;
}
