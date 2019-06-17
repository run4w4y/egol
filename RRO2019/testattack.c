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
close = 0 
/*  
    close = 0 stop
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
//new.thread = MM

v = 40
k = 2
while (true) {    //attack
    attack = 1
    v = v + 0.5
    if (k < 4) {
        k = k + 0.002
    } else {
        tone(100,100,1)
    }

    v_b = err_com * k
    
    mt.spw("B", -v_b)
    mt.spw("C", v)
    mt.spw("D", v)
}