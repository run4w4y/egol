// check if all of the ports we are using are working correctly
//check.ports("ABCD 1234")
sen.setmode(1, 1)

// read all of the calibration values from file
handle = open.r("cal.txt");

COMPASS_ALPHA = tonum(readline(handle));
ALPHA_LEFT = tonum(readline(handle));
ALPHA_RIGHT = tonum(readline(handle));
STR_MAX = tonum(readline(handle));
BACK_LIGHT_VALUE = tonum(readline(handle));
STR_VALUE_2 = tonum(readline(handle));
STR_VALUE_4 = tonum(readline(handle));
FRONT_LIGHT_VALUE = tonum(readline(handle));
STR_RANGE = tonum(readline(handle));
closef(handle);

// define global variables
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
main_lock = 0
is_aligned = 0
prev_dir = -1
dir_count = 0
mode = 1

// voids

// sensors thread
void sensors {
    while (true) {
        irseeker_array = i2c.readregs(4, 8, 73, 6)
        dir = irseeker_array[0]
        str1 = irseeker_array[1]
        str2 = irseeker_array[2]
        str3 = irseeker_array[3]
        str4 = irseeker_array[4]
        str5 = irseeker_array[5]

        strres = irseeker_array[1]
        for (i_dx = 2; i_dx < 6; i_dx++) {
            strres = max(strres, irseeker_array[i_dx])
        }
		
        compass_array = i2c.readregs(2, 1, 66, 4)
        compass = compass_array[0] * 2 + compass_array[1]
        err_com = rm(compass - alpha + 900, 360) - 180
		// we are using this in odometry, abs stands for absolute
		err_com_abs = rm(compass - COMPASS_ALPHA + 900, 360) - 180

        l_f = sen.percent(3)
		l_b = sen.percent(1)
    }
}

new.thread = sensors

func num alga_pizda(v_r, tizlek) {

    turn = err_com * 0.3 + (err_com - err_com_old) * 0.6
	err_com_old = err_com


	if (v_r < 0) {
		motor_Power_D = tizlek * (cos(rad(60+v_r)) * cos(rad(180)) - sin(rad(60+v_r)) * sin(rad(180))) + turn
    	motor_Power_B = tizlek * (cos(rad(60+v_r)) * cos(rad(300)) - sin(rad(60+v_r)) * sin(rad(300))) + turn
    	motor_Power_C = tizlek * (cos(rad(60+v_r)) * cos(rad(180)) - sin(rad(60+v_r)) * sin(rad(180))) + turn
	} else {
		if (v_r > 0) {
			motor_Power_D = tizlek * (cos(rad(v_r)) * cos(rad(60)) - sin(rad(v_r)) * sin(rad(180))) + turn
    		motor_Power_B = tizlek * (cos(rad(v_r)) * cos(rad(60)) - sin(rad(v_r)) * sin(rad(120))) + turn
    		motor_Power_C = tizlek * (cos(rad(v_r)) * cos(rad(180)) - sin(rad(v_r)) * sin(rad(180))) + turn
		} else {
			if (v_r == 0) {
				motor_Power_D = tizlek + turn
    			motor_Power_B = turn
    			motor_Power_C = -tizlek + turn
			}
		}
	}

    motor.Start("D",-motor_Power_D)
    motor.Start("B",-motor_Power_B)
    motor.Start("C",-motor_Power_C)
}

while (true) {
    alga_pizda(90, 100)
	printupd()
	print("D", motor_Power_D)
	print("B", motor_Power_B)
	print("C", motor_Power_C)
}