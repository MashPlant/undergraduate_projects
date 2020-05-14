//
// Created by mashplant on 18-9-6.
//

#include "IPDialog.hpp"
#include "ui_IPDialog.h"

IPDialog::IPDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::IPDialog),
        ip_validator(QRegExp("(25[0-5]|2[0-4]\\d|[0-1]\\d{2}|[1-9]?\\d)\\.(25[0-5]|2[0-4]\\d|[0-1]\\d{2}|[1-9]?\\d)\\."
                             "(25[0-5]|2[0-4]\\d|[0-1]\\d{2}|[1-9]?\\d)\\.(25[0-5]|2[0-4]\\d|[0-1]\\d{2}|[1-9]?\\d)")),
        port_validator(1024, 49151)
{
    ui->setupUi(this);
    ui->editIP->setValidator(&ip_validator);
    ui->editPort->setValidator(&port_validator);
    connect(ui->editIP, &QLineEdit::textChanged,
            [this] { ip = ui->editIP->text(); });
    connect(ui->editPort, &QLineEdit::textChanged,
            [this] { port = ui->editPort->text(); });
    ui->editPort->setText("8080");
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &IPDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &IPDialog::reject);
}

IPDialog::~IPDialog()
{
    delete ui;
}

std::tuple<QString, int, bool> IPDialog::get_result()
{
    int _;
    bool ok = exec() == Accepted && ip_validator.validate(ip, _) == QValidator::Acceptable &&
              port_validator.validate(port, _) == QValidator::Acceptable;
    return std::make_tuple(ip, port.toInt(), ok);
}
