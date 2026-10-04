part = "assembly";
pcb_z = 7;
pcb_t = 1.6;
height = 33;
joint = 10.6;
sensor_hole = 25.5;
button_gap = 0.35;
fit = 0.25;
lid_alpha = 1;
$fn = 96;

holes = [[3.5, 4], [30.5, 4], [3.5, 44], [30.5, 44]];
button_z = pcb_z + pcb_t + 2.5 + button_gap;

assert(pcb_z > 5 && height - 19 > pcb_z + pcb_t + 2);
assert(button_gap >= 0.2 && button_gap <= 0.6);
assert(fit >= 0.15 && fit <= 0.4);
assert(part == "base" || part == "lid" || part == "button" || part == "assembly");

module outline(inset = 0) {
    translate([17, 24])
        offset(r = 4.9 - inset)
            square([29.6, 43.6], center = true);
}

module shell(z, h, inset = 2.4) {
    translate([0, 0, z]) linear_extrude(h)
        difference() {
            outline();
            outline(inset);
        }
}

module port() {
    translate([9.5, -4, pcb_z + 0.1]) cube([15, 6, 7]);
}

module base() {
    difference() {
        union() {
            linear_extrude(2) outline();
            shell(1.9, joint - 1.9);
            translate([0, 0, joint - 0.1]) linear_extrude(1.6)
                difference() {
                    outline(1.2);
                    outline(2.4);
                }
            for (p = holes)
                translate([p[0], p[1], 1.9]) cylinder(d = 8, h = pcb_z - 1.9);
        }
        for (p = holes) {
            translate([p[0], p[1], -0.1]) cylinder(d = 2.8, h = pcb_z + 0.2);
            translate([p[0], p[1], -0.1]) cylinder(d = 5.2, h = 2.5);
        }
        port();
        translate([29, 24, pcb_z + pcb_t + 0.7]) rotate([0, 90, 0]) cylinder(d = 2.5, h = 9);
    }
}

module lid() {
    difference() {
        union() {
            shell(joint, height - joint);
            translate([0, 0, height - 2]) linear_extrude(2) outline();
            intersection() {
                translate([0, 0, pcb_z + pcb_t]) linear_extrude(height - pcb_z - pcb_t)
                    outline(2.4 + fit);
                union() for (p = holes)
                    translate([p[0], p[1], pcb_z + pcb_t])
                        cylinder(d = 8, h = height - pcb_z - pcb_t);
            }
            translate([10, 43, button_z + 1.5])
                cylinder(d = 10, h = height - 2 - button_z - 1.5 + 0.1);
        }
        translate([0, 0, joint - 0.1]) linear_extrude(1.6 + fit)
            difference() {
                outline(1.2 - fit);
                outline(2.4);
            }
        for (p = holes) {
            translate([p[0], p[1], pcb_z + pcb_t - 0.1]) cylinder(d = 2.8, h = 9.6);
            translate([p[0], p[1], pcb_z + pcb_t - 0.1])
                cylinder(d = 5.3 / cos(30), h = 2.4, $fn = 6);
        }
        translate([17, 24, height - 2.1]) cylinder(d = sensor_hole, h = 2.2);
        translate([10, 43, height - 2.1]) cylinder(d = 5.3, h = 2.2);
        translate([10, 43, button_z + 1.4])
            cylinder(d = 8, h = height - 2 - button_z - 1.4);
        port();
    }
}

module button() {
    translate([10, 43, button_z]) union() {
        cylinder(d = 2.1, h = 1);
        translate([0, 0, 0.9]) cylinder(d = 4.8, h = height + 1 - button_z - 0.9);
        translate([0, 0, height - 3.4 - button_z]) cylinder(d = 7.4, h = 1.4);
    }
}

module board() {
    color([0.13, 0.32, 0.25]) translate([17, 24, pcb_z])
        linear_extrude(pcb_t) offset(r = 2) square([30, 44], center = true);
    color("silver") translate([12.6, 0, pcb_z + pcb_t]) cube([8.8, 7, 3.3]);
    color("black") translate([7.5, 43 - 1.6, pcb_z + pcb_t]) cube([5, 3.2, 2.5]);
    color("black") translate([6, 13, pcb_z + pcb_t]) cube([8, 5, 2]);
    color("ivory") translate([4, 17, pcb_z - 3]) cube([8, 5, 3]);
    color("ivory") translate([23, 33, pcb_z - 3]) cube([5, 6, 3]);
}

module sensor() {
    color("silver") translate([17, 24, height - 17.9]) cylinder(d = 25, h = 17.9);
    color([0.1, 0.12, 0.14]) translate([17, 24, height]) cylinder(d = 28, h = 1);
    color([0.25, 0.38, 0.45]) translate([17, 24, height + 1]) cylinder(d = 23.5, h = 0.1);
}

if (part == "base") base();
if (part == "lid") translate([0, 0, height]) rotate([180, 0, 0]) lid();
if (part == "button") translate([-10, -43, -button_z]) button();
if (part == "assembly") {
    color([0.22, 0.24, 0.27]) base();
    color([0.72, 0.74, 0.76], lid_alpha) lid();
    color([0.3, 0.32, 0.35]) button();
    board();
    sensor();
}
