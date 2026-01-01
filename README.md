# Weather Station with Firebase Global Control

The repository contains complete IoT weather station system using ESP32, OLED display, and Firebase Realtime Database for remote control from anywhere in the world!

## 📂 Repository Structure

- `1. rules`                    # # Firebase database rules
- `2. arduino_code.ino`         # ESP32 firmware with Firebase
- `3. index.html`               # Web control panel


## ✨ Features

- **Real-Time Weather Data** - Live temperature, humidity, pressure, wind speed, and conditions
- **WiFi Connectivity** - Connects to your home WiFi network
- **OLED Display** - Shows weather information on a 128x64 OLED screen
- **Global Remote Control** - Control from anywhere using Firebase
- **Auto/Manual Modes** - Automatically cycle views or manually select specific data
- **Location Switching** - Change weather location instantly from web interface
- **Beautiful Web Interface** - Modern gradient design with real-time status
- **Cloud Synced** - Weather data automatically synced to Firebase
- **Live Monitoring** - See weather data update in real-time on webpage
- **Cross-Platform** - Access from any device with a web browser


## 🛠️ Hardware Requirements

| Component | Specification | Quantity |
|-----------|--------------|----------|
| ESP32 Development Board | ESP32-WROOM-32 or similar | 1 |
| OLED Display | SSD1306 128x64 (I2C) | 1 |
| USB Cable | Micro USB or USB-C | 1 |
| Breadboard (Optional) | Standard size | 1 |
| Jumper Wires | Male-to-Female | 4 |



## 🔌 Connections

Connect the OLED display to ESP32:

```
ESP32          OLED SSD1306
─────────────────────────────
3.3V     →     VCC
GND      →     GND
GPIO21   →     SDA
GPIO22   →     SCL
```

**Note**: If display doesn't work, try:
- Using 5V instead of 3.3V
- Using GPIO4 and GPIO15 (update code accordingly)
- Checking I2C address with scanner sketch



## 📦 Software Requirements

### For ESP32 Programming

1. **Arduino IDE** (version 2.0 or higher)
   - Download from [arduino.cc](https://www.arduino.cc/en/software)

2. **ESP32 Board Support**
   - Add URL to Board Manager: 
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```

3. **Required Arduino Libraries**
   - Adafruit SSD1306
   - Adafruit GFX Library
   - ArduinoJson
   - Firebase ESP Client (by Mobizt)
   - WiFi (pre-installed)
   - HTTPClient (pre-installed)

### For Web Interface

- Any modern web browser (Chrome, Firefox, Safari, Edge)
- Internet connection



## 🚀 Setup Guide

### Part A: Firebase Setup

#### Step 1: Create Firebase Project

1. Go to [Firebase Console](https://console.firebase.google.com/)
2. Click **"Add project"**
3. Enter project name: `weather-station-control`
4. Disable Google Analytics (optional)
5. Click **"Create project"**

#### Step 2: Enable Realtime Database

1. In Firebase Console, click **"Realtime Database"** from left menu
2. Click **"Create Database"**
3. Choose a location closest to you:
   - `asia-southeast1` (Singapore)
   - `us-central1` (USA)
   - `europe-west1` (Belgium)
4. Select **"Start in test mode"** (for now)
5. Click **"Enable"**

#### Step 3: Configure Database Rules

1. Go to **"Rules"** tab in Realtime Database (in Build)
2. Replace the rules with:

```json
{
  "rules": {
    "weather": {
      ".read": true,
      ".write": true
    }
  }
}
```

3. Click **"Publish"**

**Security Note**: These rules allow anyone to read/write. For production, implement proper authentication!

#### Step 4: Get Firebase Configuration

1. Click the **gear icon** (⚙️) → **Project settings**
2. Scroll down to **"Your apps"**
3. Click the **web icon** (`</>`)
4. Register app name: `Weather Control Panel`
5. Copy the `firebaseConfig` object (you'll need this later)

It looks like:
```javascript
const firebaseConfig = {
  apiKey: "AIzaSyXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
  authDomain: "weather-station-xxxxx.firebaseapp.com",
  databaseURL: "https://weather-station-xxxxx.firebaseio.com",
  projectId: "weather-station-xxxxx",
  storageBucket: "weather-station-xxxxx.appspot.com",
  messagingSenderId: "123456789",
  appId: "1:123456789:web:xxxxxxxxxxxxx"
};
```

#### Step 5: Enable Authentication (Optional but Recommended)

1. Go to **"Authentication"** from left menu
2. Click **"Get started"**
3. Select **"Email/Password"**
4. Enable it and click **"Save"**
5. Go to **"Users"** tab
6. Click **"Add user"**
7. Enter:
   - Email: `esp32@weather.com`
   - Password: `weather123`
8. Click **"Add user"**

#### Step 6: Initialize Database Structure

You need to create the initial data structure. In your current Firebase screen:

1. Click the **"+"** button next to the database URL
2. Create this structure by adding one by one:

```
weather
  ├─ command: ""
  ├─ location: ""
  ├─ status: "offline"
  └─ data
      ├─ temp: 0
      ├─ humidity: 0
      └─ condition: ""
```

**How to do it:**

- Click **"+"** → Name: `weather` → Value: (leave empty) → Click **Add**
- Click **"+"** next to `weather` → Name: `command` → Value: `""` → Add
- Click **"+"** next to `weather` → Name: `location` → Value: `""` → Add
- Click **"+"** next to `weather` → Name: `status` → Value: `"offline"` → Add
- Click **"+"** next to `weather` → Name: `data` → Value: (leave empty) → Add
- Click **"+"** next to `data` → Name: `temp` → Value: `0` → Add
- Click **"+"** next to `data` → Name: `humidity` → Value: `0` → Add
- Click **"+"** next to `data` → Name: `condition` → Value: `""` → Add



### Part B: ESP32 Setup

#### Step 1: Get OpenWeatherMap API Key

1. Go to [OpenWeatherMap](https://openweathermap.org/api)
2. Sign up for a free account
3. Navigate to **API Keys** section
4. Copy your API key

#### Step 2: Get Location Coordinates

1. Go to [Google Maps](https://www.google.com/maps)
2. Right-click on your desired location
3. Copy the coordinates (latitude, longitude)

#### Step 3: Install Arduino Libraries

1. Open Arduino IDE
2. Go to `Sketch` → `Include Library` → `Manage Libraries`
3. Install these libraries:
   - **Adafruit SSD1306**
   - **Adafruit GFX Library**
   - **ArduinoJson** (by Benoit Blanchon)
   - **Firebase ESP Client** (by Mobizt) - **IMPORTANT!**

#### Step 4: Configure Arduino Code

1. Open `2. arduino_code.ino` from the repository
2. Update WiFi credentials:

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
```

3. Update Firebase configuration:

```cpp
#define API_KEY "YOUR_FIREBASE_API_KEY"
#define DATABASE_URL "YOUR_FIREBASE_DATABASE_URL"
#define USER_EMAIL "esp32@weather.com"
#define USER_PASSWORD "weather123"
```

4. Update OpenWeatherMap settings:

```cpp
const char* weatherApiKey = "YOUR_OPENWEATHERMAP_API_KEY";
String lat = "17.2391";  // Your latitude
String lon = "78.4358";  // Your longitude
```

#### Step 5: Upload to ESP32

1. Connect ESP32 to computer via USB
2. Select board: `Tools` → `Board` → `ESP32 Dev Module`
3. Select COM port: `Tools` → `Port` → `COMx` or `/dev/ttyUSB0`
4. Click **Verify** (✓) to check for errors
5. Click **Upload** (→)
6. Wait for "Done uploading"

#### Step 6: Monitor Serial Output

1. Open Serial Monitor: `Tools` → `Serial Monitor`
2. Set baud rate to `115200`
3. You should see:
   ```
   WiFi OK
   Firebase OK!
   Weather OK
   ```


### Part C: Web Interface Setup

#### Step 1: Create HTML File

1. Download `3. index.html` from the repository
2. Open it in a text editor (VS Code, Notepad++, etc.)

#### Step 2: Add Firebase Configuration

Find this section in the HTML file:

```javascript
const firebaseConfig = {
  apiKey: " ",
  authDomain: " ",
  databaseURL: " ",
  projectId: " ",
  storageBucket: " ",
  messagingSenderId: " ",
  appId: " "
};
```

Replace it with your Firebase config from Part A, Step 4.

#### Step 3: Open the Web Interface

1. Simply double-click the HTML file
2. It will open in your default browser
3. You should see the Weather Station control panel

**That's it! No server needed!** The webpage connects directly to Firebase.


## 🎮 How to Use

### Web Interface Controls

#### Connection Status
- 🟢 **Green dot pulsing** = ESP32 is online
- 🔴 **Red dot steady** = ESP32 is offline

#### Current Weather Display
Real-time data synced from ESP32:
- 🌡️ Temperature
- 💧 Humidity
- ☁️ Weather Condition

#### Screen View Controls
Click any button to change what displays on the OLED:
- **☁️ Condition** - Weather description
- **🌡️ Temperature** - Current temperature
- **🤔 Feels Like** - Apparent temperature
- **💧 Humidity** - Humidity percentage
- **📊 Pressure** - Atmospheric pressure
- **💨 Wind Speed** - Wind speed
- **🔄 Auto Cycle** - Rotate through all views every 30 seconds

#### Change Location
Two ways to update location:

1. **Manual Entry**:
   - Enter latitude and longitude
   - Click **📌 Update**

2. **Preset Cities**:
   - Click any preset button:
     - 🗽 New York
     - 🇬🇧 London
     - 🗼 Tokyo
     - 🇮🇳 Delhi
     - 🏠 Hyderabad

#### Refresh Weather
- Click **🔄 Refresh Weather Now** to force immediate update



## 🐛 Troubleshooting

### Firebase Connection Issues

**ESP32 shows "Firebase fail"**
- Verify API_KEY and DATABASE_URL are correct
- Check Firebase Realtime Database is enabled
- Ensure database rules allow read/write
- Verify user authentication credentials
- Check WiFi connection is stable

**Web interface shows "ESP32 Offline"**
- Make sure ESP32 is powered on
- Check ESP32 Serial Monitor for errors
- Verify Firebase config in HTML matches ESP32 code
- Try refreshing the webpage

### OLED Display Problems

**Display not working**
- Check I2C address (try 0x3C or 0x3D)
- Use I2C scanner sketch to detect address
- Try 5V instead of 3.3V
- Verify SDA/SCL connections
- Try different GPIO pins (4, 15)

**Display shows garbage**
- Check I2C address
- Verify power supply is stable
- Try lower I2C speed in code

### WiFi Connection Failed

- Double-check SSID and password (case-sensitive!)
- Use 2.4GHz WiFi (ESP32 doesn't support 5GHz)
- Move ESP32 closer to router
- Check for special characters in WiFi password
- Try static IP if DHCP fails

### Weather API Issues

**No weather data**
- Verify OpenWeatherMap API key is active
- Check latitude/longitude are correct
- Ensure internet connection works
- Wait a few minutes (new API keys need activation)
- Check API usage limits

### Upload Problems

**Code won't upload**
- Hold BOOT button during upload
- Select correct COM port
- Try different USB port/cable
- Install CH340/CP2102 drivers
- Close Serial Monitor before uploading



## 📖 How It Works

### System Architecture

```
┌─────────────────┐
│   Web Browser   │
│  (index.html)   │
└────────┬────────┘
         │
         ↓
┌─────────────────┐
│    Firebase     │
│  Realtime DB    │
└────────┬────────┘
         │
         ↓
┌─────────────────┐
│      ESP32      │
│  + OLED Display │
└────────┬────────┘
         │
         ↓
┌─────────────────┐
│ OpenWeatherMap  │
│       API       │
└─────────────────┘
```

### Data Flow

1. **ESP32 Startup**
   - Connects to WiFi
   - Authenticates with Firebase
   - Sets status to "online"
   - Fetches initial weather data

2. **Weather Updates**
   - ESP32 requests data from OpenWeatherMap API every 30 seconds
   - Parses JSON response
   - Updates OLED display
   - Syncs data to Firebase (`/weather/data/`)

3. **Remote Commands**
   - User clicks button on webpage
   - Firebase SDK writes to `/weather/command`
   - ESP32 reads command every 2 seconds
   - Executes command (change view, location, etc.)
   - Clears command from database

4. **Real-time Monitoring**
   - Web interface listens to Firebase database
   - Automatically updates when ESP32 pushes new data
   - Shows connection status based on `/weather/status`




## 🎓 Learning Outcomes

By completing this project, you'll learn:

- 📡 **IoT Communication**: WiFi connectivity and HTTP requests
- ☁️ **Cloud Services**: Firebase Realtime Database integration
- 🔐 **Authentication**: Firebase user authentication
- 📊 **Data Parsing**: JSON handling with ArduinoJson
- 🖥️ **Hardware Protocols**: I2C communication with OLED
- 🌐 **Web Development**: Modern HTML5, CSS3, JavaScript
- 🔥 **Firebase SDK**: Real-time database operations
- 🎨 **UI/UX Design**: Responsive web interface design
- 🤖 **Embedded Systems**: ESP32 programming and optimization
- 🌍 **REST APIs**: OpenWeatherMap API integration



## 🚀 Future Enhancements

### Easy Additions
- 🌙 **Day/Night Theme** - Auto-switch based on sunrise/sunset times
- 📈 **Weather Graphs** - Historical data visualization
- 🔔 **Weather Alerts** - Push notifications for severe weather
- 🎨 **Custom Icons** - Weather-specific graphics on OLED
- 🌡️ **Multiple Locations** - Save favorite cities

### Intermediate Features
- 📱 **Mobile App** - Native iOS/Android app
- 🗣️ **Voice Control** - Alexa/Google Assistant integration
- 📊 **Data Logging** - Store historical weather data
- 🌈 **LED Indicators** - RGB LED for weather conditions
- 📸 **Webcam Integration** - Live weather camera feed

### Advanced Projects
- 🤖 **Home Automation** - Integrate with smart home systems
- 🧠 **AI Predictions** - Machine learning weather forecasting
- 📡 **Multi-Device Network** - Connect multiple weather stations
- 🗺️ **Weather Mapping** - Create regional weather map
- ⚡ **Solar Powered** - Battery + solar panel operation

---

## 🔒 Security Best Practices

### Current Setup (Test Mode)
The provided configuration uses test mode for ease of learning. This is **NOT secure for production**.

### For Production Use

1. **Update Firebase Rules**:
```json
{
  "rules": {
    "weather": {
      ".read": "auth != null",
      ".write": "auth != null"
    }
  }
}
```

2. **Use Environment Variables**:
   - Never commit API keys to GitHub
   - Use `.env` files (add to `.gitignore`)
   - Consider using Firebase Cloud Functions

3. **Enable HTTPS**:
   - Firebase automatically uses HTTPS
   - Ensure OpenWeatherMap uses HTTPS endpoint

4. **Implement Rate Limiting**:
   - Limit Firebase writes to prevent abuse
   - Add cooldown periods between commands

---


## 📄 License

This project is open source and available for educational purposes. 

---

## 👤 Author

[@AdhvikaECE536](https://github.com/AdhvikaECE536)

---

## 🙏 Acknowledgments
Special thanks to GMR Airports Ltd for proving me the environment to learn, explore and grow. 

---

**Note**: This is a learning project. It uses open-source libraries and public APIs. Feel free to experiment and modify the code!


---

Made with ❤️ using ESP32

