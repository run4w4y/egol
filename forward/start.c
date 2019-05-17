mt.spw("A", -50)
check.ports("ABC 1234")
mt.invert("ABC")
mt.stop("ABC", "false")

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
    y = y + dy
    b = e1
    c = e2
  }
}

void kicker {
  if abs(mt.getcount("A") > 250) {
    mt.spw("A", -100)
  }

  if (time() - t_attack > 700 and time() - time_def > 1000) {
    mt.resetcount("A")
    while abs(mt.getcount("A")-270 < 0) {
      mt.spw("A", 100)
    }
    mt.stop("A", true)

    t_attack = time()
    time_def = time()
    mt.spw("A", -50)
    fl_attack = 1
  }
}

void padik {
  if (dir < 3) {
    u=28*(dir1-5)
    v=100-30*abs(dir1-5)
  } else {
  
    v=100

    u_1=kf_dir*(dir-6)+0.065*(2*(str5-str2)+0.8*(str4-str3))//20*(dir-6)+1*(str5-str2)+0.04*(str4-str3)
    u = u_1 * v * 0.01
  } 
}

new.thread = sensors

delay(1000)

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

delay(1000)
btn.wait()

