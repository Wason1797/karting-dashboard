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
bool running = false;


class Stopwatch {

private:
  bool running = false;
  unsigned long start_time = 0;
  unsigned long stop_time = 0;
  unsigned long offset = 0;


public:
  Stopwatch() {}

  void start() {
    if (running) return;
    start_time = millis();
    running = true;
    stop_time = 0;
  }


  void stop() {
    if (!running) return;
    stop_time = millis();
    running = false;
  }

  void reset() {
    running = true;
    stop_time = 0;
    offset = 0;
    start_time = millis();
  }

  unsigned long elapsed() {
    if (running) return 0;
    return (stop_time - start_time) + offset;
  }

  unsigned long now() {
    if (!running) return 0;
    return (millis() - start_time) + offset;
  }

  void set_offset(unsigned long new_offset) {
    offset = new_offset;
  }

};


struct Point {
  double x;
  double y;
};


class CoordinateMath {

private:
  double rotation_angle = 0;
  Point starting_point{ 0, 0 };
  Point reference_point = { 0, 0 };

  double sin_factor = 0;
  double cos_factor = 0;


public:

  CoordinateMath() {}

  Point translate_origin(Point position) {
    return Point{
      position.x - starting_point.x, position.y - starting_point.y
    };
  }

  void set_starting_point(float latitude, float longitude) {
    starting_point.x = static_cast<double>(longitude);
    starting_point.y = static_cast<double>(latitude);
  }

  void set_reference_point(float latitude, float longitude) {
    reference_point.x = static_cast<double>(longitude);
    reference_point.y = static_cast<double>(latitude);
  }

  void calculate_rotation_factors() {
    Point translated_reference = translate_origin(reference_point);
    rotation_angle = atan2(translated_reference.y, translated_reference.x);
    sin_factor = sin(rotation_angle);
    cos_factor = cos(rotation_angle);
  }

  Point rotate(Point position) {
    return Point{
      (cos_factor * position.x - sin_factor * position.y), (cos_factor * position.y + sin_factor * position.x)
    };
  }

};



void print_gps_data() {
 
}

void print_laptime_info() {
  // TODO Implement
}

bool start_pressed() {
  return false;
}

Stopwatch stopwatch;
CoordinateMath coordinate_math;

void setup() {
  stopwatch = Stopwatch();
  coordinate_math = CoordinateMath();

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

      while (!running) {
        gps.f_get_position(&current_latitude, &current_longitude);

        if(start_pressed()){
          coordinate_math.set_starting_point(current_latitude, current_longitude);
          delay(1000);
          gps.f_get_position(&current_latitude, &current_longitude);
          coordinate_math.set_reference_point(current_latitude, current_longitude);
          stopwatch.start();
        }


      }
      if (running) {
        float speed = gps.f_speed_kmph();
      }
    }
  }
}