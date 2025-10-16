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

    explicit AddressCompleter(QLineEdit* lineEditChargement, QLineEdit* lineEditLivraison, QObject* parent = nullptr);
    ~AddressCompleter() = default;
    AddressCompleter(const AddressCompleter& completer) = delete;
    AddressCompleter& operator=(const AddressCompleter& completer) = delete;
    AddressCompleter(AddressCompleter&& completer) = delete;
    AddressCompleter& operator=(AddressCompleter&& completer) = delete;

    const OpenStreetMap* const getStreetMap() { return m_streetMap; }

private slots:

    void onTextChanged();
    void handleNetworkReply(QNetworkReply* reply);
    void startTimer(const QString& lineEditText);
    void onEditingFinished();
    void setCurrentModifiedLineEdit(const QString& lineEditText);

private:

    QLineEdit* m_lineEditChargement{ nullptr };
    QLineEdit* m_lineEditLivraison{ nullptr };
    QStringListModel* m_model{ new QStringListModel{this} };
    QCompleter* m_completer{ new QCompleter{m_model, this} };
    QNetworkAccessManager* m_networkManager{ new QNetworkAccessManager{this} };
    QTimer* m_debounceTimer{ new QTimer{this} };
    QStringList m_pendingSuggestions{};
    OpenStreetMap* m_streetMap{ new OpenStreetMap{ this } };
    LineEditType m_currentModifiedLineEdit{};

    static constexpr int MAX_VISIBLE_ITEMS{ 10 };
    static constexpr int TIMER_DELAY{ 400 };
    static constexpr int MIN_TEXT_LENGTH_TRIGGER{ 5 };


    void setupCompleter();
    void setupDebounceTimer();
    QNetworkRequest createRequest(const QString& queryItem);
};