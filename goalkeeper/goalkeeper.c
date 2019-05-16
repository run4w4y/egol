check.ports("ABC 1234"); // checks if everything is connected properly

// const values from calibration

handle = open.r("cal.txt"); // open the file with values

PORT_SEEKER = tonum(readline(handle));
PORT_COMPASS = tonum(readline(handle));
PORT_LIGHT = tonum(readline(handle));
PORT_BUTTON = tonum(readline(handle));
SEEKER_DELAY = tonum(readline(handle));
COMPASS_DELAY = tonum(readline(handle));
LIGHT_DELAY = tonum(readline(handle));
BUTTON_DELAY = tonum(readline(handle));
LIGHT_COLOR_NUM = tonum(readline(handle));
COMPASS_ALPHA = tonum(readline(handle));
SEEKER_DIV = tonum(readline(handle)) / 100;
PORT_MID_MOTOR_NUM = tonum(readline(handle));
PORT_LEFT_MOTOR_NUM = tonum(readline(handle));
PORT_RIGHT_MOTOR_NUM = tonum(readline(handle));
if (PORT_MID_MOTOR_NUM == 1) {
    PORT_MID_MOTOR = "A";
}
if (PORT_MID_MOTOR_NUM == 2) {
    PORT_MID_MOTOR = "B";
}
if (PORT_MID_MOTOR_NUM == 3) {
    PORT_MID_MOTOR = "C";
}
if (PORT_LEFT_MOTOR_NUM == 1) {
    PORT_LEFT_MOTOR = "A";
}
if (PORT_LEFT_MOTOR_NUM == 2) {
    PORT_LEFT_MOTOR = "B";
}
if (PORT_LEFT_MOTOR_NUM == 3) {
    PORT_LEFT_MOTOR = "C";
}
if (PORT_RIGHT_MOTOR_NUM == 1) {
    PORT_RIGHT_MOTOR = "A";
}
if (PORT_RIGHT_MOTOR_NUM == 2) {
    PORT_RIGHT_MOTOR = "B";
}
if (PORT_RIGHT_MOTOR_NUM == 3) {
    PORT_RIGHT_MOTOR = "C";
}
STR_LIMIT = tonum(readline(handle));
SEEKER_STR_MAX = tonum(readline(handle));
LIGHT_LINE_LIMIT = tonum(readline(handle));
COMPASS_LEFT_ANGLE = tonum(readline(handle));
COMPASS_RIGHT_ANGLE = tonum(readline(handle));
COMPASS_LEFT = tonum(readline(handle));
COMPASS_RIGHT = tonum(readline(handle));
STR_AWAY = 60;
CENTER_DISTANCE = 300;

// reading from sensors start

// light sensor start

sen.setmode(PORT_LIGHT, 4);

light_prev_time = time();
light_prev_value = 0;

func num light() { // get current value of the selected color based on the light sens
    if (time() - light_prev_time >= LIGHT_DELAY) {
        light_prev_time = time();
        local light_val = 0;
        light_val = sen.read.rawval(PORT_LIGHT, LIGHT_COLOR_NUM);

        light_prev_value = light_val;
        return light_val;
    } else {
       return light_prev_value;
    }
}

// light sensor end 

// compass start

compass_prev_time = time();
compass_prev_value = 0;

func num compass() { // get compass current azimut
    local compass;
    if (time() - compass_prev_time >= COMPASS_DELAY) {
        compass_prev_time = time();
        compass_array = i2c.readregs(2, 1, 66, 4);
        compass_val = compass_array[0] * 2 + compass_array[1];
        compass_prev_value = compass_val;
        return compass_val;
    } else {
        return compass_prev_value;
    }
}

func num compass_delta(compass_value) { // get compass delta based
    local error = 0;
    error = rm(compass_value - COMPASS_ALPHA + 900, 360) - 180;
    return error;
}

func num compass_delta_two(compass_value, compass_value2) { // get compass delta based
    local error = 0;
    error = rm(compass_value - compass_value2 + 900, 360) - 180;
    return error;
}

// compass end

// irseeker start

seeker_prev_time = time();
seeker_prev_dir = 0;
seeker_prev_str = 0;
seeker_prev_str1 = 0;
seeker_prev_str2 = 0;
seeker_prev_str3 = 0;
seeker_prev_str4 = 0;
seeker_prev_str5 = 0;

func num irseeker_dir() { // get seeker current dir
    if (time() - seeker_prev_time >= SEEKER_DELAY) {
        seeker_prev_time = time();
        irseeker_array = i2c.readregs(4, 8, 73, 6);
        
        dir = irseeker_array[0];
        seeker_prev_dir = dir;

        str1 = irseeker_array[1];
        seeker_prev_str1 = str1;
        str2 = irseeker_array[2];
        seeker_prev_str2 = str2;
        str3 = irseeker_array[3];
        seeker_prev_str3 = str3;
        str4 = irseeker_array[4];
        seeker_prev_str4 = str4;
        str5 = irseeker_array[5];
        seeker_prev_str1 = str5;

        if (rm(dir,2) == 0) {
            strres = (str1 + str2 + str3 + str4 + str5) / SEEKER_DIV;
        } else {
            strres = str1 + str2 + str3 + str4 + str5;
        }
        seeker_prev_str = strres;

        return dir;
    } else {
        return seeker_prev_dir;
    }
}

func num irseeker_str(strnum) { // get seeker current str
    if (time() - seeker_prev_time >= SEEKER_DELAY) {
        seeker_prev_time = time();
        irseeker_array = i2c.readregs(4, 8, 73, 6);
        
        dir = irseeker_array[0];
        seeker_prev_dir = dir;

        str1 = irseeker_array[1];
        seeker_prev_str1 = str1;
        str2 = irseeker_array[2];
        seeker_prev_str2 = str2;
        str3 = irseeker_array[3];
        seeker_prev_str3 = str3;
        str4 = irseeker_array[4];
        seeker_prev_str4 = str4;
        str5 = irseeker_array[5];
        seeker_prev_str5 = str5;

        if (rm(dir,2) == 0) {
            strres = (str1 + str2 + str3 + str4 + str5) / SEEKER_DIV;
        } else {
            strres = str1 + str2 + str3 + str4 + str5;
        }
        seeker_prev_str = strres;

        if (strnum == 0) {
            return strres;
        }
        if (strnum == 1) {
            return str1;
        }
        if (strnum == 2) {
            return str2;
        }
        if (strnum == 3) {
            return str3;
        }
        if (strnum == 4) {
            return str4;
        }
        if (strnum == 5) {
            return str5;
        }
    } else {
        if (strnum == 0) {
            return seeker_prev_str;
        }
        if (strnum == 1) {
            return seeker_prev_str1;
        }
        if (strnum == 2) {
            return seeker_prev_str2;
        }
        if (strnum == 3) {
            return seeker_prev_str3;
        }
        if (strnum == 4) {
            return seeker_prev_str4;
        }
        if (strnum == 5) {
            return seeker_prev_str5;
        }
    }
}

// irseeker end

// top button start

topbutton_prev_time = time();
topbutton_prev_value = 0;

func num button_top() { // returns 1 if the top button is pressed, else returns 0
    if (time() - topbutton_prev_time >= BUTTON_DELAY) {
        topbutton_prev_time = time();
        topbutton_value = sen.read.rawval(PORT_BUTTON, 0);
        if (topbutton_value > 1000) {
            topbutton_prev_value = 1;
            return 1;
        } else {
            topbutton_prev_value = 0;
            return 0;
        }
    } else {
        return topbutton_prev_value;
    }
}

// top button end

// bot button start

botbutton_prev_time = time();
botbutton_prev_value = 0;

func num button_bot() { // returns 1 if the bottom button is pressed, else returns 0
    if (time() - botbutton_prev_time >= BUTTON_DELAY) {
        botbutton_prev_time = time();
        if (btn.rn == "R") {
            botbutton_prev_value = 1;
            return 1;
        } else {
            botbutton_prev_value = 0;
            return 0;
        }
    } else {
        return botbutton_prev_value;
    }
}

// bot button end

// reading from sensors end

// motors start

func num l_m(speed) { // left motor move
    mt.spw(PORT_LEFT_MOTOR, speed);
}

func num r_m(speed) { // right motor move
    mt.spw(PORT_RIGHT_MOTOR, speed);
}

func num m_m(speed) { // middle motor move
    mt.spw(PORT_MID_MOTOR, speed);
}

// motors end

// odometry

odometry_prev_left = 0;
odometry_prev_right = 0;
odometry_prev_x = 0;
odometry_prev_y = 0;
weight = 720;

void odometry {
    while (true) {
        odometry_left = mt.getcount(PORT_LEFT_MOTOR);
        odometry_right = mt.getcount(PORT_RIGHT_MOTOR);
        delta_left = odometry_left - odometry_prev_left;
        delta_right = odometry_right - odometry_prev_right;
        delta = (delta_left + delta_right)/2;
        delta_com = compass_delta(compass());

        odometry_x = odometry_prev_x + delta*cos(rad(delta_com));
        odometry_y = odometry_prev_y + delta*sin(rad(delta_com));
        odometry_y = -odometry_y;

        odometry_prev_left = odometry_left;
        odometry_prev_right = odometry_right;
        odometry_prev_x = odometry_x;
        odometry_prev_y = odometry_y;

        delay(100);
    }
}

new.thread = odometry

// other functions

void justify {
    kp_justify = 0.95;
    v_justify = 0;
    compass_justify = compass();
    error_justify = compass_delta_two(COMPASS_LEFT, compass_justify);
    while (abs(error_justify) > 5) {
        compass_justify = compass();
        error_justify = compass_delta_two(COMPASS_LEFT, compass_justify);
        u_justify = error_justify * kp_justify;

        l_m(u_justify);
        r_m(-u_justify);
    }

    l_m(0);
    r_m(0);

    v_justify = 70;
    compass_justify = compass();
    error_justify = compass_delta_two(COMPASS_LEFT, compass_justify);
    while (light() >= LIGHT_LINE_LIMIT) {
        compass_justify = compass();
        error_justify = compass_delta_two(COMPASS_LEFT, compass_justify);
        u_justify = error_justify * kp_justify;

        l_m(v_justify+u_justify);
        r_m(v_justify-u_justify);
    }

    l_m(0);
    r_m(0);

    delay(500);

    v_justify = -70;
    motor_count_justify = mt.getcount(PORT_LEFT_MOTOR);
    while (abs(mt.getcount(PORT_LEFT_MOTOR) - motor_count_justify) < CENTER_DISTANCE) {
        l_m(v_justify);
        r_m(v_justify);
    }

    l_m(0);
    r_m(0);

    v_justify = 0;
    compass_justify = compass();
    error_justify = compass_delta(compass_justify);
    while (abs(error_justify) > 5) {
        compass_justify = compass();
        error_justify = compass_delta(compass_justify);
        u_justify = error_justify * kp_justify;

        l_m(-u_justify);
        r_m(u_justify);
    }

    l_m(0);
    r_m(0);

    v_justify = -85;
    while (button_top() != 1) {
        l_m(v_justify);
        r_m(v_justify);
    }

    v_justify = 65;
    motor_count_justify = mt.getcount(PORT_LEFT_MOTOR);
    while (abs(mt.getcount(PORT_LEFT_MOTOR) - motor_count_justify) < 55) {
        l_m(v_justify);
        r_m(v_justify);
    }

    l_m(0);
    r_m(0);
}

void return_goal {
    kp_return = 0.95;
    kp_justify = 0.95;
    v_return = 0;
    compass_return = compass();
    error_return = compass_delta_two(COMPASS_LEFT, compass_return);
    while (abs(error_return) > 5) {
        compass_return = compass();
        error_return = compass_delta_two(COMPASS_LEFT, compass_return);
        u_return = error_return * kp_return;

        l_m(u_return);
        r_m(-u_return);
    }

    l_m(0);
    r_m(0);
    tone(100, 100, 100);

    v_return = -70;
    compass_return = compass();
    error_return = compass_delta_two(COMPASS_LEFT, compass_return);
    exit_return = 0;
    while (exit_return == 0) {
        if (light() < LIGHT_LINE_LIMIT) {
            exit_return = 1;
        }
        if (button_bot() == 1) {
            exit_return = 2;
        }
        compass_return = compass();
        error_return = compass_delta_two(COMPASS_LEFT, compass_return);
        u_return = error_return * kp_return;

        l_m(v_return+u_return);
        r_m(v_return-u_return);
    }

    if (exit_return == 1) {
        v_justify = -70;
        motor_count_justify = mt.getcount(PORT_LEFT_MOTOR);
        while (abs(mt.getcount(PORT_LEFT_MOTOR) - motor_count_justify) < CENTER_DISTANCE) {
            l_m(v_justify);
            r_m(v_justify);
        }

        l_m(0);
        r_m(0);

        v_justify = 0;
        compass_justify = compass();
        error_justify = compass_delta(compass_justify);
        while (abs(error_justify) > 5) {
            compass_justify = compass();
            error_justify = compass_delta(compass_justify);
            u_justify = error_justify * kp_justify;

            l_m(-u_justify);
            r_m(u_justify);
        }

        l_m(0);
        r_m(0);

        v_justify = -85;
        while (button_top() != 1) {
            l_m(v_justify);
            r_m(v_justify);
        }

        v_justify = 65;
        motor_count_justify = mt.getcount(PORT_LEFT_MOTOR);
        while (abs(mt.getcount(PORT_LEFT_MOTOR) - motor_count_justify) < 55) {
            l_m(v_justify);
            r_m(v_justify);
        }

        l_m(0);
        r_m(0);
    } else {
        tone(100, 100, 100);
        v_return = 70;
        compass_return = compass();
        error_return = compass_delta_two(COMPASS_LEFT, compass_return);
        while (light() >= LIGHT_LINE_LIMIT) {
            compass_return = compass();
            error_return = compass_delta_two(COMPASS_LEFT, compass_return);
            u_return = error_return * kp_return;

            l_m(v_return+u_return);
            r_m(v_return-u_return);
        }

        l_m(0);
        r_m(0);
        // delay(500);
        tone(100, 100, 100);

        v_return = 70;
        motor_count_return = mt.getcount(PORT_LEFT_MOTOR);
        while (abs(mt.getcount(PORT_LEFT_MOTOR) - motor_count_return) < CENTER_DISTANCE) {
            l_m(v_return);
            r_m(v_return);
        }

        l_m(0);
        r_m(0);
        tone(100, 100, 100);

        v_justify = 0;
        compass_justify = compass();
        error_justify = compass_delta(compass_justify);
        while (abs(error_justify) > 5) {
            compass_justify = compass();
            error_justify = compass_delta(compass_justify);
            u_justify = error_justify * kp_justify;

            l_m(-u_justify);
            r_m(u_justify);
        }

        l_m(0);
        r_m(0);
        tone(100, 100, 100);

        v_justify = -85;
        while (button_top() != 1) {
            l_m(v_justify);
            r_m(v_justify);
        }

        v_justify = 65;
        motor_count_justify = mt.getcount(PORT_LEFT_MOTOR);
        while (abs(mt.getcount(PORT_LEFT_MOTOR) - motor_count_justify) < 55) {
            l_m(v_justify);
            r_m(v_justify);
        }

        l_m(0);
        r_m(0);
    }
}

void check_wall {
    kp_check = 0.45;
    v_check = 0;
    compass_check = compass();
    error_check = compass_delta(compass_check);
    while (abs(error_check) > 5) {
        compass_check = compass();
        error_check = compass_delta(compass_check);
        u_check = error_check * kp_check;

        l_m(-u_check);
        r_m(u_check);
    }

    l_m(0);
    r_m(0);

    v_check = -85;
    exit_check = 0;
    while (exit_check == 0) {
        if (button_top() == 1) {
            exit_check = 1;
        }
        if (button_bot() == 1) {
            exit_check = 2;
        } 

        l_m(v_check);
        r_m(v_check);
    }

    l_m(0);
    r_m(0);

    v_justify = 65;
    motor_count_justify = mt.getcount(PORT_LEFT_MOTOR);
    while (abs(mt.getcount(PORT_LEFT_MOTOR) - motor_count_justify) < 105) {
        l_m(v_justify);
        r_m(v_justify);
    }

    l_m(0);
    r_m(0);

    if (exit_check == 2) {
        return_goal();
    }
}

act_count = 0;

void watch {
    kp_watch = 0.4;
    kd_watch = 0;
    ki_watch = 0.003;
    error_watch = 0;
    error_old_watch = 0;
    i_watch = 0;
    k_dir_watch = 0;
    watch_bool = 0;
    time_start_watch = time();

    while (watch_bool == 0) {
        if (time() - time_start_watch >= 1000 and act_count >= 1 and irseeker_str(0) <= STR_AWAY) {
            justify();
            act_count = 0;
            odometry_x = 0;
            odometry_y = 0;
        }

        i_watch = i_watch + error_watch*ki_watch;
        if (irseeker_dir() != 0) {
            error_watch = irseeker_str(4) - irseeker_str(3);
            if (irseeker_str(4) == 0 and irseeker_str(3) == 0) {
                k_dir_watch = 14;
            } else {
                k_dir_watch = 0;
            }

            if (irseeker_dir() == 6) {
                i_watch = 0;
            }

            tmp_dir = irseeker_dir()
            u_watch = k_dir_watch*(tmp_dir - 6) + kp_watch*error_watch + kd_watch*error_old_watch + i_watch;
            if ((abs(error_watch) <= 20 or irseeker_dir() == 6) and k_dir_watch == 0) {
                u_watch = 0;
            }

            k_dist = (SEEKER_STR_MAX-irseeker_str(0))/SEEKER_STR_MAX;
            if (k_dist <= 0.4) {
                k_dist = 0.2;
            }

            if (u_watch <= 0 and compass_delta(compass()) <= -90) {
                u_watch = 0;
            }
            if (u_watch >= 0 and compass_delta(compass()) >= 90) {
                u_watch = 0;
            }

            l_m((k_dist*u_watch));
            r_m((0-k_dist*u_watch));

            error_old_watch = error_watch;
            prev_dir_watch = irseeker_dir();
        } else {
            l_m(0);
            r_m(0);
        }

        if (compass_delta(compass()) <= -60 or compass_delta(compass()) >= 60) {
            watch_bool = 2;
        }

        if (irseeker_str(0) > STR_LIMIT) {
            watch_bool = 1;
        }
        
        delay(10);
    }
}

void go_back {
    v_back = 0;
    kp_back = 1.15;
    left_lim_back = compass_delta(COMPASS_LEFT_ANGLE);
    right_lim_back = compass_delta(COMPASS_RIGHT_ANGLE);

    if (odometry_x < 0) {
        error_back = atan(abs(odometry_y / odometry_x));
        beta_back = rm(COMPASS_ALPHA - error_back + 360, 360);
    } else {
        error_back = atan(odometry_y / odometry_x);
        beta_back = rm(COMPASS_ALPHA + error_back, 360);
    }

    error_back = compass_delta(beta_back);
    while (abs(error_back) > 5) {
        error_back = compass_delta(beta_back);
        u_back = error_back * kp_back;

        l_m(-u_back);
        r_m(u_back);
    }

    l_m(0);
    r_m(0);

    compass_back = compass();
    if (left_lim_back < 0) {
        if (compass_delta(compass_back) < left_lim_back) {
            len_back = sqrt(CENTER_DISTANCE*CENTER_DISTANCE + odometry_y*odometry_y);
        } else {
            if (compass_delta(compass_back) > right_lim_back) {
                len_back = sqrt(CENTER_DISTANCE*CENTER_DISTANCE + odometry_y*odometry_y);
            } else {
                len_back = sqrt(odometry_x*odometry_x*1.085 + odometry_y*odometry_y);
            }
        }
    } else {
        if (compass_delta(compass_back) < right_lim_back) {
            len_back = sqrt(CENTER_DISTANCE*CENTER_DISTANCE + odometry_y*odometry_y);
        } else {
            if (compass_delta(compass_back) > left_lim_back) {
                len_back = sqrt(CENTER_DISTANCE*CENTER_DISTANCE + odometry_y*odometry_y);
            } else {
                len_back = sqrt(odometry_x*odometry_x*1.085 + odometry_y*odometry_y);
            }
        }
    }
    v_back = -85;
    motor_count_back = mt.getcount(PORT_LEFT_MOTOR);
    while (abs(mt.getcount(PORT_LEFT_MOTOR) - motor_count_back) < len_back) {
        l_m(v_back);
        r_m(v_back);
    }

    l_m(0);
    r_m(0);
    // delay(200);

    v_back = 0;
    compass_back = compass();
    error_back = compass_delta(compass_back);
    while (abs(error_back) > 5) {
        compass_back = compass();
        error_back = compass_delta(compass_back);
        u_back = error_back * kp_back;

        l_m(-u_back);
        r_m(u_back);
    }

    l_m(0);
    r_m(0);

    v_back = -85;
    while (button_top() != 1 and button_bot() != 1) {
        l_m(v_back);
        r_m(v_back);
    }

    if (button_bot() == 1) {
        return_goal();
    } else {
        v_back = 65;
        motor_count_back = mt.getcount(PORT_LEFT_MOTOR);
        while (abs(mt.getcount(PORT_LEFT_MOTOR) - motor_count_back) < 55) {
            l_m(v_back);
            r_m(v_back);
        }

        l_m(0);
        r_m(0);
    }
}

void act_side {
    kp_act = 0.4;
    v_act = 100;
    left_lim_back = compass_delta(COMPASS_LEFT_ANGLE);
    right_lim_back = compass_delta(COMPASS_RIGHT_ANGLE);

    compass_act = compass();
    act_type = "none";
    if (left_lim_back < 0) {
        delta_act = compass_delta_two(compass_act, COMPASS_LEFT);
        if (compass_delta(compass_act) < left_lim_back and abs(delta_act) < 60) {
            act_type = "left";
        } else {
            delta_act = compass_delta_two(compass_act, COMPASS_RIGHT);
            if (compass_delta(compass_act) > right_lim_back and abs(delta_act) < 60) {
                act_type = "right";
            } else {
                act_type = "none";
            }
        }
    } else {
        delta_act = compass_delta_two(compass_act, COMPASS_LEFT);
        if (compass_delta(compass_act) > left_lim_back and abs(delta_act) < 60) {
            act_type = "left";
        } else {
            delta_act = compass_delta_two(compass_act, COMPASS_RIGHT);
            if (compass_delta(compass_act) < right_lim_back and abs(delta_act) < 60) {
                act_type = "right";
            } else {
                act_type = "none"
            }
        }
    }

    m_m(100);

    while (light() >= LIGHT_LINE_LIMIT) { 
        compass_act = compass();

        if (act_type == "left") {
            error_act = compass_delta_two(COMPASS_LEFT, compass_act);
        } else {
            error_act = compass_delta_two(COMPASS_RIGHT, compass_act);
        }

        u_act = kp_act*error_act;
        if (u_act <= 0 and compass_delta(compass()) <= -90) {
            u_act = 0;
        }
        if (u_act >= 0 and compass_delta(compass()) >= 90) {
            u_act = 0;
        }

        l_m(v_act+u_act);
        r_m(v_act-u_act);
    }

    l_m(0);
    r_m(0);

    dir_act = irseeker_dir();
    while (dir_act > 3 and dir_act < 8) {
        dir_act = irseeker_dir();
        delay(20);
    }

    m_m(0);
}

void act_ball {
    kp_act = 0.45;
    
    v_act = 100;
    
    printupd();
    print("light", light());
    m_m(100);

    while (light() >= LIGHT_LINE_LIMIT) {
        error_act = irseeker_str(4) - irseeker_str(3);
        if (irseeker_str(4) == 0 and irseeker_str(3) == 0) {
            k_dir_act = 14;
        } else {
            k_dir_act = 0;
        }

        tmp_dir = irseeker_dir()
        u_watch = k_dir_act*(tmp_dir - 6) + kp_act*error_act;
        
        u_act = kp_act*error_act;
        if (u_act <= 0 and compass_delta(compass()) <= -90) {
            u_act = 0;
        }
        if (u_act >= 0 and compass_delta(compass()) >= 90) {
            u_act = 0;
        }

        l_m(v_act+u_act);
        r_m(v_act-u_act);
    }

    l_m(0);
    r_m(0);

    delay(2000);

    m_m(0);
}

void act {
    act_count = act_count + 1;

    if (watch_bool == 2) {
        act_side();
    } else {
        act_ball();
    }
}

// main part of the program

while (true) {
    // prints
    printupd();
    print("btn", button_bot());
    print("x", odometry_x);
    print("y", odometry_y);
    // print("cnt", mt.getcount(PORT_LEFT_MOTOR));

    // actions
    // watch();
    // act();
    // go_back();
    check_wall();
    delay(5000);
}