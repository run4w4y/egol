/*
TODO:
    #   wall checking
    #   side detecting by odometry
    #   kicker

*/
check.ports("ABCD 234")
who_am_i = getname();
if (who_am_i == "BOBA") {
    who_aint_me = "BIBA";
    mode = 1
} else {
    who_aint_me = "BOBA";
    mode = 0
}

mt.shd.pw("A", -100, 0, 100, 0, true)
sen.setmode(3, 1)
sen.setmode(1, 1)
mt.stop("ABCD", "false")
mt.resetcount("ABCD")
//  calibration
handle = open.r("cal.txt");

COMPASS_ALPHA = tonum(readline(handle));
ALPHA_LEFT = tonum(readline(handle));
ALPHA_RIGHT = tonum(readline(handle));
STR_MAX = tonum(readline(handle));
BACK_LIGHT_VALUE = tonum(readline(handle));
STR_VALUE_2 = tonum(readline(handle));
STR_VALUE_4 = tonum(readline(handle));
FRONT_LIGHT_VALUE = tonum(readline(handle));
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
attack2 = 0
x_res = 0
y_res = 0
time_odometry = time()
stop_modes = 0
move_lock = 0
i_dx = 0
err_com_old = 0
r = 0
g = 0
b = 0
kick = 0

//  voids

//  sensors + closes
void sensors {
    attacks_count = 0;
    str_res2 = 0;
    dir2 = 0;

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

        l_f = sen.percent(3)
        l_b = sen.percent(1)
    }
}

//  Motor manager
func num alga(forward, side) {
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
    
    v_b = 0.67*s
    v_c = -0.58*f - 0.33*s 
    v_d = 0.58*f - 0.33*s

    k_m = max(abs(f), abs(s))/max(abs(v_b), max(abs(v_c), abs(v_d)))
    turn = err_com * 0.7 + (err_com - err_com_old) * 2
    err_com_old = err_com

    mt.start("B", v_b*k_m+turn)
    mt.start("C", v_c*k_m+turn)
    mt.start("D", v_d*k_m+turn)
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
motor_koefficients[1] = 1;
motor_koefficients[2] = 1;

// define previous motor values for first odometry iteration

encoders_prev = new.vector(3, 0);

// define how frequently we check new values for odometry

odometry_frequency = 50;

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
        insignificant_count = 0;
        for (i = 0; i < 3; i = i + 1) {
            encoders_delta[i] = (encoders_current[i] - encoders_prev[i]) * motor_koefficients[i];
            if (encoders_delta[i] == 0) {
                insignificant_count = insignificant_count + 1;
            }
        }

        if (insignificant_count > 1) {
            for (i = 0; i < 3; i = i + 1) {
                encoders_delta[i] = 0;
            }
        }

        // now get the x and y vectors

        x_delta = (-2 * sin(thetta) / 3) * encoders_delta[0] + (-2 * sin(pi / 3 - thetta) / 3) * encoders_delta[1] + (-2 * cos(pi / 3 + thetta) / 3) * encoders_delta[2];
        y_delta = (2 * cos(thetta) / 3) * encoders_delta[0] + (-2 * cos(pi / 3 - thetta) / 3) * encoders_delta[1] + (-2 * cos(pi / 3 + thetta) / 3) * encoders_delta[2];

        x_res = (x_res + x_delta);
        y_res = (y_res + y_delta);

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

        printupd()
        print("x", x_res)
        print("y", y_res)
        print("mode", mode)
        print("close", close)
        print("ins", insignificant_count)
    }
}

func num reset_odometry() {
    mt.resetcount("B");
    mt.resetcount("C");
    mt.resetcount("D");
    x_res = 0;
    y_res = 0;
    time_odometry = time()

    for (i = 0; i < 3; i = i + 1) {
        encoders_prev[i] = 0;
    }
}

// odometry end ----------------


new.thread = sensors
new.thread = odometry


//  main
while (true) {
    // if (kick == 1) {
    //     mt.shd.pw("A", -100, 0, 100, 0, true)
    //     kick = 0
    // }
    
    //  attack
    if (l_f > FRONT_LIGHT_VALUE) {
        bm_block = 0
        v = 100     //60       
        k = 0.5
        t_attack = time()
        while (l_f > FRONT_LIGHT_VALUE - 5) {

            // if (time() - t_attack > 1000) {
            //     mt.shd.pw("A", 100, 0, 100, 0, true)
            //     t_attack = time()
            //     kick = 1
            // }

            // if (kick == 1 and mt.isbusy("A") == "False") {
            //     mt.shd.pw("A", -100, 0, 100, 0, true)
            //     kick = 0
            // }

            // if (v < 100) {
            //     v = v + 0.4
            // }      
            
            if (k < 1) {
                k = k + 0.007
            }

            mt.spw("B", err_com * k)
            mt.spw("C", -v)
            mt.spw("D", v)
        }
    } else {
        //  move back
        if ((abs(dir - 5) > 2 or (dir == 7  and strres < 50) or (dir == 3 and strres < 25)) and bm_block == 0) {
            alga(-80, 0)    //-100

            if (l_b > BACK_LIGHT_VALUE) {
                t0 = time()
                while (time() - t0 < 300) {
                    alga(0, 80)     //100
                    tone(100, 100, 1)
                }

                t0 = time()
                while (time() - t0 < 300) {
                    alga(-80, 0)    //-100
                    tone(100, 100, 1)
                }
            }


            // if (btn.rn == "R") {
            //     flush()
                
            //     t0 = time()
            //     while (time() - t0 < 300) {
            //         alga(80, 0)
            //     }

            //     bm_block = 1
            // }
        } else {

            if (bm_block == 1) {
                // while (abs(dir - 5) > 2 or (dir == 7  and strres < 50) or (dir == 3 and strres < 25)) {
                //     alga(0, 0)
                // }
                // bm_block = 0
            } else {
                if (abs(dir - 5) > 0) {
                    //bm_block = 0

                    if ((dir == 3 and strres > STR_VALUE_2) or (dir == 7 and strres > STR_VALUE_4)) {
                        alga(0, 100*(dir - 5))
                    } else {
                        alga(70, 100*(dir - 5))
                    }

                } else {

                    // v = (STR_MAX-strres) * (FRONT_LIGHT_VALUE - l_f) * 0.8
                    
                    // if (v < 60) {
                    //     v = 60
                    // } else {
                    //     if (v > 100) {
                    //         v = 100
                    //     }
                    // }
                    
                    alga(80, 0)     //100
                    //bm_block = 0
                }
            }
        }
    }
}