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

void padik {
  if (dir < 3) {
    u=30*(dir1-5)
    v=100-30*abs(dir1-5)
  } else {
    v = (250 - 1.5*strres)
    if (v > 100) {
      v = 100
    }

    if (v < 40) {
      v = 40
    }

    u_1 = 20*(dir-6)+1.1*(str5-str2)+0.015*(str4-str3)
    u = u_1 * v * 0.01
    } 

    mt.spw("B", v + u)
    mt.spw("C", v - u)
}

new.thread = sensors

while (strres < str_max) {
    padik()
}

tone(100,100,100)
mt.stop("BC", true)
delay(1000)