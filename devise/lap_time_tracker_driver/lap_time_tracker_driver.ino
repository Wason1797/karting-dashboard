#include <SoftwareSerial.h>  //incluimos SoftwareSerial
#include <TinyGPS.h>         //incluimos TinyGPS

#define D1 5  // ESP TX Pin
#define D2 4  // ESP RX Pin

TinyGPS gps;
SoftwareSerial serialgps(D1, D2);


int year;
byte month, day, hour, minute, second, hundredths;

unsigned long chars;
unsigned short sentences, failed_checksum;
float current_latitude, current_longitude;
float closest_lat_to_start = 0;
float closest_long_to_start = 0;
float start_latitude, start_longitude;
bool running = false;
float start_lap_time = 0;
float current_lap_time = 0;
float MIN_LAP_TIME = 25;  // TODO Define this correctly
float best_distance = 0;
float stop_time = 0;

bool start_pressed() {
  // TODO: Implement start pressed
  return true;
}

float square_distance_between_coordinates(float start_lat, float start_long, float end_lat, float end_long) {
  return ((end_lat - start_lat) * (end_lat - start_lat)) - (end_long - start_long) * (end_long - start_long);
}

bool is_in_elipse(float latitude, float longitude) {
  return false;  //TODO implement
}

void start_stopwatch() {
  start_lap_time = millis();
}

void stop_stopwatch() {
 stop_time = millis();
}

void reset_stopwatch() {
 start_lap_time = 0;
 stop_time = millis();
}


void print_gps_data() {
  // TODO Output to display
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths);
  Serial.print("Fecha: ");
  Serial.print(day, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(year);
  Serial.print(" Hora: ");
  Serial.print(hour, DEC);
  Serial.print(":");
  Serial.print(minute, DEC);
  Serial.print(":");
  Serial.print(second, DEC);
  Serial.print(".");
  Serial.println(hundredths, DEC);
  Serial.print("Altitud (metros): ");
  Serial.println(gps.f_altitude());
  Serial.print("Rumbo (grados): ");
  Serial.println(gps.f_course());

  Serial.print("Satelites: ");
  Serial.println(gps.satellites());
  Serial.println();
}

void print_laptime_info() {
  // TODO Implement
}

void setup() {
  Serial.begin(115200);   //Iniciamos el puerto serie
  serialgps.begin(9600);  //Iniciamos el puerto serie del gps
  //Imprimimos:
  Serial.println();
  Serial.println("GPS dashboard");
  Serial.println(" ---Finding Signal--- ");
  Serial.println();
}

void loop() {
  while (serialgps.available()) {
    int gps_data = serialgps.read();

    if (gps.encode(gps_data)) {
      gps.f_get_position(&current_latitude, &current_longitude);

      while (!running) {  // TODO: Check if we need this loop or the ESP is fast enough to just have a conditional
        gps.f_get_position(&current_latitude, &current_longitude);
        if (start_pressed()) {
          start_latitude = current_latitude;
          start_longitude = current_longitude;
          running = true;
          start_stopwatch();
          break;
        }
        print_gps_data();
      }

      if (running) {
        float speed = gps.f_speed_kmph();
        current_lap_time = millis();
        Serial.print("Velocidad(kmph): ");
        Serial.println(speed);

        while (is_in_elipse(current_latitude, current_longitude) && current_lap_time >= MIN_LAP_TIME) {
          float current_best_distance = square_distance_between_coordinates(current_latitude, start_latitude, current_longitude, start_longitude);
          if (best_distance == 0 || current_best_distance <= best_distance) {  // ? Could use FLT_MAX to initialize best distance
            best_distance = current_best_distance;
            stop_time = current_lap_time;
          }
          gps.f_get_position(&current_latitude, &current_longitude);
        }

        if (stop_time > 0) {
          reset_stopwatch();
          stop_time = 0;
          best_distance = 0;
        }

        print_gps_data();
        print_laptime_info();
      }
    }
  }
}