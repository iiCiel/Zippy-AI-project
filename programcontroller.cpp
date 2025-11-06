#include "programcontroller.h"
#include <iostream>

ProgramController::ProgramController(QObject *parent)
    : QObject(parent),
    settings("cob_zippy_ai.ini", QSettings::IniFormat),
    ollama(settings.value("Ollama/URL", "http://localhost:11434").toString().toStdString(),
           settings.value("Ollama/Model", "gemma3:4b").toString().toStdString(),
           settings.value("Ollama/ContextSize", 32000).toInt(),
           settings.value("Ollama/Timeout", 120).toInt()),
    currentGenerateStatus(Error)
{
    connect(&ollama, &OllamaInterface::responseReceived, this, &ProgramController::onGenerateFinished);
    connect(&ollama, &OllamaInterface::responseFinished, this, &ProgramController::onStreamFinished);
}

/*
    Sets the URL of the Ollama server.
*/
void ProgramController::setURL(QString url)
{
    ollama.setURL(url.toStdString());
    settings.setValue("Ollama/URL", url);
}

/*
    Returns the URL of the Ollama server.
*/
QString ProgramController::getURL() const
{
    return QString::fromStdString(ollama.getURL());
}

/*
    Sets the model to use.
*/
void ProgramController::setModel(QString model)
{
    ollama.setModel(model.toStdString());
    settings.setValue("Ollama/Model", model);
}

/*
    Returns the model to use.
*/
QString ProgramController::getModel() const
{
    return QString::fromStdString(ollama.getModel());
}

/*
    Sets the context size in tokens.
*/
void ProgramController::setContextSize(int tokens)
{
    ollama.setContextSize(tokens);
    settings.setValue("Ollama/ContextSize", tokens);
}

/*
    Returns the context size in tokens.
*/
int ProgramController::getContextSize() const
{
    return ollama.getContextSize();
}

/*
    Sets the timeout in seconds.
*/
void ProgramController::setTimeout(int seconds)
{
    ollama.setTimeout(seconds);
    settings.setValue("Ollama/Timeout", seconds);
}

/*
    Returns the timeout in seconds.
*/
int ProgramController::getTimeout() const
{
    return ollama.getTimeout();
}

/*
    Pings the Ollama server and returns the status.
*/
bool ProgramController::pingOllama()
{
    return ollama.ping();
}

/*
    Returns whether the Ollama server is connected.
*/
bool ProgramController::getOllamaStatus()
{
    return ollama.isConnected();
}

/*
    Prompt the model and begin waiting on response.
*/
void ProgramController::generate(const QString& prompt)
{
    QString systemPrompt = "You are Zippy, a helpful AI assistant for the University of Akron College of Business."
                           "Help users as much as you can with the information you know about the College."
                           "If you are not sure about something, say you don't know and suggest they contact the College directly.";

    ollama.sendPrompt(systemPrompt, prompt);
}

/*
    Slot to be called when Ollama finishes generating a response.
    Decodes the output and then invokes abc2midi to convert the output to a MIDI file.
*/
void ProgramController::onGenerateFinished(QString response)
{
    // connect this in QML to get the response
    emit generateFinished(response);
}

ProgramController::GenerateStatus ProgramController::getGenerateStatus() const
{
    return currentGenerateStatus;
}
void ProgramController::onStreamFinished()
{
    // This emits the new signal for QML to hear
    emit streamFinished();
}
void ProgramController::setGenerateStatus(GenerateStatus newStatus)
{
    if (currentGenerateStatus != newStatus)
    {
        currentGenerateStatus = newStatus;
        emit generateStatusChanged();
    }

}
