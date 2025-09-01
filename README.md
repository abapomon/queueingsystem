# 🎯 ESP8266 Queue Management System

A complete queueing system using ESP8266 microcontroller with physical buttons and a web-based display interface.

## 📋 Overview

This project creates a digital queue management system perfect for businesses, clinics, or any service that needs to manage customer queues efficiently. The system features:

- **Two separate queues**: Regular customers and Senior/PWD (Persons with Disabilities)
- **Physical buttons**: ESP8266-based hardware for queue increment
- **Real-time web display**: Live-updating dashboard showing current queue numbers
- **RESTful API**: Backend server with HTTP endpoints
- **Reliable networking**: Auto-reconnection and error handling

## 🏗️ System Architecture

```
┌─────────────────┐    HTTP POST     ┌─────────────────┐    WebSocket/Polling    ┌─────────────────┐
│   ESP8266       │ ───────────────> │   Next.js       │ ─────────────────────> │   Web Display   │
│                 │                  │   Server        │                        │                 │
│ • Button A      │                  │                 │                        │ • Queue Numbers │
│ • Button B      │                  │ • API Routes    │                        │ • Live Updates  │
│ • WiFi          │                  │ • Queue State   │                        │ • Responsive    │
│ • Status LEDs   │                  │ • Auto-refresh  │                        │                 │
└─────────────────┘                  └─────────────────┘                        └─────────────────┘
```

## 🚀 Features

### Hardware (ESP8266)
- ✅ **Dual Queue Buttons**: Separate buttons for Regular and Senior/PWD queues
- ✅ **WiFi Connectivity**: Automatic connection and reconnection
- ✅ **Status Indicators**: LEDs for WiFi status and request success
- ✅ **Button Debouncing**: Prevents accidental multiple presses
- ✅ **Error Handling**: Retry logic for failed requests

### Software (Next.js Server)
- ✅ **RESTful API**: Clean HTTP endpoints for queue management
- ✅ **Real-time Display**: Auto-refreshing web interface
- ✅ **Responsive Design**: Works on desktop, tablet, and mobile
- ✅ **Live Updates**: Queue numbers update every second
- ✅ **Modern UI**: Clean, professional interface with animations

## 🛠️ Hardware Setup

### Components Required
- **ESP8266** (NodeMCU or Wemos D1 Mini)
- **2x Push Buttons**
- **2x LEDs** (optional status indicators)
- **2x 220Ω Resistors** (for LEDs)
- **Jumper wires**
- **Breadboard**

### Pin Connections

| Component | ESP8266 Pin | GPIO | Description |
|-----------|-------------|------|-------------|
| Button A (Regular) | D1 | GPIO5 | Connect to GND when pressed |
| Button B (Senior/PWD) | D2 | GPIO4 | Connect to GND when pressed |
| WiFi Status LED | D3 | GPIO0 | 220Ω resistor → LED → GND |
| Success LED | D4 | GPIO2 | 220Ω resistor → LED → GND |

### Wiring Diagram
```
ESP8266 NodeMCU          Components
                        
D1 (GPIO5) ──────────── Button A ──── GND
D2 (GPIO4) ──────────── Button B ──── GND
D3 (GPIO0) ──── 220Ω ── WiFi LED ──── GND
D4 (GPIO2) ──── 220Ω ── Success LED ─ GND
```

## 💻 Software Installation

### Prerequisites
- **Arduino IDE** with ESP8266 board package
- **Node.js** (v18 or higher)
- **npm** or **yarn**

### 1. Server Setup

```bash
# Clone or download the project
cd queueing

# Install dependencies
npm install

# Start the development server
npm run dev
```

The server will start at `http://localhost:3000`

### 2. ESP8266 Setup

1. **Install ESP8266 Board Package**:
   - Open Arduino IDE
   - File → Preferences
   - Add to "Additional Board Manager URLs":
     ```
     http://arduino.esp8266.com/stable/package_esp8266com_index.json
     ```
   - Tools → Board → Boards Manager
   - Search "ESP8266" and install

2. **Configure the Code**:
   - Open `esp8266_queue_client.ino`
   - Update WiFi credentials:
     ```cpp
     const char* ssid = "YOUR_WIFI_SSID";
     const char* password = "YOUR_WIFI_PASSWORD";
     ```
   - Update server IP (find your computer's IP):
     ```cpp
     const char* serverURL = "http://YOUR_COMPUTER_IP:3000";
     ```

3. **Upload to ESP8266**:
   - Select Tools → Board → ESP8266 → NodeMCU 1.0
   - Select correct COM port
   - Upload the code

## 🔧 Configuration

### Finding Your Server IP

**Windows:**
```powershell
ipconfig
```
Look for "IPv4 Address" under your WiFi adapter.

**macOS/Linux:**
```bash
ifconfig
```

### API Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| `POST` | `/api/regular` | Increment regular queue |
| `POST` | `/api/senior` | Increment senior/PWD queue |
| `GET` | `/api/queue` | Get current queue status |

### Example API Response
```json
{
  "regularQueue": 15,
  "seniorQueue": 3
}
```

## 🎮 Usage

### Starting the System

1. **Start the server**:
   ```bash
   npm run dev
   ```

2. **Power on ESP8266**: Should automatically connect to WiFi

3. **Open web display**: Navigate to `http://localhost:3000`

### Operating the Queue

- **Button A (D1)**: Press to increment Regular Queue
- **Button B (D2)**: Press to increment Senior/PWD Queue
- **Web Display**: Shows live queue numbers with auto-refresh
- **Status LEDs**: 
  - WiFi LED (D3): ON when connected to WiFi
  - Success LED (D4): Blinks when request succeeds

### Monitoring

- **Serial Monitor**: Open at 115200 baud for debug info
- **Web Console**: Check browser console for any errors
- **LED Status**: Visual feedback on ESP8266

## 🔍 Troubleshooting

### Common Issues

| Problem | Solution |
|---------|----------|
| ESP8266 won't connect to WiFi | Check SSID/password, ensure 2.4GHz network |
| "Error -1" or "Error -11" | Check server IP, ensure server is running |
| Buttons not responding | Check wiring, ensure INPUT_PULLUP mode |
| Web page not updating | Check server status, refresh browser |

### Debug Steps

1. **Check Serial Monitor**: Look for connection status and error messages
2. **Verify Network**: Ensure ESP8266 and computer are on same network
3. **Test API Manually**: Use curl or Postman to test endpoints
4. **Check Firewall**: Ensure port 3000 is not blocked

### Testing API Manually

```bash
# Test regular queue increment
curl -X POST http://localhost:3000/api/regular

# Test senior queue increment  
curl -X POST http://localhost:3000/api/senior

# Get current status
curl http://localhost:3000/api/queue
```

## 🚀 Deployment

### Production Setup

1. **Build for production**:
   ```bash
   npm run build
   npm start
   ```

2. **Use environment variables**:
   ```bash
   PORT=3000 npm start
   ```

3. **Configure reverse proxy** (nginx, Apache)

4. **Set up SSL** for HTTPS (recommended)

### Docker Deployment (Optional)

```dockerfile
FROM node:18-alpine
WORKDIR /app
COPY package*.json ./
RUN npm ci --only=production
COPY . .
RUN npm run build
EXPOSE 3000
CMD ["npm", "start"]
```

## 📈 Future Enhancements

- [ ] **Database Integration**: PostgreSQL/MySQL for queue history
- [ ] **WebSocket Support**: True real-time updates
- [ ] **Mobile App**: React Native companion app
- [ ] **Audio Alerts**: Sound notifications for queue updates
- [ ] **Admin Panel**: Queue management and reset functionality
- [ ] **Analytics Dashboard**: Queue statistics and reporting
- [ ] **Multi-location**: Support for multiple service points
- [ ] **Printer Integration**: Ticket printing capability

## 📚 Technology Stack

- **Hardware**: ESP8266 (NodeMCU/Wemos D1 Mini)
- **Frontend**: Next.js 15, React, TailwindCSS
- **Backend**: Next.js API Routes
- **Communication**: HTTP/REST API
- **Language**: TypeScript, C++ (Arduino)

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📄 License

This project is open source and available under the [MIT License](LICENSE).

## 📞 Support

For issues and questions:
- Check the troubleshooting section
- Open an issue on GitHub
- Review the serial monitor output for debugging

---

**Made with ❤️ for efficient queue management**

*Last updated: September 2025*
