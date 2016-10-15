;********************* (C) COPYRIGHT 2013 e-Design Co.,Ltd. ********************
; File Name : DFU_Func.s     For DFU Ver3.26                       Author: bure
;*******************************************************************************
    RSEG CODE:CODE(2)

;-------------------------------------------------------------------------------
    EXPORT    __Delay_mS     
__Delay_mS:   B.W  0x08000151
    EXPORT    __Dev_SN       
__Dev_SN:     B.W  0x08000155
    EXPORT    __Chk_SYS      
__Chk_SYS:    B.W  0x08000159
    EXPORT    __Lic_ok       
__Lic_ok:     B.W  0x0800015D
;-------------------------------------------------------------------------------
    EXPORT    __LCD_Init     
__LCD_Init:   B.W  0x08000169
    EXPORT    __Add_Color    
__Add_Color:  B.W  0x0800016D
;-------------------------------------------------------------------------------
    EXPORT    __Get_Pixel    
__Get_Pixel:  B.W  0x08000171
    EXPORT    __Set_Pixel    
__Set_Pixel:  B.W  0x08000175
    EXPORT    __Disp_Logo    
__Disp_Logo:  B.W  0x08000179
    EXPORT    __Disp_Str    
__Disp_Str:   B.W  0x0800017D
;-------------------------------------------------------------------------------
    EXPORT    __SetBlock     
__SetBlock:   B.W  0x08000181
    EXPORT    __ClrScrn      
__ClrScrn:    B.W  0x08000185
    EXPORT    __Set_Posi   
__Set_Posi:   B.W  0x08000189
    EXPORT    __Font_8x14  
__Font_8x14:  B.W  0x0800018D
;-------------------------------------------------------------------------------
    EXPORT    __FlashProg    
__FlashProg:  B.W  0x08000191
    EXPORT    __FlashErase  
__FlashErase: B.W  0x08000195
    EXPORT    __DevInfoStr   
__DevInfoStr: B.W  0x08000199
    EXPORT    __EraseColor   
__EraseColor: B.W  0x0800019D
;-------------------------------------------------------------------------------
    EXPORT    __Set_Pixels   
__Set_Pixels: B.W  0x080001A1
    EXPORT    __SendPixels   
__SendPixels: B.W  0x080001A5



    END

;******************************* END OF FILE ***********************************


