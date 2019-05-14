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
fl_attack = 0
kf_dir = 27
block = 0

//flags 
flag1 = 0 //атака

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

    l1 = sen.percent(1)
    l2 = sen.percent(3)

    mt.spw("B", v + u)
    mt.spw("C", v - u)

    e1 = mt.getcount("B")
    e2 = mt.getcount("C")

    dx = (e1 - b + e2 - c)*0.0453*sin(err_com*pi/180)
    dy = (e1 - b + e2 - c)*0.0453*cos(err_com*pi/180)
    x = x - dx
    y = y - dy
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

  err_real = (rm(abs(err_com+(dir-6)*30) + 900, 360)-180)*(err_com+(dir-6)*30)/abs(err_com+(dir-6)*30)
  scr.clear()
  txt(1,10,10,2,err_real)
  btn.wait()
  
  scr.clear()
  if (err_real < 0) {
    txt(1,10,10,2,"L")
  } else {
    txt(1,10,10,2,"R")
  }
  fl_attack = 0
  // // поворот на мяч перед орбитой
  // t0 = time()
  // while (dir != 6 and time() - t0 < 1000 and block == 0) {
  //   u=-(30*(6-dir)+0.08*(str3-str4))
  //   v = 10

  //   if (strres < 90) {
  //     Goto exit2
  //   }
  // }
 
/*err_com-(dir-6)*45 > 0*/
  // определение стороны орбиты
  if (err_real > 0) {  //err_com > 0
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

      if (dir1 == 3) {
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
          if (strres < 90) {
            Goto exit2
          }
        }
      }
    }
    
    t0 = time()

    //поворот на мяч
    //r3
  
    if (err90 < 21) {
      while (abs(err_com) > 20 and l1 + l2 < l1_cal + l2_cal and dir - 6 < 0) {
      
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
          if (strres < 90) {
            Goto exit2
          }
        }
      }
    }

    t0 = time()
    //поворот на мяч
    //l3
    if (err90 > -22) {
      while (abs(err_com) > 20 and l1 + l2 < l1_cal + l2_cal and dir - 6 > 0) {

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

new.thread = sensors

delay(1000)
orbit()
v = 0
u = 0
mt.stop("BC", true)
btn.wait()