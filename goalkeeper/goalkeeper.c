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
        compass_odometry = compass(); 
        delta_com = compass_delta_two(compass_right, compass_odometry);

        odometry_x = odometry_prev_x + delta*cos(rad(delta_com));
        odometry_y = odometry_prev_y + delta*sin(rad(delta_com));

        odometry_prev_left = odometry_left;
        odometry_prev_right = odometry_right;
        odometry_prev_x = odometry_x;
        odometry_prev_y = odometry_y;

        delay(200);
    }
}

new.thread = odometry

void reset_odometry {
    odometry_x = 0;
    odometry_prev_x = 0;
    odometry_y = 0;
    odometry_prev_y = 0;
}

// movements

func num stop() {
    l_m(0);
    r_m(0);
}

func num go_both(speed) {
    l_m(speed);
    r_m(speed);
}

func num turn_angle(alpha_angle) {
    local kp = 1.15;
    local u = 0;
    local com = 0;
    local error = 0;
    tmp = alpha_angle;
    com = compass();
    error = compass_delta_two(tmp, com);
    while (abs(error) > 5) {
        com = compass();
        error = compass_delta_two(tmp, com);
        u = error * kp;

        l_m(u);
        r_m(-u);
    }

    stop();
}

func num go_angle(alpha_angle, speed) {
    local kp = 1.15;
    local u = 0;
    local com = 0;
    local error = 0;
    local v = 0;
    v = speed;
    tmp = alpha_angle;
    com = compass();
    error = compass_delta_two(tmp, com);
    u = error * kp;

    l_m(v + u);
    r_m(v - u);
}

func num go_enc(distance, speed) {
    local v = 0;
    v = speed;
    local count = 0;
    count = mt.getcount(PORT_LEFT_MOTOR);
    while (abs(mt.getcount(PORT_LEFT_MOTOR) - count) < distance) {
        go_both(v);
    }

    stop();
}

void goal_justify {
    while (button_top() != 1) {
        go_both(-85);
    }
    go_enc(55, 65);
    stop();
}

i_glob = 0;
prev_dir_glob = 0;
func num go_ball(speed) {
    local kp = 0.4;
    local kd = 0;
    local ki = 0.003;
    local kdir = 0;
    local v = 0;
    local i = 0;
    local error = 0;
    local error_old = 0;
    local u = 0;
    local k_dist = 0;
    local tmp_dir = 0;
    local prev_dir = 0;
    prev_dir = prev_dir_glob;
    v = speed;
    i = i_glob;
    i = i + error * ki;
    if (irseeker_dir() != 0) {
        error = irseeker_str(4) - irseeker_str(3);
        if (irseeker_str(4) == 0 and irseeker_str(3) == 0) {
            kdir = 14;
        } else {
            kdir = 0;
        }

        if (irseeker_dir() == 6) {
            i = 0;
        }

        tmp_dir = irseeker_dir();
        u = kdir*(tmp_dir - 6) + kp * error + kd * error_old + i;
        if ((abs(error) <= 20 or irseeker_dir() == 6) and kdir == 0) {
            u = 0;
        }

        k_dist = (SEEKER_STR_MAX-irseeker_str(0))/SEEKER_STR_MAX;
        if (k_dist <= 0.4) {
            k_dist = 0.2;
        }

        if (u <= 0 and compass_delta(compass()) <= -90) {
            u = 0;
        }
        if (u >= 0 and compass_delta(compass()) >= 90) {
            u = 0;
        }
        u = k_dist * u;

        l_m(v + u);
        r_m(v - u);

        error_old = error;
        prev_dir = irseeker_dir();
    } else {
        stop();
    }

    i_glob = i;
    prev_dir_glob = prev_dir;
}

// other functions

void justify {
    turn_angle(COMPASS_LEFT);
    while (light() >= LIGHT_LINE_LIMIT) {
        go_angle(COMPASS_LEFT, 70);
    }

    stop();
    delay(500);

    go_enc(CENTER_DISTANCE, -70);
    turn_angle(COMPASS_ALPHA);
    goal_justify();
}

void return_goal {
    turn_angle(COMPASS_LEFT);

    exit_return = 0;
    while (exit_return == 0) {
        if (light() < LIGHT_LINE_LIMIT) {
            exit_return = 1;
        }
        if (button_bot() == 1) {
            exit_return = 2;
        }
        go_angle(COMPASS_LEFT, -70);
    }
    stop();

    if (exit_return == 1) {
        go_enc(CENTER_DISTANCE, -70);
        turn_angle(COMPASS_ALPHA);
        goal_justify();
    } else {
        while (light() >= LIGHT_LINE_LIMIT) {
            go_angle(COMPASS_LEFT, 70);
        }
        stop();
        go_enc(CENTER_DISTANCE, 70);
        turn_angle(COMPASS_ALPHA);
        goal_justify();
    }
}

void check_wall {
    turn_angle(COMPASS_ALPHA);

    exit_check = 0;
    while (exit_check == 0) {
        if (button_top() == 1) {
            exit_check = 1;
        }
        if (button_bot() == 1) {
            exit_check = 2;
        } 

        go_both(-85);
    }

    stop();
    go_enc(105, 65);

    if (exit_check == 2) {
        return_goal();
        reset_odometry();
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
    check_wall_time = time();

    while (watch_bool == 0) {
        if (time() - time_start_watch >= 1000 and act_count >= 1 and irseeker_str(0) <= STR_AWAY) {
            justify();
            reset_odometry();
            check_wall_time = time();
            act_count = 0;
        }

        if (time() - check_wall_time >= 10000 and irseeker_str(0) <= STR_AWAY) {
            check_wall();
            check_wall_time = time();
        }

        go_ball(0);

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
    beta_err = deg(atan(odometry_y / abs(odometry_x)));
    if (COMPASS_RIGHT < COMPASS_LEFT) {
        if (odometry_x > 0) {
            beta_res = rm(COMPASS_RIGHT + beta_err, 360);
        } else {
            beta_res = rm(COMPASS_LEFT - beta_err + 360, 360);
        }
    } else {
        if (odometry_x > 0) {
            beta_res = rm(COMPASS_RIGHT - beta_err + 360, 360);
        } else {
            beta_res = rm(COMPASS_LEFT + beta_err, 360);
        }
    }

    turn_angle(beta_res);

    len_back = sqrt(odometry_x*odometry_x*1.085 + odometry_y*odometry_y);
    go_enc(len_back, -85);
    turn_angle(COMPASS_ALPHA);

    while (button_top() != 1 and button_bot() != 1) {
        go_both(-85);
    }

    if (button_bot() == 1) {
        return_goal();
    } else {
        go_enc(55, 65);
    }
}

void act_side {
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
        if (act_type == "left") {
            go_angle(COMPASS_LEFT, 100);
        } else {
            go_angle(COMPASS_RIGHT, 100);
        }
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
    m_m(100);
    while (light() >= LIGHT_LINE_LIMIT) {
        go_ball(100);
    }
    stop();

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
    print("d", delta_com);

    // actions
    watch();
    act();
    go_back();
}