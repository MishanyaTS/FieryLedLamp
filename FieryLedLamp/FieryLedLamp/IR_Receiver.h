#include <Arduino.h>

#if USE_IR_RECEIVER

extern uint32_t IR_ON_OFF;
extern uint32_t IR_MUTE;
extern uint32_t IR_PREV;
extern uint32_t IR_NEXT;
extern uint32_t IR_CYCLE;
extern uint32_t IR_EQ;
extern uint32_t IR_TIME;
extern uint32_t IR_WEATHER;
extern uint32_t IR_VOL_DOWN;
extern uint32_t IR_VOL_UP;
extern uint32_t IR_IP;
extern uint32_t IR_BR_UP;
extern uint32_t IR_BR_DOWN;
extern uint32_t IR_SP_UP;
extern uint32_t IR_SP_DOWN;
extern uint32_t IR_SC_UP;
extern uint32_t IR_SC_DOWN;
extern uint32_t IR_FAV_ADD;
extern uint32_t IR_FAV_DEL;
extern uint32_t IR_RND;
extern uint32_t IR_DEF;
extern uint32_t IR_FOLD_PREV;
extern uint32_t IR_FOLD_NEXT;
extern uint32_t IR_1;
extern uint32_t IR_2;
extern uint32_t IR_3;
extern uint32_t IR_4;
extern uint32_t IR_5;
extern uint32_t IR_6;
extern uint32_t IR_7;
extern uint32_t IR_8;
extern uint32_t IR_9;
extern uint32_t IR_0;

void IR_LoadConfigFromFile();
bool IR_LearnStart(const char* key);
void IR_LearnGetStatusJson(String &out);

#endif  //USE_IR_RECEIVER
