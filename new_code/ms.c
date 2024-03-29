// check if all of the ports we are using are working correctly
check.ports("ABCD 1234")
sen.setmode(1, 1)
sen.setmode(3, 1)

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
kicker_done = 0
kicker_time = 0
connection_status = 0

// voids

prev_dir = 0
zeroes_cnt = 0

func num filter_dir(raw_dir) {
    if (raw_dir == 0 and prev_dir != 0 and prev_dir != 1 and prev_dir != 9) {
        if (zeroes_cnt < 15) {
            zeroes_cnt = zeroes_cnt + 1
        } else {
            zeroes_cnt = 0
            prev_dir = raw_dir
        }
    } else {
        zeroes_cnt = 0
        prev_dir = raw_dir
    }
    return prev_dir
}

// sensors thread
void sensors {
    while (true) {
        irseeker_array = i2c.readregs(4, 8, 73, 6)
        dir = filter_dir(irseeker_array[0])
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

		if (kicker_done == 1 and time() - kicker_time > 500) {
			kicker_done = 0
			mt.shd.pw("A", -100, 0, 100, 0, true)
		}
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
			if (enc_delta[i] == 0) {
				stopped_cnt = stopped_cnt
			}
		}

		if (stopped_cnt > 1) {
			for (i = 0; i < 3; i++) {
				enc_delta[i] = 0
			}
		}

		// calculating x and y of the delta vector
		x_del = (-2 * sin(thetta) / 3) * enc_delta[0] + (-2 * sin(pi/3 - thetta) / 3) * enc_delta[1] + (-2 * cos(pi/3 + thetta) / 3) * enc_delta[2]
		y_del = (2 * cos(thetta) / 3) * enc_delta[0] + (-2 * cos(pi/3 - thetta) / 3) * enc_delta[1] + (-2 * cos(pi/3 + thetta) / 3) * enc_delta[2]

		// robot position
		x_res = x_res + x_del
		y_res = y_res + y_del
		
		// save values from this iteration
		for (i = 0; i < 3; i++) {
			enc_prev[i] = enc_cur[i]
		}
		
		// delay
		while (time() - iter_start < freq) {
			printupd()
			print("x", x_res)
			print("y", y_res)
			print("dir", dir)
			print("conn", connection_status)
			print("mode", mode)
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

	v_b = -0.67*s
	v_c = 0.58*f + 0.33*s
	v_d = -0.58*f + 0.33*s

	k_m = max(abs(f), abs(s))/max(abs(v_b), max(abs(v_c), abs(v_d)))
	turn = err_com * 0.5 + (err_com - err_com_old) * 1.5
	err_com_old = err_com

	mt.spw("B", v_b*k_m - turn)
	mt.spw("C", v_c*k_m - turn)
	mt.spw("D", v_d*k_m - turn)
}

// interface functions go there 

// kicker function
func num kicker(attack_start) {
	if (time() - attack_start < 500) {
		return 0
	}
	if (kicker_done == 0) {
		mt.shd.pw("A", -100, 0, 100, 0, true)
		kicker_time = time()
		kicker_done = 1
	}
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

// go_back function
func num go_back() {
	alga(-100, 0)

	if (check_back() == 1) {
		avoid_ball()
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
		alga(100, 0)
	}
	alga(0, 0)
	// kicker()
	alpha = COMPASS_ALPHA
	while (wall_button() == 0) {
		alga(-100, 0)
	}
	t_start = time()
	while (time() - t_start < 750) {
		alga(100, -100)
	}
	alga(0, 0)
}

// action on the LEFT button press
func num left_button() {
	return 0 // disabled atm

	alpha = ALPHA_LEFT
	t_start = time()
	while (time() - t_start < 750) { 
		alga(100, 0)
	}
	alga(0, 0)
	kicker()
	alpha = COMPASS_ALPHA
	while (wall_button() == 0) {
		alga(-100, 0)
	}
	t_start = time()
	while (time() - t_start < 750) { 
		alga(100, 100)
	}
	alga(0, 0)
}

// action on the DOWN button press
func num down_button() {
	return 0 // disabled atm

	t_start = time()
	while (time() - t_start < 750) {
		alga(100, 0)
	}
	alga(0, 0)
	while (dir > 2 and dir < 8 and strres < 100) {
		yield()
	}
}

right_buff = 0
// action on the RIGHT button press
func num right_button() {
	right_buff = 1
}

// action on the ENTER button press
func num enter_button() {
	btn.wait.release()
	while (btn.rn == "") {
		alga(0, 0)
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

// set up mailboxes and other bt stuff
my_name = getname()
who_am_i = my_name
if (my_name == "BIBA") {
	other_name = "BOBA"
	other_mail = "mailbox_boba"
} else {
	if (my_name == "BOBA") {
		other_name = "BIBA"
		other_mail = "mailbox_biba"
	} else {
		other_name = ""
	}
}
who_aint_me = other_name

mail_biba = new.mailbox("mail_biba")
mail_boba = new.mailbox("mail_boba")
mail_mode = new.mailbox("mail_mode")

bt_iteration = 0;
bt_iteration2 = -1;
bt_loss_count = 0;

// bluetooth function, returns mode
// 1 - attack
// 0 - go back
func num bluetooth() { 
	bt_iteration = bt_iteration + 1;
	bt_iteration = rm(bt_iteration, 100);
	// prepare data to be sent
	btstr = bt_iteration + " " + strres + " " + dir
	// put the data into the mailbox
	bt.send(other_name, other_mail, btstr)

	// now check mailbox and parse data from there
	if (my_name == "BIBA") {
		btstr2 = bt.last(mail_biba)
	} else {
		btstr2 = bt.last(mail_boba)
	}
	
	res_arr = parse(3, btstr2)
	prev_bt2 = bt_iteration2
	bt_iteration2 = tonum(res_arr[0])
	strres2 = tonum(res_arr[1])
	dir2 = tonum(res_arr[2])

	// check if the other robot is alive
	if (bt_iteration2 == prev_bt2) {
		if (bt_loss_count < 50) {
			bt_loss_count = bt_loss_count + 1;
		} else {
			connection_status = 0;
		}
	} else {
		bt_loss_count = 0;
		connection_status = 1;
	}

	// if not, then we go into the attack mode
	if (connection_status == 0) {
		return 1
	}

	// compare values and measure the mode
	if (abs(strres - strres2) < 15) {
		if (abs(dir - 5) == abs(dir2 - 5)) {
			if (strres > strres2) {
				return 1;
			} else {
				return 0;
			}
		} else {
			if (abs(dir - 5) > abs(dir2 - 5)) {
				return 0;
			} else {
				return 1;
			}
		}
	} else {
		if (strres > strres2) {
			return 1;
		} else {
			return 0;
		}
	}
}

// buttons thread
void buttons {
	while (true) {
		btn_code = check_buttons()
		if (btn_code == 1) {
			main_resume()
		}

		// if (other_name != "") {
		// 	mode = bluetooth()
		// } else {
		// 	tone(100, 100, 100)
		// 	mode = 1
		// }


		if (mode == 1) {
			led(2)
		} else {
			led(1)
		}
	}
}

void bt {
    bt_iteration = 0;
    bt_iteration2 = -1;
    bt_loss_count = 0;
    while (true) {
        start_bt:

        // while (stop_modes == 1) {
        //     thread.yield();
        // }

        str_scaled = round(strres * 100 / STR_MAX);

        if (who_am_i == "BIBA") {
            mybtstr = attack + " " + bt_iteration + " " + str_scaled + " " + dir;
        } else {
            mybtstr = str_scaled + " " + dir + " " + attack + " " + bt_iteration;
        }
        
        if (who_am_i == "BIBA") {
            bt.send(who_aint_me, "mail_biba", mybtstr);

            btstr2 = bt.last(mail_boba);
            res = parse(4, btstr2);

            str_res2 = res[0];
            dir2 = res[1];
            attack2 = res[2];
            prev_bt2 = bt_iteration2;
            bt_iteration2 = res[3];
        } else {
            bt.send(who_aint_me, "mail_boba", mybtstr);

            btstr2 = bt.last(mail_biba);
            res = parse(2, btstr2);

            attack2 = res[0];
            prev_bt2 = bt_iteration2;
            bt_iteration2 = res[1];
            str_res2 = res[2];

            mode = tonum(bt.last(mail_mode));
        }
            
        if (bt_iteration2 == prev_bt2) {
            if (bt_loss_count < 50) {
                bt_loss_count = bt_loss_count + 1;
            } else {
                connection_status = 0;
            }
        } else {
            bt_loss_count = 0;
            connection_status = 1;
        }

        if (connection_status == 0) {
            // tone(100, 100, 100);
            mode = 1;
            goto start_bt;
        }
        
        //////////////////////////////////////////////////////////////////////
        
        if (who_am_i == "BIBA") {
            // if (attack == 1 and attack2 == 0) {
            //     mode = 1;
            // } else {
            //     if (attack == 0 and attack2 == 1) {
            //         mode = 0;
            //     } else {
            //         if (abs(str_scaled - str_res2) < 15) {
            //             if (abs(dir - 5) == abs(dir2 - 5)) {
            //                 if (str_scaled > str_res2) {
            //                     mode = 1;
            //                 } else {
            //                     mode = 0;
            //                 }
            //             } else {
            //                 if (abs(dir - 5) > abs(dir2 - 5)) {
            //                     mode = 0;
            //                 } else {
            //                     mode = 1;
            //                 }
            //             }
            //         } else {
            //             if (str_scaled > str_res2) {
            //                 mode = 1;
            //             } else {
            //                 mode = 0;
            //             }
            //         }
            //     }
            // }

            if (abs(str_scaled - str_res2) < 15) {
                if (abs(dir - 5) == abs(dir2 - 5)) {
                    if (str_scaled > str_res2) {
                        mode = 1;
                    } else {
                        mode = 0;
                    }
                } else {
                    if (abs(dir - 5) > abs(dir2 - 5)) {
                        mode = 0;
                    } else {
                        mode = 1;
                    }
                }
            } else {
                if (str_scaled > str_res2) {
                    mode = 1;
                } else {
                    mode = 0;
                }
            }
            
            bt.send(who_aint_me, "mail_mode", abs(mode-1));
        }

        bt_iteration = bt_iteration + 1;
        bt_iteration = rm(bt_iteration, 100);
    }
}

// start all of the previously defined threads
new.thread = sensors
new.thread = odometry
new.thread = buttons
// new.thread = bt

// TODO:
// 1. fix odometry behavior when only one wheel is working

//  main thread
while (true) {
	while (main_lock == 1) {
		yield()
	}

	if (mode == 1) {
		right_buff = 0
		is_aligned = 0
		if (l_f > FRONT_LIGHT_VALUE) {
			//attack
			t_attack = time()
			k = 2
			while (l_f - FRONT_LIGHT_VALUE > -5 and main_lock == 0 and mode == 1) {
				// if (time() < 20000) {
				// 	if (current_zone() == 0) {
				// 		turn = rm(compass - ALPHA_LEFT + 900, 360) - 180
				// 	} else {
				// 		if (current_zone() == 2) {
				// 			turn = rm(compass - ALPHA_RIGHT + 900, 360) - 180
				// 		} else {
				// 			turn = err_com
				// 		}
				// 	}
				// }

				turn = err_com

				if (k < 4) { 
            		k = k + 0.001
        		}

				mt.start("B", -turn * k)
				mt.start("C", 100)
				mt.start("D", -100)
				kicker(t_attack)
				
				
				// if (time() - t_attack > 1000) {
				// 	mt.shd.pw("A", 100, 0, 80, 0, true)
				// 	kick = 1
				// } 
				
				// if (time() - t_attack > 1500 and time() - t_attack < 1800) {
				// 	mt.shd.pw("A", -100, 0, 90, 0, true)
				// }
			}
		} else {
			//back
			if (abs(dir - 5) > 2 or (dir == 7 and strres < 40)) {
				go_back()
			} else {
				if (dir != 5) {
					if (strres < 50) {
						v = (133 - strres) * 1.1 + 20
						
						if (v < 35) {
							v = 35
						}
						tone(100, 100, 1)
						alga(50, v*(dir-5)/abs(dir-5))
					} else {
						v = (133 - strres) * 1.68 + 50
						
						if (v < 50) {
							v = 50
						}
						alga(-20, v*(dir-5)/abs(dir-5))
					}
				} else {
					alga(100, 0)
				}
			}
		}
	} else {
		if (right_buff = 0) {
			go_back()
		} else {
			alga(0, 0)
			if (is_aligned == 0) {
				tmp_zone = current_zone()
				if (tmp_zone == 0) {
					t_start = time()
					while (time() - t_start < 750 and main_lock == 0) {
						alga(100, 100)
					}
				} else {
					if (tmp_zone == 1) {
						t_start = time()
						while (time() - t_start < 750 and main_lock == 0) {
							alga(100, 0)
						}
					} else {
						t_start = time()
						while (time() - t_start < 750 and main_lock == 0) {
							alga(100, -100)
						}
					}
				}
				is_aligned = 1
			}
		}
	}
}