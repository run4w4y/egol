//calibration
handle = open.r("cal.txt")

if (handle == 0) {
  str_max = 0
  com_1 = 0
  com_2 = 0
  com_3 = 0
  com_4 = 0
  com_5 = 0
  com_l = 0
  com_r = 0
  l1_cal = 0
  l2_cal = 0
} else {
  str_max = tonum(readline(handle))
  com_1 = tonum(readline(handle))
  com_2 = tonum(readline(handle))
  com_3 = tonum(readline(handle))
  com_4 = tonum(readline(handle))
  com_5 = tonum(readline(handle))
  com_l = tonum(readline(handle))
  com_r = tonum(readline(handle))
  l1_cal = tonum(readline(handle))
  l2_cal = tonum(readline(handle))
}

//variables

v = 0
u = 0

er_str_old = 0
er_dir_old = 0
alpha = com_3
lim_com_down = 0
lim_com_up = 0
dir2 = 0
dir3 = 0
name_sen = ""
name = getname()

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

    err_dir = dir - 6
    err_str = str4 - str3
    
    dir1 = rm(dir+9, 10)
    err_dir1 = dir1 - 5
    if (rm(dir,2) == 0) {
      strres = (str1 + str2 + str3 + str4 + str5)/1.57
    } else {
      strres = str1 + str2 + str3 + str4 + str5
    }

    compass_array = i2c.readregs(2, 1, 66, 4)
    compass = compass_array[0] * 2 + compass_array[1]
    err_com = rm(compass - alpha + 900, 360) - 180

    l1 = sen.percent(1)
    l2 = sen.percent(3)

    mt.spw("B", v + u)
    mt.spw("C", v - u)

  }
}

void bottom { // values in the bottom of the screen
	dotline(1, 0, 105, 178, 105, 4, 4)
	txt(1, 89 - len(name)*4, 114, 1, name)
	txt(1, 5, 114, 0, round(voltagern*100)/100)
	txt.left(1, 173, 114, 0, round(ampersrn()*100)/100)
}

void top {  // values in the top of the screen
    dotline(1, 0, 20, 178, 20, 4, 4)
    LCD.CenterText(1, 10, 1, name_sen)
}

new.thread = sensors

// interface

btn.exitdisallow(1)
i = 0
button_counter = 0
prev_button = ""
duration = 30 // duration (not in ms, it's sth else, idk lol)

while (true) {
	start: // instead of operator continue
	   
    btn = btn.rn
	if (btn == "L") { // left button action

		i = i - 1
		if (i == -1) {
			i = 2
		}
	}
	
	if (btn == "R") { // right button action

		i = i + 1
		if (i == 3) {
			i = 0
		}
	}

    if (i == 0) {
        name_sen = "IRseeker"
        printupd()
        print("dir", dir)
        print("dir1", dir1)
        print("str2", str2)
        print("str3", str3)
        print("str4", str4)
        print("str5", str5)
        print("str_res", strres)
        print("err_dir", err_dir)
        print("err_dir1", err_dir1)
        print("err_str", err_str)
    }

    if (i == 1) {
        name_sen = "compass"
        printupd()
        print("cm", compass)
        print("err1", rm(compass - com_1 + 900, 360) - 180)
        print("err2", rm(compass - com_2 + 900, 360) - 180)
        print("err3", rm(compass - com_3 + 900, 360) - 180)
        print("err4", rm(compass - com_4 + 900, 360) - 180)
        print("err5", rm(compass - com_5 + 900, 360) - 180)
        print("err_l", rm(compass - com_l + 900, 360) - 180)
        print("err_r", rm(compass - com_r + 900, 360) - 180)
    }

    if (i == 2) {
        name_sen = "lights" 
        printupd()
        print("l_1", l1)
        print("l_2", l2)
        print("sum", l1 + l2)
    }

    prev_button = btn
    delay(100)
    flush()
}