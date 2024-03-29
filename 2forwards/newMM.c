check.ports("ABCD 234")

//  BT

who_am_i = getname();

if (who_am_i == "BOBA") {
	who_aint_me = "BIBA";
} else {
	who_aint_me = "BOBA";
}

connect(who_aint_me);

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
attack = 0


mode = 1;
attack = 0;
//flags 

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

    l_b = sen.percent(3)
    }
}

void bt {
	while (true) {
		if (who_am_i == "BIBA") {
			mybtstr = attack + "";
		} else {
			mybtstr = strres + " " + dir + " " + attack;
		}
		
		if (who_am_i == "BIBA") {
			bt.send(who_aint_me, "mailbox_biba", mybtstr);

			btstr2 = bt.last(mailbox_boba);
			res = parse(4, btstr2);

			str_res2 = res[0];
			dir2 = res[1];
			attack2 = res[2];
		} else {
			bt.send(who_aint_me, "mailbox_boba", mybtstr);

			btstr2 = bt.last(mailbox_biba);
			res = parse(2, btstr2);

			attack2 = res[0];

			mode = tonum(bt.last(mailbox_mode));
		}
		
		//////////////////////////////////////////////////////////////////////
		
		if (who_am_i == "BIBA") {
            if (abs(strres - str_res2) < 30) {
                if (abs(dir - 5) == abs(dir2 - 5)) {
                    if (strres > str_res2) {
                        mode = 1;
                    } else {
                        mode = 0;
                    }
                } else {
                    if (abs(dir - 5) > abs(dir2 - 5)) {
                        mode = 0;
                    } else {
                        mode = 1;
                    }
                }
            } else {
                if (strres > str_res2) {
                    mode = 1;
                } else {
                    mode = 0;
                }
            }
			bt.send(who_aint_me, "mailbox_mode", abs(mode-1));
		}

		// buttons will be here
	}
}


new.thread = sensors
new.thread = bt
t = time()

while (true) {
  while (mode == 1) {
    led(2)
    if (abs(err_com) < 25 and abs(dir - 5) > 3) {
      pw = 50
      b_power = 0
      while (abs(err_com) < 25 and abs(dir - 5) > 3) {
        if (pw < 100) {
          pw = pw + 3
        }
        // if (time() - t > 1700 and time() - t < 1900) {
        //   mt.spw("a", -100)
        // } else {
        //   mt.stop("a", true)
        // }
        b_power = 1 * err_com + err_com*pw*0.01;
        if (abs(err_com) < 5) {
          b_power = 0;
        } else {
          if (abs(b_power) < 10) {
            if (b_power > 0) {
              b_power = 10;
            } else {
              b_power = -10;
            }
          }
        }

        u = err_com * 0.008
        mt.spw("C", -pw+u)
        mt.spw("D", -pw-u)
        mt.spw("B", b_power)
        delay(5)
        fl1 = 1

        if (l_b > BACK_LIGHT_VALUE) {
          t_avoid = time()
          while (time() - t_avoid < 100) {
            mt.spw("C", 50)
            mt.spw("D", -33 + err_com)
            mt.spw("B", 100)
          }
        }
      }
    }

    // //  slowing
    // if (fl1 == 1) { 
    //   while (pw > 33) {
    //     if (pw > 33) {
    //       pw = pw - 3
    //     }
    //     // if (time() - t > 1700 and time() - t < 1900) {
    //     //   mt.spw("a", -100)
    //     // } else {
    //     //   mt.stop("a", true)
    //     // }
    //     b_power = 1 * err_com + err_com*pw*0.01;
    //     if (abs(err_com) < 5) {
    //       b_power = 0;
    //     } else {
    //       if (abs(b_power) < 10) {
    //         if (b_power > 0) {
    //           b_power = 10;
    //         } else {
    //           b_power = -10;
    //         }
    //       }
    //     }

    //     u = err_com * 0.008
    //     mt.spw("C", -pw)
    //     mt.spw("D", -pw)
    //     mt.spw("B", b_power)
    //     delay(5)
    //   }

    //   fl1 = 0
    // }

    while (dir != 5 and abs(dir - 5) < 3) {
      if (dir > 5) {
        if (str4 > STR_VALUE_4) { 
          mt.spw("C", 41)
          mt.spw("D", -27 + err_com)
          mt.spw("B", 83)
        } else {
          mt.spw("C", 50)
          mt.spw("D", -33 + err_com)
          mt.spw("B", 100)
        }
      } else {
        if (str2 > STR_VALUE_2) {
          mt.spw("C", -27 - err_com)
          mt.spw("D", 41)
          mt.spw("B", -83)
        } else {
          mt.spw("C", -33 - err_com)
          mt.spw("D", 50)
          mt.spw("B", -100)
        }
      }
    }

    while (dir == 5 and attack == 0) {
      if (pw < 100) {
        pw = pw + 2
      }
      // if (time() - t > 1700 and time() - t < 1900) {
      //   mt.spw("a", -100)
      // } else {
      //   mt.stop("a", true)
      // }
      b_power = 1 * err_com + err_com*pw*0.01;
      if (abs(err_com) < 5) {
        b_power = 0;
      } else {
        if (abs(b_power) < 10) {
          if (b_power > 0) {
            b_power = 10;
          } else {
            b_power = -10;
          }
        }
      }

      u = err_com * 0.008
      mt.spw("C", pw-u)
      mt.spw("D", pw+u)
      mt.spw("B", b_power)
      delay(5)

      if (str3 > STR_MAX - 5) {
        attack = 1
      }
    }

    if (attack == 1) {
      pw = 50
      b_power = 0
      t = time()
      while (strres > 130) {
        tone(100,100,100)
        if (pw < 100) {
          pw = pw + 2
        }

        if (time() - t > 700 and time() - t < 900) {
          mt.spw("a", -100)
        } else {
          mt.shd.pw("a", 50, 0, 300, 0, "false")
        }

        

        b_power = 1 * err_com + err_com*pw*0.01;
        if (abs(err_com) < 5) {
          b_power = 0;
        } else {
          if (abs(b_power) < 10) {
            if (b_power > 0) {
              b_power = 10;
            } else {
              b_power = -10;
            }
          }
        }

        u = err_com * 0.008
        mt.spw("C", pw-u)
        mt.spw("D", pw+u)
        mt.spw("B", b_power)
        delay(5)
      }
      attack = 0
    }
  }

  while (mode == 0) {
    led(1)
    if (abs(dir - 5) < 3) {
      mt.spw("C", -err_com * 0.67)
      mt.spw("D", err_com * 0.67)
      mt.spw("B", err_com * 1)
    } else {
      if (abs(err_com) < 25 and abs(dir - 5) > 3) {
        pw = 50
        b_power = 0
        while (abs(err_com) < 25 and abs(dir - 5) > 3) {
          if (pw < 100) {
            pw = pw + 3
          }
          // if (time() - t > 1700 and time() - t < 1900) {
          //   mt.spw("a", -100)
          // } else {
          //   mt.stop("a", true)
          // }
          b_power = 1 * err_com + err_com*pw*0.01;
          if (abs(err_com) < 5) {
            b_power = 0;
          } else {
            if (abs(b_power) < 10) {
              if (b_power > 0) {
                b_power = 10;
              } else {
                b_power = -10;
              }
            }
          }

          u = err_com * 0.008
          mt.spw("C", -pw+u)
          mt.spw("D", -pw-u)
          mt.spw("B", b_power)
          delay(5)
          fl2 = 1

          if (l_b > BACK_LIGHT_VALUE) {
            t_avoid = time()
            while (time() - t_avoid < 100) {
              mt.spw("C", 50)
              mt.spw("D", -33 + err_com)
              mt.spw("B", 100)
            }
          }
        }
      }

      // //  slowing
      // if (fl2 == 1) { 
      //   while (pw > 33) {
      //     if (pw > 33) {
      //       pw = pw - 3
      //     }
      //     // if (time() - t > 1700 and time() - t < 1900) {
      //     //   mt.spw("a", -100)
      //     // } else {
      //     //   mt.stop("a", true)
      //     // }
      //     b_power = 1 * err_com + err_com*pw*0.01;
      //     if (abs(err_com) < 5) {
      //       b_power = 0;
      //     } else {
      //       if (abs(b_power) < 10) {
      //         if (b_power > 0) {
      //           b_power = 10;
      //         } else {
      //           b_power = -10;
      //         }
      //       }
      //     }

      //     u = err_com * 0.008
      //     mt.spw("C", -pw)
      //     mt.spw("D", -pw)
      //     mt.spw("B", b_power)
      //     delay(5)
      //   }

      //   fl2 = 0
      // }
    }
  }
}