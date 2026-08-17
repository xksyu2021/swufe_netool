import { app, BrowserWindow, ipcMain } from 'electron'
import path from 'node:path'
import { spawn } from 'node:child_process'

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

    let kernel = spawn(
        'kernel.exe',
        [],{
            stdio: ['pipe','pipe','pipe'],
            windowsHide: true
        })



})