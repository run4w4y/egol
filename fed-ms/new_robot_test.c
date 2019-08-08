check.ports("ABCD 234")
who_am_i = getname();
if (who_am_i == "BOBA") {
  who_aint_me = "BIBA";
    mode = 1
} else {
  who_aint_me = "BOBA";
    mode = 0
}

mt.shd.pw("A", -100, 0, 100, 0, true)
sen.setmode(3, 1)
sen.setmode(1, 1)
mt.stop("ABCD", "false")
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
FRONT_LIGHT_VALUE = tonum(readline(handle));
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
err_com_old = 0
r = 0
g = 0
b = 0
kick = 0

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

        // sen.read.rgb(1, r, g, b)
    
        // if (btn.rn == "R") {
        //     state = "goal"
        // } else {
        //     if (r > 2 and r < 20) {
        //         state = "wall"
        //     } else {
        //         if (r > 20) {
        //             state = "ball"
        //         } else {
        //             state = "none"
        //         }
        //     }
        l_f = sen.percent(3)
        l_b = sen.percent(1)
        //}
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
    
    v_b = 0.67*s
    v_c = -0.58*f - 0.33*s 
    v_d = 0.58*f - 0.33*s

    k_m = max(abs(f), abs(s))/max(abs(v_b), max(abs(v_c), abs(v_d)))
    turn = err_com * 0.5 + (err_com - err_com_old) * 2
    err_com_old = err_com

    mt.spw("B", v_b*k_m+turn)
    mt.spw("C", v_c*k_m+turn)
    mt.spw("D", v_d*k_m+turn)
}

new.thread = sensors

//  main
while (true) {
    // if (kick == 1) {
    //     mt.shd.pw("A", -100, 0, 100, 0, true)
    //     kick = 0
    // }
    if (l_f > FRONT_LIGHT_VALUE) {
        k = 0.5
        t_attack = time()
        while (l_f > FRONT_LIGHT_VALUE - 5) {

            // if (time() - t_attack > 1000) {
            //     mt.shd.pw("A", 100, 0, 100, 0, true)
            //     t_attack = time()
            //     kick = 1
            // }

            // if (kick == 1 and mt.isbusy("A") == "False") {
            //     mt.shd.pw("A", -100, 0, 100, 0, true)
            //     kick = 0
            // }
                    
            v = 100
            if (k < 1.2) {
                k = k + 0.005
            }
            mt.spw("B", err_com * k)
            mt.spw("C", -v)
            mt.spw("D", v)

            tone(50, 100, 1)
        }
    } else {
        if (abs(dir - 5) > 2 or (dir == 7  and strres < 55) or (dir == 3 and strres < 25)) {
            alga(-100, 0)

            if (l_b > BACK_LIGHT_VALUE) {
                t0 = time()
                while (time() - t0 < 300) {
                    alga(0, 100)
                    tone(100, 100, 1)
                }

                t0 = time()
                while (time() - t0 < 300) {
                    alga(-100, 0)
                    tone(100, 100, 1)
                }
            }
        } else {
            if (abs(dir - 5) > 1) {
                if (strres < 80) {
                    alga(50, 50*(dir-5))
                } else {
                    alga(-20, 50*(dir-5))
                } 
            } else{


                // if (l_b - 20 < 0) {
                //     l_acc = 0
                // } else {
                //     l_acc = (l_acc - 20) * 1.5
                // }
                // v = 280-1.5*strres + l_acc

                alga(100, 0)
            }
        }
    }
}