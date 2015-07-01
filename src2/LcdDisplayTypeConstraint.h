#ifndef NATIVESCANNER_LCDDISPLAYTYPECONSTRAINT_H
#define NATIVESCANNER_LCDDISPLAYTYPECONSTRAINT_H

#include "tclap/Constraint.h"
#include "../lcd/AbstractLcdDisplay.h"

/**
 * A custom constraint to map strings into MsgPublisherType enums
 */
class LcdDisplayTypeConstraint : public TCLAP::Constraint<std::string> {
public:
    /**
    * Returns a description of the Constraint.
    */
    virtual std::string description() const {
        return "Validate that the lcdType maps to a LcdDisplayType enum value";
    }

    /**
    * Returns the short ID for the Constraint.
    */
    virtual std::string shortID() const {
        return "LcdDisplayTypeConstraint";
    }

    /**
    * The method used to verify that the value parsed from the command line meets the constraint.
    * param value - The value that will be checked.
    */
    virtual bool check(const std::string& value) const {
        if(value.compare("HD44780U") == 0)
            return true;
        if(value.compare("PCD8544") == 0)
            return true;
        return false;
    }

    LcdDisplayType toType(std::string& value) {
        if(value.compare("HD44780U") == 0)
            return LcdDisplayType::HD44780U;
        if(value.compare("PCD8544") == 0)
            return LcdDisplayType::PCD8544;
        return LcdDisplayType::INVALID_LCD_TYPE;
    }
};

#endif //NATIVESCANNER_LCDDISPLAYTYPECONSTRAINT_H
