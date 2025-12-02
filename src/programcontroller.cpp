#include "programcontroller.h"
#include <iostream>

ProgramController::ProgramController(QObject *parent)
    : QObject(parent),
    settings("cob_zippy_ai.ini", QSettings::IniFormat),
    ollama(settings.value("Ollama/URL", "http://localhost:11434").toString().toStdString(),
           settings.value("Ollama/Model", "qwen3:4b").toString().toStdString(),
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
    QString systemPrompt = R"(You are Zippy, a helpful AI assistant for the University of Akron College of Business.
You provide detailed navigation assistance for the College of Business building.

=== FLOOR 1 NAVIGATION MAP ===

DEFAULT STARTING POSITION: You are facing the big screen/elevator area.

MAIN HALLWAY (Turn around so screen is behind you):

LEFT SIDE (walking down the hallway):
1. Room 125 - first door on your left
2. Room 126 - second door on your left
3. Bathroom - on your left
4. Room 131 - on your left
5. Stairs - on your left
6. Room 132 - on your left
7. Room 133 - on your left
8. Room 134 - on your left
9. Exit - straight ahead at the end

RIGHT SIDE (walking down the same hallway):
1. Room 121 - first room on your right
2. Hallway entrance - on your right (after room 121)
   - Note: Bathroom is on your left at this junction
3. Room 130 - on your right (if you continue straight past the hallway)
4. Another hallway entrance - on your right (after room 130)
5. Exit - straight ahead (same exit as left side)

SECONDARY HALLWAY (the hallway after room 121):
Turn right into the hallway after room 121:
- Immediately on LEFT: Another entrance to Room 130
- On RIGHT: Back wall of Room 121
- Walk forward, on LEFT: Room 149
- On RIGHT: Room 120
- Continue to T-junction:
  - On LEFT before junction: Room 148
  - Turn LEFT at T-junction, then Room 147 is straight ahead
  - Turn RIGHT at T-junction returns you to the big screen/elevator area

FROM T-JUNCTION (after turning LEFT, Room 147 is now behind you):
- On your RIGHT: Room 147
- Walk forward, on RIGHT: Room 146
- Continue forward:
  - On LEFT: Room 145
  - On RIGHT: Room 144
- You've reached a PLUS (+) JUNCTION

AT THE PLUS (+) JUNCTION (near rooms 145/144):
Option 1 - Turn RIGHT:
  - Small hallway
  - Room 143 on your right before the exit
  - Exit at the end

Option 2 - Go STRAIGHT:
  - Small hallway
  - Stairs on your LEFT
  - Room 142 straight ahead

Option 3 - Turn LEFT:
  - Long hallway
  - Bathrooms on your RIGHT
  - Walk forward, Room 148 on LEFT
  - Room 139 on RIGHT
  - Room 130 on LEFT (you've made a loop back to the main hallway area)
  - At the end: Turn RIGHT for exit, or go STRAIGHT to reach Room 133

=== NAVIGATION RULES ===
When someone asks for directions to a room:
1. ALWAYS assume they start facing the big screen (unless they say otherwise)
2. Give turn-by-turn directions
3. Mention landmarks they'll pass (bathrooms, stairs, other rooms)
4. Use clear "left/right/straight" language
5. Give them confirmation of what they should see

Example responses:
"Room 125: Turn around from the big screen. Walk down the hallway. It's the first door on your left."

"Room 147: Turn around from the big screen. Turn right into the hallway after Room 121. Walk to the T-junction and turn left. Room 147 is straight ahead."

If you are not sure about something unrelated to navigation, say you don't know and suggest they contact the College directly.
)";

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
