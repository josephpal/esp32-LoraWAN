# General:
- Baord type:   TTGO LORA SX1278 ESP32 0.96 OLED Display Module 16 Mt Bytes 128 Mt bit) 433Mhz For Arduino 
- manufactor:   TTGO
- Banggood:     [Banggood Link](https://www.banggood.com/2Pcs-LILYGO-TTGO-LORA-SX1278-ESP32-0_96-OLED-Display-Module-16-Mt-Bytes-128-Mt-bit-433Mhz-For-Arduino-p-1270420.html)
- Pinout:       [Pinout Image Link](https://imgaz.staticbg.com/images/oaupload/ser1/banggood/images/15/B3/40996a08-9df2-46a1-b320-9f9b1a8a16a1.jpg)

## Install ESP32 and Arduino IDE

src:&nbsp;&nbsp;[randomnerdtutorials](https://randomnerdtutorials.com/esp32-lora-rfm95-transceiver-arduino-ide/)  
add: instructions for [windows](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)
and for [ubuntu](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-mac-and-linux-instructions/)

1) Download the arduino ide from: https://www.arduino.cc/en/main/software
2) __For Windows__: File -> Preferences -> Enter ```https://dl.espressif.com/dl/package_esp32_index.json``` into the “Additional Board Manager URLs”
                Tools > Board > Boards Manager -> Search for ESP32 and press install button for the “ESP32 by Espressif Systems“

   __For Linux__:   Arduino -> Preferences -> Enter https://dl.espressif.com/dl/package_esp32_index.json into the “Additional Board Manager URLs”
                Tools > Board > Boards Manager -> Search for ESP32 and press install button for the “ESP32 by Espressif Systems“

## Install Heltec ESP32 Board&Examples (also works for our board)

src: [Install ESP32 Arduino IDE](https://docs.heltec.cn/#/en/user_manual/how_to_install_esp32_Arduino)  
add: [link ref. 1](https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series), 
[link ref. 2](https://imgaz.staticbg.com/images/oaupload/ser1/banggood/images/15/B3/40996a08-9df2-46a1-b320-9f9b1a8a16a1.jpg), [link ref. 3](https://iotbyhvm.ooo/wifi-lora-32-esp32/)

1) File -> Preferences: https://docs.heltec.cn/download/package_heltec_esp32_index.json
2) Board Manager: Heltec ESP32 install
3) Manage Libraries: Heltec ESP32 install
4) Example -> Custom Library Examples -> Heltec ESP32 Dev-Boards -> Factory_Test -> WiFi_Lora_32Factory_Test
5) Tools > Board and select the WiFi LoRA 32
6) Change Band to 433E6

---
## Use the TTGo Board and a example which is special made for this type of board  

src: https://randomnerdtutorials.com/ttgo-lora32-sx1276-arduino-ide/
add: https://iotbyhvm.ooo/wifi-lora-32-esp32/

1) Manage Libraries: LoRa by Sandeep Mistry, SSD1306, GFx
2) Tools > Board and select the TTGO LoRa32-OLED V1 board
3) Send Code:
4) Receive Code:
5) Change Band to 433E6
