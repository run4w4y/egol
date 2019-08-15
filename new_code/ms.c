// check if all of the ports we are using are working correctly
check.ports("ABCD 234")

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
speed = 75

// voids

// sensors thread
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
    }
}

// odometry thread
void odometry {
	thetta_base = -3*pi/2
	base_length = 1

	// NOTE: might wanna change those
	motor_ks = new.vector(3, 0)
	motor_ks[0] = 1
	motor_ks[1] = 1
	motor_ks[2] = 1

	enc_prev = new.vector(3, 0)
	freq = 50 // odometry frequency, might wanna make it bigger
	// start the actual odometry thread
	while (true) {
		iter_start = time()

		// NOTE: might wanna remove thetta shit, idk
		cur_delta = err_com_abs
		thetta = rad(cur_delta) + thetta_base
		
		// reading encoders
		enc_cur = new.vector(3, 0)
		enc_cur[0] = mt.getcount("B")
		enc_cur[1] = mt.getcount("C")
		enc_cur[2] = mt.getcount("D")
		
		enc_delta = new.vector(3, 0)
		stopped_cnt = 0
		for (i = 0; i < 3; i++) {
			enc_delta[i] = (enc_cur[i] - enc_prev[i]) * motor_ks[i]
			if (enc_delta == 0) {
				stopped_cnt = stopped_cnt
			}
		}

		if (stopped_cnt > 1) {
			for (i = 0; i < 3; i++) {
				enc_delta[i] = 0
			}
		}

		// calculating x and y of the delta vector
		x_del = (-2 * sin(thetta) / 3) * enc_del[0] + (-2 * sin(pi/3 - thetta) / 3) * enc_del[1] + (-2 * cos(pi/3 + thetta) / 3) * enc_del[2]
		y_del = (2 * cos(thetta) / 3) * enc_del[0] + (-2 * cos(pi/3 - thetta) / 3) * enc_del[1] + (-2 * cos(pi/3 + thetta) / 3) * enc_del[2]

		// robot position
		x_res = x_res + x_del
		y_res = y_res + y_del
		
		// save values from this iteration
		for (i = 0; i < 3; i++) {
			enc_prev[i] = enc_cur[i]
		}
		
		// delay
		while (time() - iter_start < freq) {
			yield()
		}
	}
}

// reset odometry function
func num odometry_reset() {
	mt.resetcount("BCD")
	time_odometry = time()
	x_res = 0
	y_res = 0
	
	for (i = 0; i < 3; i++) {
		enc_prev[i] = 0
	}
}

// get the number of the current zone
//  -  0  -   |   -  1  -   |   -  2  - 
//    left    |    middle   |    right
func num current_zone() {
	if (x_res < -200) {
		return 0
	}
	if (x_res > 200) {
		return 2
	}
	return 1
}
	

// stop main thread execution
func num main_pause() {
	main_lock = 1
}

// resume main thread execution
func num main_resume() {
	main_lock = 0
}

// check if the wall button is pressed
func num wall_button() {
	if (btn.rn == "R") {
		return 1
	}
	return 0
}

// action on the UP button press
func num up_button() {
	alpha = ALPHA_RIGHT
	t_start = time()
	while (time() - t_start < 750) {
		alga(speed, 0)
	}
	alga(0, 0)
	kicker()
	alpha = COMPASS_ALPHA
	while (wall_button() == 0) {
		alga(-speed, 0)
	}
	t_start = time()
	while (time() - t_start < 750) {
		alga(speed, -speed)
	}
	alga(0, 0)
}

// action on the LEFT button press
func num left_button() {
	alpha = ALPHA_LEFT
	t_start = time()
	while (time() - t_start < 750) { 
		alga(speed, 0)
	}
	alga(0, 0)
	kicker()
	alpha = COMPASS_ALPHA
	while (wall_button() == 0) {
		alga(-speed, 0)
	}
	t_start = time()
	while (time() - t_start < 750) { 
		alga(speed, speed)
	}
	alga(0, 0)
}

// action on the DOWN button press
func num down_button() {
	t_start = time()
	while (time() - t_start < 750) {
		alga(speed, 0)
	}
	alga(0, 0)
	while (dir > 2 and dir < 8 and strres < 100) {
		yield()
	}
}

// action on the RIGHT button press
func num right_button() {
	alga(0, 0)
	tmp_zone = current_zone()
	if (tmp_zone == 0) {
		t_start = time()
		while (time() - t_start < 750) {
			alga(speed, speed)
		}
	} else {
		if (tmp_zone == 1) {
			t_start = time()
			while (time() - t_start < 750) {
				alga(speed, 0)
			}
		} else {
			t_start = time()
			while (time() - t_start < 750) {
				alga(speed, -100)
			}
		} 
	}
}

// action on the ENTER button press
func num enter_button() {
	while (btn.rn == "") {
		yield()
	}
	odometry_reset()
}

func num check_buttons() {
	cur_btn = btn.rn()

	if (cur_btn == "") {
		return 0
	} else {
		main_pause()
	}
	
	if (cur_btn == "E") {
		enter_button()
		return 1
	}
	if (cur_btn == "U") {
		up_button()
		return 1
	}
	if (cur_btn == "L") {
		left_button()
		return 1
	}
	if (cur_btn == "D") {
		down_button()
		return 1
	}
	if (cur_btn == "R") {
		right_button()
		return 1
	}
}

// buttons thread
void buttons {
	while (true) {
		btn_code = check_buttons()
		if (btn_code == 1) {
			main_resume()
		}
	}
}

// start all of the previously defined threads
new.thread = sensors
new.thread = odometry
new.thread = buttons

// alga function
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
	turn = err_com * 0.25 + (err_com - err_com_old) * 0.75
	err_com_old = err_com

	mt.spw("B", v_b*k_m + turn)
	mt.spw("C", v_c*k_m + turn)
	mt.spw("D", v_d*k_m + turn)
}

// interface functions go there 

// kicker function
func num kicker() {
	// ... 
}

// attack function
func num attack() {
	// ...
}

// check if back light sensor has detected a ball behind the robot
func num check_back() {
	if (l_b > BACK_LIGHT_VALUE) {
		return 1
	} else {
		return 0
	}
}

func num avoid_ball() {
	alga(0, speed)
}

// go_back function
func num go_back() {
	// NOTE: might be incorrect way to go back, idk
	alga(-speed, 0)
	if (check_back() == 1) {
		avoid_ball()
	} 
} 

// TODO:
// 1. add to calibration str range, after which we consider the dir we got is 0

//  main thread
while (true) {
	while (main_lock == 1) {
		yield()
	}

	if (dir > 2 and dir < 9) {
		attack()
	} else {
		go_back()
	}
}
