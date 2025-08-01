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
#include "streetmap/streetMap.h"

class AddressCompleter : public QObject
{
    Q_OBJECT

public:

    enum class LineEditType
    {
        chargement,
        livraison,
    };

    explicit AddressCompleter(QLineEdit* lineEdit, QLineEdit* lineEditLivraison, QObject* parent = nullptr);

    const OpenStreetMap* const getStreetMap() { return m_streetMap; }

private slots:

    void onTextChanged();
    void handleNetworkReply(QNetworkReply* reply);
    void startTimer(const QString& lineEditText);
    void onEditingFinished();
    void setCurrentModifiedLineEdit(const QString& lineEditText);

private:

    QLineEdit* m_lineEditChargement;
    QLineEdit* m_lineEditLivraison;
    QCompleter* m_completer;
    QStringListModel* m_model;
    QNetworkAccessManager* m_networkManager;
    QTimer* m_debounceTimer;
    QStringList m_pendingSuggestions;
    OpenStreetMap* m_streetMap;
    LineEditType m_currentModifiedLineEdit;

    const int MAX_VISIBLE_ITEMS{ 10 };
    const int TIMER_DELAY{ 400 };
    const int MIN_TEXT_LENGTH_TRIGGER{ 5 };


    void setupCompleter();
    void setupDebounceTimer();
    QNetworkRequest createRequest(const QString& queryItem);
};