#include <SoftwareSerial.h>  //incluimos SoftwareSerial
#include <TinyGPS.h>         //incluimos TinyGPS

#define D1 5  // ESP RX Pin
#define D2 4  // ESP TX Pin

TinyGPS gps;
SoftwareSerial serialgps(D2, D1);


int year;
byte month, day, hour, minute, second, hundredths;

unsigned long chars;
unsigned short sentences, failed_checksum;

float current_latitude, current_longitude;
bool running = false;
bool use_real_gps = false;


const float MAX_LAP_TIME = 250000;

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
  Point origin = { 0, 0 };
  Point reference_point = { 0, 0 };

  double rectangle_actual_height = 0;
  double rectangle_actual_width = 0;

  double sin_factor = 0;
  double cos_factor = 0;

  double rectangle_half_width = 3;   //meters
  double rectangle_half_height = 2;  //meters

  double earth_radious = 6378137;

  double scale = 1;


public:

  CoordinateMath() {}

  Point translate_origin(Point position) {
    return Point{
      (position.x - starting_point.x) * scale, (position.y - starting_point.y) * scale
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
    rotation_angle = atan2(translated_reference.x, translated_reference.y);
    Serial.println("Angle Factor");
    Serial.println(rotation_angle, 5);

    sin_factor = sin(rotation_angle);
    cos_factor = cos(rotation_angle);

    Serial.println("Sin Factor");
    Serial.println(sin_factor, 8);
    Serial.println("Cos Factor");
    Serial.println(cos_factor, 8);
  }

  Point rotate(Point position) {
    return Point{
      (cos_factor * position.x - sin_factor * position.y), (cos_factor * position.y + sin_factor * position.x)
    };
  }

  double to_radians(double deg) {
    return deg * PI / 180;
  }

  double to_deg(double radian) {
    return radian * 180 / PI;
  }

  Point offset_by_meters(Point position, double x_meters = 0, double y_meters = 0) {
    double y_offset = y_meters / earth_radious;
    double x_offset = x_meters / (earth_radious * cos(to_radians(position.y)));
    return Point{ position.x + to_deg(x_offset), position.y + to_deg(y_offset) };
  }

  void set_starting_square() {
    Point top_right = translate_origin(offset_by_meters(starting_point, rectangle_half_width, rectangle_half_height));
    rectangle_actual_height = top_right.y * 2;
    rectangle_actual_width = top_right.x * 2;
  }

  bool collides_with_start_rectangle(Point position) {
    return (
      position.x >= 0 && position.x <= rectangle_actual_width && position.y >= 0 && position.y <= rectangle_actual_height);
  }
};


class DummyGPS {
private:
  Point coords[45] = {
    Point{ -0.193117, -78.487032 },
    Point{ -0.193157, -78.487017 },
    Point{ -0.193199, -78.486999 },
    Point{ -0.193254, -78.486973 },
    Point{ -0.193298, -78.486932 },
    Point{ -0.193310, -78.486864 },
    Point{ -0.193302, -78.486836 },
    Point{ -0.193290, -78.486801 },
    Point{ -0.193261, -78.486749 },
    Point{ -0.193233, -78.486699 },
    Point{ -0.193186, -78.486644 },
    Point{ -0.193118, -78.486632 },
    Point{ -0.193061, -78.486647 },
    Point{ -0.192934, -78.486706 },
    Point{ -0.192836, -78.486739 },
    Point{ -0.192806, -78.486799 },
    Point{ -0.192801, -78.486866 },
    Point{ -0.192829, -78.486915 },
    Point{ -0.192895, -78.486943 },
    Point{ -0.192952, -78.486929 },
    Point{ -0.192992, -78.486909 },
    Point{ -0.193077, -78.486870 },
    Point{ -0.193113, -78.486840 },
    Point{ -0.193098, -78.486802 },
    Point{ -0.193054, -78.486801 },
    Point{ -0.193015, -78.486824 },
    Point{ -0.192970, -78.486852 },
    Point{ -0.192915, -78.486876 },
    Point{ -0.192878, -78.486867 },
    Point{ -0.192867, -78.486823 },
    Point{ -0.192893, -78.486801 },
    Point{ -0.193028, -78.486738 },
    Point{ -0.193099, -78.486730 },
    Point{ -0.193150, -78.486750 },
    Point{ -0.193184, -78.486789 },
    Point{ -0.193197, -78.486843 },
    Point{ -0.193185, -78.486896 },
    Point{ -0.193153, -78.486934 },
    Point{ -0.193084, -78.486960 },
    Point{ -0.193007, -78.486989 },
    Point{ -0.192998, -78.487023 },
    Point{ -0.193013, -78.487058 },
    Point{ -0.193044, -78.487063 },
    Point{ -0.193086, -78.487058 },
    Point{ -0.193122, -78.487047 },
  };
  int current = 0;
  int size = 45;
public:

  DummyGPS() {}

  void get_position(float *latitude, float *longitude) {
    delay(1000);
    *latitude = static_cast<float>(coords[current].x);
    *longitude = static_cast<float>(coords[current].y);
    current = (current + 1) % size;
  }

  Point get_start_point() {
    return coords[0];
  }

  Point get_reference_point() {
    return coords[1];
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
DummyGPS dummy_gps;
float current_speed = 0;

void setup() {
  stopwatch = Stopwatch();
  coordinate_math = CoordinateMath();
  dummy_gps = DummyGPS();

  Serial.begin(115200);   //Iniciamos el puerto serie
  serialgps.begin(9600);  //Iniciamos el puerto serie del gps
  //Imprimimos:
  Serial.println();
  Serial.println("GPS dashboard");
  Serial.println(" ---Finding Signal--- ");
  Serial.println();
}



void loop() {
  unsigned long current_time = 0;
  while (serialgps.available() && use_real_gps) {
    int gps_data = serialgps.read();

    if (gps.encode(gps_data)) {
      gps.f_get_position(&current_latitude, &current_longitude);

      while (!running) {
        gps.f_get_position(&current_latitude, &current_longitude);

        if (start_pressed()) {
          coordinate_math.set_starting_point(current_latitude, current_longitude);
          delay(2000);
          gps.f_get_position(&current_latitude, &current_longitude);
          coordinate_math.set_reference_point(current_latitude, current_longitude);
          coordinate_math.calculate_rotation_factors();
          stopwatch.start();
        }
      }

      if (running) {
        current_speed = gps.f_speed_kmph();
      }
    }
  }

  bool configured = false;


  while (!use_real_gps) {

    if (!configured) {
      Point start_point = dummy_gps.get_start_point();
      Point reference_point = dummy_gps.get_reference_point();
      coordinate_math.set_starting_point(start_point.x, start_point.y);
      coordinate_math.set_reference_point(reference_point.x, reference_point.y);
      coordinate_math.calculate_rotation_factors();
      coordinate_math.set_starting_square();
      stopwatch.start();
      configured = true;
    }
    dummy_gps.get_position(&current_latitude, &current_longitude);

    Point current_coord = coordinate_math.rotate(coordinate_math.translate_origin(Point{ current_longitude, current_latitude }));
    current_time = stopwatch.now();

    while (coordinate_math.collides_with_start_rectangle(current_coord) && current_time >= MAX_LAP_TIME) {
      // Add the logic here
    }

    Serial.print("x/y: ");
    Serial.print(current_coord.x, 10);
    Serial.print(", ");
    Serial.println(current_coord.y, 10);
    delay(2000);
  }
}