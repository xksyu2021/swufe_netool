enum pos{
    DORM = "dorm",
    TEACH = "teach",
}
enum pos_layout {
    DORM = "dorm-layout",
    TEACH = "teach-layout"
}
enum type{
    CMCC = "cmcc",
    CUCC = "cucc",
    CNET = "cnet",
    ISWUFE = "iswufe",
    EDU = "edu",
}

let isp = type.CMCC

function colorChange<T extends Record<string, string>>(p:T[keyof T], list: T){
    for(let item of Object.values(list)){
        document.getElementById(item)?.style.setProperty("background-color","none")
    }
    document.getElementById(p)?.style.setProperty("background-color","beige")
}

function viewChange_pos(p: pos){
    switch (p) {
        case pos.DORM: {
            document.getElementById(pos_layout.DORM)?.style.setProperty("display", "flex")
            document.getElementById(pos_layout.TEACH)?.style.setProperty("display", "none")
            colorChange(pos.DORM,pos)
            colorChange(type.CMCC,type)
            break
        }
        case pos.TEACH: {
            document.getElementById(pos_layout.DORM)?.style.setProperty("display", "none")
            document.getElementById(pos_layout.TEACH)?.style.setProperty("display", "flex")
            colorChange(pos.TEACH,pos)
            colorChange(type.ISWUFE,type)
            break
        }
        default:
            break
    }
}

function viewChange_type(t: type){
    colorChange(t,type)
    isp = t
}

document.getElementById(pos.DORM)?.addEventListener(
    'click',() => viewChange_pos(pos.DORM)
)
document.getElementById(pos.TEACH)?.addEventListener(
    'click',() => viewChange_pos(pos.TEACH)
)