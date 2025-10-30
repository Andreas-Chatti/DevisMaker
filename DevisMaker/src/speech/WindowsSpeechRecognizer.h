#pragma once

#include <QObject>
#include <QString>
#include <QMutex>

// Windows headers pour SAPI (Speech API)
#include <windows.h>
#include <sapi.h>      // Speech API de Windows
#include <sphelper.h>  // Helpers pour SAPI

/**
 * @class WindowsSpeechRecognizer
 * @brief Classe de reconnaissance vocale utilisant Windows SAPI
 *
 * Cette classe encapsule la Speech API (SAPI) de Windows pour permettre
 * la reconnaissance vocale en temps réel. Elle hérite de QObject pour
 * pouvoir utiliser le système de signaux/slots de Qt.
 *
 * SAPI utilise COM (Component Object Model), un système de Windows qui
 * permet aux composants logiciels de communiquer entre eux.
 *
 * Analogie : Pense à SAPI comme un traducteur simultané qui écoute ce que
 * tu dis et le transforme en texte au fur et à mesure.
 */
class WindowsSpeechRecognizer : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructeur
     * @param parent L'objet parent Qt (pour la gestion automatique de la mémoire)
     */
    explicit WindowsSpeechRecognizer(QObject* parent = nullptr);

    /**
     * @brief Destructeur - nettoie les ressources COM
     */
    ~WindowsSpeechRecognizer();

    /**
     * @brief Démarre la reconnaissance vocale
     * @return true si le démarrage réussit, false sinon
     */
    bool startListening();

    /**
     * @brief Arrête la reconnaissance vocale
     */
    void stopListening();

    /**
     * @brief Vérifie si la reconnaissance est active
     * @return true si en cours d'écoute, false sinon
     */
    bool isListening() const;

signals:
    /**
     * @brief Signal émis quand du texte est reconnu
     * @param text Le texte reconnu par SAPI
     *
     * Ce signal est émis en temps réel à chaque fois que SAPI
     * reconnaît une phrase ou un mot. On peut le connecter à
     * un QTextEdit pour afficher progressivement le résultat.
     */
    void textRecognized(const QString& text);

    /**
     * @brief Signal émis en cas d'erreur
     * @param errorMessage Description de l'erreur
     */
    void errorOccurred(const QString& errorMessage);

    /**
     * @brief Signal émis quand la reconnaissance démarre
     */
    void listeningStarted();

    /**
     * @brief Signal émis quand la reconnaissance s'arrête
     */
    void listeningStopped();

private:
    /**
     * @brief Initialise COM (Component Object Model)
     *
     * COM est le système de Windows qui permet d'utiliser SAPI.
     * Analogie : C'est comme "allumer" le système avant de l'utiliser.
     *
     * @return true si l'initialisation réussit
     */
    bool initializeCOM();

    /**
     * @brief Initialise SAPI et crée le moteur de reconnaissance
     * @return true si l'initialisation réussit
     */
    bool initializeSAPI();

    /**
     * @brief Nettoie les ressources COM/SAPI
     */
    void cleanup();

    /**
     * @brief Méthode statique appelée par SAPI pour les événements
     *
     * Cette méthode est un "callback" : SAPI l'appelle automatiquement
     * quand il reconnaît quelque chose. C'est un peu comme une sonnette :
     * on configure SAPI pour qu'il "sonne" cette fonction à chaque reconnaissance.
     *
     * @param wParam Paramètre Windows (non utilisé)
     * @param lParam Contient le contexte de reconnaissance (notre objet)
     */
    static void CALLBACK SpeechEventCallback(WPARAM wParam, LPARAM lParam);

    /**
     * @brief Traite les événements de reconnaissance
     *
     * Cette méthode est appelée par SpeechEventCallback et fait
     * le vrai travail : extraire le texte reconnu et émettre le signal.
     */
    void handleRecognitionEvent();

private:
    // Interfaces COM pour SAPI
    ISpRecognizer* m_recognizer{ nullptr };         // Moteur de reconnaissance
    ISpRecoContext* m_recoContext{ nullptr };       // Contexte de reconnaissance
    ISpRecoGrammar* m_recoGrammar{ nullptr };       // Grammaire (règles de reconnaissance)

    // Handle Windows pour les événements
    HANDLE m_eventHandle{ nullptr };

    // État de la reconnaissance
    bool m_isListening{ false };
    bool m_comInitialized{ false };

    // Mutex pour thread-safety (protection contre les accès concurrents)
    // Analogie : Un verrou pour empêcher deux personnes de modifier
    // la même chose en même temps
    mutable QMutex m_mutex;
};
