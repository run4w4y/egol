/*TODO:
    +1. attack
    2. modes
    3. если робот один на поле - 1 мод
    3. odometry
    4. attack angles
    5. starts 
*/

check.ports("ABCD 234")

//  BT
who_am_i = getname();

if (who_am_i == "BOBA") {
	who_aint_me = "BIBA";
} else {
	who_aint_me = "BOBA";
}

connect(who_aint_me);

mailbox_biba = new.mailbox("mailbox_biba");
mailbox_boba = new.mailbox("mailbox_boba");
mailbox_mode = new.mailbox("mailbox_mode");

sen.setmode(3, 1)
mt.stop("ABCD", "false")
mt.spw("a", 50)

//  calibration
handle = open.r("cal.txt");

COMPASS_ALPHA = tonum(readline(handle));
SEEKER_DIV = tonum(readline(handle)) / 100;
KP_MOVE = tonum(readline(handle))/100;
STR_MAX = tonum(readline(handle));
BACK_LIGHT_VALUE = tonum(readline(handle));
STR_VALUE_2 = tonum(readline(handle));
STR_VALUE_4 = tonum(readline(handle));

closef(handle);

//  variables
alpha = COMPASS_ALPHA
forward = 0
side = 0
v = 50
f = 0
s = 0
close = 0 
attack = 0
mode = 1

/*  
    close = 0 stop
    close = 1 ball is behind
    close = 2 ball is in side
    close = 3 ball is in front
    close = 4 attack
    close = 5 avoid ball
    close = 6 diagonal
*/

//  voids

//  sensors + closes
void sensors {
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
            led(2)
            if (dir == 5) {
                close = 3
                if (str3 > STR_MAX - 5) {
                    close = 4
                }
            } else {
                if (abs(dir - 5) < 3 and strres > 30) {
                    close = 2
                    if (str3 > STR_MAX - 5) {
                        close = 4
                    }
                } else {
                    close = 1
                    if (l_b > BACK_LIGHT_VALUE) {
                        close = 5
                    }
                }
            }
        } else {
            led(1)
            if (abs(dir - 5) < 3 and (str4 > 30 or str2 > 30)) {
                close = 0
            } else {
                close = 1
            }
        }
    }
}

//  Motor manager
void MM {
    while (true) {
        while (attack == 0) {
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

            mt.spw("B", v_b*k_m-err_com*1.5)
            mt.spw("C", v_c*k_m-err_com*0.8)
            mt.spw("D", v_d*k_m+err_com*0.8)
        }
    }
}

//  bluetooth
void bt {
	while (true) {
		if (who_am_i == "BIBA") {
			mybtstr = attack + "";
		} else {
			mybtstr = strres + " " + dir + " " + attack;
		}
		
		if (who_am_i == "BIBA") {
			bt.send(who_aint_me, "mailbox_biba", mybtstr);

			btstr2 = bt.last(mailbox_boba);
			res = parse(4, btstr2);

			str_res2 = res[0];
			dir2 = res[1];
			attack2 = res[2];
		} else {
			bt.send(who_aint_me, "mailbox_boba", mybtstr);

			btstr2 = bt.last(mailbox_biba);
			res = parse(2, btstr2);

			attack2 = res[0];

			mode = tonum(bt.last(mailbox_mode));
		}
		
		//////////////////////////////////////////////////////////////////////
		
		if (who_am_i == "BIBA") {
            if (abs(strres - str_res2) < 30) {
                if (abs(dir - 5) == abs(dir2 - 5)) {
                    if (strres > str_res2) {
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
                if (strres > str_res2) {
                    mode = 1;
                } else {
                    mode = 0;
                }
            }
			bt.send(who_aint_me, "mailbox_mode", abs(mode-1));
		}

		// buttons will be here
        printupd()
        print("mode", mode)
	}
}

//  threads
new.thread = sensors
new.thread = MM
new.thread = bt

//  main
while (true) {
    mt.spw("A", 50)
    while (close == 0) {
        forward = 0
        side = 0
    }

    while (close == 1) {    //ball is behind
        forward = -100
        side = 0
    }

    while (close == 2) {    //ball is in side
        forward = 0
        side = (dir - 5) * 100
    }

    v = 40
    while (close == 3) {    //ball is in front
        forward = v//STR_MAX - str3 + 40
        side = 0
        if (v < 100) {
            v = v + 0.5
        }
    }

    t = time()
    v = 40
    k = 2
    while (close == 4 or str3 > STR_MAX - 20) {    //attack
        attack = 1
        v = v + 1

        v_b = -err_com * k
        if (k < 4) { 
            k = k + 0.005
        } else {
            tone(100,100,1)
        }

        mt.spw("B", v_b)
        mt.spw("C", v)
        mt.spw("D", v)

        if (time() - t > 1500 and time() - t < 1700) {  //kicker
            mt.spw("A", -100)
        } else {
            mt.spw("A", 50)
        }
    }
    attack = 0

    while (close == 5) {    //avoid ball
        forward = 0
        side = 100
        tone(100,1000,1)
    }
}