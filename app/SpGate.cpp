#include "SpGate.hpp"

#include "interfaces/SerialPort.hpp"
#include "interfaces/TagAccessor.hpp"
#include "interfaces/FeProcessor.hpp"

#include "types/SerialPortSetup.hpp"
#include "types/DataRequest.hpp"
#include "types/DataRespond.hpp"
#include "types/TagType.hpp"
#include "types/AppError.hpp"

namespace sg
{

SpGate::SpGate(Init const& init)
    : ta(init.ta)
    , port(init.port)
    , fe(init.fe)
    , state(SpGateState::init)
{
}

SpGate::~SpGate()
{
}

void SpGate::tickInd()
{
    switch (state)
    {
    case SpGateState::init:
        processInit();
    return;
    case SpGateState::idle:
        processIdle();
    return;
    case SpGateState::request:
        processRequest();
    return;
    case SpGateState::wait:
        processWait();
    return;
    case SpGateState::done:
        processDone();
    return;
    case SpGateState::error:
        processError();
    return;
    }
}

bool SpGate::validateTags()
{
    bool invalid = false;
    
    invalid |= (ta.getType("spgSerialPort") != TagType::tagShort);
    invalid |= (ta.getType("spgSerialPortSpeed") != TagType::tagShort);
    invalid |= (ta.getType("spgSerialPortBits") != TagType::tagShort);
    invalid |= (ta.getType("spgSerialPortStopBits") != TagType::tagShort);
    invalid |= (ta.getType("spgSerialPortParity") != TagType::tagShort);
    invalid |= (ta.getType("spgSerialPortReady") != TagType::tagBool);
    invalid |= (ta.getType("spgSerialPortErr") != TagType::tagBool);
    invalid |= (ta.getType("spgSerialPortErrN") != TagType::tagShort);
    
    if (invalid)
    {
        ta.setShort("spgDeviceNetErrN", AppError::appTagNotFoundOrTypeMismatch);
        ta.setBool("spgDeviceNetErr", true);
    }
    
    return !invalid;
}

bool SpGate::initSerialPort()
{
    SerialPortSetup req;
    req.port     = ta.getShort("spgSerialPort");
    req.speed    = ta.getShort("spgSerialPortSpeed");
    req.bits     = ta.getShort("spgSerialPortBits");
    req.stopBits = ta.getShort("spgSerialPortStopBits");
    req.parity   = ta.getShort("spgSerialPortParity");
    int const errN = port.setup(req);
    if (errN)
    {
        ta.setBool("spgSerialPortReady", false);
        ta.setBool("spgSerialPortErr", true);
        ta.setShort("spgSerialPortErrN", errN);
        return false;
    }
    ta.setBool("spgSerialPortReady", true);
    ta.setBool("spgSerialPortErr", false);
    ta.setShort("spgSerialPortErrN", 0);
    return true;
}

void SpGate::processInit()
{
    if (!validateTags())
    {
        chageState(SpGateState::error);
        return;
    }

    if (!initSerialPort())
    {
        chageState(SpGateState::error);
        return;
    }

    chageState(SpGateState::idle);
}

void SpGate::processIdle()
{
    if (ta.getBool("spgReqExecute"))
    {
        chageState(SpGateState::request);
    }
}

void SpGate::processRequest()
{
    DataRequest req;
    req.dad          = ta.getShort("spgReqDad");
    req.sad          = ta.getShort("spgReqSad");
    req.chanId       = ta.getShort("spgReqChanId");
    req.paramId      = ta.getShort("spgReqParamId");
    req.timeout      = ta.getShort("spgReqTimeout");
    req.typeOfResult = ta.getShort("spgReqTypeOfResult");

    bool const isReady = fe.request(req);
    if (!isReady)
    {
        ta.setBool("spgRspError", true);
        ta.setShort("spgRspErrN", AppError::appInvalidRequest);
        chageState(SpGateState::done);
        return;
    }

    ta.setBool("spgRspBusy", true);
    ta.setBool("spgRspError", false);
    ta.setShort("spgRspErrN", 0);

    chageState(SpGateState::wait);
}

void SpGate::processWait()
{
    DataRespond rsp = fe.respond();
    if (rsp.done)
    {
        ta.setBool("spgRspError", rsp.error);
        ta.setShort("spgRspErrN", rsp.errorNum);
        switch (rsp.valueType)
        {
        case 0:
            ta.setBool("spgRspBool", rsp.valueBool);
        break;
        case 1:
            ta.setShort("spgRspShort", rsp.valueShort);
        break;
        case 2:
            ta.setLong("spgRspLong", rsp.valueLong);
        break;
        case 3:
            ta.setFloat("spgRspFloat", rsp.valueFloat);
        break;
        }
        chageState(SpGateState::done);
    }
}

void SpGate::processDone()
{
    ta.setBool("spgRspBusy", false);
    if (!ta.getBool("spgReqExecute"))
    {
        chageState(SpGateState::idle);
    }
}

void SpGate::processError()
{
}

void SpGate::chageState(SpGateState newSt)
{
    state = newSt;
}

SpGateState SpGate::getState() const
{
    return state;
}

}
