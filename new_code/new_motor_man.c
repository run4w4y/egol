

func num alga(Vector_R, tizlek) {
    v_r = 0
    if (Vector_R < 0) {
        motor_Power_A = tizlek * (cos(rad(360+Vector_R)) * cos(rad(330)) - sin(rad(360+Vector_R)) * sin(rad(330))) + v_r
        motor_Power_B = tizlek * (cos(rad(360+Vector_R)) * cos(rad(90)) - sin(rad(360+Vector_R)) * sin(rad(90))) + v_r
        motor_Power_C = tizlek * (cos(rad(360+Vector_R)) * cos(rad(210)) - sin(rad(360+Vector_R)) * sin(rad(210))) + v_r
    } else {
        if (Vector_R > 0) {
            motor_Power_A = tizlek * (cos(rad(Vector_R)) * cos(rad(330)) - sin(rad(Vector_R)) * sin(rad(330))) + v_r
            motor_Power_B = tizlek * (cos(rad(Vector_R)) * cos(rad(90)) - sin(rad(Vector_R)) * sin(rad(90))) + v_r
            motor_Power_C = tizlek * (cos(rad(Vector_R)) * cos(rad(210)) - sin(rad(Vector_R)) * sin(rad(210))) + v_r
        } else {
            if (Vector_R == 0) {
                motor_Power_A = tizlek + v_r
                motor_Power_B = v_r
                motor_Power_C = -tizlek + v_r
            }
        }
    }

    mt.start("C", motor_Power_A)
    mt.start("B", motor_Power_B)
    mt.start("D", motor_Power_C)
}

while (true) {
    t0 = time()
    while (time() - t0 < 500) {
        alga(0, 50)
    }

    t0 = time()
    while (time() - t0 < 500) {
        alga(-90, 50)
    }

    t0 = time()
    while (time() - t0 < 500) {
        alga(180, 50)
    }

    t0 = time()
    while (time() - t0 < 500) {
        alga(90, 50)
    }
}