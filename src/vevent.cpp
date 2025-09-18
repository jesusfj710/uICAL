/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include <cctype>
#include "uICAL/types.h"
#include "uICAL/util.h"
#include "uICAL/datetime.h"
#include "uICAL/rrule.h"
#include "uICAL/vevent.h"
#include "uICAL/vline.h"
#include "uICAL/vobject.h"

namespace uICAL {
    VEvent::VEvent(const VObject_ptr& obj, const TZMap_ptr& tzmap) {

        VLine_ptr dtStart = obj->getPropertyByName("DTSTART");
        VLine_ptr dtEnd = obj->getPropertyByName("DTEND");
        VLine_ptr rRule = obj->getPropertyByName("RRULE");
        VLine_ptr summary = obj->getPropertyByName("SUMMARY");

        auto isDateOnly = [](const VLine_ptr& line) {
            if (line == nullptr) {
                return false;
            }

            string valueParam = line->getParam("VALUE");
            if (!valueParam.empty()) {
                string upper = valueParam;
                std::transform(upper.begin(), upper.end(), upper.begin(), [](unsigned char ch) {
                    return (char)std::toupper(ch);
                });
                if (upper == "DATE") {
                    return true;
                }
            }

            const string& value = line->value;
            return value.find("T") == string::npos && value.length() == 8;
        };

        auto buildDateTimeValue = [&](const VLine_ptr& line, const string& fallbackTz) {
            if (line == nullptr) {
                return string::none();
            }

            string value = line->value;
            if (isDateOnly(line) && value.length() == 8) {
                value += "T000000";
            }

            string tz = line->getParam("TZID");
            if (tz.empty()) {
                tz = fallbackTz;
            }

            if (!tz.empty()) {
                value += tz;
            }

            return value;
        };

        string dtStartValue = buildDateTimeValue(dtStart, string());
        string dtStartTz = dtStart != nullptr ? dtStart->getParam("TZID") : string();
        string dtEndValue = buildDateTimeValue(dtEnd, dtStartTz);

        this->start = DateTime(dtStartValue, tzmap);
        if (!dtEndValue.empty()) {
            this->end = DateTime(dtEndValue, tzmap);
        }
        else {
            this->end = this->start;
        }

        this->summary = summary->value;

        if (rRule == nullptr) {
            this->rrule = new_ptr<RRule>(string::none(), this->start);
        } else {
            this->rrule = new_ptr<RRule>(rRule->value, this->start);
        }
    }

    void VEvent::str(ostream& out) const {
        out << "VEVENT: " << this->summary << uICAL::endl;
        out << " - start: " << this->start << uICAL::endl;
        out << " - end: " << this->end << uICAL::endl;
        out << " - rrule: " << this->rrule << uICAL::endl;
    }
}
