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

alpha = com_3
x = 0
y = 0
b = 0
c = 0
mt.resetcount("ABC")
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

new.thread = sensors

while (true) {
    
    if (x < -15) {
      if (y < 0) {
        zone = 1
      } else {
        zone = 2
      }
    } else {
      if (x > -15 and x < 15) {
        zone = 3
      } else {
        if (y < 0) {
          zone = 5
        } else {
          zone = 4
        }
      }
    }

    printupd()
    print("X", x)
    print("Y", y)
    print("zone", zone)
}