t0 = time()
mt.resetcount("BCD")

while (time() - t0 < 1000) {
    mt.spw("BCD", 100)
}

mt.stop("BCD", true)
delay(500)

flush()
while (btn.rn != "E") {
    printupd()
    print("B", mt.getcount("B"))
    print("C", mt.getcount("C"))
    print("D", mt.getcount("D"))
}