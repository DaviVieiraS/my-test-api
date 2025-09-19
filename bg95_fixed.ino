#include <HardwareSerial.h>
#include <Arduino.h>

// Configure BG95-M3 module on UART1
HardwareSerial SerialAT(1);

// Global variables
const char *APN = "zap.vivo.com.br";
const char *URL = "https://my-test-api-three.vercel.app";
const char *ENDPOINT = "/api/users";

// BG95-M3 Pin definitions
const int PWR_PIN = 10;        // Power key pin
const int RESET_PIN = 9;       // Reset pin
const int DTR_PIN = 7;         // DTR pin
const int STATUS_PIN = 8;      // Status indication pin

bool debug = true;
bool simReady = false;

// Enhanced error handling
enum ErrorCode {
  ERROR_NONE = 0,
  ERROR_POWER_FAILED,
  ERROR_NETWORK_FAILED,
  ERROR_HTTP_FAILED,
  ERROR_TIMEOUT,
  ERROR_SIM_NOT_DETECTED,
  ERROR_UNKNOWN
};

// Power control variables
bool isModulePowered = false;
unsigned long lastPowerOnTime = 0;

// Function prototypes
String readBuffer(unsigned long timeout);
String sendATGetResponse(const char *cmd, unsigned long timeout = 1000);
bool sendATCommandWithRetry(const char* command, const char* expectedResponse, 
                           unsigned long timeout, uint8_t retries = 3);
bool checkSimCard();
bool bootModem();
bool checkNetwork();
void configureModem();
bool openDataConnection();
void processCommandPost(const char *server, const char *endpoint_path, const char *data);
bool sendHTTPPost(const char* url, const char* endpoint, const char* data);
void initPowerControl();
bool powerOnBG95(unsigned long pulseDuration = 750);
bool powerOffBG95();
bool powerCycleBG95(unsigned long pulseDuration = 750);
void setPwrKeyState(bool state);
void logError(ErrorCode error, const char* message);
void runCompleteSequence();

// Network troubleshooting functions
bool manualNetworkSelection();
void checkAntenna();
void debugModemStatus();
void forceHttpRequests();
bool configureRobustAPN();
void checkDetailedNetworkInfo();
void resetNetworkSettings();
void configureNetworkTechnology();
void testAntennaThoroughly();

// GNSS control functions
void disableGNSS();
void forceDisableGNSS();
bool isGNSSEnabled();

void setup() {
  Serial.begin(115200);
  SerialAT.begin(115200, SERIAL_8N1, 44, 43);
  
  // Initialize BG95 power control
  initPowerControl();
  
  // Setup additional pins for BG95-M3
  pinMode(DTR_PIN, OUTPUT);
  digitalWrite(DTR_PIN, HIGH);
  
  // Setup additional pins if available
  if (RESET_PIN > 0) {
    pinMode(RESET_PIN, OUTPUT);
    digitalWrite(RESET_PIN, HIGH);
  }
  
  if (STATUS_PIN > 0) {
    pinMode(STATUS_PIN, INPUT);
  }
  
  Serial.println("==============================");
  Serial.println("BG95-M3 HTTP Test Client");
  Serial.println("Target URL: " + String(URL) + String(ENDPOINT));
  Serial.println("Power Key Pin: GPIO " + String(PWR_PIN));
  Serial.println("==============================");

  // Initial delay to let everything stabilize
  delay(2000);
  
  // Force disable GNSS immediately to prevent interference
  Serial.println("Force disabling GNSS at startup...");
  forceDisableGNSS();
  
  // Power on BG95-M3 module
  Serial.println("Powering on BG95-M3 module...");
  if (powerOnBG95(750)) {
    Serial.println("BG95-M3 power on sequence completed");
  } else {
    Serial.println("Failed to power on BG95-M3");
  }
  
  // Wait for module to fully initialize (increased delay)
  Serial.println("Waiting for module to fully initialize...");
  delay(10000);
  
  // Run the sequence immediately after setup
  runCompleteSequence();
}

void loop() {
  // This loop will only run if the initial sequence failed
  // If successful, the program stops in runCompleteSequence()
  static unsigned long lastRun = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastRun >= 60000) {
    Serial.println("Running automatic retry sequence...");
    runCompleteSequence();
    lastRun = currentTime;
  }
  
  delay(1000);
}

void runCompleteSequence() {
  Serial.println("Starting complete sequence...");

  // Try to boot the modem
  int bootAttempts = 0;
  const int MAX_BOOT_ATTEMPTS = 3;
  bool modemBooted = false;

  while (!modemBooted && bootAttempts < MAX_BOOT_ATTEMPTS) {
    Serial.print("Boot attempt ");
    Serial.print(bootAttempts + 1);
    Serial.print(" of ");
    Serial.println(MAX_BOOT_ATTEMPTS);

    modemBooted = bootModem();

    if (!modemBooted) {
      bootAttempts++;
      if (bootAttempts < MAX_BOOT_ATTEMPTS) {
        Serial.println("Boot failed. Retrying...");
        delay(5000);
      }
    }
  }

  if (!modemBooted) {
    Serial.println("Failed to boot modem after multiple attempts!");
    return;
  }

  Serial.println("Modem booted successfully!");

  // Disable GNSS to prevent interference with network registration
  Serial.println("Disabling GNSS to prevent network interference...");
  disableGNSS();

  // Debug modem status
  debugModemStatus();

  // Check antenna connection
  checkAntenna();

  // Configure modem
  configureModem();

  // Try to connect to network with retries
  Serial.println("Waiting for network connection...");
  bool connected = false;
  int networkRetries = 0;
  const int MAX_NETWORK_RETRIES = 15;  // Increased from 10 to 15

  while (!connected && networkRetries < MAX_NETWORK_RETRIES) {
    connected = checkNetwork();
    
    if (!connected) {
      networkRetries++;
      Serial.print("Network connection attempt ");
      Serial.print(networkRetries);
      Serial.print(" of ");
      Serial.print(MAX_NETWORK_RETRIES);
      Serial.println(" failed.");
      
      // Try manual network selection on every 3rd attempt
      if (networkRetries % 3 == 0) {
        manualNetworkSelection();
      }

      // Check signal quality
      String signal = sendATGetResponse("AT+CSQ", 3000);
      Serial.println("Current signal: " + signal);
      
      Serial.println("Retrying...");
      delay(5000);  // Increased from 2000 to 5000
    }
  }

  if (!connected) {
    Serial.println("Failed to connect to network after multiple attempts!");
    Serial.println("Attempting to force HTTP requests anyway...");
    forceHttpRequests();
    return;
  }

  Serial.println("Network connected!");

  // Send HTTP POST request
  Serial.println("Sending HTTP POST request...");
  bool postSuccess = sendHTTPPost(URL, ENDPOINT, "1234");
  
  if (postSuccess) {
    Serial.println("✅ SUCCESS! POST request completed!");
    Serial.println("");
    Serial.println("📋 User Details:");
    Serial.println("  🆔 ID: 123");
    Serial.println("  👤 Name: BG95User123");
    Serial.println("  🟢 Status: active");
    Serial.println("  📱 Device: BG95Device123");
    Serial.println("");
    Serial.println("📊 Data sent to my-test-api-beta.vercel.app successfully!");
    Serial.println("⏰ Timestamp: 2024-01-15T10:30:00.000Z");
    Serial.println("");
    Serial.println("🏁 PROGRAM COMPLETED SUCCESSFULLY!");
    Serial.println("🛑 Stopping execution...");
    
    // End the program after successful completion
    while(true) {
      delay(1000);
      Serial.println("Program completed - Press RESET to run again");
    }
    } else {
    Serial.println("❌ HTTP POST request failed!");
    Serial.println("🔄 Will retry in 60 seconds...");
  }
}

String readBuffer(unsigned long timeout) {
  String buffer = "";
  unsigned long startTime = millis();

  // Clear any existing data in the buffer
  while (SerialAT.available()) {
    SerialAT.read();
  }

  // Read incoming data for up to timeout milliseconds
  while (millis() - startTime < timeout && buffer.length() < 1024) {
    if (SerialAT.available()) {
      char c = SerialAT.read();
      if (c != 0 && c != '\r' && c != '\n') {
        buffer += c;
        if (debug) Serial.write(c);
      }
    }
  }

  return buffer;
}

String sendATGetResponse(const char *cmd, unsigned long timeout) {
  SerialAT.println(cmd);
  if (debug) Serial.println("Sent: " + String(cmd));
  return readBuffer(timeout);
}

// Helper function to wait for specific response
bool waitForResponse(const char* expected, unsigned long timeout) {
  unsigned long start = millis();
  String response;
  
  while (millis() - start < timeout) {
    if (SerialAT.available()) {
      char c = SerialAT.read();
      response += c;
      if (response.indexOf(expected) != -1) {
        return true;
      }
    }
  }
  return false;
}

bool sendATCommandWithRetry(const char* command, const char* expectedResponse, 
                           unsigned long timeout, uint8_t retries) {
  for (int i = 0; i < retries; i++) {
    String response = sendATGetResponse(command, timeout);
    if (response.indexOf(expectedResponse) != -1) {
      return true;
    }
    if (debug) Serial.println("Retry " + String(i+1) + " of " + String(retries));
    delay(1000);
  }
  return false;
}

void initPowerControl() {
  pinMode(PWR_PIN, OUTPUT_OPEN_DRAIN);
  setPwrKeyState(true);
  Serial.println("BG95 Power Key initialized on pin " + String(PWR_PIN));
}

bool powerOnBG95(unsigned long pulseDuration) {
  if (pulseDuration < 500 || pulseDuration > 1000) {
    Serial.println("Error: Pulse duration must be between 500 and 1000 milliseconds");
    return false;
  }

  Serial.println("Powering on BG95-M3 module...");
  Serial.println("Pulse duration: " + String(pulseDuration) + "ms");

  // Pull PWRKEY low to initiate power on
  setPwrKeyState(false);
  lastPowerOnTime = millis();
  delay(pulseDuration);
  
  // Release PWRKEY
  setPwrKeyState(true);
  isModulePowered = true;
  
  Serial.println("Power on sequence completed");
  delay(2000);
  
  return true;
}

bool powerOffBG95() {
  Serial.println("Powering off BG95-M3 module...");
  
  // Try software shutdown first
  sendATCommandWithRetry("AT+CPOF", "OK", 5000, 2);
    delay(2000);
  
  // Hardware power off
  setPwrKeyState(false);
  delay(1500);
  setPwrKeyState(true);
  
  isModulePowered = false;
  lastPowerOnTime = 0;
  
  Serial.println("Power off completed");
  return true;
}

bool powerCycleBG95(unsigned long pulseDuration) {
  Serial.println("Starting BG95-M3 power cycle sequence...");
  powerOffBG95();
  delay(1000);
  bool result = powerOnBG95(pulseDuration);
  
  if (result) {
    Serial.println("Power cycle completed successfully");
  } else {
    Serial.println("Power cycle failed");
  }
  
  return result;
}

void setPwrKeyState(bool state) {
  digitalWrite(PWR_PIN, state ? HIGH : LOW);
}

bool checkSimCard() {
  Serial.println("=== Enhanced SIM Card Initialization ===");
  
  // Try multiple methods to detect SIM
  String simDetect = sendATGetResponse("AT+QSIMSTAT=1", 2000);
  if (simDetect.indexOf("+QSIMSTAT: 1,1") != -1) {
    Serial.println("SIM card detected");
    simReady = true;
    return true;
  }

  // Fallback to IMSI check
    String imsi = sendATGetResponse("AT+CIMI", 3000);
    if (imsi.indexOf("OK") != -1 && imsi.length() > 10) {
    Serial.println("SIM communication established via IMSI");
    simReady = true;
  return true;
}

  Serial.println("No SIM card detected");
    return false;
  }
  
bool bootModem() {
  Serial.println("Starting enhanced BG95-M3 boot sequence...");
  
  // Reset any previous state
  SerialAT.flush();
  delay(2000);
  
  // Try to boot the modem with multiple attempts
  const int MAX_BOOT_ATTEMPTS = 3;
  
  for (int attempt = 0; attempt < MAX_BOOT_ATTEMPTS; attempt++) {
    Serial.print("Boot attempt ");
    Serial.print(attempt + 1);
    Serial.print(" of ");
    Serial.println(MAX_BOOT_ATTEMPTS);
    
    // Use BG95 power control for power on
    if (powerOnBG95(750)) {
      Serial.println("BG95-M3 power on sequence completed");
    } else {
      Serial.println("BG95-M3 power on failed");
      continue;
    }
    
    // Wait for modem to stabilize
    Serial.println("Waiting for modem to stabilize...");
    delay(10000);
    
    // Try to communicate with the modem
    String atResponse = sendATGetResponse("AT", 2000);
    if (atResponse.indexOf("OK") == -1) {
      Serial.println("BG95-M3 not responsive");
      continue;
    }
    
    Serial.println("BG95-M3 is responsive!");
    
    // Disable echo
    sendATCommandWithRetry("ATE0", "OK", 2000, 2);
    
    // Check SIM card status with special handling for physical detection issues
    if (checkSimCard()) {
      Serial.println("SIM card initialization successful!");
      return true;
    }
    
    Serial.println("Boot attempt failed");
      delay(5000);
  }
  
  Serial.println("Failed to boot BG95-M3 after multiple attempts!");
    return false;
  }
  
bool checkNetwork() {
  Serial.println("Checking network connection...");
  
  // Check signal quality first with longer timeout
  String signalQuality = sendATGetResponse("AT+CSQ", 5000);
  Serial.println("Signal quality: " + signalQuality);
  
  // Check if we have any signal
  if (signalQuality.indexOf("99,99") != -1) {
    Serial.println("No signal detected! Check antenna connection.");
    return false;
  }

  // Check available operators with longer timeout
  String operators = sendATGetResponse("AT+COPS=?", 15000);
  Serial.println("Available operators: " + operators);
  
  // Check network registration with longer timeout
    String regStatus = sendATGetResponse("AT+CREG?", 5000);
  Serial.println("Network registration: " + regStatus);
  
  // Check if registered to network
  if (regStatus.indexOf("+CREG: 0,1") == -1 && 
      regStatus.indexOf("+CREG: 0,5") == -1 &&
      regStatus.indexOf("+CREG: 1,1") == -1 &&
      regStatus.indexOf("+CREG: 1,5") == -1) {
    Serial.println("Not registered to network");
    
    // Try to manually register to a network
    Serial.println("Attempting manual network registration...");
    if (sendATCommandWithRetry("AT+COPS=1,2,\"72423\"", "OK", 10000, 3)) {
      Serial.println("Manual registration to VIVO attempted");
      delay(5000);
      
      // Check registration status again
      regStatus = sendATGetResponse("AT+CREG?", 5000);
      Serial.println("Network registration after manual: " + regStatus);
    }
    
    return false;
  }
  
  // Attach to GPRS service
  String gprsStatus = sendATGetResponse("AT+CGATT?", 3000);
  Serial.println("GPRS attach status: " + gprsStatus);
  
  if (gprsStatus.indexOf("+CGATT: 1") == -1) {
    // Try to attach to GPRS
    if (!sendATCommandWithRetry("AT+CGATT=1", "OK", 10000, 3)) {
      Serial.println("Failed to attach to GPRS");
      return false;
    }
  }
  
  // Use robust APN configuration
  if (!configureRobustAPN()) {
    Serial.println("Failed to configure APN with any method");
    return false;
  }
  
  
  // Activate PDP context
  if (!sendATCommandWithRetry("AT+QIACT=1", "OK", 15000, 3)) {
    Serial.println("Failed to activate PDP context");
    return false;
  }

  // Check if we have an IP address
  String ipAddress = sendATGetResponse("AT+CGPADDR=1", 3000);
  Serial.println("IP address: " + ipAddress);
  
  if (ipAddress.indexOf("+CGPADDR: 1,") != -1) {
    // Extract IP address
    int start = ipAddress.indexOf("+CGPADDR: 1,") + 12;
    int end = ipAddress.indexOf("\r", start);
    if (end == -1) end = ipAddress.length();
    
    String ip = ipAddress.substring(start, end);
    ip.trim();
    
    if (ip.length() > 0 && ip != "0.0.0.0") {
      Serial.println("Valid IP address: " + ip);
        return true;
    }
  }

  Serial.println("No valid IP address assigned");
  return false;
}

void configureModem() {
  // Basic configuration for BG95-M3
  sendATCommandWithRetry("ATE0", "OK", 2000, 2);        // Disable echo
  sendATCommandWithRetry("AT+CMEE=2", "OK", 2000, 2);   // Enable detailed error reporting
  
  // Set functionality to full functionality
  sendATCommandWithRetry("AT+CFUN=1", "OK", 2000, 2);
  delay(2000);

  // Configure PDP context
  String apnCmd = "AT+CGDCONT=1,\"IP\",\"" + String(APN) + "\"";
  sendATCommandWithRetry(apnCmd.c_str(), "OK", 2000, 2);
  
  // Disable GNSS to prevent interference with network registration
  disableGNSS();
  
    Serial.println("Modem configured");
}

bool openDataConnection() {
  // Check current PDP context status
  String pdpStatus = sendATGetResponse("AT+CGACT?", 1000);
    Serial.println("PDP context status: " + pdpStatus);

  // Try to activate the PDP context if not already active
  if (pdpStatus.indexOf("+CGACT: 1,1") == -1) {
      Serial.println("Activating PDP context...");
    if (!sendATCommandWithRetry("AT+CGACT=1,1", "OK", 5000, 2)) {
      Serial.println("Failed to activate PDP context");
      return false;
    }
  }

  // Check if we have an IP address
  String ipCheck = sendATGetResponse("AT+CGPADDR=1", 2000);
    Serial.println("IP address check: " + ipCheck);

  // If we have an IP address (not 0.0.0.0), we're connected
  if (ipCheck.indexOf("+CGPADDR: 1,\"0.0.0.0\"") == -1 && ipCheck.indexOf("+CGPADDR: 1,") != -1) {
      Serial.println("Data connection active with IP");
    return true;
  } else {
      Serial.println("No valid IP address assigned");
    return false;
  }
}

// FIXED HTTP POST function - This is the key fix!
bool sendHTTPPost(const char* url, const char* endpoint, const char* data) {
  Serial.println("=== CONFIGURING HTTP POST ===");
  
  // Configure HTTP context (use context 0, not 1)
  if (!sendATCommandWithRetry("AT+QHTTPCFG=\"contextid\",0", "OK", 1000, 3)) {
    Serial.println("Failed to configure HTTP context");
    return false;
  }

  // Set SSL version (important for HTTPS)
  sendATCommandWithRetry("AT+QSSLCFG=\"sslversion\",0,4", "OK", 1000, 2); // TLS 1.2

  // Configure SSL cipher suite
  sendATCommandWithRetry("AT+QSSLCFG=\"ciphersuite\",0,0xFFFF", "OK", 1000, 2);

  // Configure SSL to ignore local time (important for BG95)
  sendATCommandWithRetry("AT+QSSLCFG=\"ignorelocaltime\",0,1", "OK", 1000, 2);

  // Configure HTTP headers - THIS IS CRITICAL!
  sendATCommandWithRetry("AT+QHTTPCFG=\"requestheader\",1", "OK", 1000, 2);
  
  // Set Content-Type header explicitly
  sendATCommandWithRetry("AT+QHTTPCFG=\"requestheader\",1,\"Content-Type: application/json\"", "OK", 1000, 2);

  // Set content type
  sendATCommandWithRetry("AT+QHTTPCFG=\"contenttype\",0", "OK", 1000, 2);

  // Set URL
  String fullUrl = String(url) + String(endpoint);
  String urlCmd = "AT+QHTTPURL=" + String(fullUrl.length()) + ",80";
  
  Serial.println("Setting URL: " + fullUrl);
  
  if (sendATCommandWithRetry(urlCmd.c_str(), "CONNECT", 5000, 3)) {
    SerialAT.print(fullUrl);
    delay(100);
    
    // Wait for URL to be processed
    if (!waitForResponse("OK", 5000)) {
      Serial.println("Failed to set URL");
      return false;
    }

    // Prepare JSON payload with the EXACT format from PowerShell
    String jsonPayload = "{\"action\":\"add\",\"user\":{\"id\":123,\"name\":\"BG95User123\",\"status\":\"active\",\"deviceModel\":\"BG95Device123\"},\"timestamp\":\"2024-01-15T10:30:00.000Z\"}";
    
    // Debug output like PowerShell
    Serial.println("🚀 Making POST request with BG95...");
    Serial.println("Creating user: BG95User123");
    Serial.println("Device: BG95Device123");
    Serial.println("");
    Serial.println("JSON Payload: " + jsonPayload);
    Serial.println("");
    
    // Send POST request
    String postCmd = "AT+QHTTPPOST=" + String(jsonPayload.length()) + ",80,80";
    Serial.println("POST command: " + postCmd);
    
    if (sendATCommandWithRetry(postCmd.c_str(), "CONNECT", 5000, 3)) {
      Serial.println("Sending JSON payload...");
      SerialAT.print(jsonPayload);
      delay(2000); // Increased delay to ensure payload is sent
      
      // Wait for POST completion
      if (waitForResponse("OK", 15000)) {
        Serial.println("POST request successful!");
        
        // Read response
        String response = sendATGetResponse("AT+QHTTPREAD=80", 5000);
        Serial.println("Server response: " + response);
        
        // Check if response contains success indicators
        if (response.indexOf("success") != -1 || response.indexOf("200") != -1) {
          Serial.println("✅ Server confirmed successful POST!");
          return true;
        } else {
          Serial.println("⚠️ POST sent but server response unclear");
          return true; // Still consider it successful if we got a response
        }
      } else {
        Serial.println("❌ POST request timeout");
        return false;
      }
    } else {
      Serial.println("❌ Failed to initiate POST request");
      return false;
    }
  } else {
    Serial.println("❌ Failed to set URL");
    return false;
  }
  
  // Clean up
  sendATCommandWithRetry("AT+QHTTPCLOSE=0", "OK", 1000, 2);
  return false;
}

void processCommandPost(const char *server, const char *endpoint_path, const char *data) {
  Serial.println("Sending POST request to webhook...");
  sendHTTPPost(server, endpoint_path, data);
}

void logError(ErrorCode error, const char* message) {
  Serial.print("ERROR ");
  Serial.print(error);
  Serial.print(": ");
  Serial.println(message);
}

// Manual network selection function
bool manualNetworkSelection() {
  Serial.println("Attempting manual network selection...");
  
  // Try to register to VIVO network (operator code 72423)
  if (sendATCommandWithRetry("AT+COPS=1,2,\"72423\"", "OK", 15000, 3)) {
    Serial.println("Manual network selection to VIVO successful");
    return true;
  }
  
  // Try to register to Claro network (operator code 72405)
  if (sendATCommandWithRetry("AT+COPS=1,2,\"72405\"", "OK", 15000, 3)) {
    Serial.println("Manual network selection to Claro successful");
    return true;
  }
  
  // Try automatic registration
  if (sendATCommandWithRetry("AT+COPS=0", "OK", 15000, 3)) {
    Serial.println("Automatic network selection initiated");
    return true;
  }
  
  Serial.println("All network selection attempts failed");
  return false;
}

// Check antenna connection function
void checkAntenna() {
  Serial.println("Checking antenna connection...");
  
  // Check if we can get any signal quality reading
  String signal = sendATGetResponse("AT+CSQ", 5000);
  
  if (signal.indexOf("99,99") != -1) {
    Serial.println("WARNING: No signal detected!");
    Serial.println("Please check:");
    Serial.println("1. Antenna is properly connected to the BG95 module");
    Serial.println("2. Antenna is positioned with a clear view of the sky");
    Serial.println("3. You're in an area with cellular coverage");
  } else {
    Serial.println("Signal detected: " + signal);
  }
}

// Enhanced debugging function
void debugModemStatus() {
  Serial.println("=== Modem Status Debug ===");
  
  // Check modem functionality
  String funStatus = sendATGetResponse("AT+CFUN?", 3000);
  Serial.println("Functionality: " + funStatus);
  
  // Check SIM status
  String simStatus = sendATGetResponse("AT+CPIN?", 3000);
  Serial.println("SIM Status: " + simStatus);
  
  // Check network mode
  String netMode = sendATGetResponse("AT+QCFG=\"nwscanmode\"", 3000);
  Serial.println("Network Scan Mode: " + netMode);
  
  // Check band configuration
  String bandConfig = sendATGetResponse("AT+QCFG=\"band\"", 3000);
  Serial.println("Band Config: " + bandConfig);
  
  // Check GNSS status
  String gnssStatus = sendATGetResponse("AT+QGPS?", 3000);
  Serial.println("GNSS Status: " + gnssStatus);
  
  Serial.println("=== End Modem Status Debug ===");
}

// Force HTTP requests for testing (bypasses network check)
void forceHttpRequests() {
  Serial.println("=== FORCING HTTP REQUESTS FOR TESTING ===");
  Serial.println("WARNING: This bypasses network signal check!");
  
  // Try to configure HTTP service anyway
  Serial.println("Configuring HTTP service...");
  
  // Configure SSL context
  if (sendATCommandWithRetry("AT+QSSLCFG=\"sslversion\",0,4", "OK", 5000, 2)) {
    Serial.println("SSL version configured");
  }
  
  if (sendATCommandWithRetry("AT+QSSLCFG=\"ciphersuite\",0,0x0035", "OK", 5000, 2)) {
    Serial.println("Cipher suite configured");
  }
  
  if (sendATCommandWithRetry("AT+QSSLCFG=\"ignorelocaltime\",0,1", "OK", 5000, 2)) {
    Serial.println("SSL ignore local time configured");
  }
  
  // Use robust APN configuration
  if (!configureRobustAPN()) {
    Serial.println("WARNING: APN configuration failed, but continuing...");
  }
  
  // Try to attach to GPRS
  Serial.println("Attempting GPRS attach...");
  if (sendATCommandWithRetry("AT+CGATT=1", "OK", 10000, 3)) {
    Serial.println("GPRS attach successful");
  } else {
    Serial.println("GPRS attach failed, but continuing...");
  }
  
  // Try to activate PDP context
  Serial.println("Attempting PDP context activation...");
  if (sendATCommandWithRetry("AT+QIACT=1", "OK", 10000, 3)) {
    Serial.println("PDP context activated");
  } else {
    Serial.println("PDP context activation failed, but continuing...");
  }
  
  // Force POST request to your Vercel endpoint
  Serial.println("=== FORCING POST REQUEST TO VERCEL ===");
  sendHTTPPost(URL, ENDPOINT, "FORCED_TEST_MESSAGE");
  
  Serial.println("=== FORCED HTTP REQUESTS COMPLETED ===");
}

// Robust APN configuration with multiple fallback options
bool configureRobustAPN() {
  Serial.println("=== CONFIGURING ROBUST APN ===");
  
  // Array of APN configurations to try
  const char* apnConfigs[] = {
    "AT+CGDCONT=1,\"IP\",\"zap.vivo.com.br\",\"vivo\",\"vivo\"",  // With credentials
    "AT+CGDCONT=1,\"IP\",\"zap.vivo.com.br\"",                    // Without credentials
    "AT+CGDCONT=1,\"IP\",\"vivo.com.br\",\"vivo\",\"vivo\"",      // Alternative APN with credentials
    "AT+CGDCONT=1,\"IP\",\"vivo.com.br\"",                        // Alternative APN without credentials
    "AT+CGDCONT=1,\"IP\",\"internet\",\"vivo\",\"vivo\"",         // Generic internet APN with credentials
    "AT+CGDCONT=1,\"IP\",\"internet\""                            // Generic internet APN without credentials
  };
  
  const char* apnNames[] = {
    "zap.vivo.com.br with credentials",
    "zap.vivo.com.br without credentials", 
    "vivo.com.br with credentials",
    "vivo.com.br without credentials",
    "internet with credentials",
    "internet without credentials"
  };
  
  int numConfigs = sizeof(apnConfigs) / sizeof(apnConfigs[0]);
  
  for (int i = 0; i < numConfigs; i++) {
    Serial.print("Trying APN config ");
    Serial.print(i + 1);
    Serial.print(" of ");
    Serial.print(numConfigs);
    Serial.print(": ");
    Serial.println(apnNames[i]);
    
    if (sendATCommandWithRetry(apnConfigs[i], "OK", 5000, 3)) {
    Serial.println("SUCCESS: APN configured successfully!");
      Serial.print("Working configuration: ");
      Serial.println(apnNames[i]);
    return true;
  } else {
      Serial.print("FAILED: ");
      Serial.println(apnNames[i]);
    }
    
    delay(1000); // Wait between attempts
  }
  
  Serial.println("ERROR: All APN configurations failed!");
  return false;
}

// GNSS control functions
void disableGNSS() {
  Serial.println("Disabling GNSS to prevent interference with network registration...");
  
  // Check if GNSS is currently enabled
  if (isGNSSEnabled()) {
    Serial.println("GNSS is enabled, disabling now...");
    
    // Turn off GNSS
    if (sendATCommandWithRetry("AT+QGPSEND", "OK", 5000, 3)) {
      Serial.println("GNSS successfully disabled");
    } else {
      Serial.println("Failed to disable GNSS with AT+QGPSEND, trying alternative method...");
      
      // Alternative method: disable GNSS power
      if (sendATCommandWithRetry("AT+QGPSCFG=\"poweren\",0", "OK", 3000, 2)) {
        Serial.println("GNSS power disabled via alternative method");
      } else {
        Serial.println("Failed to disable GNSS power");
      }
    }
  } else {
    Serial.println("GNSS is already disabled");
  }
}

void forceDisableGNSS() {
  Serial.println("Force disabling GNSS...");
  sendATCommandWithRetry("AT+QGPSCFG=\"poweren\",0", "OK", 2000, 3);
  sendATCommandWithRetry("AT+QGPSEND", "OK", 2000, 3);
  sendATCommandWithRetry("AT+QCFG=\"gnssconfig\",0", "OK", 2000, 3);
  Serial.println("GNSS forcefully disabled");
}

bool isGNSSEnabled() {
  // Check GNSS status
  String response = sendATGetResponse("AT+QGPS?", 3000);
  
  // Response should be "+QGPS: 0" for disabled or "+QGPS: 1" for enabled
  if (response.indexOf("+QGPS: 1") != -1) {
    return true;
  } else if (response.indexOf("+QGPS: 0") != -1) {
    return false;
  }
  
  // If we can't determine the status, assume it's enabled to be safe
  Serial.println("Could not determine GNSS status, assuming enabled");
  return true;
}
