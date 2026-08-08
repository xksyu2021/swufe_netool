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

type Info = {
    isp: string,
    account: string,
    password: string
}

interface Window {
    conn: {
        submit: (info: Info) => Promise<{
            success: boolean
            message: string
        }>
    }
}

let isp:type

function colorChange<T extends Record<string, string>>(p:T[keyof T], list: T){
    for(let item of Object.values(list)){
        document.getElementById(item)?.style.setProperty("background-color","unset")
    }
    document.getElementById(p)?.style.setProperty("background-color","beige")
}

function viewChange_pos(p: pos){
    switch (p) {
        case pos.DORM: {
            document.getElementById(pos_layout.DORM)?.style.setProperty("display", "flex")
            document.getElementById(pos_layout.TEACH)?.style.setProperty("display", "none")
            colorChange(pos.DORM,pos)
            viewChange_type(type.CMCC)
            break
        }
        case pos.TEACH: {
            document.getElementById(pos_layout.DORM)?.style.setProperty("display", "none")
            document.getElementById(pos_layout.TEACH)?.style.setProperty("display", "flex")
            colorChange(pos.TEACH,pos)
            viewChange_type(type.ISWUFE)
            break
        }
        default:
            break
    }
}

function viewChange_type(t: type){
    colorChange(t,type)
    isp = t
    document.getElementById("edu-domain")?.style.setProperty("display", "none")
    if(t === type.EDU){
        document.getElementById("edu-domain")?.style.setProperty("display", "flex")
    }
}

function init(){
    isp = type.CMCC
    viewChange_pos(pos.DORM)
}

function submit(){
    const info: Info = {
        isp,
        account: document.querySelector<HTMLInputElement>("#acc")?.value ?? "",
        password: document.querySelector<HTMLInputElement>("#pwd")?.value ?? ""
    }

    window.conn.submit(info).then(r => {

    })
}

init()

for(let item of Object.values(pos)){
    document.getElementById(item)?.addEventListener(
        'click',() => viewChange_pos(item)
    )
}
for(let item of Object.values(type)){
    document.getElementById(item)?.addEventListener(
        'click',() => viewChange_type(item)
    )
}
document.getElementById("conn")?.addEventListener(
    'click',() => submit()
)