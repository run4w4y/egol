/*TODO:
    +1. attack
    +2. modes
    3. odometry
    4. attack angles
    5. starts 
*/

check.ports("ABCD 234")

//  BT
who_am_i = getname();

if (who_am_i == "BOBA") {
	who_aint_me = "BIBA";
} else {
	who_aint_me = "BOBA";
}

connect(who_aint_me);

mailbox_biba = new.mailbox("mailbox_biba");
mailbox_boba = new.mailbox("mailbox_boba");
mailbox_mode = new.mailbox("mailbox_mode");

sen.setmode(3, 1)
mt.stop("ABCD", "false")
mt.spw("a", 50)

//  calibration
handle = open.r("cal.txt");

COMPASS_ALPHA = tonum(readline(handle));
SEEKER_DIV = tonum(readline(handle)) / 100;
KP_MOVE = tonum(readline(handle))/100;
STR_MAX = tonum(readline(handle));
BACK_LIGHT_VALUE = tonum(readline(handle));
STR_VALUE_2 = tonum(readline(handle));
STR_VALUE_4 = tonum(readline(handle));

closef(handle);

//  variables
PORT_FIRST_MOTOR = "B"
PORT_SECOND_MOTOR = "D"
PORT_THIRD_MOTOR = "C"
alpha = COMPASS_ALPHA
forward = 0
side = 0
v = 50
f = 0
s = 0
close = 0 
attack = 0
mode = 1

/*  
    close = 0 stop
    close = 1 ball is behind
    close = 2 ball is in side
    close = 3 ball is in front
    close = 4 attack
    close = 5 avoid ball
    close = 6 diagonal
*/

//  voids

//  sensors + closes
void sensors {
    while (true) {
        irseeker_array = i2c.readregs(4, 8, 73, 6)
        dir = irseeker_array[0]
        str1 = irseeker_array[1]
        str2 = irseeker_array[2]
        str3 = irseeker_array[3]
        str4 = irseeker_array[4]
        str5 = irseeker_array[5]

        // if (rm(dir,2) == 0) {
        //   strres = (str1 + str2 + str3 + str4 + str5)/1.57 //1.9
        // } else {
        //   strres = str1 + str2 + str3 + str4 + str5
        // }

        strres = irseeker_array[1]
        for (i_dx = 2; i_dx < 6; i_dx++) {
            strres = max(strres, irseeker_array[i_dx])
        } 

        compass_array = i2c.readregs(2, 1, 66, 4)
        compass = compass_array[0] * 2 + compass_array[1]
        err_com = rm(compass - alpha + 900, 360) - 180
        turn = err_com

        l_b = sen.percent(3)
        // FSM

        if (mode == 1) {
            led(2)
            if (dir == 5) {
                close = 3
                if (str3 > STR_MAX - 5) {
                    close = 4
                }
            } else {
                if (abs(dir - 5) < 3 and strres > 30) {
                    close = 2
                    if (str3 > STR_MAX - 5) {
                        close = 4
                    }
                } else {
                    close = 1
                    if (l_b > BACK_LIGHT_VALUE) {
                        close = 5
                    }
                }
            }
        } else {
            led(1)
            if (abs(dir - 5) < 3 and (str4 > 30 or str2 > 30)) {
                close = 0
            } else {
                close = 1
                if (l_b > BACK_LIGHT_VALUE) {
                    close = 5
                }
            }
        }
    }
}

//  Motor manager
void MM {
    while (true) {
        while (attack == 0) {
            if (abs(forward) > 100) {
                f = 100 * forward / abs(forward)
            } else {
                f = forward
            }

            if (abs(side) > 100) {
                s = 100 * side / abs(side)
            } else {
                s = side
            }
            
            v_b = 1.2*(-0.67*s)
            v_c = 0.58*f + 0.33*s 
            v_d = 0.58*f - 0.33*s

            k_m = max(abs(f), abs(s))/max(abs(v_b), max(abs(v_c), abs(v_d)))

            mt.spw("B", v_b*k_m-err_com*1.5)
            mt.spw("C", v_c*k_m-err_com*0.8)
            mt.spw("D", v_d*k_m+err_com*0.8)
        }
    }
}

// odometry start -----------

// first we state the motor ports

motor_ports = new.vector(3, 0);
motor_ports[0] = todec(tohex(tonum("B") + 9));
motor_ports[1] = todec(tohex(tonum("D") + 9));
motor_ports[2] = todec(tohex(tonum("C") + 9));

// then we define the thetta and robot base

thetta_base = -3*pi/2;
base_length = 1;

// then we define motor koefficients

motor_koefficients = new.vector(3, 0);
motor_koefficients[0] = 1;
motor_koefficients[1] = -1.5;
motor_koefficients[2] = 1.5;

// define previous motor values for first odometry iteration

encoders_prev = new.vector(3, 0);

// define how frequently we check new values for odometry

odometry_frequency = 50;

// define x_res and y_res variables 

x_res = 0;
y_res = 0;

// main odometry thread

void odometry {
    while (true) {
        // define current thetta

        current_angle_delta = err_com;
        thetta = rad(current_angle_delta) + thetta_base;

        // read encoders

        encoders_current = new.vector(3, 0);

        encoders_current[0] = mt.getcount(PORT_FIRST_MOTOR);
        encoders_current[1] = mt.getcount(PORT_SECOND_MOTOR);
        encoders_current[2] = mt.getcount(PORT_THIRD_MOTOR);

        // calculate encoders delta

        encoders_delta = new.vector(3, 0);
        for (i = 0; i < 3; i = i + 1) {
            encoders_delta[i] = (encoders_current[i] - encoders_prev[i]) * motor_koefficients[i];
        }

        // now get the x and y vectors

        x_delta = (-2 * sin(thetta) / 3) * encoders_delta[0] + (-2 * sin(pi / 3 - thetta) / 3) * encoders_delta[1] + (-2 * cos(pi / 3 + thetta) / 3) * encoders_delta[2];
        y_delta = (2 * cos(thetta) / 3) * encoders_delta[0] + (-2 * cos(pi / 3 - thetta) / 3) * encoders_delta[1] + (-2 * cos(pi / 3 + thetta) / 3) * encoders_delta[2];

        x_res = x_res + x_delta;
        y_res = y_res + y_delta;

        // save current encoder values for future

        for (i = 0; i < 3; i = i + 1) {
            encoders_prev[i] = encoders_current[i];
        }

        // wait for new encoder values
        delay(odometry_frequency);

        if (x_res > 1100) {
            x = 1100
        } else {
            if (x_res < -1100) {
                x = -1100
            }
        }
        
    }
}

// odometry end ----------------

// odometry end ----------------


new.thread = sensors
new.thread = odometry

while (true) {
    printupd()
    print("x", x_res)
    print("y", y_res)
}