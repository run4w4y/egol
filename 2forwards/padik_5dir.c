// check.ports("ABC 1234"); // checks if everything is connected properly

// const values from calibration

handle = open.r("cal.txt"); // open the file with values

PORT_SEEKER = tonum(readline(handle));
PORT_COMPASS = tonum(readline(handle));
PORT_LIGHT_FRONT = tonum(readline(handle));
PORT_LIGHT_BACK = tonum(readline(handle));
SEEKER_DELAY = tonum(readline(handle));
COMPASS_DELAY = tonum(readline(handle));
LIGHT_DELAY = tonum(readline(handle));
COMPASS_ALPHA = tonum(readline(handle));
SEEKER_DIV = tonum(readline(handle)) / 100;
PORT_FIRST_MOTOR = tohex(tonum(readline(handle)) + 9);
PORT_SECOND_MOTOR = tohex(tonum(readline(handle)) + 9);
PORT_THIRD_MOTOR = tohex(tonum(readline(handle)) + 9);
PORT_KICKER = tohex(tonum(readline(handle)) + 9);
KP_MOVE = tonum(readline(handle))/100;
LIGHT_FRONT_COLOR = 1;
LIGHT_BACK_COLOR = 1;

// reading from sensors start

// setting up bt start

who_am_i = getname() 

if (who_am_i == "BOBA") {
	who_aint_me = "BIBA"
} else {
	who_aint_me = "BOBA"
}

mode = 1

connect(who_aint_me)

mailbox_biba = new.mailbox("mailbox_biba")
mailbox_boba = new.mailbox("mailbox_boba")
mailbox_mode = new.mailbox("mailbox_mode")

// setting up bt end

// light sensor front start

sen.setmode(PORT_LIGHT_FRONT, 4);

light_front_prev_time = time();
light_front_prev_value = 0;

func num light_front() { // get current value of the selected color based on the light sens
    if (time() - light_front_prev_time >= LIGHT_DELAY) {
        light_front_prev_time = time();
        local light_val = 0;
        light_val = sen.read.rawval(PORT_LIGHT_FRONT, LIGHT_FRONT_COLOR);

        light_front_prev_value = light_val;
        return light_val;
    } else {
       return light_front_prev_value;
    }
}

// light sensor front end 

// light sensor back start

sen.setmode(PORT_LIGHT_BACK, 4);

light_back_prev_time = time();
light_back_prev_value = 0;

func num light_back() { // get current value of the selected color based on the light sens
    if (time() - light_back_prev_time >= LIGHT_DELAY) {
        light_back_prev_time = time();
        local light_val = 0;
        light_val = sen.read.rawval(PORT_LIGHT_BACK, LIGHT_BACK_COLOR);

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

        // if (rm(dir,2) == 0) {
        //     strres = (str1 + str2 + str3 + str4 + str5) / SEEKER_DIV;
        // } else {
        //     strres = str1 + str2 + str3 + str4 + str5;
        // }

        strres = irseeker_array[1]
        for (i_dx = 2; i_dx < 6; i_dx++) {
            strres = max(strres, irseeker_array[i_dx])
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
    x_gl = x1;
    y_gl = y1;
    t_gl = compass_delta(compass());
    r_base = pi;

    v1 = -sin(pi/3)*x_gl + cos(pi/3)*y_gl + r_base*t_gl*KP_MOVE;
    v2 = sin(pi/3)*x_gl + cos(pi/3)*y_gl - r_base*t_gl*KP_MOVE;
    v3 = -x_gl + r_base*t_gl*KP_MOVE;

    mt.spw(PORT_FIRST_MOTOR, v1);
    mt.spw(PORT_SECOND_MOTOR, v2);
    mt.spw(PORT_THIRD_MOTOR, v3);
}

while (true) {
    
    //  padik
//    while (strres < 150) {
        if (dir == 5) {
            move(0, 100);
        } else {
            if (abs(dir - 5) < 3) {
                move((dir - 5)*40, 0);
            } else {
                move(0, -100);
            }
        }
//    }

//  attack
    // while (str > 150) {
    //     attack()
    // }

}
