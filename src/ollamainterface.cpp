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
        QJsonObject systemMessage;
        systemMessage["role"] = "system";
        systemMessage["content"] = systemPrompt;
        messageHistory.append(systemMessage);
    }
    QJsonObject userMessage;
    userMessage["role"] = "user";
    userMessage["content"] = userPrompt;
    messageHistory.append(userMessage);

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
                emit responseFinished();
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
