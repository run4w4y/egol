//check.ports("ABCD 234")

//  BT

who_am_i = getname();

if (who_am_i == "BOBA") {
	who_aint_me = "BIBA";
} else {
	who_aint_me = "BOBA";
}

//connect(who_aint_me);

mailbox_biba = new.mailbox("mailbox_biba");
mailbox_boba = new.mailbox("mailbox_boba");
mailbox_mode = new.mailbox("mailbox_mode");


sen.setmode(3, 1)
mt.stop("ABCD", "false")
mt.spw("a", 50)
//calibration
handle = open.r("cal.txt");

COMPASS_ALPHA = tonum(readline(handle));
SEEKER_DIV = tonum(readline(handle)) / 100;
KP_MOVE = tonum(readline(handle))/100;
STR_MAX = tonum(readline(handle));
BACK_LIGHT_VALUE = tonum(readline(handle));
STR_VALUE_2 = tonum(readline(handle));
STR_VALUE_4 = tonum(readline(handle));

closef(handle);

//variables
alpha = COMPASS_ALPHA
forward = 0
side = 0
v = 50
f = 0
s = 0
close = 1 
/*  
    close = 1 ball is behind
    close = 2 ball is in side
    close = 3 ball is in front
    close = 4 attack
    close = 5 avoid ball
    close = 6 diagonal
*/

//flags 
attack = 0

//Subs

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
        if (dir == 5) {
            close = 3
            if (str3 > STR_MAX - 5) {
                close = 4
            }
        } else {
            if (abs(dir - 5) < 3 and (str2 > 30 or str4 > 30)) {
                close = 2
            } else {
                if (dir == 1 or dir == 2 or dir == 8 or (dir == 7 and str4 < 40)) {
                    close = 6
                } else {
                    close = 1
                    if (l_b > BACK_LIGHT_VALUE) {
                        close = 5
                    }
                }
            }
        }
    }
}

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
new.thread = sensors
new.thread = MM

while (true) {

    printupd()
    print("state", close)
    mt.spw("A", 50)

    while (close == 1) {    //ball is behind
        forward = -100
        side = 0
    }

    while (close == 2) {    //ball is in side
        forward = 0
        side = (dir - 5) * 50
    }

    while (close == 3) {    //ball is in front
        forward = STR_MAX - str3 + 40
        side = 0
    }

    t = time()
    v = 50

    while (close == 4 or str3 > 180) {    //attack
        attack = 1
        forward = 0
        side = 0

        b_power = 1.5 * err_com + err_com*v*0.01;

        tone(100,100,100)
        if (v < 100) {
          v = v + 2
        }

        if (abs(err_com) < 5) {
          b_power = 0;
        } else {
          if (abs(b_power) < 10) {
            if (b_power > 0) {
              b_power = 10;
            } else {
              b_power = -10;
            }
          }
        }

        u = err_com * 0.008
        mt.spw("C", v-u)
        mt.spw("D", v+u)
        mt.spw("B", -b_power)

        if (time() - t > 1200 and time() - t < 1400) {
          mt.spw("a", -100)
        } else {
          mt.spw("a", 50)
        }
    }
    attack = 0

    t_avoid = time()
    while (close == 5 or time() - t_avoid < 500) {    //avoid ball
        forward = 0
        side = 100
    }

    while (close == 6) {
        tone(100, 1000, 100)
        forward = -100
        side = (dir - 5) * 100
    }
}