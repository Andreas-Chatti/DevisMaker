#pragma once
#include <QObject>
#include <QLineEdit>
#include <QCompleter>
#include <QStringListModel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QUrlQuery>
#include <QDebug>

class AddressCompleter : public QObject
{
    Q_OBJECT

public:

    explicit AddressCompleter(QLineEdit* lineEdit, QObject* parent = nullptr);

private slots:

    void onTextChanged();
    void handleNetworkReply(QNetworkReply* reply);

private:

    QLineEdit* m_lineEdit;
    QCompleter* m_completer;
    QStringListModel* m_model;
    QNetworkAccessManager* m_networkManager;
    QTimer* m_debounceTimer;
    QStringList m_pendingSuggestions;  // Pour stocker les suggestions combinées
};