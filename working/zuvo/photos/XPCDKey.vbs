ON ERROR RESUME NEXT

Dim VOL_PROD_KEY
if Wscript.arguments.count<1 then
                                        VOL_PROD_KEY=InputBox("�п�J���T���Ǹ�"&vbCr&vbCr&" "&vbCr&vbCr&" ","Windows XP �Ǹ��󴫾�","DP7CM-PD6MC-6BKXT-M8JJ6-RPXGJ")
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
                                           Wscript.echo "�ק令�\�C"
                                        end if

                                        if err <> 0 then
                                           Wscript.echo "����! ��JCD-Key�����T!"
                                           Err.Clear
                                        end if

Next