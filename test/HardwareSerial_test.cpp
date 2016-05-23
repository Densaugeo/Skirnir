#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "HardwareSerial.h"

TEST_CASE("HardwareSerial()") {
  SECTION("Empty port is empty") {
    HardwareSerial port = HardwareSerial();
    
    REQUIRE(port.inputAvailable == 0);
    REQUIRE(port.outputAvailable == 0);
  }
  
  SECTION("Constructor passes initial input") {
    HardwareSerial port = HardwareSerial((uint8_t*) "asdf", 5);
    
    REQUIRE(port.inputAvailable == 5);
    REQUIRE(memcmp(port.inputBuffer, "asdf", 5) == 0);
    REQUIRE(port.outputAvailable == 0);
  }
  
  SECTION("Initial input is truncated to 300 bytes") {
    HardwareSerial port = HardwareSerial((uint8_t*) "E&TvMT2HKbBUcF!rm;B1om$fuLQQtMr<Z$[zs|n]#faTyq;cyHFDO,:o7*V3vF#.M(KxVPBYMJ{4kIL];%rxR;eu9p-dyyBH-#[i?hU!G4L1[k#t*^*u2p{)3)wW01YDBuOf3)tGFKevPWw<<x* nzJzo7Z$ |$^Ek3NbZ!k@I,ee%zL[+--`=u^M59L=paB.{h#8cNd13/$5xj9/W@WzXW&V -R6&W@hBuA{m=)=916B;kAuAItI]yOZ3YJ1czN4F`X75AOczh}`)@jtLjr<.[9dki;*0$>I 5JHYXi+&KE*t*Bj2ut9D", 310);
    
    REQUIRE(port.inputAvailable == 300);
    REQUIRE(memcmp(port.inputBuffer, "E&TvMT2HKbBUcF!rm;B1om$fuLQQtMr<Z$[zs|n]#faTyq;cyHFDO,:o7*V3vF#.M(KxVPBYMJ{4kIL];%rxR;eu9p-dyyBH-#[i?hU!G4L1[k#t*^*u2p{)3)wW01YDBuOf3)tGFKevPWw<<x* nzJzo7Z$ |$^Ek3NbZ!k@I,ee%zL[+--`=u^M59L=paB.{h#8cNd13/$5xj9/W@WzXW&V -R6&W@hBuA{m=)=916B;kAuAItI]yOZ3YJ1czN4F`X75AOczh}`)@jtLjr<.[9dki;*0$>I 5JHYXi+&KE", 300) == 0);
    REQUIRE(port.outputAvailable == 0);
  }
}

TEST_CASE("HardwareSerial.addInput()") {
  HardwareSerial port = HardwareSerial();
  
  SECTION("Adding zero bytes does nothing") {
    port.addInput((uint8_t*) "foo", 0);
    
    REQUIRE(port.inputAvailable == 0);
  }
  
  SECTION("Adding bytes puts them in .inputBuffer") {
    port.addInput((uint8_t*) "foo", 4);
    
    REQUIRE(port.inputAvailable == 4);
    REQUIRE(memcmp(port.inputBuffer, "foo", 4) == 0);
  }
  
  SECTION("Adding more bytes appends to .inputBuffer") {
    port.addInput((uint8_t*) "foo", 4);
    port.addInput((uint8_t*) "foo", 4);
    
    REQUIRE(port.inputAvailable == 8);
    REQUIRE(memcmp(port.inputBuffer, "foo\0foo", 8) == 0);
  }
  
  SECTION("New bytes are ignored after .inputBuffer is full") {
    port.addInput((uint8_t*) "P*6FDzB<WR [*R%r1NdfVCmASpGW-cd&w;GA< Y@h2Pt/WJj{`wbBT2cWu:tJr]u]6U,ro|?*&39X0P0<`:-%rEm3uxq]0!Y:V`HCbFltG]`dN'HvNOz-Dhmgt6VfX,TZE?{)]A1k/^BgE&p-DQ0$(446[XwdD8q@R2%|)}l#Y>j3]#2=T* ]wrsvr+U@g&}{t]8lJ6JD&eE@Xf@VY}apyY`N`gBCD(iSGgVaOnvSf,_qh[?!sgAla<f{fk0V/wgszcT2}4e.`5^%8+p6S'jA1BprTUx0WtBFT{bq-_nG2LWZ[!0o(DQvt", 310);
    
    REQUIRE(port.inputAvailable == 300);
    REQUIRE(memcmp(port.inputBuffer, "P*6FDzB<WR [*R%r1NdfVCmASpGW-cd&w;GA< Y@h2Pt/WJj{`wbBT2cWu:tJr]u]6U,ro|?*&39X0P0<`:-%rEm3uxq]0!Y:V`HCbFltG]`dN'HvNOz-Dhmgt6VfX,TZE?{)]A1k/^BgE&p-DQ0$(446[XwdD8q@R2%|)}l#Y>j3]#2=T* ]wrsvr+U@g&}{t]8lJ6JD&eE@Xf@VY}apyY`N`gBCD(iSGgVaOnvSf,_qh[?!sgAla<f{fk0V/wgszcT2}4e.`5^%8+p6S'jA1BprTUx0WtBFT{bq-_nG2LW", 300) == 0);
  }
}

TEST_CASE("HardwareSerial.available()") {
  HardwareSerial port = HardwareSerial();
  
  SECTION("Zero bytes available at start") {
    REQUIRE(port.available() == 0);
  }
  
  SECTION("Constucting with initial input makes it available") {
    HardwareSerial port_2 = HardwareSerial((uint8_t*) "asdf", 4);
    
    REQUIRE(port_2.available() == 4);
  }
  
  SECTION("Adding bytes makes them available") {
    port.addInput((uint8_t*) "asdf", 4);
    
    REQUIRE(port.available() == 4);
  }
  
  SECTION("Adding bytes successively makes them available") {
    port.addInput((uint8_t*) "asdf", 4);
    port.addInput((uint8_t*) "asdf", 4);
    
    REQUIRE(port.available() == 8);
  }
  
  SECTION("Reading bytes makes them not available any more") {
    port.addInput((uint8_t*) "asdf", 4);
    
    port.read();
    
    REQUIRE(port.available() == 3);
  }
  
  SECTION("Reading bytes successively makes them not available any more") {
    port.addInput((uint8_t*) "asdf", 4);
    
    port.read();
    port.read();
    
    REQUIRE(port.available() == 2);
  }
  
  SECTION("Reading from and empty port doesn't make availability negative") {
    port.read();
    
    REQUIRE(port.available() == 0);
  }
}

TEST_CASE("HardwareSerial.read()") {
  HardwareSerial port = HardwareSerial((uint8_t*) "1337", 4);
  
  SECTION("Reading an empty port returns -1") {
    HardwareSerial port_2 = HardwareSerial();
    
    REQUIRE(port_2.read() == -1);
  }
  
  SECTION("Reading a non-empty port returns the first byte") {
    REQUIRE(port.read() == '1');
  }
  
  SECTION("Reading a non-empty port removes the first byte from .inputBuffer") {
    port.read();
    
    REQUIRE(port.inputAvailable == 3);
    REQUIRE(memcmp(port.inputBuffer, "337", 3) == 0);
  }
  
  SECTION("Reading a non-empty port successively returns the first bytes") {
    REQUIRE(port.read() == '1');
    REQUIRE(port.read() == '3');
  }
  
  SECTION("Reading a non-empty port successively removes the first bytes from .inputBuffer") {
    port.read();
    port.read();
    
    REQUIRE(port.inputAvailable == 2);
    REQUIRE(memcmp(port.inputBuffer, "37", 2) == 0);
  }
}

TEST_CASE("HardwareSerial.write()") {
  HardwareSerial port = HardwareSerial();
  
  SECTION("Writing a byte puts it in .outputBuffer") {
    port.write('!');
    
    REQUIRE(port.outputAvailable == 1);
    REQUIRE(memcmp(port.outputBuffer, "!", 1) == 0);
  }
  
  SECTION("Writing bytes successively puts them in .outputBuffer") {
    port.write('!');
    port.write('@');
    
    REQUIRE(port.outputAvailable == 2);
    REQUIRE(memcmp(port.outputBuffer, "!@", 2) == 0);
  }
  
  SECTION("Writing bytes to a full buffer does nothing") {
    port.outputAvailable = 300;
    memcpy(port.outputBuffer, (uint8_t*) "12", 2);
    
    port.write('!');
    port.write('@');
    
    REQUIRE(port.outputAvailable == 300);
    REQUIRE(memcmp(port.outputBuffer, "12", 2) == 0);
  }
  
  SECTION("Writing a string puts several bytes in .outputBuffer") {
    REQUIRE(port.write("this is a string") == 16);
    
    REQUIRE(port.outputAvailable == 16);
    REQUIRE(memcmp(port.outputBuffer, "this is a string", 16) == 0);
  }
  
  SECTION("Writing string successively puts their bytes in .outputBuffer") {
    REQUIRE(port.write("this is a string") == 16);
    REQUIRE(port.write("this is another string") == 22);
    
    REQUIRE(port.outputAvailable == 38);
    REQUIRE(memcmp(port.outputBuffer, "this is a stringthis is another string", 38) == 0);
  }
  
  SECTION("Long strings are truncated when .outputBuffer is full") {
    REQUIRE(port.write(";j(VUGRP*YUFbD,}T:9q]uX:,T#p'@5IeJ[G<;ICIS'z[2351 Nojw8.4Bq)WD/EsC/7.@I??Uw)s3b1A`krP$+8jV@ F!(suG:rA%tgBujnc@PTpS2$[+%xYOc+}(F:6nZ(/Kj}Jo^FU2*u=:4''hxw+G|O8*lB<@?( NKEdtjB$MI9w9]dpf}(QY?JMHmCCp3DT+nk6H,aE@%>0dySnEzEKw<]tt`mfZBMCLA]oU8T,hjYb=4w&,Io:8<:UBY+%)@I!d4O;)I3{w]Q2)$,>s(;Z=Mjoin+}qIP]jBjhq[sP%}S7^cmcS") == 300);
    
    REQUIRE(port.outputAvailable == 300);
    REQUIRE(memcmp(port.outputBuffer, ";j(VUGRP*YUFbD,}T:9q]uX:,T#p'@5IeJ[G<;ICIS'z[2351 Nojw8.4Bq)WD/EsC/7.@I??Uw)s3b1A`krP$+8jV@ F!(suG:rA%tgBujnc@PTpS2$[+%xYOc+}(F:6nZ(/Kj}Jo^FU2*u=:4''hxw+G|O8*lB<@?( NKEdtjB$MI9w9]dpf}(QY?JMHmCCp3DT+nk6H,aE@%>0dySnEzEKw<]tt`mfZBMCLA]oU8T,hjYb=4w&,Io:8<:UBY+%)@I!d4O;)I3{w]Q2)$,>s(;Z=Mjoin+}qIP]jBjhq[s", 300) == 0);
  }
}
