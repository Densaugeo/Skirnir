#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Skirnir180.hpp"

TEST_CASE("Skirnir180.receive()") {
  HardwareSerial port = HardwareSerial();
  
  Skirnir180 skirnir = Skirnir180(&port);
  
  SECTION("When nothing is detected, returns zero") {
    REQUIRE(skirnir.receive('1') == 0);
    REQUIRE(skirnir.receive('2') == 0);
    REQUIRE(skirnir.receive('#') == 0);
    REQUIRE(skirnir.receive('4') == 0);
    REQUIRE(skirnir.receive('$') == 0);
    REQUIRE(skirnir.receive('6') == 0);
    REQUIRE(skirnir.receive('\n') == 0);
    REQUIRE(skirnir.receive('8') == 0);
    REQUIRE(skirnir.receive('9') == 0);
    REQUIRE(skirnir.receive('0') == 0);
  }
  
  SECTION("When a ping intermediate is detected, returns zero") {
    REQUIRE(skirnir.receive('-') == 0);
    REQUIRE(skirnir.receive('\n') == 0);
    
    REQUIRE(port.outputAvailable == 2);
    REQUIRE(memcmp(port.outputBuffer, ">\n", 2) == 0);
  }
  
  SECTION("When a small packet is detected, returns 45 only on the packet's final byte") {
    REQUIRE(skirnir.receive('#') == 0);
    REQUIRE(skirnir.receive('S') == 0);
    REQUIRE(skirnir.receive('E') == 0);
    REQUIRE(skirnir.receive('1') == 0);
    
    port.addInput((uint8_t*) "jVXJPRW1QWHFPJGlGJzdOZlRvMUhxI0h1YFtWeTZdQ15bUVlrKHpbI", 54);
    REQUIRE(skirnir.receiveUntilPacket() == 0);
    
    REQUIRE(skirnir.receive('y') == 0);
    REQUIRE(skirnir.receive('R') == 0);
    REQUIRE(skirnir.receive('T') == 0);
    REQUIRE(skirnir.receive('\n') == 45);
    
    REQUIRE(port.outputAvailable == 0);
    REQUIRE(memcmp(skirnir.receiveBuffer, "HMcUrOEmPXqO$iF'7NfTo1Hq#Hu`[Vy6]C^[QYk(z[#$S", 45) == 0);
  }
  
  SECTION("When a large packet is detected, returns 180 only on the packet's final byte") {
    REQUIRE(skirnir.receive('&') == 0);
    REQUIRE(skirnir.receive('d') == 0);
    REQUIRE(skirnir.receive('C') == 0);
    REQUIRE(skirnir.receive('Y') == 0);
    
    port.addInput((uint8_t*) "9KXkqQlp0UidfanVDbFFdQzowLG43OllVYH0wP1F5I15FVk92PWxdMipa", 57);
    REQUIRE(skirnir.receiveUntilPacket() == 0);
    
    port.addInput((uint8_t*) "RGdiZ3ZFQF1eVl80R24+J2I9SFk1N3F3cVJ0a2Ugeyl7Pks4SFgyIFMyZTEz", 60);
    REQUIRE(skirnir.receiveUntilPacket() == 0);
    
    port.addInput((uint8_t*) "KGt4RUxGTXdIOnpTd09lbiZAe1tZaSNZNDZHOn1DJi4kcyk8e289PmhdbFQs", 60);
    REQUIRE(skirnir.receiveUntilPacket() == 0);
    
    port.addInput((uint8_t*) "JDJvclQ1VGM0KDZALmJzLjc8ajBscG49U156eU1dUyR7O1JVLXp5bFUhW", 57);
    REQUIRE(skirnir.receiveUntilPacket() == 0);
    
    REQUIRE(skirnir.receive('X') == 0);
    REQUIRE(skirnir.receive('c') == 0);
    REQUIRE(skirnir.receive('5') == 0);
    REQUIRE(skirnir.receive('\n') == 180);
    
    REQUIRE(port.outputAvailable == 0);
    REQUIRE(memcmp(skirnir.receiveBuffer, "t&=)y*BZtR'_juClQ]C:0,n7:YU`}0?Qy#^EVOv=l]2*ZDgbgvE@]^V_4Gn>'b=HY57qwqRtke {){>K8HX2 S2e13(kxELFMwH:zSwOen&@{[Yi#Y46G:}C&.$s)<{o=>h]lT,$2orT5Tc4(6@.bs.7<j0lpn=S^zyM]S${;RU-zylU!Yw9", 180) == 0);
  }
}
