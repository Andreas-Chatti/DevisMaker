#include "companyInfoDialog.h"

CompanyInfoDialog::CompanyInfoDialog(QWidget* parent, User* const user)
	: QDialog(parent)
	, m_user(user)
{
	ui.setupUi(this);

	setWindowTitle("Informations Entreprise");
	setModal(true);

    loadUserInfo();
}


void CompanyInfoDialog::on_buttonBox_accepted()
{
    m_user->setCompanyName(getCompanyName());
    m_user->setCompanyAddress(getCompanyAddress());
    m_user->setCompanyPhoneNumber(getCompanyPhone());
    m_user->setCompanyMail(getCompanyEmail());
    m_user->setSiretNumber(getCompanySiret());
    m_user->setApeNumber(getCompanyApe());
    m_user->setTvaNumber(getCompanyTva());
    m_user->setUserName(getManagerName());
    m_user->setUserPhoneNumber(getManagerPhone());
    m_user->setUserMail(getManagerEmail());

    m_user->saveUserFile();
}


void CompanyInfoDialog::loadUserInfo() const
{
    ui.companyNameLineEdit->setText(m_user->getCompanyName());
    ui.companyAddressTextEdit->setPlainText(m_user->getCompanyAddress());
    ui.companyPhoneLineEdit->setText(m_user->getCompanyPhoneNumber());
    ui.companyEmailLineEdit->setText(m_user->getCompanyMail());
    ui.companySiretLineEdit->setText(m_user->getSiretNumber());
    ui.companyApeLineEdit->setText(m_user->getApeNumber());
    ui.companyTvaLineEdit->setText(m_user->getTvaNumber());
    ui.managerNameLineEdit->setText(m_user->getUserName());
    ui.managerPhoneLineEdit->setText(m_user->getUserPhoneNumber());
    ui.managerEmailLineEdit->setText(m_user->getUserMail());
}