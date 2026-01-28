#pragma once
#include <QString>
#include <optional>
class FileGateway {
public:
    static FileGateway& instance();
    std::optional<QString> readAll(const QString& path);
    bool writeAll(const QString& path, const QString& text);
    bool appendLine(const QString& path, const QString& line);
private:
    FileGateway() = default;
    FileGateway(const FileGateway&) = delete;
    FileGateway& operator=(const FileGateway&) = delete;
};
