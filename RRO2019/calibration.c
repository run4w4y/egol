// ports
handle = open.r("ports.txt"); // open the file with values

PORT_SEEKER = tonum(readline(handle));
PORT_COMPASS = tonum(readline(handle));
PORT_LIGHT_FRONT = tonum(readline(handle));
PORT_LIGHT_BACK = tonum(readline(handle));
SEEKER_DELAY = tonum(readline(handle));
COMPASS_DELAY = tonum(readline(handle));
LIGHT_DELAY = tonum(readline(handle));
PORT_FIRST_MOTOR = tohex(tonum(readline(handle)) + 9);
PORT_SECOND_MOTOR = tohex(tonum(readline(handle)) + 9);
PORT_THIRD_MOTOR = tohex(tonum(readline(handle)) + 9);
PORT_KICKER = tohex(tonum(readline(handle)) + 9);

closef(handle);

// reading from file

handle = open.r("cal.txt");

values_len = 7;
values = new.vector(values_len, 0);

names[0] = "alpha";
names[1] = "seek_div";
names[2] = "kp_com";
names[3] = "strmax";
names[4] = "back_lt";
names[5] = "str2";
names[6] = "str4";

if (handle == 0) {
	COMPASS_ALPHA = 0;
	values[0] = COMPASS_ALPHA;
	SEEKER_DIV = 100;
	values[1] = SEEKER_DIV;
	KP_COM = 75;
	values[2] = KP_COM;
	STR_MAX = 0;
	values[3] = STR_MAX;
	BACK_LIGHT_VALUE = 0;
	values[4] = BACK_LIGHT_VALUE;
	STR_VALUE_2 = 0;
	values[5] = STR_VALUE_2;
	STR_VALUE_4 = 0;
	values[6] = STR_VALUE_4;
} else {
	COMPASS_ALPHA = tonum(readline(handle));
	values[0] = COMPASS_ALPHA;
	SEEKER_DIV = tonum(readline(handle));
	values[1] = SEEKER_DIV;
	KP_COM = tonum(readline(handle));
	values[2] = KP_COM;
	STR_MAX = tonum(readline(handle));
	values[3] = STR_MAX;
	BACK_LIGHT_VALUE = tonum(readline(handle));
	values[4] = BACK_LIGHT_VALUE;
	STR_VALUE_2 = tonum(readline(handle));
	values[5] = STR_VALUE_2;
	STR_VALUE_4 = tonum(readline(handle));
	values[6] = STR_VALUE_4;
}

closef(handle);

// sensors

sen.setmode(PORT_LIGHT_BACK, 1);

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
			compass_array = i2c.readregs(PORT_COMPASS, 1, 66, 4);
        	compass = compass_array[0] * 2 + compass_array[1];
			values[i] = compass;
		}

		if (i == 3) {
			strmax = 0;
			for (j = 0; j < 600; ++j) { // progress bar
				if (ispressed(6) == true) { // exit button
					goto end;
				}
				
				irseeker_array = i2c.readregs(PORT_SEEKER, 8, 73, 6);

				strnow = irseeker_array[3];
				if (strnow > strmax) {
					strmax = strnow;
				}

				p = floor(j/6);
				scr.clear();
				scr.rect(1, 37, 48, 105, 10);
				line(1, 39, 50, 39+p, 50);
				line(1, 39, 51, 39+p, 51);
				line(1, 39, 52, 39+p, 52);
				line(1, 39, 53, 39+p, 53);
				line(1, 39, 54, 39+p, 54);
				line(1, 39, 55, 39+p, 55);
				txt(1, 89-len(p)*4, 30, 1, p+"%");
				delay(5);
			}
			values[i] = strmax;
		}

		if (i == 4) {
			senball_calib1 = 0
			for (j = 0; j < 600; ++j) { // progress bar
				if (ispressed(6) == true) { // exit button
					goto end
				}
				senball_calib1 = senball_calib1 + sen.percent(PORT_LIGHT_BACK);
				p = floor(j/6)
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
			senball_calib1 = senball_calib1 / 600
			values[i] = senball_calib1
		}

		if (i == 5) {
			irseeker_array = i2c.readregs(PORT_SEEKER, 8, 73, 6);
			values[i] = irseeker_array[2];
		}

		if (i == 6) {
			irseeker_array = i2c.readregs(PORT_SEEKER, 8, 73, 6);
			values[i] = irseeker_array[4];
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