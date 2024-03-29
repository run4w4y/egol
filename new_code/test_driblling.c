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
prev_mode = 1

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

		if (kicker_done == 1 and time() - kicker_time > 500) {
			kicker_done = 0
			mt.shd.pw("A", -100, 0, 100, 0, true)
		}
    }
}

//  Motor manager
func num alga(Vector_R, tizlek) {
    v_r = err_com * 0.5 + (err_com - err_com_old) * 1.7
    err_com_old = err_com

    if (Vector_R < 0) {
        motor_Power_A = tizlek * (cos(rad(360+Vector_R)) * cos(rad(330)) - sin(rad(360+Vector_R)) * sin(rad(330))) - v_r
        motor_Power_B = tizlek * (cos(rad(360+Vector_R)) * cos(rad(90)) - sin(rad(360+Vector_R)) * sin(rad(90))) - v_r
        motor_Power_C = tizlek * (cos(rad(360+Vector_R)) * cos(rad(210)) - sin(rad(360+Vector_R)) * sin(rad(210))) - v_r
    } else {
        if (Vector_R > 0) {
            motor_Power_A = tizlek * (cos(rad(Vector_R)) * cos(rad(330)) - sin(rad(Vector_R)) * sin(rad(330))) - v_r
            motor_Power_B = tizlek * (cos(rad(Vector_R)) * cos(rad(90)) - sin(rad(Vector_R)) * sin(rad(90))) - v_r
            motor_Power_C = tizlek * (cos(rad(Vector_R)) * cos(rad(210)) - sin(rad(Vector_R)) * sin(rad(210))) - v_r
        } else {
            if (Vector_R == 0) {
                motor_Power_A = tizlek - v_r
                motor_Power_B = -v_r
                motor_Power_C = -tizlek - v_r
            }
        }
    }

    mt.start("C", motor_Power_A)
    mt.start("B", motor_Power_B)
    mt.start("D", motor_Power_C)
}

new.thread = sensors
delay(1000)

t0 = time()
while (time() - t0 < 500 /*and l_f - FRONT_LIGHT_VALUE > -5*/) {
    alga(30, 50)
}

tone(100, 100, 100)

k = 1
while (abs(-rm(compass - ALPHA_RIGHT + 900, 360) - 180) > 10) {
    if (k < 2.5) { 
        k = k + 0.001
    }
    mt.start("B", -(rm(compass - ALPHA_RIGHT + 900, 360) - 180)*k)
    mt.start("C", 50)
    mt.start("D", -50)
}

tone(100, 100, 100)

k = 1
while (l_f - FRONT_LIGHT_VALUE > -5) {
    turn = err_com

    if (k < 2.5) { 
        k = k + 0.001
    }

    mt.start("B", -turn * k)
    mt.start("C", 100)
    mt.start("D", -100)
}