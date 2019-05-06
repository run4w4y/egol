/*TODO:
  Main: check new idea of definition side of orbit 
  1. slowing
  2. padik
  3. right orbit
  4. attack arc  
  5. communication with goalkeeper
  6. odometry side_check  
  7. block checking
*/

mt.spw("A", -50)
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
t_attack = time()
x = 0
y = 0
b = 0
c = 0

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
      strres = (str1 + str2 + str3 + str4 + str5)/1.57 //1.9
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

    e1 = mt.getcount("B")
    e2 = mt.getcount("C")

    dx = (e1 - b + e2 - c)*0.0453*sin(err_com*pi/180)
    dy = (e1 - b + e2 - c)*0.0453*cos(err_com*pi/180)
    x = x + dx
    y = -(y + dy)
    b = e1
    c = e2
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
    u=-(30*(6-dir)+0.08*(str3-str4))
    v = 10

    if (strres < 90) {
      Goto exit2
    }
  }
 

  // определение стороны орбиты
  if (/*err_com-(dir-6)*45 > 0*/err_com > 0) {
  //----------------------RIGHT------------------------
    
    err90 = 0
    t0 = time()
    // заход на орбиту
    //r1
    while (true) {
      u=-(20*((3-dir1))-((3-dir1)/5*10))
      err90=rm(compass - com_r + 900, 360) - 180
      v=70

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
    while (err90 > 0) {
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
        u=-((1*(d*90)/(2*l))+(0.45*(r-strres)))
      } else {
        u=-((20*((3-dir1))-((3-dir1)/5*10))+(0.4*(r-strres)))
        v=70
      }


      if (time() - t0 > 4500) {
        t0 = time()
        Goto exit2
      } else {
        if (time() - t0 > 1500) {
          if (compass > lim_com_down) {
            if (compass < lim_com_up) {
              if (strres < 90) {
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
      while (abs(err_com) > 10 and l1 + l2 < l1_cal + l2_cal and dir - 6 < 0) {
      
        v = 50
        u = -40

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

      u=-(20*((7-dir2))-((7-dir2)/5*10))
      err90 = rm(compass - com_l + 900, 360) - 180
      v=70
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

    while (err90 < 0) {
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
              if (strres < 90) {
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
      while (abs(err_com) > 10 and l1 + l2 < l1_cal + l2_cal and dir - 6 > 0) {

        v = 50
        u = 40

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
  if (dir < 3) {
    u=36*(dir1-5)
    v=100-30*abs(dir1-5)
  } else {
    v = (220-1.45*strres)

    if (v > 100) {
      v = 100
    }

    if (v < 40) {
      v = 40
    } 

    u_1 = 0*(dir-6)+1.1*(str5-str2)+0.3*(str4-str3)
    u = u_1 * v * 0.01
  } 
}

void kicker {
  if (time() - t_attack > 500 and time() - time_def > 1000) {
    mt.resetcount("A")
    while abs(mt.getcount("A")-270 < 0) {
      mt.spw("A", 100)
    }
    mt.stop("A", true)

    t_attack = time()
    time_def = time()
    mt.spw("A", -50)
  }
}
//Threads

new.thread = sensors

// Main
while (true) {

  if (strres > str_max - 5) {
    if (abs(err_com)>80) {
      tone(100,100,100)
      orbit()
    } else {
      while (strres > str_max - 30 and l1 + l2 < l1_cal + l2_cal) { //slow padik
        padik()
      }
      t_attack = time()
      while (l1 + l2 > l1_cal + l2_cal - 10 /*and abs(dir - 6) < 2*/) {
        tone(100,100,100)
        v = 100
        u = -err_com
        kicker()
      }
    }
  } else {
    while (strres < str_max - 5) {  //padik fast
      padik()
    }
  }
}