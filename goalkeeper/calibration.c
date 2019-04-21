// reading from file

handle = open.r("cal.txt");

values = new.vector(10, 0);

names[0] = "PORT_SEEKER";
names[1] = "PORT_COMPASS";
names[2] = "PORT_LIGHT";
names[3] = "PORT_BUTTON";
names[4] = "SEEKER_DELAY";
names[5] = "COMPASS_DELAY";
names[6] = "LIGHT_DELAY";
names[7] = "BUTTON_DELAY";
names[8] = "LIGHT_COLOR_NUM"; 
names[9] = "COMPASS_ALPHA";
names[10] = "SEEKER_DIV";

if (handle == 0) {
	PORT_SEEKER = 4;
	PORT_COMPASS = 2;
	PORT_LIGHT = 1;
	PORT_BUTTON = 3;
	SEEKER_DELAY = 50;
	COMPASS_DELAY = 50;
	LIGHT_DELAY = 50;
	BUTTON_DELAY = 50;
	LIGHT_COLOR_NUM = 1;
	COMPASS_ALPHA = 100;
	SEEKER_DIV = 100;
} else {
	PORT_SEEKER = tonum(readline(handle));
	values[0] = PORT_SEEKER;
	PORT_COMPASS = tonum(readline(handle));
	values[1] = PORT_COMPASS;
	PORT_LIGHT = tonum(readline(handle));
	values[2] = PORT_LIGHT;
	PORT_BUTTON = tonum(readline(handle));
	values[3] = PORT_BUTTON;
	SEEKER_DELAY = tonum(readline(handle));
	values[4] = SEEKER_DELAY;
	COMPASS_DELAY = tonum(readline(handle));
	values[5] = COMPASS_DELAY;
	LIGHT_DELAY = tonum(readline(handle));
	values[6] = LIGHT_DELAY;
	BUTTON_DELAY = tonum(readline(handle));
	values[7] = BUTTON_DELAY;
	LIGHT_COLOR_NUM = tonum(readline(handle));
	values[8] = LIGHT_COLOR_NUM;
	COMPASS_ALPHA = tonum(readline(handle));
	values[9] = COMPASS_ALPHA;
	SEEKER_DIV = tonum(readline(handle));
	values[10] = SEEKER_DIV;
}

closef(handle);

// sensors

sen.setmode(1, 1);
sen.setmode(3, 4);

com_kp = 1;
com_kd = 0.001;
com_ki = 0.0001;
err_com_old = 0;
com_i = 0;

alpha = com_mid;
name = getname();

void bottom { // values in the bottom of the screen
	dotline(1, 0, 105, 178, 105, 4, 4);
	txt(1, 89 - len(name)*4, 114, 1, name);
	txt(1, 5, 114, 0, round(voltagern*100)/100);
	txt.left(1, 173, 114, 0, round(ampersrn()*100)/100);
}

// interface

btn.exitdisallow(1);
i = 0;
button_counter = 0;
prev_button = "";
duration = 30; // duration (not in ms, it's sth else, idk lol)

while (true) {
	start: // instead of operator continue
	
	scr.clear();
	size_n = len(names[i])*8;
	size_v = len(values[i])*8;
	txt(1, 89 - floor(size_n/2), 30, 1, names[i]);
	txt(1, 89 - floor(size_v/2), 50, 1, values[i]);
	bottom();
	
	ip = i-1;
	if (ip == -1) {
		ip = 9;
	}
	txt(1, 0 - floor(len(names[ip])*4), 30, 0, names[ip]);
	txt(1, 0 - floor(len(values[ip])*4), 50, 0, values[ip]);
	in = i+1;
	if (in == 10) {
		in = 0;
	}
	txt(1, 170 - floor(len(names[in])*4), 30, 0, names[in]);
	txt(1, 170 - floor(len(values[in])*4), 50, 0, values[in]);
	t1 = time;
	btn.wait();
	if (time - t1 > 250) {
		button_counter = 0;
	}
	if (ispressed(6) == true) { // exit button
		goto end;
	}
	btn = btn.rn;
	
	if (btn == "L") { // left button action
		scr.clear();
		
		size_n2 = len(names[ip])*8;
		size_v2 = len(values[ip])*8;
		
		for (j = 0; j < duration; ++j) { // animation
			if (j < floor(duration/2)) {
				font = 1;
			} else {
				font = 0;
			}
			
			bottom();
			p1 = round(j*99/duration);
			p2 = round(j*99/duration);
			p3 = round(j*89/duration);
			p4 = round(j*89/duration);
			txt(1, 89 - floor(size_n/2) + p1, 30, font, names[i]);
			txt(1, 89 - floor(size_v/2) + p2, 50, font, values[i]);
			txt(1, 0 - floor(size_n2/2) + p3, 30, abs(font-1), names[ip]);
			txt(1, 0 - floor(size_v2/2) + p4, 50, abs(font-1), values[ip]);
			
			delay(1);
			if (rm(j, 2) == 0) {
				scr.clear();
			}
		}
		
		i = i - 1;
		if (i == -1) {
			i = 10;
		}
	}
	
	if (btn == "R") { // right button action
		scr.clear();
		
		size_n2 = len(names[in])*8;
		size_v2 = len(values[in])*8;
		
		for (j = 0; j < duration; ++j) { // animation
			if (j < floor(duration/2)) {
				font = 1;
			} else {
				font = 0;
			}
			
			bottom();
			p1 = round(j*99/duration);
			p2 = round(j*99/duration);
			p3 = round(j*89/duration);
			p4 = round(j*89/duration);
			txt(1, 89 - floor(size_n/2) - p1, 30, font, names[i]);
			txt(1, 89 - floor(size_v/2) - p2, 50, font, values[i]);
			txt(1, 178 - floor(size_n2/2) - p3, 30, abs(font-1), names[in]);
			txt(1, 178 - floor(size_v2/2) - p4, 50, abs(font-1), values[in]);
			
			delay(1);
			if (rm(j, 2) == 0) {
				scr.clear();
			}
		}
		
		i = i + 1;
		if (i == 11) {
			i = 0;
		}
	}
	
	if (btn == "U") { // up button action
		if (prev_button = "U") {
			button_counter = button_counter + 1;
		} else {
			button_counter = 0;
		}
		if (button_counter < 6) {
			values[i] = values[i] + 1;
		} else {
			values[i] = values[i] + 5;
		}
	}
	
	if (btn == "D") { // down button action
		if (i != ) {
			if (prev_button = "D") {
				button_counter = button_counter + 1;
			} else {
				button_counter = 0;
			}
			if (button_counter < 6) {
				values[i] = values[i] - 1;
			} else {
				values[i] = values[i] - 5;
			}
		} else {

		}
	}
	
	if (btn == "E") { // enter button action
		flush();
		scr.clear();
		txt.center(1, 61, 1, "You sure?");
		scr.rect(1, 14, 31, 149, 69);
		btn.wait();
		btn = btn.rn;
		flush();
		delay(100);
		if (btn != "E") {
			goto start;
		}
		
		if (i == 0) {
			if (values[i] <= 1) {
				values[i] = 1;
			} else {
				if (values >= 4) {
					values[i] = 4;
				}
			}
		}
		
		if (i == 1) {
			if (values[i] <= 1) {
				values[i] = 1;
			} else {
				if (values >= 4) {
					values[i] = 4;
				}
			}
		}
		
		if (i == 2) {
			if (values[i] <= 1) {
				values[i] = 1;
			} else {
				if (values >= 4) {
					values[i] = 4;
				}
			}
		}
		
		if (i == 3) {
			if (values[i] <= 1) {
				values[i] = 1;
			} else {
				if (values >= 4) {
					values[i] = 4;
				}
			}
		}
		
		if (i == 4) {
			if (values[i] <= 1) {
				values[i] = 1;
			}
		}
		
		if (i == 5) {
			if (values[i] <= 1) {
				values[i] = 1;
			}
		}
		
		if (i == 6) {
			if (values[i] <= 1) {
				values[i] = 1;
			}
		}
		
		if (i == 7) {
			if (values[i] <= 1) {
				values[i] = 1;
			}
		}
		
		if (i == 8) {
			if (values[i] <= 0) {
				values[i] = 0;
			} else {
				if (values[i] >= 2) {
					values[i] = 2;
				}
			}
		}
		
		if (i == 9) {
			compass_array = i2c.readregs(2, 1, 66, 4);
        	compass = compass_array[0] * 2 + compass_array[1];
			values[i] = compass;
		}

		if (i == 10) {
			if (values[i] <= 1) {
				values[i] = 1;
			}
		}
	}
	
	prev_button = btn;
	flush();
	delay(100);
}

end: // instead of operator break

scr.clear();
handle = open.w("cal.txt"); // writing to the file
for (i = 0; i < 11; ++i) {
	writeline(handle, values[i]);
}
closef(handle);