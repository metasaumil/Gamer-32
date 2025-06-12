<h1>Gamer-32</h1>

<h2>Description</h2>
<p>This repository contains the complete source code and resources for Gamer-32 — a custom ESP32-based handheld gaming console. It supports native gameplay on a TFT display and also integrates with PC games using a joystick-based wireless controller.</p>

<h2>Table of Contents</h2>
<ul>
    <li>Installation</li>
    <li>How to Use</li>
    <li>Projects</li>
</ul>

<h2>Installation</h2>
<p>Clone the repository to your local machine:</p>
<pre><code>git clone https://github.com/metasaumil/Gamer-32.git
cd Gamer-32
</code></pre>

<h2>How to Use</h2>

<h3>🎮 Option 1: ESP32 TFT Display (e.g., Flappy Bird)</h3>
<ol>
    <li>Connect your ILI9341 TFT display to the ESP32 using the correct SPI pins.</li>
    <li>Open the <code>FlappyBird</code> sketch in the Arduino IDE.</li>
    <li>Install required libraries: <code>Adafruit_GFX</code>, <code>Adafruit_ILI9341</code>, <code>SPI</code>.</li>
    <li>Select the correct board (ESP32 Dev Module) and COM port.</li>
    <li>Click <strong>Upload</strong> and watch the game on your TFT screen!</li>
</ol>

<h3>🕹️ Option 2: ESP32 Joystick Controller + AntimicroX</h3>
<ol>
    <li>Flash the ESP32 joystick controller sketch (usually under <code>Gamepad_Controller</code> or similar).</li>
    <li>Ensure the ESP32 is advertising as a Bluetooth HID gamepad.</li>
    <li>On your PC, pair the ESP32 over Bluetooth.</li>
    <li>Install <a href="https://github.com/AntiMicroX/antimicrox">AntiMicroX</a> on your computer.</li>
    <li>Open AntiMicroX and import the config file from the <code>scripts/</code> folder of this repo.</li>
    <li>Map buttons/joystick to desired keyboard inputs (already configured in the file).</li>
    <li>Run your favorite PC game and control it using the ESP32 joystick!</li>
</ol>

<h2>Projects</h2>

- **ESPNOW_Master**
  - `ESPNOW_Master.ino`

- **ESPNOW_Slave**
  - `ESPNOW_Slave.ino`

- **FlappyBird**
  - `FlappyBird.ino`

- **Scripts**

- **Wireless_Gamepad**
  - `Wireless_Gamepad.ino`

- **Wireless_Gamepad_digital**
  - `Wireless_Gamepad_digital.ino`

