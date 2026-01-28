#pragma once

#include <QDialog>
#include <vector>

namespace Ui { class Payview; }

// Forward declaration of Payroll::Line from core/Payroll.h
namespace Payroll { struct Line; }

class Payview : public QDialog {
    Q_OBJECT
public:
    explicit Payview(QWidget* parent = nullptr);
    ~Payview();

private:
    Ui::Payview* ui;
    std::vector<Payroll::Line> m_lines; // store current weekly payroll
};
