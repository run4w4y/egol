/*TODO:
    !   norm MotoMan
    #   attack
    #   modes
    #   slowing in padik
    #   avoid the ball
*/
// check.ports("ABC 1234"); // checks if everything is connected properly

// const values from calibration

// ports
handle = open.r("ports.txt"); // open the file with values

PORT_SEEKER = tonum(readline(handle));
PORT_COMPASS = tonum(readline(handle));
PORT_LIGHT_FRONT = tonum(readline(handle));
PORT_LIGHT_BACK = tonum(readline(handle));
SEEKER_DELAY = tonum(readline(handle));
COMPASS_DELAY = tonum(readline(handle));
LIGHT_DELAY = tonum(readline(handle));
PORT_FIRST_MOTOR = tohex(tonum(readline(handle)) + 9);
PORT_SECOND_MOTOR = tohex(tonum(readline(handle)) + 9);
PORT_THIRD_MOTOR = tohex(tonum(readline(handle)) + 9);
PORT_KICKER = tohex(tonum(readline(handle)) + 9);

closef(handle);

// values
handle = open.r("cal.txt");

COMPASS_ALPHA = tonum(readline(handle));
SEEKER_DIV = tonum(readline(handle)) / 100;
KP_MOVE = tonum(readline(handle))/100;
STR_MAX = tonum(readline(handle));
BACK_LIGHT_VALUE = tonum(readline(handle));
STR_VALUE_2 = tonum(readline(handle));
STR_VALUE_4 = tonum(readline(handle));

closef(handle);

// setting up bt start

who_am_i = getname();

if (who_am_i == "BOBA") {
	who_aint_me = "BIBA";
} else {
	who_aint_me = "BOBA";
}

mode = 1;
attack = 0;

connect(who_aint_me);

mailbox_biba = new.mailbox("mailbox_biba");
mailbox_boba = new.mailbox("mailbox_boba");
mailbox_mode = new.mailbox("mailbox_mode");

// setting up bt end

// reading from sensors start

// light sensor back start

sen.setmode(PORT_LIGHT_BACK, 1);

light_back_prev_time = time();
light_back_prev_value = 0;

func num light_back() { // get current value of the selected color based on the light sens
    if (time() - light_back_prev_time >= LIGHT_DELAY) {
        light_back_prev_time = time();
        local light_val = 0;
        light_val = sen.percent(PORT_LIGHT_BACK);

        light_back_prev_value = light_val;
        return light_val;
    } else {
       return light_back_prev_value;
    }
}

// light sensor back end 

// compass start

compass_prev_time = time();
compass_prev_value = 0;

func num compass() { // get compass current azimut
    local compass;
    if (time() - compass_prev_time >= COMPASS_DELAY) {
        compass_prev_time = time();
        compass_array = i2c.readregs(PORT_COMPASS, 1, 66, 4);
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
        irseeker_array = i2c.readregs(PORT_SEEKER, 8, 73, 6);
        
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

// kinematics start 

func num move(x1, y1) {
    x_gl = -1.15*x1;
    y_gl = 1.15*y1;
    t_gl = compass_delta(compass());
    r_base = pi;
    kp_gl = KP_MOVE;

    thetta = 3*pi/2;
    v1 = 1.5*(-x_gl + r_base*t_gl*kp_gl);
    max_speed = abs(v1);
    v2 = (sin(pi/3 - thetta)*x_gl - cos(pi/3 - thetta)*y_gl + r_base*t_gl*kp_gl)*0.97;
    if (abs(v2) > max_speed) {
        max_speed = abs(v2);
    }
    v3 = sin(pi/3 + thetta)*x_gl + cos(pi/3 + thetta)*y_gl + r_base*t_gl*kp_gl;
    if (abs(v3) > max_speed) {
        max_speed = abs(v3);
    }

    if (max_speed > 115) {
        v1 = v1 * 100 / max_speed;
        v2 = v2 * 100 / max_speed;
        v3 = v3 * 100 / max_speed;
        tone(100, 100, 100);
    }

    mt.spw(PORT_FIRST_MOTOR, v1);
    mt.spw(PORT_SECOND_MOTOR, v2);
    mt.spw(PORT_THIRD_MOTOR, v3);
}

// kinematics end

// odometry start

// odometry end

// bluetooth start

void bt {
	while (true) {
        str_res = irseeker_str(0);
        dir_res = irseeker_dir();
        back_lt = light_back();

        ///////////////////////////////////////////////////////////////////////

		if (who_am_i == "BIBA") {
			mybtstr = attack + ""; 
		} else {
			mybtstr = strres + " " + dir + " " + attack 
		}
		
		if (who_am_i == "BIBA") {
			bt.send(who_aint_me, "mailbox_biba", mybtstr)

			btstr2 = bt.last(mailbox_boba)
			res = parse(4, btstr2)

			str_res2 = res[0]
			dir2 = res[1]
			attack2 = res[2]
		} else {
			bt.send(who_aint_me, "mailbox_boba", mybtstr)

			btstr2 = bt.last(mailbox_biba)
			res = parse(2, btstr2)

			attack2 = res[0]

			mode = tonum(bt.last(mailbox_mode))
		}
		
		//////////////////////////////////////////////////////////////////////
		
		if (who_am_i == "BIBA") {
            if (abs(str_res - str_res2) < 30) {
                if (abs(dir_res - 5) == abs(dir2 - 5)) {
                    if (str_res > str_res2) {
                        mode = 1
                    } else {
                        mode = 0
                    }
                } else {
                    if (abs(dir_res - 5) > abs(dir2 - 5)) {
                        mode = 0
                    } else {
                        mode = 1
                    }
                }
            } else {
                if (str_res > str_res2) {
                    mode = 1
                } else {
                    mode = 0
                }
            }
			bt.send(who_aint_me, "mailbox_mode", abs(mode-1))
		}
		
		////////////////////////////////////////////////////////////////////////
		
        printupd();
        print("v1", v1);
        print("v2", v2);
        print("v3", v3);

        ////////////////////////////////////////////////////////////////////////

		// buttons will be here
	}
}

new.thread = bt

while (true) {
    if (dir_res == 5) {
        if (str_res < 200) {
            move(0, 100);
        } else {
            while (str_res > 150) {
                move(0, 100);
                tone(100, 100, 100);
            }
        }

    } else {
        if (abs(dir_res - 5) < 3) {
            v = (dir_res - 5)*100;
            move(v, 0);
        } else {
            move(0, -100);
        }
    }
}
