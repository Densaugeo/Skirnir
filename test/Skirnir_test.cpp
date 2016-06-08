#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Skirnir.hpp"

TEST_CASE("Skirnir.heartbeat()") {
  SECTION("Sends \"<\\n\"") {
    HardwareSerial port = HardwareSerial();
    
    Skirnir skirnir = Skirnir(&port);
    
    skirnir.heartbeat();
    
    REQUIRE(port.outputAvailable == 2);
    REQUIRE(memcmp(port.outputBuffer, "<\n", 2) == 0);
  }
}

TEST_CASE("Skirnir.send()") {
  HardwareSerial port = HardwareSerial();
  
  Skirnir skirnir = Skirnir(&port);
  
  SECTION("Sends correct packets (1)") {
    skirnir.send((uint8_t*) "X*8MM377oca8=Mf)R5n*Ax0iOk!^N~9'[SU+mn$6mFj9z", 45);
    
    REQUIRE(port.outputAvailable == 62);
    REQUIRE(memcmp(port.outputBuffer, "#WCo4TU0zNzdvY2E4PU1mKVI1bipBeDBpT2shXk5+OSdbU1UrbW4kNm1Gajl6\n", 62) == 0);
  }
  
  SECTION("Sends correct packets (2)") {
    skirnir.send((uint8_t*) "G6]z5J+bF~g:r'C}@IvNB2pi/+TL*JirzQ.h`-eqgo/8G", 45);
    
    REQUIRE(port.outputAvailable == 62);
    REQUIRE(memcmp(port.outputBuffer, "#RzZdejVKK2JGfmc6cidDfUBJdk5CMnBpLytUTCpKaXJ6US5oYC1lcWdvLzhH\n", 62) == 0);
  }
  
  SECTION("Sends correct packets (3)") {
    skirnir.send((uint8_t*) "7tKAZUeZ*oa;</p|V,'TL|S2lKMW|9y2792bH:mK=L.8Q", 45);
    
    REQUIRE(port.outputAvailable == 62);
    REQUIRE(memcmp(port.outputBuffer, "#N3RLQVpVZVoqb2E7PC9wfFYsJ1RMfFMybEtNV3w5eTI3OTJiSDptSz1MLjhR\n", 62) == 0);
  }
  
  SECTION("Sends correct packets (4)") {
    skirnir.send((uint8_t*) "a8ufVEic):k^O$*NA$R']0<0fU/S^5(8!?v=Zf{/:c-oV", 45);
    
    REQUIRE(port.outputAvailable == 62);
    REQUIRE(memcmp(port.outputBuffer, "#YTh1ZlZFaWMpOmteTyQqTkEkUiddMDwwZlUvU141KDghP3Y9WmZ7LzpjLW9W\n", 62) == 0);
  }
  
  SECTION("Sending no data sends a packet full of zeroes") {
    skirnir.send((uint8_t*) "", 0);
    
    REQUIRE(port.outputAvailable == 62);
    REQUIRE(memcmp(port.outputBuffer, "#AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n", 62) == 0);
  }
  
  SECTION("Short payloads are padded with zeroes") {
    skirnir.send((uint8_t*) "A#u^gde5YH.^#565Ru;I[_HGPh:;p", 29);
    
    REQUIRE(port.outputAvailable == 62);
    REQUIRE(memcmp(port.outputBuffer, "#QSN1XmdkZTVZSC5eIzU2NVJ1O0lbX0hHUGg6O3AAAAAAAAAAAAAAAAAAAAAA\n", 62) == 0);
  }
  
  SECTION("Sending a 45 byte payload sends exactly the full payload") {
    skirnir.send((uint8_t*) "A#u^gde5YH.^#565Ru;I[_HGPh:;pjf7r4^!.,h[++=ei", 45);
    
    REQUIRE(port.outputAvailable == 62);
    REQUIRE(memcmp(port.outputBuffer, "#QSN1XmdkZTVZSC5eIzU2NVJ1O0lbX0hHUGg6O3BqZjdyNF4hLixoWysrPWVp\n", 62) == 0);
  }
  
  SECTION("Sending between 45 and 180 bytes sends a large packet padded to 180 bytes") {
    skirnir.send((uint8_t*) "A#u^gde5YH.^#565Ru;I[_HGPh:;pjf7r4^!.,h[++=ei h@~~;.vjsd&we3", 60);
    
    REQUIRE(port.outputAvailable == 242);
    REQUIRE(memcmp(port.outputBuffer, "&QSN1XmdkZTVZSC5eIzU2NVJ1O0lbX0hHUGg6O3BqZjdyNF4hLixoWysrPWVpIGhAfn47LnZqc2Qmd2UzAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n", 242) == 0);
  }
  
  SECTION("Sending a 180 byte payload sends exactly the full payload in a large packet") {
    skirnir.send((uint8_t*) ".^f;T6?@p{f,&^6!vZ( OW+CyWQV#1-]T4k'L!<8Skwc4a%-b&-'0)X4_96!?:>t<]):^lLTa.fh9gBxl,.H_qIt^Q{hi^Q<][Effg+b3w4ol4F6fUiKDqA./+i(l&RP!c7wdp#a%R(l_#AD)}fQL}|S@B9)oUw #U-]fCe(b%V(6:bl$zX(", 180);
    
    REQUIRE(port.outputAvailable == 242);
    REQUIRE(memcmp(port.outputBuffer, "&Ll5mO1Q2P0Bwe2YsJl42IXZaKCBPVytDeVdRViMxLV1UNGsnTCE8OFNrd2M0YSUtYiYtJzApWDRfOTYhPzo+dDxdKTpebExUYS5maDlnQnhsLC5IX3FJdF5Re2hpXlE8XVtFZmZnK2IzdzRvbDRGNmZVaUtEcUEuLytpKGwmUlAhYzd3ZHAjYSVSKGxfI0FEKX1mUUx9fFNAQjkpb1V3ICNVLV1mQ2UoYiVWKDY6Ymwkelgo\n", 242) == 0);
  }
  
  SECTION("Sending a >180 byte payload sends the first 180 bytes in a large packet") {
    skirnir.send((uint8_t*) ":!?b0];_@R2jsCPg$Onuo-VG{K]cR7fQZx##N>&EiU<h*+K'S?c5m6nCfGn2myELx'y>}<]OH6PTm>3.&Fj#]:aEN%@s^W`x`BnqDi|TH5m00_]YDd{AS0dMq9q#3K3w]w:t5_uc1u8c>--*^.>EPps?,`wIa/UKZCx$^9I#xm-aD]4YEYi[bX#-zX@r?u", 190);
    
    REQUIRE(port.outputAvailable == 242);
    REQUIRE(memcmp(port.outputBuffer, "&OiE/YjBdO19AUjJqc0NQZyRPbnVvLVZHe0tdY1I3ZlFaeCMjTj4mRWlVPGgqK0snUz9jNW02bkNmR24ybXlFTHgneT59PF1PSDZQVG0+My4mRmojXTphRU4lQHNeV2B4YEJucURpfFRINW0wMF9dWURke0FTMGRNcTlxIzNLM3dddzp0NV91YzF1OGM+LS0qXi4+RVBwcz8sYHdJYS9VS1pDeCReOUkjeG0tYURdNFlFWWlb\n", 242) == 0);
  }
}

TEST_CASE("Skirnir.receive()") {
  HardwareSerial port = HardwareSerial();
  
  Skirnir skirnir = Skirnir(&port);
  
  uint8_t payload[45];
  
  SECTION("When nothing is detected, returns zero") {
    REQUIRE(skirnir.receive(payload, '1') == 0);
    REQUIRE(skirnir.receive(payload, '2') == 0);
    REQUIRE(skirnir.receive(payload, '#') == 0);
    REQUIRE(skirnir.receive(payload, '4') == 0);
    REQUIRE(skirnir.receive(payload, '$') == 0);
    REQUIRE(skirnir.receive(payload, '6') == 0);
    REQUIRE(skirnir.receive(payload, '\n') == 0);
    REQUIRE(skirnir.receive(payload, '8') == 0);
    REQUIRE(skirnir.receive(payload, '9') == 0);
    REQUIRE(skirnir.receive(payload, '0') == 0);
  }
  
  SECTION("When a ping intermediate is detected, returns zero") {
    REQUIRE(skirnir.receive(payload, '-') == 0);
    REQUIRE(skirnir.receive(payload, '\n') == 0);
    
    REQUIRE(port.outputAvailable == 2);
    REQUIRE(memcmp(port.outputBuffer, ">\n", 2) == 0);
  }
  
  SECTION("When a packet is detected, returns 45 only on the packet's final byte") {
    REQUIRE(skirnir.receive(payload, '#') == 0);
    REQUIRE(skirnir.receive(payload, 'S') == 0);
    REQUIRE(skirnir.receive(payload, 'E') == 0);
    REQUIRE(skirnir.receive(payload, '1') == 0);
    
    port.addInput((uint8_t*) "jVXJPRW1QWHFPJGlGJzdOZlRvMUhxI0h1YFtWeTZdQ15bUVlrKHpbI", 54);
    REQUIRE(skirnir.receive_until_packet(payload) == 0);
    
    REQUIRE(skirnir.receive(payload, 'y') == 0);
    REQUIRE(skirnir.receive(payload, 'R') == 0);
    REQUIRE(skirnir.receive(payload, 'T') == 0);
    REQUIRE(skirnir.receive(payload, '\n') == 45);
    
    REQUIRE(port.outputAvailable == 0);
    REQUIRE(memcmp(payload, "HMcUrOEmPXqO$iF'7NfTo1Hq#Hu`[Vy6]C^[QYk(z[#$S", 45) == 0);
  }
  
  SECTION("When a large packet is received, do not respond") {
    REQUIRE(skirnir.receive(payload, '&') == 0);
    REQUIRE(skirnir.receive(payload, 'a') == 0);
    REQUIRE(skirnir.receive(payload, 'n') == 0);
    REQUIRE(skirnir.receive(payload, 'R') == 0);
    
    port.addInput((uint8_t*) "BeHooXVR7RG9tMltjdTdIM3JQLms7eDheaF0jVW1HX1khcnBvKU9Dd2Ey", 57);
    REQUIRE(skirnir.receive_until_packet(payload) == 0);
    
    port.addInput((uint8_t*) "SHFRLyV6On17cUF0Y2s6SSVmTmdUOkM4aic4P1FgbSV5Oy5ARSVOTWwjVml0", 60);
    REQUIRE(skirnir.receive_until_packet(payload) == 0);
    
    port.addInput((uint8_t*) "P3clMkhYbFdsI2hbaCdqJiMrTmI5X1lDUFppUnwtc1tYLTlfR0pvXTQrfTwh", 60);
    REQUIRE(skirnir.receive_until_packet(payload) == 0);
    
    port.addInput((uint8_t*) "NHxBWTxUZlN2IUsrLGc0T312Wl52WW1hbEdXUzFeaz5KTj5NbnRtQi4oW", 57);
    REQUIRE(skirnir.receive_until_packet(payload) == 0);
    
    REQUIRE(skirnir.receive(payload, 'U') == 0);
    REQUIRE(skirnir.receive(payload, 'h') == 0);
    REQUIRE(skirnir.receive(payload, 'T') == 0);
    REQUIRE(skirnir.receive(payload, '\n') == 0);
    
    REQUIRE(port.outputAvailable == 0);
  }
}

TEST_CASE("Skirnir.receive_until_packet()") {
  HardwareSerial port = HardwareSerial();
  
  Skirnir skirnir = Skirnir(&port);
  
  uint8_t payload[45];
  
  SECTION("When port is empty, no response is sent") {
    REQUIRE(skirnir.receive_until_packet(payload) == 0);
    
    REQUIRE(port.inputAvailable == 0);
    REQUIRE(port.outputAvailable == 0);
  }
  
  SECTION("When port is full of garbage, no response is sent") {
    port.addInput((uint8_t*) "7DZ<h1#K2EMRQ?QylX5E$txjT>YCLx^R1i3IB;z$w):9X~Z+xj8a3U_i)zt-zrcO", 64);
    REQUIRE(skirnir.receive_until_packet(payload) == 0);
    
    REQUIRE(port.inputAvailable == 0);
    REQUIRE(port.outputAvailable == 0);
  }
  
  SECTION("When port contains a ping intermediate, a ping response is sent") {
    port.addInput((uint8_t*) "l++j}pjjN~T.-^kf<P3JFIzPGNBz^mDYy?D-\nXX*:}33+E]AEIp)eyNVuz(U][4G", 64);
    REQUIRE(skirnir.receive_until_packet(payload) == 0);
    
    REQUIRE(port.inputAvailable == 0);
    REQUIRE(port.outputAvailable == 2);
    REQUIRE(memcmp(port.outputBuffer, ">\n", 2) == 0);
  }
  
  SECTION("When port contains multiple ping intermediates, multiple ping responses are sent") {
    port.addInput((uint8_t*) "WO>>/s-\n`R4#_CI>@:57T!MI4a_Nhy-\n~`l,Kkb=67uB##0{-\nM`9Pv8(/l4c>hI", 64);
    REQUIRE(skirnir.receive_until_packet(payload) == 0);
    
    REQUIRE(port.inputAvailable == 0);
    REQUIRE(port.outputAvailable == 6);
    REQUIRE(memcmp(port.outputBuffer, ">\n>\n>\n", 6) == 0);
  }
  
  SECTION("Ping intermediates divided across calls can be detected") {
    port.addInput((uint8_t*) "tmH#@eG-", 8);
    REQUIRE(skirnir.receive_until_packet(payload) == 0);
    
    port.addInput((uint8_t*) "\nX=Fhc+;'kM1", 12);
    REQUIRE(skirnir.receive_until_packet(payload) == 0);
    
    REQUIRE(port.inputAvailable == 0);
    REQUIRE(port.outputAvailable == 2);
    REQUIRE(memcmp(port.outputBuffer, ">\n", 2) == 0);
  }
  
  SECTION("When port holds a packet, it is received") {
    port.addInput((uint8_t*) "#MTE5PDNAZkp5IytreHlfR1ZmZzhtLD4hdT1GODw1RTtuZ1VDWXxTO1lfaXQy\n", 62);
    REQUIRE(skirnir.receive_until_packet(payload) == 45);
    
    REQUIRE(port.inputAvailable == 0);
    REQUIRE(port.outputAvailable == 0);
    REQUIRE(memcmp(payload, "119<3@fJy#+kxy_GVfg8m,>!u=F8<5E;ngUCY|S;Y_it2", 45) == 0);
  }
  
  SECTION("When a packet is detected, stop immediately") {
    port.addInput((uint8_t*) "#KyxwWkg8eFIyYyRCQ3JPOyhvRTI/IzUkI20mQXRpZiMzPGF5W2h9SVB0cTEu\nab", 64);
    REQUIRE(skirnir.receive_until_packet(payload) == 45);
    
    REQUIRE(port.inputAvailable == 2);
    REQUIRE(memcmp(port.inputBuffer, "ab", 2) == 0);
    REQUIRE(port.outputAvailable == 0);
    REQUIRE(memcmp(payload, "+,pZH<xR2c$BCrO;(oE2?#5$#m&Atif#3<ay[h}IPtq1.", 45) == 0);
  }
  
  SECTION("Packets divided across calls can be detected") {
    port.addInput((uint8_t*) "#aiYwO1MnI2htJyZZPGB4RnQreUR3", 29);
    REQUIRE(skirnir.receive_until_packet(payload) == 0);
    
    port.addInput((uint8_t*) "YicoYUlBNig5X09xW09DIyYmfkBNR3h+\n", 33);
    REQUIRE(skirnir.receive_until_packet(payload) == 45);
    
    REQUIRE(port.inputAvailable == 0);
    REQUIRE(port.outputAvailable == 0);
    REQUIRE(memcmp(payload, "j&0;S'#hm'&Y<`xFt+yDwb'(aIA6(9_Oq[OC#&&~@MGx~", 45) == 0);
  }
  
  SECTION("Packets in a stream of random bytes can be detected") {
    port.addInput((uint8_t*) "3@fJy#tmH#@+kxeG.AX=Fhc+;'kM119<#LFVNMTdPMy48cUJWJ3o/L1RAIWJXMGF", 64);
    REQUIRE(skirnir.receive_until_packet(payload) == 0);
    
    port.addInput((uint8_t*) "yOTZqdDx0Jl8kbmFYSCxWYE82QzAh\ny_GVfg8m,>!u=F8<5E;ngUCY|S;Y_it22w", 64);
    REQUIRE(skirnir.receive_until_packet(payload) == 45);
    
    REQUIRE(port.inputAvailable == 34);
    REQUIRE(memcmp(port.inputBuffer, "y_GVfg8m,>!u=F8<5E;ngUCY|S;Y_it22w", 34) == 0);
    REQUIRE(port.outputAvailable == 0);
    REQUIRE(memcmp(payload, ",UM17O3.<qBV'z?/T@!bW0ar96jt<t&_$naXH,V`O6C0!", 45) == 0);
  }
  
  SECTION("If a ping intermediate is found inside a packet, only the packet is ignored") {
    port.addInput((uint8_t*) "#YXx9dHMqXSsjeCNsZkppdns/T0tRKlA7eDtqJk-\nFvd35rUmVSe2skMXN6JHJI\n", 64);
    REQUIRE(skirnir.receive_until_packet(payload) == 0);
    
    REQUIRE(port.inputAvailable == 0);
    REQUIRE(port.outputAvailable == 2);
    REQUIRE(memcmp(port.outputBuffer, ">\n", 2) == 0);
  }
  
  SECTION("If a packet is found inside a ping intermediate, only the ping is ignored") {
    port.addInput((uint8_t*) "-#R3c6dmM/NlZSUS9+P3RPVCEnW3s5Z3RFbG8jSD98UjwmQUM+LWszcDhiZjBr\n\n", 64);
    REQUIRE(skirnir.receive_until_packet(payload) == 45);
    
    REQUIRE(port.inputAvailable == 1);
    REQUIRE(memcmp(port.inputBuffer, "\n", 1) == 0);
    REQUIRE(port.outputAvailable == 0);
    REQUIRE(memcmp(payload, "Gw:vc?6VRQ/~?tOT!'[{9gtElo#H?|R<&AC>-k3p8bf0k", 45) == 0);
  }
}
