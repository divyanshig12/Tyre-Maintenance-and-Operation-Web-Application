// Firebase credentials
#define FIREBASE_HOST "majorproject-4aa9d-default-rtdb.firebaseio.com/" // Your Firebase Realtime Database URL
#define FIREBASE_AUTH "your-database-secret" // Your Firebase Database secret (not API_KEY)

FirebaseData firebaseData; // Firebase data object

void setup() {
  // Start serial communication
  Serial.begin(115200);

  // Initialize DHT sensor
  dht.begin();

  // Initialize HX710B sensor
  // if (!air_press.init()) {
  //   Serial.println(F("HX710B not Found!"));
  //   while (1);
  // }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // Initialize Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

uint32_t time_update = 0;

void loop() {
  // Read temperature from DHT sensor
  float temperature = dht.readTemperature();
  
  if (isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    // Send temperature to Firebase
    String tempPath = "/Truck 1/Temperature"; // Firebase path
    if (Firebase.setFloat(firebaseData, tempPath, temperature)) {
      Serial.println("Temperature sent to Firebase successfully!");
    } else {
      Serial.print("Error sending temperature: ");
      Serial.println(firebaseData.errorReason());
    }
  }

  // Check timing for pressure sensor update
  // if (millis() - time_update >= 2000UL) {
  //   uint32_t pressureRaw = 0;

  //   // Read pressure from HX710B sensor
  //   if (air_press.read(&pressureRaw, 1000UL) != HX710B_OK) {
  //     Serial.println(F("Failed to read from HX710B sensor!"));
  //   } else {
  //     Serial.print("Raw Pressure Data: ");
  //     Serial.println(pressureRaw);

  //     // Send pressure to Firebase
  //     String pressurePath = "/Truck 1/Pressure"; // Firebase path
  //     if (Firebase.setInt(firebaseData, pressurePath, pressureRaw)) {
  //       Serial.println("Pressure sent to Firebase successfully!");
  //     } else {
  //       Serial.print("Error sending pressure: ");
  //       Serial.println(firebaseData.errorReason());
  //     }
  //   }

  //   // Update the timing
  //   time_update = millis();
  // }

  // Delay for stability
  delay(1000);
}
#include <FirebaseESP32.h>
#include <WiFi.h>

// Replace these with your Firebase project credentials
#define FIREBASE_HOST "https://majorproject-4aa9d-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRET"

// Replace these with your Wi-Fi credentials
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

int revolutionCount = 0; // Counter to track the number of revolutions
bool isHigh = false;     // Flag to detect high threshold crossing
float distancePerRevolution = 0.001755; // Distance per revolution in KM

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" connected!");

  // Configure Firebase
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;

  // Initialize Firebase
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Check if startKey is true in the Firebase database
  if (Firebase.RTDB.getBool(&firebaseData, "/Truck 1/startKey")) {
    bool startKey = firebaseData.boolData();

    if (startKey) {
      int hallValue = hallRead();  // Read the raw value from the hall effect sensor

      // Check for threshold crossing to calculate revolutions
      if (hallValue > 100) {
        isHigh = true;  // Set the flag when hallValue exceeds the high threshold
      } else if (hallValue < 100 && isHigh) {
        // Increment the revolution counter if it drops below the low threshold
        revolutionCount++;
        isHigh = false;  // Reset the flag

        // Calculate distance traveled
        float distance = revolutionCount * distancePerRevolution;

        // Send the distance to Firebase
        if (Firebase.RTDB.setFloat(&firebaseData, "/Truck 1/Distance", distance)) {
          Serial.print("Distance updated: ");
          Serial.print(distance);
          Serial.println(" KM");
        } else {
          Serial.print("Failed to update distance: ");
          Serial.println(firebaseData.errorReason());
        }
      }
    }
  } else {
    Serial.print("Failed to get startKey: ");
    Serial.println(firebaseData.errorReason());
  }

  delay(100); // Wait 100ms before the next reading
}
