#include "ollamainterface.h"
#include <iostream>
#include <QEventLoop>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <qjsonarray.h>

OllamaInterface::OllamaInterface(string url, string model, int contextSize, int timeout)
    : connected(false), url(url), model(model), contextSize(contextSize), timeout(timeout)
{
    networkManager = new QNetworkAccessManager(this);
}

OllamaInterface::~OllamaInterface()
{
    requestThread.quit();
    requestThread.wait();
    delete networkManager;
}

bool OllamaInterface::ping()
{
    QUrl pingUrl(QString::fromStdString(url + "/ping"));
    QNetworkRequest request(pingUrl);
    QNetworkReply *reply = networkManager->get(request);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    bool success = (reply->error() == QNetworkReply::NoError || reply->error() == QNetworkReply::ContentNotFoundError);
    connected = success;
    emit pingFinished(success);

    if (!success)
        emit requestError(reply->errorString());

    reply->deleteLater();
    return success;
}

void OllamaInterface::sendPrompt(const QString &systemPrompt, const QString &userPrompt)
{
    if (!connected)
    {
        emit requestError("Not connected to Ollama server.");
        return;
    }

    QUrl endpoint(QString::fromStdString(url + "/api/chat"));
    QNetworkRequest request(endpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // build the chat message JSON objects
    if (!systemPrompt.isEmpty())
    {
        addMessageToHistory("system", systemPrompt);
    }
    addMessageToHistory("user", userPrompt);

    // build the final JSON object to send in the request
    QJsonObject json;
    json["model"] = QString::fromStdString(model);
    json["messages"] = messageHistory;
    json["stream"] = true;

    // send the POST request to the ollama server and wait for the reply
    QNetworkReply *reply = networkManager->post(request, QJsonDocument(json).toJson());
    connect(reply, &QNetworkReply::readyRead, this, [this, reply]() { onPromptReply(reply); });
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { reply->deleteLater(); });
}

void OllamaInterface::requestWebSearch(const QString &query, const QString &apiKey)
{
    if (!connected)
    {
        emit requestError("Not connected to Ollama server.");
        return;
    }

    QUrl endpoint("https://ollama.com/api/web_search");
    QNetworkRequest request(endpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // build api key header
    QString bearerKey = "Bearer " + apiKey;
    request.setRawHeader("Authorization", bearerKey.toUtf8());

    // build the final JSON object to send in the request
    QJsonObject json;
    json["query"] = query;

    // send the POST request to the ollama server and wait for the reply
    QNetworkReply *reply = networkManager->post(request, QJsonDocument(json).toJson());
    connect(reply, &QNetworkReply::readyRead, this, [this, reply]() { receiveWebSearch(reply); });
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { reply->deleteLater(); });
}

void OllamaInterface::onPingReply(QNetworkReply *reply)
{
    connected = (reply->error() == QNetworkReply::NoError);

    if (connected)
    {
        std::cout << "Ping successful." << std::endl;
    }
    else
    {
        std::cerr << "Ping failed: " << reply->errorString().toStdString() << std::endl;
        emit requestError(reply->errorString());
    }

    emit pingFinished(connected);
    reply->deleteLater();
}

void OllamaInterface::onPromptReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray responseData = reply->readAll();
        QString text;
        static QString totalMessage;

        // The response can contain multiple JSON objects separated by newlines
        QList<QByteArray> jsonLines = responseData.split('\n');

        for (const QByteArray &line : jsonLines)
        {
            if (line.trimmed().isEmpty())
                continue;

            QJsonParseError parseError;
            QJsonDocument jsonResponse = QJsonDocument::fromJson(line, &parseError);

            if (parseError.error != QJsonParseError::NoError || !jsonResponse.isObject())
            {
                // If not valid JSON, emit raw data for debugging
                // NOTE: THIS IS TEMPORARY, this should be handled properly
                text = QString::fromUtf8(line);
                emit responseReceived(text);
                continue;
            }

            QJsonObject obj = jsonResponse.object();

            if (obj.contains("message"))
            {
                QJsonObject messageObj = obj["message"].toObject();
                QString role = messageObj["role"].toString();
                QString content = messageObj["content"].toString();

                // Only use assistant message content
                if (role == "assistant")
                {
                    text = content;
                    totalMessage += text;
                    emit responseReceived(text);
                }
            }
            else
            {
                // If something unexpected, emit full JSON line
                // NOTE: THIS IS TEMPORARY, this should be handled properly
                text = QString::fromUtf8(line);
                emit responseReceived(text);
            }

            if (obj.contains("done") && obj["done"].toBool())
            {
                reply->deleteLater();
                addMessageToHistory("assistant", totalMessage);
                emit responseFinished();
                totalMessage.clear();
                return; // Stop processing once done is true
            }
        }
    }
    else
    {
        emit requestError(reply->errorString());
        reply->deleteLater();
    }
}

void OllamaInterface::receiveWebSearch(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        emit requestError(reply->errorString());
        reply->deleteLater();
    }

    QByteArray responseData = reply->readAll();
    // do we need to parse this and make it pretty for the model? were gonna say no for now
    QString text = QString::fromUtf8(responseData);

    // need some code here to actually send the prompt to the model
    // probably make a separate function for tool responses to the model
}

bool OllamaInterface::isConnected() const
{
    return connected;
}

void OllamaInterface::setURL(string newUrl)
{
    url = std::move(newUrl);
}

string OllamaInterface::getURL() const
{
    return url;
}

void OllamaInterface::setModel(string newModel)
{
    model = std::move(newModel);
}

string OllamaInterface::getModel() const
{
    return model;
}

void OllamaInterface::setContextSize(int tokens)
{
    contextSize = tokens;
}

int OllamaInterface::getContextSize() const
{
    return contextSize;
}

void OllamaInterface::setTimeout(int seconds)
{
    timeout = seconds;
}

int OllamaInterface::getTimeout() const
{
    return timeout;
}

void OllamaInterface::addMessageToHistory(QString role, QString content)
{
    QJsonObject message;
    message["role"] = role;
    message["content"] = content;
    messageHistory.append(message);
}
