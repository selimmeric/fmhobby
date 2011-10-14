ON ERROR RESUME NEXT

Dim VOL_PROD_KEY
if Wscript.arguments.count<1 then
                                        VOL_PROD_KEY=InputBox("請輸入正確的序號"&vbCr&vbCr&" "&vbCr&vbCr&" ","Windows XP 序號更換器","DP7CM-PD6MC-6BKXT-M8JJ6-RPXGJ")
                                        if VOL_PROD_KEY="" then
                                          Wscript.quit
                                        end if
else
                                        VOL_PROD_KEY = Wscript.arguments.Item(0)
end if

VOL_PROD_KEY = Replace(VOL_PROD_KEY,"-","") 'remove hyphens if any

for each Obj in GetObject("winmgmts:{impersonationLevel=impersonate}").InstancesOf ("win32_WindowsProductActivation")

                                        result = Obj.SetProductKey (VOL_PROD_KEY)

                                        if err = 0 then
                                           Wscript.echo "修改成功。"
                                        end if

                                        if err <> 0 then
                                           Wscript.echo "失敗! 輸入CD-Key不正確!"
                                           Err.Clear
                                        end if

Next