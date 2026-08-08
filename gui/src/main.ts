import { app, BrowserWindow, ipcMain } from 'electron'
import path from 'node:path'

const createWindow = () => {
    const win = new BrowserWindow({
        width: 450,
        height: 430,
        resizable: false,
        webPreferences: {
            preload: path.join(__dirname, 'preload.js')
        }
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

ipcMain.handle('submit',(_event, info) => {

    

})