func num alga_pizda(v_r, tizlek) {

    turn = 0

	if (v_r < 0) {
		motor_Power_D = tizlek * (cos(60+v_r) * cos(180) - sin(60+v_r) * sin(180)) + turn
    	motor_Power_B = tizlek * (cos(60+v_r) * cos(300) - sin(60+v_r) * sin(300)) + turn
    	motor_Power_C = tizlek * (cos(60+v_r) * cos(180) - sin(60+v_r) * sin(180)) + turn
	} else {
		if (v_r > 0) {
			motor_Power_D = tizlek * (cos(v_r) * cos(60) - sin(v_r) * sin(180)) + turn
    		motor_Power_B = tizlek * (cos(v_r) * cos(60) - sin(v_r) * sin(120)) + turn
    		motor_Power_C = tizlek * (cos(v_r) * cos(180) - sin(v_r) * sin(180)) + turn
		} else {
			if (v_r == 0) {
				motor_Power_D = tizlek + turn
    			motor_Power_B = turn
    			motor_Power_C = -tizlek + turn
			}
		}
	}

    motor.Start("D",-motor_Power_D)
    motor.Start("B",-motor_Power_B)
    motor.Start("C",-motor_Power_C)
}

while (true) {
    alga_pizda(0, 100)
}