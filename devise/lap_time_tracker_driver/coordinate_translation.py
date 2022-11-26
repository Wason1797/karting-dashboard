import math as mt
from functools import lru_cache

import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
import numpy as np
import pprint

@lru_cache
def cached_cos(angle: float) -> float:
    return mt.cos(angle)

@lru_cache
def cached_sin(angle: float) -> float:
    return mt.sin(angle)


class Point:

    def __init__(self, x:float, y:float) -> None:
        self.x = x
        self.y = y

    def __iter__(self):
        return self.x, self.y

    def __sub__(self, other: 'Point') -> 'Point':
        return Point(self.x - other.x, self.y - other.y)

    def __add__(self, other: 'Point') -> 'Point':
        return Point(self.x + other.x, self.y + other.y)

    def rotate(self, angle: float) -> 'Point':
        return Point(
            cached_cos(angle) * self.x - cached_sin(angle)*self.y,
            cached_cos(angle) * self.y + cached_sin(angle)*self.x,
        )

    def __repr__(self) -> str:
        return f"{self.x}, {self.y}"


def translate(point:Point, origin: Point) -> Point:
    return point - origin



with open('./original.txt') as raw_coordinates:
    coordinates = [Point(*[float(str_coord.strip(' ')) for str_coord in reversed(coord.split(','))]) for coord in raw_coordinates.readlines()]


translated_coordinates = [translate(coord, coordinates[0]) for coord in coordinates]

angle_point = translated_coordinates[1]

theta =  mt.atan2(angle_point.x, angle_point.y)

print(f"theta, {theta}")
print(f"cosine, {cached_cos(theta)}")
print(f"sine, {cached_sin(theta)}")

rotated_coordinates = [coord.rotate(theta) for coord in translated_coordinates[1::]]

# pprint.pprint(rotated_coordinates)

xpoints = np.array([0]+ [coord.x*100 for coord in rotated_coordinates])
ypoints = np.array([0]+ [coord.y*100 for coord in rotated_coordinates])

rectangle = [(0.0000269496, 0.0000179663), (0.0000269496, -0.0000179663), (-0.0000269496, -0.0000179663), (-0.0000269496, 0.0000179663)]

scaled_ractangle = [(x*100, y*100) for x, y in rectangle]

if __name__ == '__main__':
    ax = plt.gca()
    ax.spines['top'].set_color('none')
    ax.spines['left'].set_position('zero')
    ax.spines['right'].set_color('none')
    ax.spines['bottom'].set_position('zero')
    ax.add_patch(Polygon(scaled_ractangle, linewidth=1, edgecolor='r', facecolor='none'))
    plt.plot(xpoints, ypoints)
    plt.grid(True)
    plt.show()




# Sin Factor
# -0.93
# Cos Factor
# 0.36
#x/y: 156.78086, -78.10077


# Angle Factor
# -1.20645

# Rectangle top right
# 0.0001125202, 0.0000089832
# Rectangle bottom_left
# -0.0001125202, -0.0000089832





# # Rectangle top right
# 0.0000269496, 0.0000179663
# Rectangle bottom_left
# -0.0000269496, -0.0000179663