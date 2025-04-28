// This file can be included several times.

#ifndef MACRO_DETECTION_EVENT
#error "The event macro must be defined"
#define MACRO_DETECTION_EVENT(EnumName, StrName, Desc) ;
#endif

MACRO_DETECTION_EVENT(BOB_DE_SAMPLE, "sample", "this is just a example event")
MACRO_DETECTION_EVENT(BOB_DE_SELFREPORT, "self report", "the player did a self report in chat")

//
// Add more events for forks below this comment to avoid merge conflicts.
//
