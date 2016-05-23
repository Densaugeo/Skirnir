#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Skirnir180.hpp"

TEST_CASE("Skirnir180.send()") {
  HardwareSerial port = HardwareSerial();
  
  Skirnir180 skirnir = Skirnir180(&port);
  
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
  
  
  
  /*SECTION("Sending between 45 and 90 bytes sends a two-frame packet padded to 90 bytes") {
    skirnir.send((uint8_t*) "A#u^gde5YH.^#565Ru;I[_HGPh:;pjf7r4^!.,h[++=ei h@~~;.vjsd&we3", 60);
    
    REQUIRE(port.outputAvailable == 124);
    REQUIRE(memcmp(port.outputBuffer     , "$IGhAfn47LnZqc2Qmd2UzAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n", 62) == 0);
    REQUIRE(memcmp(port.outputBuffer + 62, "#QSN1XmdkZTVZSC5eIzU2NVJ1O0lbX0hHUGg6O3BqZjdyNF4hLixoWysrPWVp\n", 62) == 0);
  }
  
  SECTION("Sending a 90 byte payload sends exactly the full payload in a two-frame packet") {
    skirnir.send((uint8_t*) "<;B0G'O]jucQz%@E>DBXN6:V$i'%E/O9=_O2h}g;Ns9#XI`b#k*:z,!,(>,<mg^O&KZ:P+YX,3RCI.s?){?j84BE*N", 90);
    
    REQUIRE(port.outputAvailable == 124);
    REQUIRE(memcmp(port.outputBuffer     , "$SWBiI2sqOnosISwoPiw8bWdeTyZLWjpQK1lYLDNSQ0kucz8pez9qODRCRSpO\n", 62) == 0);
    REQUIRE(memcmp(port.outputBuffer + 62, "#PDtCMEcnT11qdWNReiVART5EQlhONjpWJGknJUUvTzk9X08yaH1nO05zOSNY\n", 62) == 0);
  }
  
  SECTION("Sending between 90 and 135 bytes sends a three-frame packet padded to 135 bytes") {
    skirnir.send((uint8_t*) "}sx=:nUu>+nW<5w)H$`Ng<Wnx_&BthQdpJ;BimQ{NX?#.]J(bv^tOL.J97E#t&+E1};|qb0ZYHFu9t53,e7jW=.j(THZgdDLr=/:", 100);
    
    REQUIRE(port.outputAvailable == 186);
    REQUIRE(memcmp(port.outputBuffer      , "%SFpnZERMcj0vOgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n", 62) == 0);
    REQUIRE(memcmp(port.outputBuffer +  62, "$XUooYnZedE9MLko5N0UjdCYrRTF9O3xxYjBaWUhGdTl0NTMsZTdqVz0uaihU\n", 62) == 0);
    REQUIRE(memcmp(port.outputBuffer + 124, "#fXN4PTpuVXU+K25XPDV3KUgkYE5nPFdueF8mQnRoUWRwSjtCaW1Re05YPyMu\n", 62) == 0);
  }
  
  SECTION("Sending a 135 byte payload sends exactly the full payload in a three-frame packet") {
    skirnir.send((uint8_t*) "hr2qr.3qTKD)|q7M#^Ji]W9P82ErDT@fwN'>^b9.(rXvQ3=C,F+M.*dH`>@qk `*'?f]rt,Oi+;YA``&=Y8W7/0]5F/kAQ_ADjHnfGK9dfl&v:v'`;=8$;f;&1F#]V{V7f6UO1)", 135);
    
    REQUIRE(port.outputAvailable == 186);
    REQUIRE(memcmp(port.outputBuffer      , "%L2tBUV9BRGpIbmZHSzlkZmwmdjp2J2A7PTgkO2Y7JjFGI11We1Y3ZjZVTzEp\n", 62) == 0);
    REQUIRE(memcmp(port.outputBuffer +  62, "$Mz1DLEYrTS4qZEhgPkBxayBgKic/Zl1ydCxPaSs7WUFgYCY9WThXNy8wXTVG\n", 62) == 0);
    REQUIRE(memcmp(port.outputBuffer + 124, "#aHIycXIuM3FUS0QpfHE3TSNeSmldVzlQODJFckRUQGZ3Tic+XmI5LihyWHZR\n", 62) == 0);
  }
  
  SECTION("Sending between 135 and 180 bytes sends a four-frame packet padded to 180 bytes") {
    skirnir.send((uint8_t*) "H#m;Tco e#@uq#hC]<wZawdugC@<6$&g)DM]#{3mr[ca<]H)LRf6@_#xWgru/%H;E?mg?j}+v5cka[!D2*ZFz#s[[RMX],$A.yH$;.+|.>^U's)yBv?*}}&_3`^KhXlL%e3agtO2]V<1'-UAL", 145);
    
    REQUIRE(port.outputAvailable == 248);
    REQUIRE(memcmp(port.outputBuffer      , "&Ml1WPDEnLVVBTAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n", 62) == 0);
    REQUIRE(memcmp(port.outputBuffer +  62, "%TVhdLCRBLnlIJDsuK3wuPl5VJ3MpeUJ2Pyp9fSZfM2BeS2hYbEwlZTNhZ3RP\n", 62) == 0);
    REQUIRE(memcmp(port.outputBuffer + 124, "$XUgpTFJmNkBfI3hXZ3J1LyVIO0U/bWc/an0rdjVja2FbIUQyKlpGeiNzW1tS\n", 62) == 0);
    REQUIRE(memcmp(port.outputBuffer + 186, "#SCNtO1RjbyBlI0B1cSNoQ108d1phd2R1Z0NAPDYkJmcpRE1dI3szbXJbY2E8\n", 62) == 0);
  }
  
  SECTION("Sending a 180 byte payload sends exactly the full payload in a four-frame packet") {
    skirnir.send((uint8_t*) "Z9',lo*ecyJ^9JBIjxl1>eBA^(Z&iHJXoey_kw{^TdBj@=xD?m,)jYWggOxENB&<Gr3p=lZg&/]EoHrG;?(3<#TDnwxDqRXraNsQlVbl6b`4QOc*pQta5]?G#KQ23u8OH59Ma0`n/LgNdg!:y]b,&xDORvcQi[T]?nfbTfTCG/:<?Z'pLy,>", 180);
    
    REQUIRE(port.outputAvailable == 248);
    REQUIRE(memcmp(port.outputBuffer      , "&bi9MZ05kZyE6eV1iLCZ4RE9SdmNRaVtUXT9uZmJUZlRDRy86PD9aJ3BMeSw+\n", 62) == 0);
    REQUIRE(memcmp(port.outputBuffer +  62, "%eERxUlhyYU5zUWxWYmw2YmA0UU9jKnBRdGE1XT9HI0tRMjN1OE9INTlNYTBg\n", 62) == 0);
    REQUIRE(memcmp(port.outputBuffer + 124, "$PXhEP20sKWpZV2dnT3hFTkImPEdyM3A9bFpnJi9dRW9Ickc7PygzPCNURG53\n", 62) == 0);
    REQUIRE(memcmp(port.outputBuffer + 186, "#WjknLGxvKmVjeUpeOUpCSWp4bDE+ZUJBXihaJmlISlhvZXlfa3d7XlRkQmpA\n", 62) == 0);
  }
  
  SECTION("Sending a >180 byte payload sends the first 180 bytes in a four-frame packet") {
    skirnir.send((uint8_t*) "L19v]<rTGj])e`#V.Or3qU9rc-+_,(#FCyqJ5-Y4r_^5<|;MQ)unr{3wq'.[eq2dr@%_x[ZYf9()PjZC]?QkV#$ej/UE['/3^HR-78$x?e7aY;i4 E`s>nKp=z%{@G6ie7x#bsG3YqqjJIbHJIBcL{03^:<L^T1O9c4h[WrUb${MbgKp;IhN<Bx^gh*820", 190);
    
    REQUIRE(port.outputAvailable == 248);
    REQUIRE(memcmp(port.outputBuffer      , "&M1lxcWpKSWJISklCY0x7MDNeOjxMXlQxTzljNGhbV3JVYiR7TWJnS3A7SWhO\n", 62) == 0);
    REQUIRE(memcmp(port.outputBuffer +  62, "%VUVbJy8zXkhSLTc4JHg/ZTdhWTtpNCBFYHM+bktwPXole0BHNmllN3gjYnNH\n", 62) == 0);
    REQUIRE(memcmp(port.outputBuffer + 124, "$fDtNUSl1bnJ7M3dxJy5bZXEyZHJAJV94W1pZZjkoKVBqWkNdP1FrViMkZWov\n", 62) == 0);
    REQUIRE(memcmp(port.outputBuffer + 186, "#TDE5dl08clRHal0pZWAjVi5PcjNxVTlyYy0rXywoI0ZDeXFKNS1ZNHJfXjU8\n", 62) == 0);
  }*/
  
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

TEST_CASE("Skirnir180.receive()") {
  HardwareSerial port = HardwareSerial();
  
  Skirnir180 skirnir = Skirnir180(&port);
  
  uint8_t payload[190];
  
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
  
  SECTION("When a small packet is detected, returns 45 only on the packet's final byte") {
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
  
  SECTION("When a large packet is detected, returns 180 only on the packet's final byte") {
    REQUIRE(skirnir.receive(payload, '&') == 0);
    REQUIRE(skirnir.receive(payload, 'd') == 0);
    REQUIRE(skirnir.receive(payload, 'C') == 0);
    REQUIRE(skirnir.receive(payload, 'Y') == 0);
    
    port.addInput((uint8_t*) "9KXkqQlp0UidfanVDbFFdQzowLG43OllVYH0wP1F5I15FVk92PWxdMipa", 57);
    REQUIRE(skirnir.receive_until_packet(payload) == 0);
    
    port.addInput((uint8_t*) "RGdiZ3ZFQF1eVl80R24+J2I9SFk1N3F3cVJ0a2Ugeyl7Pks4SFgyIFMyZTEz", 60);
    REQUIRE(skirnir.receive_until_packet(payload) == 0);
    
    port.addInput((uint8_t*) "KGt4RUxGTXdIOnpTd09lbiZAe1tZaSNZNDZHOn1DJi4kcyk8e289PmhdbFQs", 60);
    REQUIRE(skirnir.receive_until_packet(payload) == 0);
    
    port.addInput((uint8_t*) "JDJvclQ1VGM0KDZALmJzLjc8ajBscG49U156eU1dUyR7O1JVLXp5bFUhW", 57);
    REQUIRE(skirnir.receive_until_packet(payload) == 0);
    
    REQUIRE(skirnir.receive(payload, 'X') == 0);
    REQUIRE(skirnir.receive(payload, 'c') == 0);
    REQUIRE(skirnir.receive(payload, '5') == 0);
    REQUIRE(skirnir.receive(payload, '\n') == 180);
    
    REQUIRE(port.outputAvailable == 0);
    REQUIRE(memcmp(payload, "t&=)y*BZtR'_juClQ]C:0,n7:YU`}0?Qy#^EVOv=l]2*ZDgbgvE@]^V_4Gn>'b=HY57qwqRtke {){>K8HX2 S2e13(kxELFMwH:zSwOen&@{[Yi#Y46G:}C&.$s)<{o=>h]lT,$2orT5Tc4(6@.bs.7<j0lpn=S^zyM]S${;RU-zylU!Yw9", 180) == 0);
  }
}
