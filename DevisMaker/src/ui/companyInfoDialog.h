#pragma once
#include <QDialog>
#include "ui_CompanyInfoDialog.h"
#include "user/user.h"

class CompanyInfoDialog : public QDialog
{
    Q_OBJECT

public:

    CompanyInfoDialog(User& user, QWidget* parent = nullptr);
    ~CompanyInfoDialog() = default;

    const QString& getCompanyName() const { return ui.companyNameLineEdit->text(); }
    const QString& getCompanyAddress() const { return ui.companyAddressTextEdit->toPlainText(); }
    const QString& getCompanyPhone() const { return ui.companyPhoneLineEdit->text(); }
    const QString& getCompanyEmail() const { return ui.companyEmailLineEdit->text(); }
    const QString& getCompanySiret() const { return ui.companySiretLineEdit->text(); }
    const QString& getManagerName() const { return ui.managerNameLineEdit->text(); }
    const QString& getManagerPhone() const { return ui.managerPhoneLineEdit->text(); }
    const QString& getManagerEmail() const { return ui.managerEmailLineEdit->text(); }
    const QString& getCompanyApe() const { return ui.companyApeLineEdit->text(); }
    const QString& getCompanyTva() const { return ui.companyTvaLineEdit->text(); }

private slots:

    void on_buttonBox_accepted();

private:

    Ui::CompanyInfoDialog ui;
    User& m_user;

    void loadUserInfo() const;
};