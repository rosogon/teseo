import sys
from collections import namedtuple
from typing import NewType, NamedTuple

#
# xr: value on right
# xl: value on left
# xi: value on inner
# xo: value on outer
# x?i : initial value
# x?f : final value
# x_ : x' (value at curve coords)
#
from math import (
    pi,
    hypot,
    sin,
    cos,
    atan2,
    degrees,
    radians,
)


debug_enabled = False


def debug(*arg, **kwargs):
    if (debug_enabled):
        print(*arg, **kwargs, file=sys.stderr)


Degrees = NewType("Angle", float)

DELTA_SECONDS = 0.5
WIDTH_CM = 10
MAX_CM_PER_S = 10
MAX_SPEED = 255


class Pos(NamedTuple):
    x: float
    y: float

    def __add__(self, other):
        return Pos(self.x + other.x, self.y + other.y)

    def __sub__(self, other):
        return Pos(self.x - other.x, self.y - other.y)

    def __mul__(self, other):
        return Pos(self.x * other, self.y * other)

    
Origin = namedtuple("Origin", ("pos", "angle"))


Wheel = namedtuple("Wheel", ("pos", "space"))


def calc_forward_vector(pl, pr):
    wheel_axis = pl - pr
    #mod_v = hypot(*v)

    return Pos(wheel_axis.y, -wheel_axis.x)


def calc_forward_angle(pl, pr):
    direction = calc_forward_vector(pl, pr)
    angle = degrees(atan2(direction.y, direction.x))
    return angle


def calc_curve_angle(so: float, si: float, w: float) -> Degrees:
    return 360 * (so - si) / (2 * pi * w)


def calc_radius(s: float, beta: Degrees):
    return 360 * s / (2 * pi * beta)


def calc_curve_origin(p_i: Pos, p_o: Pos, r_i: float) -> Origin:
    io = p_o - p_i
    mod_io = hypot(*io)

    p = p_i - io * (r_i / mod_io)
    angle = degrees(atan2(io.y, io.x))
    return Origin(p, angle)


def rotate_axis_point(p: Pos, angle: Degrees) -> Pos:
    return Pos(
        p.x * cos(radians(angle)),
        p.x * sin(radians(angle))
    )

  
def change_coords(p_: Pos, o_: Origin, clockwise: bool) -> Pos:
        
    f = -1 if clockwise else 1

    # if  0 < o_.angle < pi, rotation is cw, so angle = - angle to make it negative
    # if -pi < o_.angle < 0, rotation is ccw, so angle = -angle to make it positive
    a = -radians(o_.angle)
    x_, y_ = (p_.x, f * p_.y)

    x = + x_ * cos(a) + y_ * sin(a)
    y = - x_ * sin(a) + y_ * cos(a)

    return Pos(x, y) + o_.pos


def calc_outer_inner(wl, wr):
    if wl.space > wr.space:
        return (wl, wr, True)
    elif wr.space > wl.space:
        return (wr, wl, False)
    else:
        # don't care
        return (wl, wr, False)
    

def calc_final_position(wl, wr, w):
    def log_variables():
        debug("cw:", cw)
        debug("alfa:", alfa)
        debug("beta:", beta)
        debug("ri", ri)
        debug("oi_", oi_)
        debug("oo_", oo_)

    ri = ro = 0
    wo, wi, cw = calc_outer_inner(wl, wr)

    so, poi = wo.space, wo.pos
    si, pii = wi.space, wi.pos

    alfa = calc_forward_angle(wl.pos, wr.pos)
    beta = calc_curve_angle(so, si, w)

    if beta != 0:
        ri = calc_radius(si, beta)
        ro = calc_radius(so, beta)
        oi_ = oo_ = calc_curve_origin(pii, poi, ri)
        pii_ = Pos(ri, 0)
        poi_ = Pos(ro, 0)
        pif_ = rotate_axis_point(pii_, beta)
        pof_ = rotate_axis_point(poi_, beta)
    else: 
        oi_ = Origin(pii, alfa)
        oo_ = Origin(poi, alfa)
        pif_ = Pos(si, 0)   # right
        pof_ = Pos(so, 0)   # left
    pif = change_coords(pif_, oi_, cw)
    pof = change_coords(pof_, oo_, cw)
        
    if wl.space > wr.space:
        plf, prf = pof, pif
    elif wr.space > wl.space:
        prf, plf = pof, pif
    else:
        plf, prf = pof, pif

    log_variables()
    
    return plf, prf, alfa


def invert_wheels_rotation(wl, wr, w):
    def calc_inverse_angle_arc(angle, wh):
        r = calc_radius(wh.space, angle)
        c = 2 * pi * r
        s = c * (360 - angle) / 360
        return s

    wo, wi, _ = calc_outer_inner(wl, wr)    
    angle = calc_curve_angle(wo.space, wi.space, w)

    si = calc_inverse_angle_arc(angle, wi)
    so = calc_inverse_angle_arc(angle, wo)
    
    if wl.space > wr.space:
        sl, sr = si, so
    else:
        sr, sl = si, so

    return Wheel(wr.pos, sl), Wheel(wl.pos, sr)

    
def speed_to_space(s: int, t_seconds: float) -> float:
    return s * MAX_CM_PER_S * t_seconds / MAX_SPEED


def usage():
    print(f"Usage mode 1: {sys.argv[0]} <pli> <sl> <pri> <sr>", file=sys.stderr)
    print("  p[lr]i (point [left|right] initial): x,y (e.g. 16.6,1)", file=sys.stderr)
    print("  s[rl] (speed [left|right]) (e.g. 255)", file=sys.stderr)
    print()
    print(f"Usage mode 2: {sys.argv[0]} ? <alfa> <pri> <s>", file=sys.stderr)
    print("  alfa: angle of movement in degrees with respect to wall", file=sys.stderr)
    print("  pri: right point initial: x,y (e.g. 16.6,1)", file=sys.stderr)
    print("  s: speed (e.g. 255)", file=sys.stderr)


def mode1(alfa, pri, s):
    debug(f"Values: alfa={alfa} pri={pri} s={s}")
    prf = pri + Pos(s * cos(radians(alfa)), s * sin(radians(alfa)))
    plf_ = Pos(WIDTH_CM, 0)
    o_ = Origin(prf, alfa + 90)
    plf = change_coords(plf_, o_, False)
    print(f"plf={plf.x:.5f},{plf.y:.5f};prf={prf.x:.5f},{prf.y:.5f};alfa={alfa:.5f}")


def mode2(pli, sl, pri, sr):
    debug(f"Values: pli={pli} sl={sl} pri={pri} sr={sr}")

    wl = Wheel(pli, sl)
    wr = Wheel(pri, sr)

    plf, prf, alfa = calc_final_position(wl, wr, WIDTH_CM)
    print(f"plf={plf.x:.5f},{plf.y:.5f};prf={prf.x:.5f},{prf.y:.5f};alfa={alfa:.5f}")
    

def main():
    def parse_tuple(s: str) -> tuple:
        parts = s.split(",")
        if len(parts) != 2:
            raise ValueError("Invalid 2-uple of numbers: " + s)
        return tuple((float(p) for p in parts))
    
    if len(sys.argv) < 5:
        usage();
        sys.exit(1)

    is_mode1 = (sys.argv[1] == '?')
    if is_mode1:
        alfa = float(sys.argv[2])
    else:
        pli = Pos(*parse_tuple(sys.argv[1]))
        vl = int(sys.argv[2])
        sl = speed_to_space(vl, DELTA_SECONDS)
    pri = Pos(*parse_tuple(sys.argv[3]))
    vr = int(sys.argv[4])
    sr = speed_to_space(vr, DELTA_SECONDS)

    if is_mode1:
        mode1(alfa, pri, sr)
    else:
        mode2(pli, sl, pri, sr)

if __name__ == "__main__":
    main()

#wl = Wheel(Pos(16.6, 1), 11.973); wr = Wheel(Pos(17, 0.1), 13.195); w = 1;
#wl = Wheel(Pos(16.6, 1), 19.66); wr = Wheel(Pos(17, 0.1), 21.68); w = 1;
#wl = Wheel(Pos(23.8, 10.4), 13.195); wr = Wheel(Pos(22.8, 10.4), 11.973); w = 1;


#wl = Wheel(Pos(25.2, 10.5), 9.6); wr = Wheel(Pos(27.2, 10.5), 11.16); w = 2;
#wl = Wheel(Pos(23.2, 20), 11.16); wr = Wheel(Pos(21.8, 18.6), 9.6); w = 2;

#wl = Wheel(Pos(12.5, 9.2), 9.42); wr = Wheel(Pos(13.8, 14.9), 12.57); w = 3;
#wl = Wheel(Pos(2.1, 11), 12.57); wr = Wheel(Pos(3.5, 8.5), 9.42); w = 3;

#wl, wr = invert_wheels_rotation(wl, wr, w)
