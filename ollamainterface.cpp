#include "ollamainterface.h"
#include <iostream>
#include <QEventLoop>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

OllamaInterface::OllamaInterface(string url, string model)
    : connected(false), url(url), model(model)
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

    QUrl endpoint(QString::fromStdString(url + "/api/generate"));
    QNetworkRequest request(endpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["model"] = QString::fromStdString(model);
    json["system"] = systemPrompt;
    json["prompt"] = userPrompt;
    json["stream"] = true;  // Can be set to true for streaming responses

    QNetworkReply *reply = networkManager->post(request, QJsonDocument(json).toJson());
    connect(reply, &QNetworkReply::readyRead, this, [this, reply]() { onPromptReply(reply); });
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
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QString text;

        if (jsonResponse.isObject())
        {
            QJsonObject obj = jsonResponse.object();
            if (obj.contains("response"))
                text = obj["response"].toString();
            else
                text = QString::fromUtf8(responseData);

            if (obj.contains("done"))
            {
                bool done = obj["done"].toBool();
                if (done)
                {
                    reply->deleteLater();
                    emit responseReceived(text);
                    emit responseFinished();
                    return; // Finished receiving response
                }
            }
        }
        else
        {
            text = QString::fromUtf8(responseData);
        }

        emit responseReceived(text);
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
