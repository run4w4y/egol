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
alpha = com_3
r = 130
sgn = 0
lim_com_down = 0
lim_com_up = 0
com_l = 180
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

    dir1 = rm(dir+9, 10)
    if (rm(dir,2) == 0) {
      strres = (str1 + str2 + str3 + str4 + str5)/1.76
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

void orbit_right {
  t0 = time()
  err90 = 0
  // заход на орбиту
  while (true) {
    u=(15*((dir1-3))-((dir1-3)/5*10))
    err90=rm(compass - com_l + 900, 360) - 180
    v=70

    if (err90 > 10) {
      if (dir < 5) {
        goto exit1
      }
    }

    if (time() - t0 > 80) {
      goto exit1
    }
  }
  exit1: 
  
  // v = 0
  // u = 0
  // mt.stop("BC", "True")
  // btn.wait()

  t0 = time()

  //орбита
  err90 = 999
  while (err90 > 8) {
    if (dir > 5) {
      if (dir > 6) {
        err90=rm(compass - com_l + 900, 360) - 180+45
      } else {
        err90=rm(compass - com_l + 900, 360) - 180-40
      }
    } else {
      err90=rm(compass - com_l + 900, 360) - 180
    }

    if (dir == 3) {
      v=80
      d=145
      l=r+50
      u=-(d*90)/(2*l)+0.2*(strres-r)

    } else {
      u=20*(dir1-3)-(dir1-3)/5*10+0.2*(strres-r)
      v=80
    }

    if (time() - t0 > 600) {
      t0 = time()
      goto exit2
    } else {
      if (time() - t0 > 200) {
        if (compass > lim_com_down) {
          if (compass < lim_com_up) {
            if (strres < 90) {
              goto exit2
            }
          }
        }
      } else {
        lim_com_down = compass - 4
        lim_com_up = compass + 4
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!CAME
      }
    }
  }


  t0 = time()
  //поворот на мяч

  if (err90 < 21) {
    while (abs(err_com) > 20) {
      v = 48
      u = 50

      if (dir < 7) {
        if (abs(err_com) < 69) {
          goto exit2
        }
      }

      if (time() - t0 > 100) {
        goto exit2
      }
    }
  }
}

void orbit_left {

}

//Threads
new.thread = sensors

// Main

while (true) {
  while (dir != 6) {

  // поворот на мяч перед орбитой
    u=20*(dir1-5)+0.08*((str4-str3)) + sgn*10
  
    if (u > 0) {
      sgn = 1
    }

    if (u < 0) {
      sgn = -1
    } 
  
    if (u == 0) {
      sgn = 0
    }
    v=10
  }
  // определение стороны орбиты

  //if (err_com > 0) {
    orbit_right()
  //} else {
  //  orbit_left()
  //}
  exit2:
}