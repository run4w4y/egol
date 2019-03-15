// reading from file

handle = open.r("cal.txt")

values = new.vector(10, 0)

names[0] = "str_max"
names[1] = "com_1"
names[2] = "com_2"
names[3] = "com_3"
names[4] = "com_4"
names[5] = "com_5"
names[6] = "light1"
names[7] = "light2"


if (handle == 0) {
	str_max = 0
	com_1 = 0
	com_2 = 0
	com_3 = 0
	com_4 = 0
	com_5 = 0
	light1 = 0
	light2 = 0
} else {
	str_max = tonum(readline(handle))
	values[0] = str_max
	com_1 = tonum(readline(handle))
	values[1] = com_1
	com_2 = tonum(readline(handle))
	values[2] = com_2
	com_3 = tonum(readline(handle))
	values[3] = com_3
	com_4 = tonum(readline(handle))
	values[4] = com_4
	com_5 = tonum(readline(handle))
	values[5] = com_5
	light1 = tonum(readline(handle))
	values[6] = light1
	light2 = tonum(readline(handle))
	values[7] = light2

}

closef(handle)

// sensors

forward = 0
turn = 0
alpha = 0
str_err_old = 0
err_dir_old = 0
alpha = 0

name = getname()

void sensors {
	while (true) {
		irseeker_array = i2c.readregs(4, 8, 73, 6)
		dir = irseeker_array[0]
		str1 = irseeker_array[1]
		str2 = irseeker_array[2]
		str3 = irseeker_array[3]
		str4 = irseeker_array[4]
		str5 = irseeker_array[5]

		if (rm(dir,2) == 0) {
			strres = (str1 + str2 + str3 + str4 + str5)/1.8
		} else {
			strres = str1 + str2 + str3 + str4 + str5
		}

		compass_array = i2c.readregs(2, 1, 66, 4)
		compass = compass_array[0] * 2 + compass_array[1]
		err_com = Math.Remainder(compass - alpha + 900, 360) - 180

		l_1 = sen.percent(1)
		l_2 = sen.percent(3)

	}
}

void bottom { // values in the bottom of the screen
	dotline(1, 0, 105, 178, 105, 4, 4)
	txt(1, 89 - len(name)*4, 114, 1, name)
	txt(1, 5, 114, 0, round(voltagern*100)/100)
	txt.left(1, 173, 114, 0, round(ampersrn()*100)/100)
}

new.thread = sensors

// interface

btn.exitdisallow(1)
i = 0
button_counter = 0
prev_button = ""
duration = 30 // duration (not in ms, it's sth else, idk lol)

while (true) {
	start: // instead of operator continue
	
	scr.clear()
	size_n = len(names[i])*8
	size_v = len(values[i])*8
	txt(1, 89 - floor(size_n/2), 30, 1, names[i])
	txt(1, 89 - floor(size_v/2), 50, 1, values[i])
	bottom()
	
	ip = i-1
	if (ip == -1) {
		ip = 7
	}
	txt(1, 0 - floor(len(names[ip])*4), 30, 0, names[ip])
	txt(1, 0 - floor(len(values[ip])*4), 50, 0, values[ip])
	in = i+1
	if (in == 8) {
		in = 0
	}
	txt(1, 170 - floor(len(names[in])*4), 30, 0, names[in])
	txt(1, 170 - floor(len(values[in])*4), 50, 0, values[in])
	t1 = time
	btn.wait()
	if (time - t1 > 250) {
		button_counter = 0
	}
	if (ispressed(6) == true) { // exit button
		goto end
	}
	btn = btn.rn
	
	if (btn == "L") { // left button action
		scr.clear()
		
		size_n2 = len(names[ip])*8
		size_v2 = len(values[ip])*8
		
		for (j = 0; j < duration; ++j) { // animation
			if (j < floor(duration/2)) {
				font = 1
			} else {
				font = 0
			}
			
			bottom()
			p1 = round(j*99/duration)
			p2 = round(j*99/duration)
			p3 = round(j*89/duration)
			p4 = round(j*89/duration)
			txt(1, 89 - floor(size_n/2) + p1, 30, font, names[i])
			txt(1, 89 - floor(size_v/2) + p2, 50, font, values[i])
			txt(1, 0 - floor(size_n2/2) + p3, 30, abs(font-1), names[ip])
			txt(1, 0 - floor(size_v2/2) + p4, 50, abs(font-1), values[ip])
			
			delay(1)
			if (rm(j, 2) == 0) {
				scr.clear()
			}
		}
		
		i = i - 1
		if (i == -1) {
			i = 7
		}
	}
	
	if (btn == "R") { // right button action
		scr.clear()
		
		size_n2 = len(names[in])*8
		size_v2 = len(values[in])*8
		
		for (j = 0; j < duration; ++j) { // animation
			if (j < floor(duration/2)) {
				font = 1
			} else {
				font = 0
			}
			
			bottom()
			p1 = round(j*99/duration)
			p2 = round(j*99/duration)
			p3 = round(j*89/duration)
			p4 = round(j*89/duration)
			txt(1, 89 - floor(size_n/2) - p1, 30, font, names[i])
			txt(1, 89 - floor(size_v/2) - p2, 50, font, values[i])
			txt(1, 178 - floor(size_n2/2) - p3, 30, abs(font-1), names[in])
			txt(1, 178 - floor(size_v2/2) - p4, 50, abs(font-1), values[in])
			
			delay(1)
			if (rm(j, 2) == 0) {
				scr.clear()
			}
		}
		
		i = i + 1
		if (i == 8) {
			i = 0
		}
	}
	
	if (btn == "U") { // up button action
		if (prev_button = "U") {
			button_counter = button_counter + 1
		} else {
			button_counter = 0
		}
		if (button_counter < 6) {
			values[i] = values[i] + 1
		} else {
			values[i] = values[i] + 5
		}
	}
	
	if (btn == "D") { // down button action
		if (prev_button = "D") {
			button_counter = button_counter + 1
		} else {
			button_counter = 0
		}
		if (button_counter < 6) {
			values[i] = values[i] - 1
		} else {
			values[i] = values[i] - 5
		}
	}
	
	if (btn == "E") { // enter button action
		flush()
		scr.clear()
		txt.center(1, 61, 1, "You sure?")
		scr.rect(1, 14, 31, 149, 69)
		btn.wait()
		btn = btn.rn
		flush()
		delay(100)
		if (btn != "E") {
			goto start
		}
		
		if (i == 0) {
			str_max = 0
			for (j = 0; j < 2000; ++j) { // progress bar
				
				if (ispressed(6) == true) { // exit button
					goto end
				}

				if (strres > str_max) {
					str_max = strres
				}

				p = floor(j/20)
				scr.clear()
				scr.rect(1, 37, 48, 105, 10)
				line(1, 39, 50, 39+p, 50)
				line(1, 39, 51, 39+p, 51)
				line(1, 39, 52, 39+p, 52)
				line(1, 39, 53, 39+p, 53)
				line(1, 39, 54, 39+p, 54)
				line(1, 39, 55, 39+p, 55)
				txt(1, 89-len(p)*4, 30, 1, p+"%")
				delay(5)
			}
			values[i] = str_max
		}
		
		if (i == 1) {
			values[i] = compass
		}
		
		if (i == 2) {
			values[i] = compass
		}

		if (i == 3) {
			values[i] = compass
		}

		if (i == 4) {
			values[i] = compass
		}

		if (i == 5) {
			values[i] = compass
		}
		
		if (i == 6) {
			light1_calib = 0
			light2_calib = 0
			for (j = 0; j < 1000; ++j) { // progress bar
				if (ispressed(6) == true) { // exit button
					goto end
				}

				light1_calib = light1_calib + l_1
				light2_calib = light1_calib + l_2

				p = floor(j/10)
				scr.clear()
				scr.rect(1, 37, 48, 105, 10)
				line(1, 39, 50, 39+p, 50)
				line(1, 39, 51, 39+p, 51)
				line(1, 39, 52, 39+p, 52)
				line(1, 39, 53, 39+p, 53)
				line(1, 39, 54, 39+p, 54)
				line(1, 39, 55, 39+p, 55)
				txt(1, 89-len(p)*4, 30, 1, p+"%")
				delay(5)
			}
			light1_calib = light1_calib / 1000
			light2_calib = light2_calib / 1000

			values[i] = light1_calib
			values[i+1] = light2_calib
		}
		
	}
	
	prev_button = btn
	flush()
	delay(100)
}

end: // instead of operator break

scr.clear()
handle = open.w("cal.txt") // writing to the file
for (i = 0; i < 8; ++i) {
	writeline(handle, values[i])
}
closef(handle)