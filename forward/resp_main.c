/*TODO:
  Main: check new idea of definition side of orbit  
  # mb padik
  # starting strategy
  # odometry side_check  
*/

mt.spw("A", -50)
check.ports("ABC 1234")

//  BT
// connect("BOBA")
// check.connect("BOBA")

id = new.mailbox("attack_side")

mt.invert("ABC")
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
flag_bt = 1
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
    
    tone(100,100,100)
    t0 = time()
    //орбита
    //r2
    err90 = 999
    while (err90 > 8 and block == 0) {
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
        v=90
        d=145
        l=r+30
        u=-((1*(d*90)/(2*l))+(0.45*(r-strres)))
      } else {
        u=-((20*((3-dir1))-((3-dir1)/5*10))+(0.4*(r-strres)))
        v=80
      }


      if (time() - t0 > 4500) {
        t0 = time()
        Goto exit2
      } else {
        if (time() - t0 > 1500) {
          if (strres < 70) {
            play(100, "Kung Fu")
            Goto exit2
          }
        }
      }
    }
    
    t0 = time()

    //поворот на мяч
    //r3
    tone(100,100,100)
    if (err90 < 21) {
      while (abs(err_com) > 20 and l1 + l2 < l1_cal + l2_cal and block == 0) {
      
        v = 40
        u = -38

        if (dir < 7) {
          if (abs(err_com) < 69) { 
            Goto exit2
          }
        }

        if (time() - t0 > 1000) {
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

    tone(100,100,100)
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
        v=90
        d=145
        l=r+30
        u=-((-1*(d*90)/(2*l))+(0.4*(strres - r)))
      } else {
        u=-((20*((7-dir2))-((7-dir2)/5*10))+(0.45*(strres - r)))
        dir3 = dir2
        v=80
      }

      if (time() - t0 > 4500) {
        t0 = time()
        Goto exit2
      } else {
        if (time() - t0 > 1500) {
          if (strres < 70) {
            play(100, "Kung Fu")
            Goto exit2
          }
        }
      }
    }

    t0 = time()
    //поворот на мяч
    //l3
    tone(100,100,100)
    if (err90 > -22) {
      while (abs(err_com) > 20 and l1 + l2 < l1_cal + l2_cal) {

        v = 40
        u = 38

        if (dir > 5) {
          if (abs(err_com) < 69) {
            Goto exit2
          }
        }

        if (time() - t0 > 1000) {
          Goto exit2
        }
      }
    }
  }
  exit2:
}



void padik {
  if (dir < 3) {
    u=28*(dir1-5)
    v=100-30*abs(dir1-5)
  } else {
  
    v = (225 - 1.25*strres)

    if (v > 100) {
      v = 100
    }

    if (v < 70) {
      v = 70
    }

    u_1=kf_dir*(dir-6)+0.065*(2*(str5-str2)+0.9*(str4-str3))//20*(dir-6)+1*(str5-str2)+0.04*(str4-str3)
    u = u_1 * v * 0.01
  } 
}

void kicker {
  if abs(mt.getcount("A") > 250) {
    mt.spw("A", -100)
  }

  if (time() - t_attack > 700 and time() - time_def > 800) {
    mt.resetcount("A")
    while abs(mt.getcount("A")-270 < 0) {
      mt.spw("A", 100)
    }
    mt.stop("A", true)

    time_def = time()
    mt.spw("A", -50)
    fl_attack = 1
  }
}

void BT {
  if (flag_bt == 1) {
    if (bt.available(id) == true) {
      side = tonum(bt.receive(id))
    } else {
      side = 3
    }

    if (side == 1) {
      alpha_att = com_1
    } else {
      if (side == 2) {
        alpha_att = com_2
      } else {
        if (side == 3) {
          alpha_att = com_3
        } else {
          if (side == 4) {
            alpha_att = com_4
          } else {
            if (side == 5) {
              alpha_att = com_5
            }
          }
        } 
      }
    }
  } else {
    alpha_att = com_3
  }
}

void attack {

  BT()

  t_attack = time()
  v = 40
  k = 0.2
  while ((l1 > l1_cal - 5 or l2 > l2_cal - 5) and (abs(dir - 6) < 2 or dir == 8)) {
    
    err_attack = rm(compass - alpha_att + 900, 360) - 180

    led(5)
    //tone(100,100,100)
    if (v < 100) {
      v = v + 0.3
    }
    u = -err_attack * k

    if (k < 0.8) {
      k = k + 0.0007
    }

    if (abs(err_attack) < 10 and v > 80) { //attack_angle error
      kicker()
    }
  }
}

void fast_return {
  time_def = time()
  while (abs(dir1 - 5) > 1 and time() - time_def < 3000) {
    v = -100
    u = -err_com*0.8
  }

  time_def = time()
  while (dir != 6 and time() - time_def < 1000) {
    u=-(30*(6-dir)+0.15*(str3-str4))
    v = 10
  }
}

void stop_with_tone {
  v = 0
  u = 0  
  tone(100,100,100)
  mt.stop("BC", "True")
  delay(1000)
  flush()
  btn.wait()
}

void Gastal {
  while (true) {
    com_state = compass
    str_state = strres
    enc1_state = abs(e1)
    enc2_state = abs(e2)
    t_block = time()

    while (time() - t_block < 1000) {

    }

    if ((abs(abs(e1)-abs(enc1_state)) < 10 and abs(abs(e2)-abs(enc2_state)) < 10) and v > 20 and flag1 == 0) {
      block = 1
      flag_od = 0
    } else {
      if (abs(dir - 6) < 2) {
        if (abs(compass - com_state) < 2 and abs(strres - str_state) < 3 and flag1 == 0) {
          block = 1
          flag_od = 0
        } else {
          block = 0
        }
      } else {     
        if (abs(compass - com_state) < 2 and flag1 == 0) {
          block = 1
          flag_od = 0
        } else {
          block = 0
        }
      }
      printupd()
      print("Block", block)
    }
  }
}

//Threads
new.thread = sensors
new.thread = Gastal

//tactics

btn.wait()
if (btn.rn == "E") {
  tone(100,200,100)
} else {
  if (btn.rn == "U") {
    while (abs(e1 + e2) < 200) {
      v = 100
      u = -(rm(compass - com_2 + 900, 360) - 180)
    }

    kicker()

    while ((e1+e2)>0) {
      v = -100
      u = -(rm(compass - com_1 + 900, 360) - 180)
    }

    er1 = 999
    while (abs(er1) > 5) {
      v = 10
      er1 = rm(compass - com_5 + 900, 360) - 180
      u = -er1
    }

    while (strres < 120) {
      v = 0
      u = 0
      mt.stop("BC", true)
    }
  }
}

flush()

// Main
while (true) {
  if (block == 1) {
    led(1)
    t_exit = time()
    k = 10
    while (dir != 6 and time() - t_exit < 500) {
      v = -100
      u = (dir1-5) * k
      k = k + 0.01
    }
    block = 0
    
  } else {
    led(2)
    if (fl_attack = 1 and abs(dir1 - 5) > 1) {
      fast_return()
      tone(100,1000,100)
      fl_attack = 0
    }

    if (strres > str_max) {
      if (abs(err_com)>80) {
        tone(100,100,100)
        orbit()

      } else {
        while (strres > str_max and (l1 < l1_cal or l2 < l2_cal) and block == 0 and abs(err_com)<70) { //slow padik
          kf_dir = 14
          padik()
        }
        attack()
      }
    } else {
      while (strres < str_max and block == 0) {  //padik fast
        kf_dir = 25
        padik()
      }
    }
  }
}