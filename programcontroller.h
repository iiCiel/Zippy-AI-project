#ifndef PROGRAMCONTROLLER_H
#define PROGRAMCONTROLLER_H

#include <QObject>
#include <QString>
#include <QtQmlIntegration>
#include "ollamainterface.h"
#include <QSettings>

/*
    Serves as the interface to C++ from QML.
*/
class ProgramController : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit ProgramController(QObject *parent = nullptr);

    /*
        Sets the URL of the Ollama server.
    */
    Q_INVOKABLE void setURL(QString url);

    /*
        Returns the URL of the Ollama server.
    */
    Q_INVOKABLE QString getURL() const;

    /*
        Sets the model to use.
    */
    Q_INVOKABLE void setModel(QString model);

    /*
        Returns the model to use.
    */
    Q_INVOKABLE QString getModel() const;

    /*
        Sets the context size in tokens.
    */
    Q_INVOKABLE void setContextSize(int tokens);

    /*
        Returns the context size in tokens.
    */
    Q_INVOKABLE int getContextSize() const;

    /*
        Sets the timeout in seconds.
    */
    Q_INVOKABLE void setTimeout(int seconds);

    /*
        Returns the timeout in seconds.
    */
    Q_INVOKABLE int getTimeout() const;

    /*
        Pings the Ollama server and returns the status.
    */
    Q_INVOKABLE bool pingOllama();

    /*
        Returns whether the Ollama server is connected.
    */
    Q_INVOKABLE bool getOllamaStatus();

    enum GenerateStatus
    {
        Idle,
        Generating,
        Finished,
        Error
    };
    Q_ENUM(GenerateStatus)

    /*
        Prompt the model and begin waiting on response.
    */
    Q_INVOKABLE void generate(const QString& prompt);

    Q_INVOKABLE GenerateStatus getGenerateStatus() const;

    Q_PROPERTY(GenerateStatus generateStatus READ getGenerateStatus NOTIFY generateStatusChanged);

signals:
    /*
        Signal to be emitted when Ollama finishes generating a response to pass the response on to QML.
    */
    void generateFinished(QString response);
    void streamFinished();

    /*
        Signal to be emitted when the response from Ollama could not be parsed.
    */
    void promptParserError(QString response);

    /*
        Signal to be emitted when the generate status changes.
    */
    void generateStatusChanged();

private slots:
    /*
        Slot to be called when Ollama finishes generating a response.
        Decodes the output and then invokes abc2midi to convert the output to a MIDI file.
    */
    void onGenerateFinished(QString response);
    void onStreamFinished();

private:
    QSettings settings;
    OllamaInterface ollama;
    GenerateStatus currentGenerateStatus;

    void setGenerateStatus(GenerateStatus);
};

#endif // PROGRAMCONTROLLER_H
