/*TODO:
    +1. attack
    +2. modes
    +3. odometry
    4. attack angles
    5. starts 
*/

check.ports("BCD 1234")

//  BT
who_am_i = getname();

if (who_am_i == "BOBA") {
  who_aint_me = "BIBA";
    mode = 1
} else {
  who_aint_me = "BOBA";
    mode = 0
}

//connect(who_aint_me);

mailbox_biba = new.mailbox("mailbox_biba");
mailbox_boba = new.mailbox("mailbox_boba");
mailbox_mode = new.mailbox("mailbox_mode");
connection_status = 1;

sen.setmode(1, 1)
mt.stop("ABCD", "false")
mt.resetcount("ABCD")
//  calibration
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

/*  
    close = 0 stop
    close = 1 ball is behind
    close = 2 ball is in side
    close = 3 ball is in front
    close = 4 attack
    close = 5 avoid ball
    close = 6 go to middle(odometry)
    close = 7 nixua 
    close = 8 fast side
    close = 9 slow side
*/

//  voids

//  sensors + closes
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

//  Motor manager
func num alga(Vector_R, tizlek) {

    v_r = err_com * 0.3 + (err_com - err_com_old) * 0.6
    err_com_old = err_com

    if (Vector_R < 0) {
        motor_Power_A = tizlek * (cos(rad(360+Vector_R)) * cos(rad(330)) - sin(rad(360+Vector_R)) * sin(rad(330))) + v_r
        motor_Power_B = tizlek * (cos(rad(360+Vector_R)) * cos(rad(90)) - sin(rad(360+Vector_R)) * sin(rad(90))) + v_r
        motor_Power_C = tizlek * (cos(rad(360+Vector_R)) * cos(rad(210)) - sin(rad(360+Vector_R)) * sin(rad(210))) + v_r
    } else {
        if (Vector_R > 0) {
            motor_Power_A = tizlek * (cos(rad(Vector_R)) * cos(rad(330)) - sin(rad(Vector_R)) * sin(rad(330))) + v_r
            motor_Power_B = tizlek * (cos(rad(Vector_R)) * cos(rad(90)) - sin(rad(Vector_R)) * sin(rad(90))) + v_r
            motor_Power_C = tizlek * (cos(rad(Vector_R)) * cos(rad(210)) - sin(rad(Vector_R)) * sin(rad(210))) + v_r
        } else {
            if (Vector_R == 0) {
                motor_Power_A = tizlek + v_r
                motor_Power_B = v_r
                motor_Power_C = -tizlek + v_r
            }
        }
    }

    mt.start("C", motor_Power_A)
    mt.start("B", motor_Power_B)
    mt.start("D", motor_Power_C)
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
motor_koefficients[1] = -1.5;
motor_koefficients[2] = 1.5;

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
        for (i = 0; i < 3; i = i + 1) {
            encoders_delta[i] = (encoders_current[i] - encoders_prev[i]) * motor_koefficients[i];
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

//  bluetooth
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
            bt.send(who_aint_me, "mailbox_biba", mybtstr);

            btstr2 = bt.last(mailbox_boba);
            res = parse(4, btstr2);

            str_res2 = res[0];
            dir2 = res[1];
            attack2 = res[2];
            prev_bt2 = bt_iteration2;
            bt_iteration2 = res[3];
        } else {
            bt.send(who_aint_me, "mailbox_boba", mybtstr);

            btstr2 = bt.last(mailbox_biba);
            res = parse(2, btstr2);

            attack2 = res[0];
            prev_bt2 = bt_iteration2;
            bt_iteration2 = res[1];
            str_res2 = res[2];

            mode = tonum(bt.last(mailbox_mode));
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
            
            bt.send(who_aint_me, "mailbox_mode", abs(mode-1));
        }

        bt_iteration = bt_iteration + 1;
        bt_iteration = rm(bt_iteration, 100);
    }
}

// buttons
// void buttons_thread {
//     stop_modes = 0;
//     while (true) {
//         if (btn.rn == "E") {
//             stop_modes = 1;
//             prev_mode = mode; 
//             mode = 3;
//             btn.wait.release();
//             while (btn.rn != "E") {
//                 mode = 3;
//                 thread.yield(); // do nothing
//             }
//             reset_odometry();
//             mode = prev_mode;
//             stop_modes = 0;
//             btn.wait.release();
//         }

//         time_buttons = time();
//         while (time() - time_buttons < 50) {
//             thread.yield();
//         }
//     }
// }

btn.wait.release();

//  threads
// new.thread = buttons_thread
new.thread = sensors
new.thread = bt
new.thread = odometry

//  main
while (true) {
    if (mode == 0) {
        if (attack2 = 1) {
            attacks_count = 1;
        }
        led(1)

        if (dir == 5) {
            alga(0, 0)
        } else {
            if ((abs(dir - 5) < 3) or (attacks_count == 1 and abs(str_res2 - 100) < 20)) {
            // if ((abs(dir - 5) < 3) or abs(str_res2 - 100) < 20) {
                if (strres < 100) {
                    alga(90*(5-dir)/abs(5-dir), 40)
                } else {
                    alga(90*(5-dir)/abs(5-dir), 70)
                }
            } else {
                alga(180, 100)
                if (l_b > BACK_LIGHT_VALUE) {
                    t0 = time()
                    while (time() - t0 < 300) {
                        alga(-90, 100)     //100
                        tone(100, 100, 1)
                    }

                    t0 = time()
                    while (time() - t0 < 300) {
                        alga(180, 100)    //-100
                        tone(100, 100, 1)
                    }
                }
            }
        }
    } else {
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
			    alga(180, 100)
                if (l_b > BACK_LIGHT_VALUE) {
                    t0 = time()
                    while (time() - t0 < 300) {
                        alga(-90, 100)     //100
                        tone(100, 100, 1)
                    }

                    t0 = time()
                    while (time() - t0 < 300) {
                        alga(180, 100)    //-100
                        tone(100, 100, 1)
                    }
                }
			} else {
				if (dir != 5) {
					if (strres < 50) {
						v = (133 - strres) * 1 + 20
						
						if (v < 45) {
							v = 45
						}
						tone(100, 100, 1)
						alga(60*(5-dir), v)
					} else {
						v = (133 - strres) * 1.68 + 50
						
						if (v < 50) {
							v = 50
						}
						alga(110*(5-dir), v)
					}
				} else {
					alga(0, 100)
				}
			}
		}
    }
}
