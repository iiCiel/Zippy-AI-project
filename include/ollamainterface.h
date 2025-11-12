#ifndef OLLAMAINTERFACE_H
#define OLLAMAINTERFACE_H

#include <QObject>
#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <string>
#include "threadworker.h"

using std::string;

// data structure that represents a single chat exchange between a user and the model or a tool and the model
// includes the prompt and the response from the model
// intended to represent the message object described in https://docs.ollama.com/api/chat
struct ChatExchange
{
    QString system; // system prompt
    QString user; // user prompt
    QString tool; // tool calls
    QString assistant; // response from the model
};

class OllamaInterface : public QObject
{
    Q_OBJECT
public:
    explicit OllamaInterface(string url, string model, int contextSize, int timeout);
    ~OllamaInterface();

    // Ping the Ollama server
    bool ping();

    // Send a prompt to the model and receive the result asynchronously
    void sendPrompt(const QString &systemPrompt, const QString &userPrompt);

    bool isConnected() const;
    void setURL(string url);
    string getURL() const;
    void setModel(string model);
    string getModel() const;
    void setContextSize(int tokens);
    int getContextSize() const;
    void setTimeout(int seconds);
    int getTimeout() const;

signals:
    void pingFinished(bool success);
    void responseReceived(const QString &response);
    void responseFinished();
    void requestError(const QString &error);

private slots:
    void onPingReply(QNetworkReply *reply);
    void onPromptReply(QNetworkReply *reply);

private:
    bool connected;
    string url;
    string model;
    int contextSize; // in tokens
    int timeout; // in seconds
    std::vector<ChatExchange> chatHistory;
    ChatExchange currentExchange;

    QNetworkAccessManager *networkManager;
    QThread requestThread;
    ThreadWorker worker;
};

#endif // OLLAMAINTERFACE_H
