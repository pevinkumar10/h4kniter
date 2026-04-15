#include "cstring"

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


uint8_t deauthPacket[26] = {
  /*  0 - 1  */ 0xC0, 0x00,                         // Frame Control (Type: Management, Subtype: Deauth)
  /*  2 - 3  */ 0x00, 0x00,                         // Duration
  /*  4 - 9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination MAC (Client)
  /* 10 - 15 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Source MAC (AP)
  /* 16 - 21 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID (AP)
  /* 22 - 23 */ 0x00, 0x00,                         // Fragment & Sequence Number
  /* 24 - 25 */ 0x07, 0x00                          // Reason Code (7: Class 3 frame received from nonassociated station)
};