#ifndef MsgPublisherTypeConstraint_H
#define MsgPublisherTypeConstraint_H

#include "tclap/Constraint.h"

/**
 * A custom constraint to map strings into MsgPublisherType enums
 */
class MsgPublisherTypeConstraint : public TCLAP::Constraint<std::string> {
public:
    /**
    * Returns a description of the Constraint.
    */
    virtual std::string description() const {
        return "Validate that the pubType maps to a MsgPublisherType enum value";
    }

    /**
    * Returns the short ID for the Constraint.
    */
    virtual std::string shortID() const {
        return "MsgPublisherTypeConstraint";
    }

    /**
    * The method used to verify that the value parsed from the command
    * line meets the constraint.
    * param value - The value that will be checked.
    */
    virtual bool check(const std::string& value) const {
        if(value.compare("PAHO_MQTT") == 0)
            return true;
        if(value.compare("AMQP_PROTON") == 0)
            return true;
        if(value.compare("AMQP_CMS") == 0)
            return true;
        if(value.compare("AMQP_QPID") == 0)
            return true;
        if(value.compare("SOCKET") == 0)
            return true;
        return false;
    }

    MsgPublisherType toType(std::string& value) {
        if(value.compare("PAHO_MQTT") == 0)
            return MsgPublisherType::PAHO_MQTT;
        if(value.compare("AMQP_PROTON") == 0)
            return MsgPublisherType::AMQP_PROTON;
        if(value.compare("AMQP_CMS") == 0)
            return MsgPublisherType::AMQP_CMS;
        if(value.compare("AMQP_QPID") == 0)
            return MsgPublisherType::AMQP_QPID;
        if(value.compare("SOCKET") == 0)
            return MsgPublisherType::SOCKET;
        return MsgPublisherType::INVALID_MSG_PUB_TYPE;
    }
};

#endif
