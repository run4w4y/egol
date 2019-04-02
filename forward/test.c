mt.invert("BC")
mt.stop("BC", "false")

//calibration
handle = open.r("cal.txt")

if (handle == 0) {
  str_max = 0
  com_1 = 0
  com_2 = 0
  com_3 = 0
  com_4 = 0
  com_5 = 0
  l1_cal = 0
  l2_cal = 0
} else {
  str_max = tonum(readline(handle))
  com_1 = tonum(readline(handle))
  com_2 = tonum(readline(handle))
  com_3 = tonum(readline(handle))
  com_4 = tonum(readline(handle))
  com_5 = tonum(readline(handle))
  l1_cal = tonum(readline(handle))
  l2_cal = tonum(readline(handle))
}

//variables

v = 0
u = 0
alpha = 0
er_str_old = 0
er_dir_old = 0
t_padik = time()
i = 0

//Subs

void sensors {
  while (true) {
    irseeker_array = i2c.readregs(4, 8, 73, 6)
    dir1 = irseeker_array[0]
    str1 = irseeker_array[1]
    str2 = irseeker_array[2]
    str3 = irseeker_array[3]
    str4 = irseeker_array[4]
    str5 = irseeker_array[5]

    dir = rm(dir1+9, 10)
    if (rm(dir1,2) == 0) {
      strres = (str1 + str2 + str3 + str4 + str5)/1.76
    } else {
      strres = str1 + str2 + str3 + str4 + str5
    }

    compass_array = i2c.readregs(2, 1, 66, 4)
    compass = compass_array[0] * 2 + compass_array[1]
    err_com = rm(compass - alpha + 900, 360) - 180

    l1 = sen.percent(1)
    l2 = sen.percent(3)

  }
}

//Threads

new.thread = sensors

// Main

while (true) {
  while (strres < str_max - 60 and l1 < 30 and l2 < 30) {

    if (abs(dir - 5) > 1) {
      while (dir <> 5) {
        v = 0
        er_str = str4 - str3
        er_dir = dir - 5
        u = er_dir * 20 + (er_dir - er_dir_old)*66 + er_str * 0.03

        er_dir_old = er_dir
        er_str_old = er_str

        mt.spw("B", v + u)
        mt.spw("C", v - u)
      }
    } else {

      if (time() - t_padik < 1000) {
        if (strres > str_max - 40) {
          v = (str_max - strres) * 0.45 + 40

          if (v < 40) {
            v = 40
          }

        } else {
          v = 100
        }

        er_str = str4 - str3
        er_dir = dir - 5
        u_1 = er_dir * 15 + (er_dir - er_dir_old)*66 + er_str *0.02 + (er_str - er_str_old) * 66 + i * 0.003
        u = u_1 * v * 0.01

        if (abs(i) < 50) {
          i = i + er_str
        }

        if (dir == 5) {
          i = 0
          t_padik = time()
        }
      
        er_dir_old = er_dir
        er_str_old = er_str

        mt.spw("B", v + u)
        mt.spw("C", v - u)


      } else {
          
        tone(100,100,100)
        i = 0
        v = (str_max - strres) * 0.45

        if (v < 40) {
          v = 40
        }

        er_dir = dir - 5
        u = er_dir * 25 + (er_dir - er_dir_old)*66
        er_dir_old = er_dir

        mt.spw("B", v + u)
        mt.spw("C", v - u)

        if (dir == 5) {
          t_padik = time()
        }
      }
    }
  }

  mt.stop("BC", true)
  delay(2000)
}
