/*
 * IRremote - Minimal NEC-only version for memory optimization
 * Only NEC protocol decoding, no sending capability
 */

#include "IRremote.h"
#include "IRremoteInt.h"
#include <avr/interrupt.h>

volatile irparams_t irparams;

// Inline MATCH macros for efficiency
#define MATCH(m, d) ((m) >= TICKS_LOW(d) && (m) <= TICKS_HIGH(d))
#define MATCH_MARK(m, d) MATCH(m, (d) + MARK_EXCESS)
#define MATCH_SPACE(m, d) MATCH(m, (d) - MARK_EXCESS)

IRrecv::IRrecv(int recvpin) {
  irparams.recvpin = recvpin;
  irparams.blinkflag = 0;
}

void IRrecv::enableIRIn() {
  cli();
  TIMER_CONFIG_NORMAL();
  TIMER_ENABLE_INTR;
  TIMER_RESET;
  sei();
  irparams.rcvstate = STATE_IDLE;
  irparams.rawlen = 0;
  pinMode(irparams.recvpin, INPUT);
}

void IRrecv::blink13(int blinkflag) {
  irparams.blinkflag = blinkflag;
  if (blinkflag) pinMode(BLINKLED, OUTPUT);
}

// Timer ISR - collect raw IR data
ISR(TIMER_INTR_NAME) {
  TIMER_RESET;
  uint8_t irdata = (uint8_t)digitalRead(irparams.recvpin);
  irparams.timer++;

  if (irparams.rawlen >= RAWBUF) {
    irparams.rcvstate = STATE_STOP;
  }

  switch(irparams.rcvstate) {
  case STATE_IDLE:
    if (irdata == MARK) {
      if (irparams.timer < GAP_TICKS) {
        irparams.timer = 0;
      } else {
        irparams.rawlen = 0;
        irparams.rawbuf[irparams.rawlen++] = irparams.timer;
        irparams.timer = 0;
        irparams.rcvstate = STATE_MARK;
      }
    }
    break;
  case STATE_MARK:
    if (irdata == SPACE) {
      irparams.rawbuf[irparams.rawlen++] = irparams.timer;
      irparams.timer = 0;
      irparams.rcvstate = STATE_SPACE;
    }
    break;
  case STATE_SPACE:
    if (irdata == MARK) {
      irparams.rawbuf[irparams.rawlen++] = irparams.timer;
      irparams.timer = 0;
      irparams.rcvstate = STATE_MARK;
    } else if (irparams.timer > GAP_TICKS) {
      irparams.rcvstate = STATE_STOP;
    }
    break;
  case STATE_STOP:
    if (irdata == MARK) irparams.timer = 0;
    break;
  }

  if (irparams.blinkflag) {
    if (irdata == MARK) BLINKLED_ON(); else BLINKLED_OFF();
  }
}

void IRrecv::resume() {
  irparams.rcvstate = STATE_IDLE;
  irparams.rawlen = 0;
}

// Decode - NEC only
int IRrecv::decode(decode_results *results) {
  results->rawbuf = irparams.rawbuf;
  results->rawlen = irparams.rawlen;
  if (irparams.rcvstate != STATE_STOP) return ERR;
  if (decodeNEC(results)) return DECODED;
  resume();
  return ERR;
}

// NEC decoder
long IRrecv::decodeNEC(decode_results *results) {
  long data = 0;
  int offset = 1;

  if (!MATCH_MARK(results->rawbuf[offset], NEC_HDR_MARK)) return ERR;
  offset++;

  // Check for repeat
  if (irparams.rawlen == 4 &&
      MATCH_SPACE(results->rawbuf[offset], NEC_RPT_SPACE) &&
      MATCH_MARK(results->rawbuf[offset+1], NEC_BIT_MARK)) {
    results->bits = 0;
    results->value = REPEAT;
    results->decode_type = NEC;
    return DECODED;
  }

  if (irparams.rawlen < 2 * NEC_BITS + 4) return ERR;
  if (!MATCH_SPACE(results->rawbuf[offset], NEC_HDR_SPACE)) return ERR;
  offset++;

  for (int i = 0; i < NEC_BITS; i++) {
    if (!MATCH_MARK(results->rawbuf[offset], NEC_BIT_MARK)) return ERR;
    offset++;
    if (MATCH_SPACE(results->rawbuf[offset], NEC_ONE_SPACE)) {
      data = (data << 1) | 1;
    } else if (MATCH_SPACE(results->rawbuf[offset], NEC_ZERO_SPACE)) {
      data <<= 1;
    } else {
      return ERR;
    }
    offset++;
  }

  results->bits = NEC_BITS;
  results->value = data;
  results->decode_type = NEC;
  return DECODED;
}
