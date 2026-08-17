import { app, BrowserWindow, ipcMain } from 'electron'
import path from 'node:path'
import { spawn } from 'node:child_process'
import {StringDecoder} from "node:string_decoder";

// basic window configs
const createWindow = () => {
    const win = new BrowserWindow({
        width: 450,
        height: 430,
        resizable: false,
        webPreferences: {
            preload: path.join(__dirname, 'preload.js')
        },
        icon: path.join(__dirname, '../assets/icon.ico')
    })
    win.removeMenu()
    win.loadFile('render/index.html')
}
app.whenReady().then(() => {
    createWindow()
})
app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') app.quit()
})

// submit structure
type Info = {
    isp: string,
    account: string,
    password: string
}

// handle submit action
ipcMain.handle('submit',(_event, info:Info) => {
    return new Promise((resolve, reject) => {

        // preparation for receiving stdout and stderr
        let output = '';
        let error = ''
        const out_decoder = new StringDecoder('utf8');
        const err_decoder = new StringDecoder('utf8');

        // create kernel process
        let kernel = spawn(
            'kernel.exe',
            [], {
                stdio: ['pipe', 'pipe', 'pipe'],
                windowsHide: true
            })

        // read stdout and stderr
        kernel.stdout.on('data', (chunk) => {
            const raw = out_decoder.write(chunk)
            output += raw
            handle_submit()
        })
        kernel.stderr.on('data', (chunk) => {
            const raw = err_decoder.write(chunk)
            error += raw
            handle_error()
        })

        // submit info and get connection
        function handle_submit() {
            if (output.includes("*isp")) {
                kernel.stdin.write(`${info.isp}\n`)
                output = output.slice(
                    output.lastIndexOf("*isp") + "*isp".length,
                );
            }
            if (output.includes("*acc")) {
                kernel.stdin.write(`${info.account}\n`)
                output = output.slice(
                    output.lastIndexOf("*acc") + "*acc".length,
                );
            }
            if (output.includes("*pwd")) {
                kernel.stdin.write(`${info.password}\n`)
                output = output.slice(
                    output.lastIndexOf("*pwd") + "*pwd".length,
                );
            }

            if(output.includes("*suc")){
                kernel.kill()
                resolve({
                    ok: true,
                    message: ''
                })
            }
        }

        // start analyzer when getting errors
        function handle_error() {
            let err = '*000#'
            if (err.includes("#")) {
                let index_start = error.lastIndexOf("*")
                let index_end = error.lastIndexOf("#")
                err = error.substring(index_start, index_end+1)
                kernel.kill()

                let analyzer = spawn(
                    'analyzer.exe',
                    ['--code', err], {
                        stdio: ['pipe', 'pipe', 'pipe'],
                        windowsHide: true
                    })

                // read the result of analyzer and return to the render process
                {
                    let output = '';
                    const out_decoder = new StringDecoder('utf8');
                    analyzer.stdout.on('data', (chunk) => {
                        const raw = out_decoder.write(chunk)
                        output += raw
                    })

                    analyzer.on('close', (code)=>{
                        resolve({
                            ok: false,
                            message: output
                        })
                    })
                }
            }
        }
    })
})