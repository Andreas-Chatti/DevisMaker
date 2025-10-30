#include "WindowsSpeechRecognizer.h"
#include <QDebug>

/**
 * EXPLICATION GÉNÉRALE :
 *
 * Cette classe utilise SAPI (Speech API) de Windows pour la reconnaissance vocale.
 * SAPI fonctionne avec COM (Component Object Model), qui est un système Windows
 * pour faire communiquer des composants logiciels.
 *
 * Le flux de fonctionnement :
 * 1. Initialiser COM (comme "allumer le système")
 * 2. Créer le moteur de reconnaissance (ISpRecognizer)
 * 3. Créer un contexte de reconnaissance (ISpRecoContext)
 * 4. Activer la "dictation grammar" (reconnaissance de texte libre)
 * 5. Écouter les événements et extraire le texte reconnu
 * 6. Nettoyer proprement les ressources
 */

WindowsSpeechRecognizer::WindowsSpeechRecognizer(QObject* parent)
    : QObject(parent)
{
    // Le constructeur reste léger - l'initialisation se fait dans startListening()
    qDebug() << "WindowsSpeechRecognizer créé";
}

WindowsSpeechRecognizer::~WindowsSpeechRecognizer()
{
    // Nettoyage automatique à la destruction
    cleanup();
    qDebug() << "WindowsSpeechRecognizer détruit";
}

bool WindowsSpeechRecognizer::initializeCOM()
{
    // COM doit être initialisé une fois par thread
    // COINIT_APARTMENTTHREADED : modèle d'appartement pour l'UI
    // Analogie : C'est comme brancher l'appareil avant de l'utiliser
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
    {
        // RPC_E_CHANGED_MODE signifie que COM est déjà initialisé (pas grave)
        emit errorOccurred("Échec de l'initialisation COM");
        return false;
    }

    m_comInitialized = true;
    qDebug() << "COM initialisé avec succès";
    return true;
}

bool WindowsSpeechRecognizer::initializeSAPI()
{
    HRESULT hr;

    // Étape 1 : Créer le moteur de reconnaissance vocale
    // CoCreateInstance : fonction COM pour créer des objets
    // CLSID_SpSharedRecognizer : ID du moteur de reconnaissance partagé
    // Analogie : On "commande" le moteur de reconnaissance à Windows
    hr = CoCreateInstance(
        CLSID_SpSharedRecognizer,           // Type d'objet à créer
        nullptr,                             // Pas d'agrégation
        CLSCTX_ALL,                         // Contexte d'exécution
        IID_ISpRecognizer,                  // Interface demandée
        reinterpret_cast<void**>(&m_recognizer) // Pointeur de sortie
    );

    if (FAILED(hr))
    {
        emit errorOccurred("Impossible de créer le moteur de reconnaissance");
        return false;
    }

    qDebug() << "Moteur de reconnaissance créé";

    // Étape 2 : Créer le contexte de reconnaissance
    // Le contexte gère les événements et les résultats de reconnaissance
    // Analogie : Si le moteur est un traducteur, le contexte est son bureau de travail
    hr = m_recognizer->CreateRecoContext(&m_recoContext);
    if (FAILED(hr))
    {
        emit errorOccurred("Impossible de créer le contexte de reconnaissance");
        return false;
    }

    qDebug() << "Contexte de reconnaissance créé";

    // Étape 3 : Créer un handle d'événement Windows
    // Ce handle sera "signalé" par SAPI quand il reconnaît quelque chose
    // Analogie : C'est comme une sonnette que SAPI peut appuyer
    m_eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_eventHandle == nullptr)
    {
        emit errorOccurred("Impossible de créer l'handle d'événement");
        return false;
    }

    // Étape 4 : Configurer les événements à surveiller
    // SPFEI_RECOGNITION : nous voulons être notifiés des reconnaissances
    hr = m_recoContext->SetNotifyWin32Event();
    if (FAILED(hr))
    {
        emit errorOccurred("Impossible de configurer les notifications");
        return false;
    }

    // Spécifier quels types d'événements nous intéressent
    hr = m_recoContext->SetInterest(
        SPFEI(SPEI_RECOGNITION),  // Événement de reconnaissance complète
        SPFEI(SPEI_RECOGNITION)
    );
    if (FAILED(hr))
    {
        emit errorOccurred("Impossible de définir les intérêts d'événements");
        return false;
    }

    // Associer notre handle d'événement au contexte
    hr = m_recoContext->SetNotifyWindowMessage(
        reinterpret_cast<HWND>(m_eventHandle),
        WM_USER,
        0,
        reinterpret_cast<LPARAM>(this)  // Passer 'this' pour le callback
    );
    if (FAILED(hr))
    {
        emit errorOccurred("Impossible d'associer l'événement");
        return false;
    }

    qDebug() << "Événements configurés";

    // Étape 5 : Créer et activer la grammaire de dictée
    // La "dictation grammar" permet de reconnaître du texte libre
    // (contrairement à une grammaire fixe avec des commandes spécifiques)
    hr = m_recoContext->CreateGrammar(0, &m_recoGrammar);
    if (FAILED(hr))
    {
        emit errorOccurred("Impossible de créer la grammaire");
        return false;
    }

    // Charger la grammaire de dictée (reconnaissance de texte libre)
    hr = m_recoGrammar->LoadDictation(nullptr, SPLO_STATIC);
    if (FAILED(hr))
    {
        emit errorOccurred("Impossible de charger la grammaire de dictée");
        return false;
    }

    // Activer les règles de dictée
    hr = m_recoGrammar->SetDictationState(SPRS_ACTIVE);
    if (FAILED(hr))
    {
        emit errorOccurred("Impossible d'activer la dictée");
        return false;
    }

    qDebug() << "Grammaire de dictée activée";

    return true;
}

bool WindowsSpeechRecognizer::startListening()
{
    QMutexLocker locker(&m_mutex);

    // Vérifier si on n'écoute pas déjà
    if (m_isListening)
    {
        qWarning() << "La reconnaissance est déjà active";
        return false;
    }

    // Initialiser COM si ce n'est pas déjà fait
    if (!m_comInitialized)
    {
        if (!initializeCOM())
        {
            return false;
        }
    }

    // Initialiser SAPI
    if (!initializeSAPI())
    {
        cleanup();
        return false;
    }

    m_isListening = true;
    emit listeningStarted();
    qDebug() << "Reconnaissance vocale démarrée";

    return true;
}

void WindowsSpeechRecognizer::stopListening()
{
    QMutexLocker locker(&m_mutex);

    if (!m_isListening)
    {
        return;
    }

    // Désactiver la grammaire
    if (m_recoGrammar)
    {
        m_recoGrammar->SetDictationState(SPRS_INACTIVE);
    }

    cleanup();
    m_isListening = false;

    emit listeningStopped();
    qDebug() << "Reconnaissance vocale arrêtée";
}

bool WindowsSpeechRecognizer::isListening() const
{
    QMutexLocker locker(&m_mutex);
    return m_isListening;
}

void WindowsSpeechRecognizer::cleanup()
{
    // Nettoyer dans l'ordre inverse de création
    // Important : toujours libérer les ressources COM avec Release()

    if (m_recoGrammar)
    {
        m_recoGrammar->Release();
        m_recoGrammar = nullptr;
    }

    if (m_recoContext)
    {
        m_recoContext->Release();
        m_recoContext = nullptr;
    }

    if (m_recognizer)
    {
        m_recognizer->Release();
        m_recognizer = nullptr;
    }

    if (m_eventHandle)
    {
        CloseHandle(m_eventHandle);
        m_eventHandle = nullptr;
    }

    // Désinitialiser COM
    if (m_comInitialized)
    {
        CoUninitialize();
        m_comInitialized = false;
    }

    qDebug() << "Ressources SAPI nettoyées";
}

void CALLBACK WindowsSpeechRecognizer::SpeechEventCallback(WPARAM wParam, LPARAM lParam)
{
    // Cette fonction est appelée par Windows quand un événement SAPI se produit
    // lParam contient le pointeur vers notre objet WindowsSpeechRecognizer
    // qu'on a passé dans SetNotifyWindowMessage()

    WindowsSpeechRecognizer* recognizer = reinterpret_cast<WindowsSpeechRecognizer*>(lParam);
    if (recognizer)
    {
        recognizer->handleRecognitionEvent();
    }
}

void WindowsSpeechRecognizer::handleRecognitionEvent()
{
    if (!m_recoContext)
    {
        return;
    }

    // Récupérer l'événement de reconnaissance
    SPEVENT event;
    while (m_recoContext->GetEvents(1, &event, nullptr) == S_OK)
    {
        // Vérifier que c'est bien un événement de reconnaissance
        if (event.eEventId == SPEI_RECOGNITION)
        {
            // Obtenir le résultat de la reconnaissance
            ISpRecoResult* result = reinterpret_cast<ISpRecoResult*>(event.lParam);

            if (result)
            {
                // Extraire le texte reconnu
                LPWSTR text = nullptr;
                HRESULT hr = result->GetText(
                    SP_GETWHOLEPHRASE,  // Obtenir toute la phrase
                    SP_GETWHOLEPHRASE,
                    TRUE,               // Inclure les remplacements
                    &text,
                    nullptr
                );

                if (SUCCEEDED(hr) && text)
                {
                    // Convertir le texte Windows (LPWSTR) en QString
                    QString recognizedText = QString::fromWCharArray(text);

                    // Libérer la mémoire allouée par GetText
                    CoTaskMemFree(text);

                    // Émettre le signal avec le texte reconnu
                    qDebug() << "Texte reconnu :" << recognizedText;
                    emit textRecognized(recognizedText);
                }

                // Libérer le résultat
                result->Release();
            }
        }

        // Libérer les données de l'événement
        if (event.elParamType == SPET_LPARAM_IS_OBJECT)
        {
            IUnknown* unknown = reinterpret_cast<IUnknown*>(event.lParam);
            if (unknown)
            {
                unknown->Release();
            }
        }
    }
}
