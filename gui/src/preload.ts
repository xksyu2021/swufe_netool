import { contextBridge, ipcRenderer } from 'electron'

// submit structure
type Info = {
    isp: string,
    account: string,
    password: string
}

// bridge between render process and main process
contextBridge.exposeInMainWorld('conn', {
    submit: (info:Info) => ipcRenderer.invoke('submit',info)
})