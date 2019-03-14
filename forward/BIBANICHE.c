mt.invert("BC")

//calibration
handle = open.r("cal.txt")

if (handle == 0) {
  str_max = 0
  com_1 = 0
  com_2 = 0
  com_3 = 0
  com_4 = 0
  com_5 = 0
  light1 = 0
  light2 = 0
} else {
  str_max = tonum(readline(handle))
  values[0] = str_max
  com_1 = tonum(readline(handle))
  values[1] = com_1
  com_2 = tonum(readline(handle))
  values[2] = com_2
  com_3 = tonum(readline(handle))
  values[3] = com_3
  com_4 = tonum(readline(handle))
  values[4] = com_4
  com_5 = tonum(readline(handle))
  values[5] = com_5
  light1 = tonum(readline(handle))
  values[6] = light1
  light2 = tonum(readline(handle))
  values[7] = light2

}

//variables

forward = 0
turn = 0
alpha = 0
er_str_old = 0
er_dir_old = 0
t_padik = 0
i = 0

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

    if (rm(dir,2) == 0) {
      strres = (str1 + str2 + str3 + str4 + str5)/1.8
    } else {
      strres = str1 + str2 + str3 + str4 + str5
    }

    compass_array = i2c.readregs(2, 1, 66, 4)
    compass = compass_array[0] * 2 + compass_array[1]
    err_com = rm(compass - alpha + 900, 360) - 180

    l_1 = sen.percent(1)
    l_2 = sen.percent(3)

    //MotorManager

    mt.spw("B", forward + turn)
    mt.spw("C", forward - turn)
  }
}

//Threads

new.thread = sensors

// Main

t_padik = time()
while (true) {

  if (abs(dir - 6) > 1) {
    while (dir <> 6) {
      forward = 0
      er_dir = dir - 6
      turn = er_dir * 17 + (er_dir - er_dir_old)*66

      er_dir_old = er_dir
    }
  } else {
    if (time() - t_padik > 500) {
      forward = (str_max - strres)*0.45 + 60

      er_dir = dir - 6
      turn = er_dir * 40 + (er_dir - er_dir_old) * 66
      er_dir_old = er_dir
      tone(10,100,100)
      if (dir == 6) {
        t_padik = time()
      }

    } else {
      
      forward = (str_max - strres)*0.45 + 60
      er_dir = dir - 6
      er_str = str4 - str3

      turn = er_dir * 10 + (er_dir - er_dir_old) * 20 + er_str * 0.05 + (er_str - er_str_old) * 66 + i * 0.0001

      if (abs(i) < 30) {
        i = i + er_str
      }

      er_dir_old = er_dir
      er_str_old = er_str 

      if (dir == 6) {
        t_padik = time()
        i = 0
      }

    }
  }
}