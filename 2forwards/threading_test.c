void thread1 {
    done1 = 0;
    sum1 = 0;
    time_start1 = time();
    while (time() - time_start1 < 1000) {
        sum1 = sum1 + 1;
    }
    done1 = 1;
}

void thread2 {
    done2 = 0;
    sum2 = 0;
    time_start2 = time();
    while (time() - time_start1 < 1000) {
        sum2 = sum2 + 1;
    }
    done2 = 1;
}

time_start = time();
sums = 0;
while (time() - time_start < 1000) {
    sums = sums + 1;
}

new.thread = thread1;
new.thread = thread2;

time_start = time();
sum = 0;
while (time() - time_start < 1000) {
    sum = sum + 1;
}

while (done1 != 1 or done2 != 1) {
    delay(0);
}

while (btn.rn == "") {
    printupd();
    print("single", sums);
    print("multi", sum + sum1 + sum2);
}
