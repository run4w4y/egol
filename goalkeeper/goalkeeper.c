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
PORT_MID_MOTOR = "A";
PORT_LEFT_MOTOR = "B";
PORT_RIGHT_MOTOR = "C";
STR_LIMIT = 130;

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
        compass = compass_array[0] * 2 + compass_array[1];
        compass_prev_value = compass;
        return compass;
    } else {
        return compass_prev_value;
    }
}

func num compass_delta(compass_value) { // get compass delta based
    local error = 0;
    error = rm(compass_value - COMPASS_ALPHA + 900, 360) - 180;
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
    local dir;
    local str1;
    local str2;
    local str3;
    local str4;
    local str5;

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

        local strres = 0;
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
    local dir;
    local str1;
    local str2;
    local str3;
    local str4;
    local str5;

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

        local strres = 0;
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
    local val;

    if (time() - topbutton_prev_time >= BUTTON_DELAY) {
        topbutton_prev_time = time();
        val = sen.percent(PORT_BUTTON);
        if (val == 100) {
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

func num button_bot() { // returns 1 if the top button is pressed, else returns 0
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

// other functions

void watch {
    kp_watch = 0.4;
    kd_watch = 0;
    ki_watch = 0.003;
    error_watch = 0;
    error_old_watch = 0;
    i_watch = 0;
    k_dir_watch = 0;
    watch_bool = 0;

    SEEKER_STR_MAX = 195;

    while (watch_bool == 0) {
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

            u_watch = k_dir_watch*(irseeker_dir() - 6) + kp_watch*error_watch + kd_watch*error_old_watch + i_watch;
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

        if (irseeker_str(0) > STR_LIMIT) {
            watch_bool = 1;
        }
        
        delay(10);
    }
}

void go_back {
    v_back = 80;
    kp_back = 0.4;

    while (button_top() != 1) {
        error_back = compass_delta();
    }
}

void act {
    kp_act = 0.4;
    v_act = 80;
    
    printupd();
    print("light", light());

    while (light() >= 10) {
        error_act = irseeker_str(4) - irseeker_str(3);
        u_act = kp_act*error_act;
        if (u_act <= 0 and compass_delta(compass()) <= -90) {
            u_act = 0;
        }
        if (u_act >= 0 and compass_delta(compass()) >= 90) {
            u_act = 0;
        }

        l_m((v_act+k_dist*u_watch));
        r_m((v_act-k_dist*u_watch));
    }

    l_m(0);
    r_m(0);
    m_m(100);

    delay(2000);

    m_m(0);
}

// main part of the program

while (true) {
    watch();
    act();
}