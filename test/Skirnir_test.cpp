#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Skirnir.hpp"

#include "base64.hpp"

TEST_CASE("Skirnir.heartbeat()") {
  SECTION("Sends \"<\\n\"") {
    HardwareSerial port = HardwareSerial();
    
    Skirnir skirnir = Skirnir(&port);
    
    skirnir.heartbeat();
    
    REQUIRE(port.outputAvailable == 2);
    REQUIRE(memcmp(port.outputBuffer, "<\n", 2) == 0);
  }
}

TEST_CASE("Skirnir.send45()") {
  HardwareSerial port = HardwareSerial();
  
  Skirnir skirnir = Skirnir(&port);
  
  SECTION("Sends correct packets (1)") {
    skirnir.send45((uint8_t*) "X*8MM377oca8=Mf)R5n*Ax0iOk!^N~9'[SU+mn$6mFj9z");
    
    REQUIRE(port.outputAvailable == 62);
    REQUIRE(memcmp(port.outputBuffer, "#WCo4TU0zNzdvY2E4PU1mKVI1bipBeDBpT2shXk5+OSdbU1UrbW4kNm1Gajl6\n", 62) == 0);
  }
  
  SECTION("Sends correct packets (2)") {
    skirnir.send45((uint8_t*) "G6]z5J+bF~g:r'C}@IvNB2pi/+TL*JirzQ.h`-eqgo/8G");
    
    REQUIRE(port.outputAvailable == 62);
    REQUIRE(memcmp(port.outputBuffer, "#RzZdejVKK2JGfmc6cidDfUBJdk5CMnBpLytUTCpKaXJ6US5oYC1lcWdvLzhH\n", 62) == 0);
  }
  
  SECTION("Sends correct packets (3)") {
    skirnir.send45((uint8_t*) "7tKAZUeZ*oa;</p|V,'TL|S2lKMW|9y2792bH:mK=L.8Q");
    
    REQUIRE(port.outputAvailable == 62);
    REQUIRE(memcmp(port.outputBuffer, "#N3RLQVpVZVoqb2E7PC9wfFYsJ1RMfFMybEtNV3w5eTI3OTJiSDptSz1MLjhR\n", 62) == 0);
  }
  
  SECTION("Sends correct packets (4)") {
    skirnir.send45((uint8_t*) "a8ufVEic):k^O$*NA$R']0<0fU/S^5(8!?v=Zf{/:c-oV");
    
    REQUIRE(port.outputAvailable == 62);
    REQUIRE(memcmp(port.outputBuffer, "#YTh1ZlZFaWMpOmteTyQqTkEkUiddMDwwZlUvU141KDghP3Y9WmZ7LzpjLW9W\n", 62) == 0);
  }
}

TEST_CASE("Skirnir.send()") {
  HardwareSerial port = HardwareSerial();
  
  Skirnir skirnir = Skirnir(&port);
  
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
  
  SECTION("Sending a >45 byte payload sends the first 45 bytes") {
    skirnir.send((uint8_t*) "A#u^gde5YH.^#565Ru;I[_HGPh:;pjf7r4^!.,h[++=ei h@~~;.vjsd&we3", 60);
    
    REQUIRE(port.outputAvailable == 62);
    REQUIRE(memcmp(port.outputBuffer, "#QSN1XmdkZTVZSC5eIzU2NVJ1O0lbX0hHUGg6O3BqZjdyNF4hLixoWysrPWVp\n", 62) == 0);
  }
}

TEST_CASE("Skirnir.receive()") {
  HardwareSerial port = HardwareSerial();
  
  Skirnir skirnir = Skirnir(&port);
  
  uint8_t payload[45];
  
  SECTION("When nothing is detected, returns false") {
    REQUIRE(skirnir.receive(payload, '1') == false);
    REQUIRE(skirnir.receive(payload, '2') == false);
    REQUIRE(skirnir.receive(payload, '#') == false);
    REQUIRE(skirnir.receive(payload, '4') == false);
    REQUIRE(skirnir.receive(payload, '5') == false);
    REQUIRE(skirnir.receive(payload, '6') == false);
    REQUIRE(skirnir.receive(payload, '\n') == false);
    REQUIRE(skirnir.receive(payload, '8') == false);
    REQUIRE(skirnir.receive(payload, '9') == false);
    REQUIRE(skirnir.receive(payload, '0') == false);
  }
  
  SECTION("When a ping intermediate is detected, returns false") {
    REQUIRE(skirnir.receive(payload, '-') == false);
    REQUIRE(skirnir.receive(payload, '\n') == false);
    
    REQUIRE(port.outputAvailable == 2);
    REQUIRE(memcmp(port.outputBuffer, ">\n", 2) == 0);
  }
  
  SECTION("When a packet is detected, returns true only on the packet's final byte") {
    REQUIRE(skirnir.receive(payload, '#') == false);
    REQUIRE(skirnir.receive(payload, 'S') == false);
    REQUIRE(skirnir.receive(payload, 'E') == false);
    REQUIRE(skirnir.receive(payload, '1') == false);
    
    port.addInput((uint8_t*) "jVXJPRW1QWHFPJGlGJzdOZlRvMUhxI0h1YFtWeTZdQ15bUVlrKHpbI", 54);
    REQUIRE(skirnir.receive_until_packet(payload) == false);
    
    REQUIRE(skirnir.receive(payload, 'y') == false);
    REQUIRE(skirnir.receive(payload, 'R') == false);
    REQUIRE(skirnir.receive(payload, 'T') == false);
    REQUIRE(skirnir.receive(payload, '\n') == true);
    
    REQUIRE(port.outputAvailable == 0);
    REQUIRE(memcmp(payload, "HMcUrOEmPXqO$iF'7NfTo1Hq#Hu`[Vy6]C^[QYk(z[#$S", 45) == 0);
  }
}

TEST_CASE("Skirnir.receive_until_packet()") {
  HardwareSerial port = HardwareSerial();
  
  Skirnir skirnir = Skirnir(&port);
  
  uint8_t payload[45];
  
  SECTION("When port is empty, no response is sent") {
    REQUIRE(skirnir.receive_until_packet(payload) == false);
    
    REQUIRE(port.inputAvailable == 0);
    REQUIRE(port.outputAvailable == 0);
  }
  
  SECTION("When port is full of garbage, no response is sent") {
    port.addInput((uint8_t*) "7DZ<h1#K2EMRQ?QylX5E$txjT>YCLx^R1i3IB;z$w):9X~Z+xj8a3U_i)zt-zrcO", 64);
    REQUIRE(skirnir.receive_until_packet(payload) == false);
    
    REQUIRE(port.inputAvailable == 0);
    REQUIRE(port.outputAvailable == 0);
  }
  
  SECTION("When port contains a ping intermediate, a ping response is sent") {
    port.addInput((uint8_t*) "l++j}pjjN~T.-^kf<P3JFIzPGNBz^mDYy?D-\nXX*:}33+E]AEIp)eyNVuz(U][4G", 64);
    REQUIRE(skirnir.receive_until_packet(payload) == false);
    
    REQUIRE(port.inputAvailable == 0);
    REQUIRE(port.outputAvailable == 2);
    REQUIRE(memcmp(port.outputBuffer, ">\n", 2) == 0);
  }
  
  SECTION("When port contains multiple ping intermediates, multiple ping responses are sent") {
    port.addInput((uint8_t*) "WO>>/s-\n`R4#_CI>@:57T!MI4a_Nhy-\n~`l,Kkb=67uB##0{-\nM`9Pv8(/l4c>hI", 64);
    REQUIRE(skirnir.receive_until_packet(payload) == false);
    
    REQUIRE(port.inputAvailable == 0);
    REQUIRE(port.outputAvailable == 6);
    REQUIRE(memcmp(port.outputBuffer, ">\n>\n>\n", 6) == 0);
  }
  
  SECTION("Ping intermediates divided across calls can be detected") {
    port.addInput((uint8_t*) "tmH#@eG-", 8);
    REQUIRE(skirnir.receive_until_packet(payload) == false);
    
    port.addInput((uint8_t*) "\nX=Fhc+;'kM1", 12);
    REQUIRE(skirnir.receive_until_packet(payload) == false);
    
    REQUIRE(port.inputAvailable == 0);
    REQUIRE(port.outputAvailable == 2);
    REQUIRE(memcmp(port.outputBuffer, ">\n", 2) == 0);
  }
  
  SECTION("When port holds a packet, it is received") {
    port.addInput((uint8_t*) "#MTE5PDNAZkp5IytreHlfR1ZmZzhtLD4hdT1GODw1RTtuZ1VDWXxTO1lfaXQy\n", 62);
    REQUIRE(skirnir.receive_until_packet(payload) == true);
    
    REQUIRE(port.inputAvailable == 0);
    REQUIRE(port.outputAvailable == 0);
    REQUIRE(memcmp(payload, "119<3@fJy#+kxy_GVfg8m,>!u=F8<5E;ngUCY|S;Y_it2", 45) == 0);
  }
  
  SECTION("When a packet is detected, stop immediately") {
    port.addInput((uint8_t*) "#KyxwWkg8eFIyYyRCQ3JPOyhvRTI/IzUkI20mQXRpZiMzPGF5W2h9SVB0cTEu\nab", 64);
    REQUIRE(skirnir.receive_until_packet(payload) == true);
    
    REQUIRE(port.inputAvailable == 2);
    REQUIRE(memcmp(port.inputBuffer, "ab", 2) == 0);
    REQUIRE(port.outputAvailable == 0);
    REQUIRE(memcmp(payload, "+,pZH<xR2c$BCrO;(oE2?#5$#m&Atif#3<ay[h}IPtq1.", 45) == 0);
  }
  
  SECTION("Packets divided across calls can be detected") {
    port.addInput((uint8_t*) "#aiYwO1MnI2htJyZZPGB4RnQreUR3", 29);
    REQUIRE(skirnir.receive_until_packet(payload) == false);
    
    port.addInput((uint8_t*) "YicoYUlBNig5X09xW09DIyYmfkBNR3h+\n", 33);
    REQUIRE(skirnir.receive_until_packet(payload) == true);
    
    REQUIRE(port.inputAvailable == 0);
    REQUIRE(port.outputAvailable == 0);
    REQUIRE(memcmp(payload, "j&0;S'#hm'&Y<`xFt+yDwb'(aIA6(9_Oq[OC#&&~@MGx~", 45) == 0);
  }
  
  SECTION("Packets in a stream of random bytes can be detected") {
    port.addInput((uint8_t*) "3@fJy#tmH#@+kxeG.AX=Fhc+;'kM119<#LFVNMTdPMy48cUJWJ3o/L1RAIWJXMGF", 64);
    REQUIRE(skirnir.receive_until_packet(payload) == false);
    
    port.addInput((uint8_t*) "yOTZqdDx0Jl8kbmFYSCxWYE82QzAh\ny_GVfg8m,>!u=F8<5E;ngUCY|S;Y_it22w", 64);
    REQUIRE(skirnir.receive_until_packet(payload) == true);
    
    REQUIRE(port.inputAvailable == 34);
    REQUIRE(memcmp(port.inputBuffer, "y_GVfg8m,>!u=F8<5E;ngUCY|S;Y_it22w", 34) == 0);
    REQUIRE(port.outputAvailable == 0);
    REQUIRE(memcmp(payload, ",UM17O3.<qBV'z?/T@!bW0ar96jt<t&_$naXH,V`O6C0!", 45) == 0);
  }
  
  SECTION("If a ping intermediate is found inside a packet, only the packet is ignored") {
    port.addInput((uint8_t*) "#YXx9dHMqXSsjeCNsZkppdns/T0tRKlA7eDtqJk-\nFvd35rUmVSe2skMXN6JHJI\n", 64);
    REQUIRE(skirnir.receive_until_packet(payload) == false);
    
    REQUIRE(port.inputAvailable == 0);
    REQUIRE(port.outputAvailable == 2);
    REQUIRE(memcmp(port.outputBuffer, ">\n", 2) == 0);
  }
  
  SECTION("If a packet is found inside a ping intermediate, only the ping is ignored") {
    port.addInput((uint8_t*) "-#R3c6dmM/NlZSUS9+P3RPVCEnW3s5Z3RFbG8jSD98UjwmQUM+LWszcDhiZjBr\n\n", 64);
    REQUIRE(skirnir.receive_until_packet(payload) == true);
    
    REQUIRE(port.inputAvailable == 1);
    REQUIRE(memcmp(port.inputBuffer, "\n", 1) == 0);
    REQUIRE(port.outputAvailable == 0);
    REQUIRE(memcmp(payload, "Gw:vc?6VRQ/~?tOT!'[{9gtElo#H?|R<&AC>-k3p8bf0k", 45) == 0);
  }
}
