// reading from file

handle = open.r("cal.txt");

values_len = 14;
values = new.vector(values_len, 0);

names[0] = "p_seek";
names[1] = "p_com";
names[2] = "p_lt_f";
names[3] = "p_lt_b";
names[4] = "seek_del";
names[5] = "com_del";
names[6] = "lt_del";
names[7] = "alpha";
names[8] = "seek_div";
names[9] = "p_mt_1";
names[10] = "p_mt_2";
names[11] = "p_mt_3";
names[12] = "p_kick";
names[13] = "kp_com";

if (handle == 0) {
	PORT_SEEKER = 4;
	values[0] = PORT_SEEKER;
	PORT_COMPASS = 2;
	values[1] = PORT_COMPASS;
	PORT_LIGHT_FRONT = 1;
	values[2] = PORT_LIGHT_FRONT;
	PORT_LIGHT_BACK = 3;
	values[3] = PORT_LIGHT_BACK;
	SEEKER_DELAY = 50;
	values[4] = SEEKER_DELAY;
	COMPASS_DELAY = 50;
	values[5] = COMPASS_DELAY;
	LIGHT_DELAY = 50;
	values[6] = LIGHT_DELAY;
	COMPASS_ALPHA = 0;
	values[7] = COMPASS_ALPHA;
	SEEKER_DIV = 100;
	values[8] = SEEKER_DIV;
    PORT_FIRST_MOTOR = 1;
    values[9] = PORT_FIRST_MOTOR;
    PORT_SECOND_MOTOR = 1;
    values[10] = PORT_SECOND_MOTOR;
    PORT_THIRD_MOTOR = 1;
    values[11] = PORT_THIRD_MOTOR;
	PORT_KICKER = 1;
	values[12] = PORT_KICKER;
	KP_COM = 75;
	values[13] = KP_COM;
} else {
	PORT_SEEKER = tonum(readline(handle));
	values[0] = PORT_SEEKER;
	PORT_COMPASS = tonum(readline(handle));
	values[1] = PORT_COMPASS;
	PORT_LIGHT_FRONT = tonum(readline(handle));
	values[2] = PORT_LIGHT_FRONT;
	PORT_LIGHT_BACK = tonum(readline(handle));
	values[3] = PORT_LIGHT_BACK;
	SEEKER_DELAY = tonum(readline(handle));
	values[4] = SEEKER_DELAY;
	COMPASS_DELAY = tonum(readline(handle));
	values[5] = COMPASS_DELAY;
	LIGHT_DELAY = tonum(readline(handle));
	values[6] = LIGHT_DELAY;
	COMPASS_ALPHA = tonum(readline(handle));
	values[7] = COMPASS_ALPHA;
	SEEKER_DIV = tonum(readline(handle));
	values[8] = SEEKER_DIV;
    PORT_FIRST_MOTOR = tonum(readline(handle));
    values[9] = PORT_FIRST_MOTOR;
    PORT_SECOND_MOTOR = tonum(readline(handle));
    values[10] = PORT_SECOND_MOTOR;
    PORT_THIRD_MOTOR = tonum(readline(handle));
    values[11] = PORT_THIRD_MOTOR;
	PORT_KICKER = tonum(readline(handle));
	values[12] = PORT_KICKER;
	KP_COM = tonum(readline(handle));
	values[13] = KP_COM;
}

closef(handle);

// sensors

sen.setmode(1, 1);
sen.setmode(3, 4);

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
		ip = values_len-1;
	}
	txt(1, 0 - floor(len(names[ip])*4), 30, 0, names[ip]);
	txt(1, 0 - floor(len(values[ip])*4), 50, 0, values[ip]);
	in = i+1;
	if (in == values_len) {
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
			i = values_len-1;
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
		if (i == values_len) {
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
				if (values[i] >= 4) {
					values[i] = 4;
				}
			}
		}
		
		if (i == 1) {
			if (values[i] <= 1) {
				values[i] = 1;
			} else {
				if (values[i] >= 4) {
					values[i] = 4;
				}
			}
		}
		
		if (i == 2) {
			if (values[i] <= 1) {
				values[i] = 1;
			} else {
				if (values[i] >= 4) {
					values[i] = 4;
				}
			}
		}
		
		if (i == 3) {
			if (values[i] <= 1) {
				values[i] = 1;
			} else {
				if (values[i] >= 4) {
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
			compass_array = i2c.readregs(2, 1, 66, 4);
        	compass = compass_array[0] * 2 + compass_array[1];
			values[i] = compass;
		}

		if (i == 8) {
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
for (i = 0; i < values_len; ++i) {
	writeline(handle, values[i]);
}
closef(handle);