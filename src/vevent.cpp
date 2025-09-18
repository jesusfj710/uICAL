/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
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

        string dtStartValue = dtStart->value;
        if (dtStart->getParam("VALUE") == "DATE") {
            dtStartValue += "T000000";
        }
        string dtStartTz = dtStart->getParam("TZID");

        string dtEndValue = dtEnd->value;
        if (dtEnd->getParam("VALUE") == "DATE") {
            dtEndValue += "T000000";
        }
        string dtEndTz = dtEnd->getParam("TZID");
        if (dtEndTz.empty()) {
            dtEndTz = dtStartTz;
        }

        this->start = DateTime(dtStartValue + dtStartTz, tzmap);
        this->end = DateTime(dtEndValue + dtEndTz, tzmap);

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
