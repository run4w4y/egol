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

// reading from sensors start

// light sensor start

LIGHT_COLOR = 'g';

if (LIGHT_COLOR_NUM == 0) {
    LIGHT_COLOR = 'r';
} else {
    if (LIGHT_COLOR_NUM == 1) {
        LIGHT_COLOR = 'g';
    } else {
        if (LIGHT_COLOR_NUM == 2) {
            LIGHT_COLOR = 'b';
        }
    }
}

sen.setmode(PORT_LIGHT, 4);

light_prev_time = time();
light_prev_value = 0;

func num light() { // get current value of the selected color based on the light sens
    if (time() - light_prev_time >= LIGHT_DELAY) {
        local r;
        local g;
        local b;
        local light_val;
        sen.readrgb(PORT_LIGHT, r, g, b);

        if (LIGHT_COLOR == 'r') {
            light_val = r;
        } else { 
            if (LIGHT_COLOR == 'g') {
                light_val = g;
            } else { 
                if (LIGHT_COLOR == 'b') {
                    light_val = b;
                }
            }
        }

        light_prev_value = light_val;
        return light_val;
    } else {
       return light_prev_value;
    }

    light_prev_time = time();
}

// light sensor end 

// compass start

compass_prev_time = time();
compass_prev_value = 0;

func num compass() { // get compass current azimut
    local compass_array;
    local compass;
    if (time() - compass_prev_time >= COMPASS_DELAY) {
        compass_array = i2c.readregs(2, 1, 66, 4);
        compass = compass_array[0] * 2 + compass_array[1];
        compass_prev_value = compass;
        return compass;
    } else {
        return compass_prev_value;
    }

    compass_prev_time = time();
}

func num compass_delta(compass_value) { // get compass delta based
    local error = rm(compass_value - COMPASS_ALPHA + 900, 360) - 180;
    return error;
}

// compass end

// irseeker start

seeker_prev_time = time();
seeker_prev_dir = 0;
seeker_prev_str = 0;

func num irseeker_dir() { // get seeker current dir
    local irseeker_array;
    local dir;
    local str1;
    local str2;
    local str3;
    local str4;
    local str5;

    if (time() - seeker_prev_time >= SEEKER_DELAY) {
        irseeker_array = i2c.readregs(4, 8, 73, 6);
        
        dir = irseeker_array[0];
        seeker_prev_dir = dir;

        str1 = irseeker_array[1];
        str2 = irseeker_array[2];
        str3 = irseeker_array[3];
        str4 = irseeker_array[4];
        str5 = irseeker_array[5];

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

    seeker_prev_time = time();
}

func num irseeker_str() { // get seeker current str
    local irseeker_array;
    local dir;
    local str1;
    local str2;
    local str3;
    local str4;
    local str5;

    if (time() - seeker_prev_time >= SEEKER_DELAY) {
        irseeker_array = i2c.readregs(4, 8, 73, 6);
        
        dir = irseeker_array[0];
        seeker_prev_dir = dir;

        str1 = irseeker_array[1];
        str2 = irseeker_array[2];
        str3 = irseeker_array[3];
        str4 = irseeker_array[4];
        str5 = irseeker_array[5];

        local strres = 0;
        if (rm(dir,2) == 0) {
            strres = (str1 + str2 + str3 + str4 + str5) / SEEKER_DIV;
        } else {
            strres = str1 + str2 + str3 + str4 + str5;
        }
        seeker_prev_str = strres;

        return strres;
    } else {
        return seeker_prev_str;
    }

    seeker_prev_time = time();
}

// irseeker end

// button start



// button end

// reading from sensors end

// main loop goes here

while (true) {
    // some code
}
