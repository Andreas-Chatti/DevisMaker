#pragma once
#include <QDialog>
#include "ui_CompanyInfoDialog.h"
#include "user/user.h"

class CompanyInfoDialog : public QDialog
{
    Q_OBJECT

public:

    explicit CompanyInfoDialog(QWidget* parent = nullptr, User* const = nullptr);
    ~CompanyInfoDialog() = default;

    QString getCompanyName() const { return ui.companyNameLineEdit->text(); }
    QString getCompanyAddress() const { return ui.companyAddressTextEdit->toPlainText(); }
    QString getCompanyPhone() const { return ui.companyPhoneLineEdit->text(); }
    QString getCompanyEmail() const { return ui.companyEmailLineEdit->text(); }
    QString getCompanySiret() const { return ui.companySiretLineEdit->text(); }
    QString getManagerName() const { return ui.managerNameLineEdit->text(); }
    QString getManagerPhone() const { return ui.managerPhoneLineEdit->text(); }
    QString getManagerEmail() const { return ui.managerEmailLineEdit->text(); }
    QString getCompanyApe() const { return ui.companyApeLineEdit->text(); }
    QString getCompanyTva() const { return ui.companyTvaLineEdit->text(); }

private slots:

    void on_buttonBox_accepted();

private:

    Ui::CompanyInfoDialog ui;
    User* const m_user;

    void loadUserInfo() const;
};