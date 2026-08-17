// id groups used in traverse
let pos_bt_list = ["bt-dorm","bt-teach"]
let isp_bt_list = ["bt-cmcc","bt-cucc","bt-cnet","bt-iswufe","bt-edu"]

// isp var used for submit
let isp:string = "cmcc"

// change the color of buttons to show which is selected
function colorChange(bt:string, list:string[]){
    for(let item of Object.values(list)){
        document.getElementById(item)?.style.setProperty("background-color","unset")
    }
    document.getElementById(bt)?.style.setProperty("background-color","beige")
}

// change the layout when buttons are pressed
function viewChange_pos(p: string){
    switch (p) {
        case "bt-dorm": {
            document.getElementById("bt-dorm")?.style.setProperty("display", "flex")
            document.getElementById("bt-teach")?.style.setProperty("display", "none")
            colorChange("bt-dorm",pos_bt_list)
            viewChange_type("cmcc")
            break
        }
        case "bt-teach": {
            document.getElementById("bt-dorm")?.style.setProperty("display", "none")
            document.getElementById("bt-teach")?.style.setProperty("display", "flex")
            colorChange("bt-teach",pos_bt_list)
            viewChange_type("iswufe")
            break
        }
        default:
            break
    }
}
function viewChange_type(t: string){
    colorChange(t,isp_bt_list)
    isp = t
    if(t === "bt-edu"){
        document.getElementById("st-domain")?.style.setProperty("display", "flex")
    } else {
        document.getElementById("st-domain")?.style.setProperty("display", "none")
    }
}

// submit structure
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

// submit to main process
function submit(){
    const info: Info = {
        isp,
        account: document.querySelector<HTMLInputElement>("#acc")?.value ?? "",
        password: document.querySelector<HTMLInputElement>("#pwd")?.value ?? ""
    }

    window.conn.submit(info).then(r => {

    })
}

// button listeners
for(let item of Object.values(pos_bt_list)){
    document.getElementById(item)?.addEventListener(
        'click',() => viewChange_pos(item)
    )
}
for(let item of Object.values(isp_bt_list)){
    document.getElementById(isp)?.addEventListener(
        'click',() => viewChange_type(item)
    )
}
document.getElementById("conn")?.addEventListener(
    'click',() => submit()
)

// init the layout
viewChange_pos("dorm")