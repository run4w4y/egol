// reading from file

handle = open.r("cal.txt");

values_len = 19;
values = new.vector(values_len, 0);

names[0] = "p_seek";
names[1] = "p_com";
names[2] = "p_lt";
names[3] = "p_btn";
names[4] = "seek_del";
names[5] = "com_del";
names[6] = "lt_del";
names[7] = "btn_del";
names[8] = "color_n"; 
names[9] = "alpha";
names[10] = "seek_div";
names[11] = "p_mmid";
names[12] = "p_mleft";
names[13] = "p_mright";
names[14] = "str_lim";
names[15] = "str_inf";
names[16] = "line_lim";
names[17] = "left_ang";
names[18] = "right_ang";

if (handle == 0) {
	PORT_SEEKER = 4;
	values[0] = PORT_SEEKER;
	PORT_COMPASS = 2;
	values[1] = PORT_COMPASS;
	PORT_LIGHT = 1;
	values[2] = PORT_LIGHT;
	PORT_BUTTON = 3;
	values[3] = PORT_BUTTON;
	SEEKER_DELAY = 50;
	values[4] = SEEKER_DELAY;
	COMPASS_DELAY = 50;
	values[5] = COMPASS_DELAY;
	LIGHT_DELAY = 50;
	values[6] = LIGHT_DELAY;
	BUTTON_DELAY = 50;
	values[7] = BUTTON_DELAY;
	LIGHT_COLOR_NUM = 1;
	values[8] = LIGHT_COLOR_NUM;
	COMPASS_ALPHA = 100;
	values[9] = COMPASS_ALPHA;
	SEEKER_DIV = 100;
	values[10] = SEEKER_DIV;
	PORT_MID_MOTOR = 1;
	values[11] = PORT_MID_MOTOR;
	PORT_LEFT_MOTOR = 2;
	values[12] = PORT_LEFT_MOTOR;
	PORT_RIGHT_MOTOR = 2;
	values[13] = PORT_RIGHT_MOTOR;
	STR_LIMIT = 130;
	values[14] = STR_LIMIT;
	SEEKER_STR_MAX = 195;
	values[15] = SEEKER_STR_MAX;
	LIGHT_LINE_LIMIT = 10;
	values[16] = LIGHT_LINE_LIMIT;
	COMPASS_LEFT_ANGLE = 0;
	values[17] = COMPASS_LEFT_ANGLE;
	COMPASS_RIGHT_ANGLE = 0;
	values[18] = COMPASS_RIGHT_ANGLE;
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
	PORT_MID_MOTOR = tonum(readline(handle));
	values[11] = PORT_MID_MOTOR;
	PORT_LEFT_MOTOR = tonum(readline(handle));
	values[12] = PORT_LEFT_MOTOR;
	PORT_RIGHT_MOTOR = tonum(readline(handle));
	values[13] = PORT_RIGHT_MOTOR;
	STR_LIMIT = tonum(readline(handle));
	values[14] = STR_LIMIT;
	SEEKER_STR_MAX = tonum(readline(handle));
	values[15] = SEEKER_STR_MAX;
	LIGHT_LINE_LIMIT = tonum(readline(handle));
	values[16] = LIGHT_LINE_LIMIT;
	COMPASS_LEFT_ANGLE = tonum(readline(handle));
	values[17] = COMPASS_LEFT_ANGLE;
	COMPASS_RIGHT_ANGLE = tonum(readline(handle));
	values[18] = COMPASS_RIGHT_ANGLE;
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

		if (i == 11) {
			if (values[i] <= 1) {
				values[i] = 1;
			} else {
				if (values[i] >= 3) {
					values[i] = 3;
				}
			}
		}

		if (i == 12) {
			if (values[i] <= 1) {
				values[i] = 1;
			} else {
				if (values[i] >= 3) {
					values[i] = 3;
				}
			}
		}

		if (i == 13) {
			if (values[i] <= 1) {
				values[i] = 1;
			} else {
				if (values[i] >= 3) {
					values[i] = 3;
				}
			}
		}

		if (i == 14) {
			irseeker_array = i2c.readregs(4, 8, 73, 6);
        
			dir = irseeker_array[0];
			str1 = irseeker_array[1];
			str2 = irseeker_array[2];
			str3 = irseeker_array[3];
			str4 = irseeker_array[4];
			str5 = irseeker_array[5];

			if (rm(dir,2) == 0) {
				strres = (str1 + str2 + str3 + str4 + str5) / SEEKER_DIV;
			} else {
				strres = str1 + str2 + str3 + str4 + str5;
			}

			values[i] = strres;
		}

		if (i == 15) {
			irseeker_array = i2c.readregs(4, 8, 73, 6);
        
			dir = irseeker_array[0];
			str1 = irseeker_array[1];
			str2 = irseeker_array[2];
			str3 = irseeker_array[3];
			str4 = irseeker_array[4];
			str5 = irseeker_array[5];

			if (rm(dir,2) == 0) {
				strres = (str1 + str2 + str3 + str4 + str5) / SEEKER_DIV;
			} else {
				strres = str1 + str2 + str3 + str4 + str5;
			}
			
			values[i] = strres;
		}

		if (i == 16) {
			light_val = sen.read.rawval(PORT_LIGHT, LIGHT_COLOR_NUM);
			values[i] = light_val;
		}

		if (i == 17) {
			compass_array = i2c.readregs(2, 1, 66, 4);
        	compass = compass_array[0] * 2 + compass_array[1];
			values[i] = compass;
		}

		if (i == 18) {
			compass_array = i2c.readregs(2, 1, 66, 4);
        	compass = compass_array[0] * 2 + compass_array[1];
			values[i] = compass;
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