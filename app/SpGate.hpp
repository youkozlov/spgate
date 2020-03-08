#pragma once

namespace sg
{

class TagAccessor;
class SerialPort;
class FeProcessor;

enum class SpGateState
{
    init,
    idle,
    request,
    wait,
    done,
    error
};

class SpGate
{
public:
    struct Init
    {
        TagAccessor& ta;
        SerialPort& port;
        FeProcessor& fe;
    };
    
    explicit SpGate(Init const&);
    
    ~SpGate();
    
    void tickInd();

    SpGateState getState() const;

private:
    bool validateTags();
    bool initSerialPort();
    void processInit();
    void processIdle();
    void processRequest();
    void processWait();
    void processDone();
    void processError();
    void chageState(SpGateState);
    
    TagAccessor& ta;
    SerialPort& port;
    FeProcessor& fe;
    SpGateState state;
};

}