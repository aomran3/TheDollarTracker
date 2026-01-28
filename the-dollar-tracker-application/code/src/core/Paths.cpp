#include "Paths.h"
#include <QDir>
#include <QProcessEnvironment>
namespace Paths {
static QString resolveDataDir(){
    const QString envDir = QProcessEnvironment::systemEnvironment().value("DOLLAR_DATA_DIR");
    if (!envDir.isEmpty()){
        QDir d(envDir);
        if (!d.exists()) d.mkpath(".");
        return d.absolutePath();
    }
    QDir d(QDir::homePath() + "/Desktop/dollar_files");
    if (!d.exists()) d.mkpath(".");
    return d.absolutePath();
}
QString dataDir(){ static const QString dd = resolveDataDir(); return dd; }
QString file(const QString& name){ return QDir(dataDir()).filePath(name); }
}
