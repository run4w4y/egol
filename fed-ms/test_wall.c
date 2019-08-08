sen.setmode(1, 4)
r = 0
g = 0
b = 0
state = "none"

while (true) {
    printupd()
    sen.read.rgb(1, r, g, b)
    print("r", r)
    print("g", g)
    print("b", b)
    if (r > 2 and r < 20) {
        state = "wall"
    } else {
        if (r > 20) {
            state = "ball"
        } else {
            state = "none"
        }
    }
    print("state", state)
}