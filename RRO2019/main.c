/*TODO:
    +1. attack
    +2. modes
    +3. odometry
    4. attack angles
    5. starts 
*/

check.ports("ABCD 234")

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

sen.setmode(3, 1)
mt.stop("ABCD", "false")
mt.spw("a", 50)
mt.resetcount("ABCD")
//  calibration
handle = open.r("cal.txt");

COMPASS_ALPHA = tonum(readline(handle));
ALPHA_LEFT = tonum(readline(handle));
ALPHA_RIGHT = tonum(readline(handle));
STR_MAX = tonum(readline(handle));
BACK_LIGHT_VALUE = tonum(readline(handle));
STR_VALUE_2 = tonum(readline(handle));
STR_VALUE_4 = tonum(readline(handle));

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

        // if (rm(dir,2) == 0) {
        //   strres = (str1 + str2 + str3 + str4 + str5)/1.57 //1.9
        // } else {
        //   strres = str1 + str2 + str3 + str4 + str5
        // }

        strres = irseeker_array[1]
        for (i_dx = 2; i_dx < 6; i_dx++) {
            strres = max(strres, irseeker_array[i_dx])
        } 

        compass_array = i2c.readregs(2, 1, 66, 4)
        compass = compass_array[0] * 2 + compass_array[1]
        err_com = rm(compass - alpha + 900, 360) - 180
        turn = err_com

        l_b = sen.percent(3)
        // FSM

        if (mode == 1) {
            attacks_count = 0;
            led(2)
            if (dir == 5) {
                close = 3
                if (str3 > STR_MAX - 5) {
                    close = 4
                }
            } else {
                if (abs(dir - 5) < 3) {
                    if (strres < 100) {
                        close = 8
                    } else {
                        close = 2
                    }
                    if (str3 > STR_MAX - 5) {
                        close = 4
                    }
                } else {
                    if (dir == 0 and dir2 == 0 and connection_status == 1) {
                        close = 3;
                        tone(100, 100, 100);
                    } else {
                        close = 1
                        if (l_b > BACK_LIGHT_VALUE) {
                            close = 5
                        }
                    }
                }
            }
        } else {
            if (mode == 0) {
                if (attack2 = 1) {
                    attacks_count = 1;
                }
                led(1)

                if (dir == 5) {
                    close = 0
                } else {
                    if ((abs(dir - 5) < 3) or (attacks_count == 1 and abs(str_res2 - 100) < 20)) {
                    // if ((abs(dir - 5) < 3) or abs(str_res2 - 100) < 20) {
                        if (strres < 100) {
                            close = 9
                        } else {
                            close = 2
                        }
                    } else {
                        close = 1
                        if (l_b > BACK_LIGHT_VALUE) {
                            close = 5
                        }
                    }
                }
            } else {
                attacks_count = 0;
                led(0);
                close = 7;
            }
        }
    }
}

//  Motor manager
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
    
    v_b = 1.2*(-0.67*s)
    v_c = 0.58*f + 0.33*s 
    v_d = 0.58*f - 0.33*s

    k_m = max(abs(f), abs(s))/max(abs(v_b), max(abs(v_c), abs(v_d)))

    mt.spw("B", v_b*k_m-err_com*1.8)
    mt.spw("C", v_c*k_m-err_com*1)
    mt.spw("D", v_d*k_m+err_com*1)
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
    mt.spw("A", 50)
    while (close == 0) {
        alga(0, 0)
    }

    t_def = time()
    while (close == 1) {    //ball is behind
        alga(-100, 0)

        if (time() - t_def > 3000 and mode == 0) {
            t_h = time()
            while (time() - t_h < 500) {
                alga(100, 0)
            }
            t_def = time()
            move_lock = 1
        }
    }
    
    while (move_lock == 1 and mode == 0) {
        mt.spw("B", 0);
        mt.spw("C", 0);
        mt.spw("D", 0);
        thread.yield(); // do nothing
    }

    t_def = time()
    while (close == 2) {    //ball is in side
        move_lock = 0
        alga(-20, (dir - 5) * 100)

        if (time() - t_def > 4000) {
            t_h = time()
            while (time() - t_h < 350) {
                alga(100, 0)
            }
            t_def = time()
        }
    }

    v = 40
    while (close == 3) {    //ball is in front
        move_lock = 0
        alga(100, 0)
        // if (v < 100) {
        //     v = v + 1.5
        // }
    }

    t_attack = time()
    v = 40
    k = 2
    while (close == 4 or str3 > STR_MAX - 30) {    //attack
        move_lock = 0
        attack = 1
        //v = v + 1.5
        if (x_res > 500 and time() - time_odometry < 20000) {
            alpha_att = rm(compass - ALPHA_RIGHT + 900, 360) - 180
        } else {
            if (x_res < -500 and time() - time_odometry < 20000) {
                alpha_att = rm(compass - ALPHA_LEFT + 900, 360) - 180
            } else {
                alpha_att = err_com
            }
        }

        v_b = -alpha_att * k    
        if (k < 4) { 
            k = k + 0.002
        } else {
            tone(100,100,1)
        }

        mt.spw("B", v_b)
        mt.spw("C", 100)
        mt.spw("D", 100)

        if (time() - t_attack > 800 and time() - t_attack < 1000) {  //kicker
            mt.spw("A", -100)
        } else {
            mt.spw("A", 50)
        }
        tone(100,100,1)
    }
    attack = 0

    while (close == 5) {    //avoid ball
        move_lock = 0
        alga(0, 100)
        tone(100,1000,1)
    }

    while (close == 6) {
        alga(20, -x_res * 2)
    }

    while (close == 7) {
        mt.spw("B", 0);
        mt.spw("C", 0);
        mt.spw("D", 0);
        thread.yield(); // do nothing
    }

    t_def = time()
    while (close == 8) {
        move_lock = 0
        alga(10, (dir - 5) * 100)

        if (time() - t_def > 5000) {
            t_h = time()
            while (time() - t_h < 350) {
                alga(100, 0)
            }
            t_def = time()
        }

    }

    t_def = time()
    while (close == 9) {
        move_lock = 0
        alga(0, (dir - 5) * 100)

        if (time() - t_def > 5000) {
            t_h = time()
            while (time() - t_h < 350) {
                alga(100, 0)
            }
            t_def = time()
        }
    }
}