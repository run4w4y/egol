/*TODO:
  Main: check new idea of definition side of orbit 
  1. kicker
  2. attack
  3. odometry
*/

check.ports("ABC 1234")
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
t_padik = time()
i = 0
alpha = com_3
r = 130
sgn = 0
lim_com_down = 0
lim_com_up = 0
dir2 = 0
dir3 = 0
time_def = time()
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

void dir_orbit {
  if (dir > 4) {
    dir2 = dir1
  } else {
    if (dir3 == 9) {
      dir2 = 9
    } else {
      if (dir3 == 0) {
        dir2 = 9
      } else {
        dir2 = dir1
      }
    }
  }
}

void orbit {
    // поворот на мяч перед орбитой
  t0 = time()
  while (dir != 6 and time() - t0 < 1000) {
    u=-(22*(6-dir)+0.08*((str3-str4))+sgn*10)
  
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
  if (err_com > 0) {
  //----------------------RIGHT------------------------
    
    err90 = 0
    t0 = time()
    // заход на орбиту
    //r1
    while (true) {
      u=-(15*((3-dir1))-((3-dir1)/5*10))
      err90=rm(compass - com_r + 900, 360) - 180
      v=50

      if (err90 > 10) {
        if (dir < 5) {
          Goto exit1
        }
      }

      if (time() - t0 > 800) {
        Goto exit1
      }
    }
    exit1: 
    
    t0 = time()
    //орбита
    //r2
    err90 = 999
    while (err90 > 8) {
      if (dir > 5) {
        if (dir > 6) {
          err90=rm(compass - com_r + 900, 360) - 180+45
        } else {
          err90=rm(compass - com_r + 900, 360) - 180-40
        }
      } else {
        err90=rm(compass - com_r + 900, 360) - 180
      }

      if (dir == 3) {
        v=80
        d=145
        l=r+30
        u=-((1*(d*90)/(2*l))+(0.4*(r-strres)))

      } else {
        u=-((20*((3-dir1))-((3-dir1)/5*10))+(0.45*(r-strres)))
        v=70
      }


      if (time() - t0 > 4500) {
        t0 = time()
        Goto exit2
      } else {
        if (time() - t0 > 1500) {
          if (compass > lim_com_down) {
            if (compass < lim_com_up) {
              if (strres < 140) {
                Goto exit2
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
    //r3
  
    if (err90 < 21) {
      while (abs(err_com) > 20 and l1 + l2 < l1_cal + l2_cal) {
      
        v = 48
        u = -50

        if (dir < 7) {
          if (abs(err_com) < 69) { 
            Goto exit2
          }
        }

        if (time() - t0 > 800) {
          Goto exit2
        }
      }
    }
  } else {

    //----------------------LEFT------------------------  


    err90 = 0

    t0 = time()
    // заход на орбиту
    //l1

    while (true) {

      dir_orbit()

      u=-(15*((7-dir2))-((7-dir2)/5*10))
      err90 = rm(compass - com_l + 900, 360) - 180
      v=50
      dir3 = dir2

      if (err90 < -9) {
       if (dir > 7) {
          Goto exit3
        }
      }

      if (time() - t0 > 800) {
        Goto exit3
      } 
    }
    exit3:
    
    t0 = time()
    // орбита
    //l2

    err90 = -999
    v = 0
    u = 0
    dir2 = dir1
    dir3 = dir2

    while (err90 < -8) {
      if (dir > 5) {
        if (dir > 6) {
          err90 = rm(compass - com_l + 900, 360) - 180+40
        } else {
          err90 = rm(compass - com_l + 900, 360) - 180-45
        }

      } else {
        err90 = rm(compass - com_l + 900, 360) - 180
      }

      dir_orbit()

      if (dir == 8) {
        v=80
        d=145
        l=r+30
        u=-((-1*(d*90)/(2*l))+(0.4*(strres - r)))
      } else {
        u=-((20*((7-dir2))-((7-dir2)/5*10))+(0.45*(strres - r)))
        dir3 = dir2
        v=70
      }

      if (time() - t0 > 4500) {
        t0 = time()
        Goto exit2
      } else {
        if (time() - t0 > 1500) {
          if (compass > lim_com_down) {
            if (compass < lim_com_up) {
              if (strres < 140) {
                Goto exit2
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
    //l3
    if (err90 > -22) {
      while (abs(err_com) > 20 and l1 + l2 < l1_cal + l2_cal) {

        v = 48
        u = 50

        if (dir > 5) {
          if (abs(err_com) < 69) {
            Goto exit2
          }
        }

        if (time() - t0 > 800) {
          Goto exit2
        }
      }
    }
  }
  exit2:
}

void padik {
  if (abs(dir1 - 5) > 1) {
    u=20*(dir1-5)
    v=0
  } else {
    v = (120-0.5*strres)
    u = 20*(dir-6)+0.065*((0.1*(str5-str2))+(0.6*(str3-str4)))
  } 
}

void kicker {
  mt.shd.pw("A", 100, 0, 200, 0, true)
}
//Threads

new.thread = sensors



// Main
while (true) {
  if (strres > str_max - 5) {
    if (abs(err_com)>90) {
      tone(100,100,100)
      orbit()
    } else {
      i = 0
      er_dir_old = 0
      er_str_old = 0
      while (strres > str_max - 30 and l1 + l2 < l1_cal + l2_cal) { //slow padik
        padik()
      }
      t_attack = time()
      while (l1 + l2 > l1_cal + l2_cal - 10) {
        tone(100,100,100)
        v = 100
        u = -err_com

        if (time() - t_attack > 500 and time() - time_def > 1000) {
          kicker()
          t_attack = time()
          time_def = time()
        }
      }
    }
  } else {
    i = 0
    er_dir_old = 0
    er_str_old = 0
    while (strres < str_max - 5) {  //padik fast
      padik()
    }
  }
}