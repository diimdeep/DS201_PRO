;/******************** (C) COPYRIGHT 2013 e-Design Co.,Ltd. ********************
; File Name : Display.s                                           Author : bure
;******************************************************************************/
  RSEG CODE:CODE(2)

LCD_DATA_PORT = 0X40011800
LCD_Ctrl_PORT = 0X40011400
ODR           = 0x0C
LCD_WR_RS     = 0x0022
LCD_WR        = 0x0020
LCD_RS        = 0x0002
OUT_H         = 0x10
OUT_L         = 0x14

BACKGROUND    = 0x00                 ;// 背景底色
GRID          = 0x7BEF               ;// 定义背景网格线的颜色
GRID_COLOR    = 0x7BEF7BEF           ;// 32bits 网格线的颜色
WINDOW_Y0     = 19                   ;// 波形显示窗的起始高度

;//=============================================================================
;//                 波形显示相关汇编语言子程序
;//=============================================================================
;// void DrawWindow(u16* pBuf, u16* pTab)
;// R0:pDat, R1:pTab, R2:Col, R3:Var, R4:pCol, R5:Cnt, R6:Tmp,
;// R0:pBuf, R1:pTab, R2:Col, R3:LCD, R4:Ctrl, R5:WR,  R6:Tmp, R7:Cnt, R8:RS
;//=============================================================================
    EXPORT __DrawWindow
__DrawWindow
    PUSH    {R4-R12,LR}
    SUB     SP,  SP, #408          ;// 预留 204*2 Bytes 列数据缓冲区
    MOV     R2,  #0                ;// 列计数初始值
    ADD     R10, R0, #900          ;// 出窗口缓冲区指针初始值
    LDRH    R11, [R1, #60+4*2]     ;// 弹出窗口的水平开始位置
    LDRH    R12, [R1, #60+5*2]     ;// 弹出窗口的水平宽度
    ADD     R12, R11, R12          ;// 弹出窗口的水平结束位置

;//----------- 画背景 -----------//
Draw_Loop
    CMP     R2,  #0
    ITT     EQ
    BLEQ    Buld_0                 ;// 建立外沿列缓冲区的背景数据
    BEQ     Draw_Wave
    CMP     R2,  #302
    ITT     EQ
    BLEQ    Buld_0                 ;// 建立外沿列缓冲区的背景数据
    BEQ     Draw_Wave

    CMP     R2,  #1
    ITT     EQ
    BLEQ    Buld_1                 ;// 建立边线列缓冲区的背景数据
    BEQ     Draw_Wave
    MOV     R3,  #301
    CMP     R2,  R3
    ITT     EQ
    BLEQ    Buld_1                 ;// 建立边线列缓冲区的背景数据
    BEQ     Draw_Wave

    SUB     R3,  R2, #1
    MOV     R6,  #25
    UDIV    R5,  R3, R6
    MUL     R5,  R5, R6
    SUBS    R5,  R3, R5
    ITT     EQ
    BLEQ    Buld_4                 ;// 建立格线列缓冲区的背景数据
    BEQ     Draw_Wave

    MOV     R6,  #5
    UDIV    R5,  R3, R6
    MUL     R5,  R5, R6
    SUBS    R5,  R3, R5
    ITT     EQ
    BLEQ    Buld_3                 ;// 建立格点列缓冲区的背景数据
    BEQ     Draw_Wave
    BL      Buld_2                 ;// 建立格内列缓冲区的背景数据

;//--------- 画波形曲线 ---------//
Draw_Wave
    CMP     R2,  #3                ;// 从3~299
    BCC     Horizontal
    CMP     R2,  #300
    BCS     Horizontal

    LDRH    R3,  [R1, #3*2]        ;// 取CH_3波形曲线的消隐标志
    TST     R3,  #4
    ITTTT   EQ
    MOVEQ   R3,  #40+3*2           ;// R3 = CH_3波形曲线颜色表偏移
    LDRHEQ  R4,  [R1, #20+3*2]     ;// R4 = CH_3波形曲线的基线位置
    MOVEQ   R8,  #8                ;// R8 = CH_3波形数据的屏蔽位
    BLEQ    Draw_Digit

    LDRH    R3,  [R1, #2*2]        ;// 取CH_2波形曲线的消隐标志
    TST     R3,  #4
    ITTTT   EQ
    MOVEQ   R3,  #40+2*2           ;// R3 = CH_2波形曲线颜色表偏移
    LDRHEQ  R4,  [R1, #20+2*2]     ;// R4 = CH_2波形曲线的基线位置
    MOVEQ   R8,  #4                ;// R8 = CH_2波形数据的屏蔽位
    BLEQ    Draw_Digit

    LDRH    R3,  [R1, #1*2]        ;// 取CH_1波形曲线的消隐标志
    TST     R3,  #4
    ITTT    EQ
    MOVEQ   R3,  #40+1*2           ;// R3 = CH_1波形曲线颜色表偏移
    ADDEQ   R4,  R0, #1*300
    BLEQ    Draw_Analog

    LDRH    R3,  [R1, #0*2]        ;// 取CH_0波形曲线的消隐标志
    TST     R3,  #4
    ITTT    EQ
    MOVEQ   R3,  #40+0*2           ;// R3 = CH_0波形曲线颜色表偏移
    ADDEQ   R4,  R0, #0*300
    BLEQ    Draw_Analog

;//------- 画水平方向游标 -------//
Horizontal
    CMP     R2,  #0
    ITT     EQ
    BLEQ    Cursor_0               ;// 外沿列画游标端点
    BEQ     Vertical
    CMP     R2,  #302
    ITT     EQ
    BLEQ    Cursor_0               ;// 外沿列画游标端点
    BEQ     Vertical

    CMP     R2,  #1
    ITT     EQ
    BLEQ    Cursor_1               ;// 边线列画游标端点
    BEQ     Vertical
    MOV     R5,  #301
    CMP     R2,  R5
    ITT     EQ
    BLEQ    Cursor_1               ;// 边线列画游标端点
    BEQ     Vertical

    CMP     R2,  #2
    ITT     EQ
    BLEQ    Cursor_2               ;// 内沿列画游标端点
    BEQ     Vertical
    CMP     R2,  #300
    IT      EQ
    BLEQ    Cursor_2               ;// 内沿列画游标端点
    BEQ     Vertical
    BL      Cursor_3               ;// 其余列画游标线

;//------- 画垂直方向游标 -------//
Vertical
    BL      Cursor_4

;//--------- 画弹出窗口 ---------//

    LDRH    R3,  [R1, #60+8*2]     ;// 取弹出窗口的消隐标志
    TST     R3,  #1
    BNE     Send
    CMP     R2,  R11               ;// 判断弹出窗口列处理开始
    BLT     Send
    CMP     R2,  R12               ;// 判断弹出窗口列处理结束
    IT      LT
    BLLT    Draw_Pop               ;// 列计数在弹出窗口内

;//--------- 显示列数据 ---------//
Send
    BL      Send_LCD               ;// 从缓冲区传送一列数据到LCD
    CMP     R2,  #302
    ITT     NE
    ADDNE   R2,  R2, #1
    BNE     Draw_Loop              ;// 处理下1列显示数据

    ADD     SP,  SP, #408
    POP     {R4-R12,PC}
;//=============================================================================
; Draw_Pop(R2:Col, R10:pPop, R11:By0, R12:By1)// 画弹出窗口 Used: R5-R7
;//=============================================================================
Draw_Pop
    MOV     R7,  #0x10001            ;// 起始位置

    LDRH    R5,  [R1, #60+6*2]     ;// 取弹出窗口的垂直开始位置
    LDRH    R6,  [R1, #60+7*2]     ;// 取弹出窗口的垂直高度

    ADD     R3,  R11, #0           ;// 弹出窗口第0列圆角处理
    CMP     R2,  R3
    ITTT    EQ
    ADDEQ   R5,  R5, #6
    SUBEQ   R6,  R6, #12
    MOVEQ   R7,  #0x80008

    ADD     R3,  R11, #1           ;// 弹出窗口第1列圆角处理
    CMP     R2,  R3
    ITTT    EQ
    ADDEQ   R5,  R5, #4
    SUBEQ   R6,  R6, #8
    MOVEQ   R7,  #0x40004

    ADD     R3,  R11, #2           ;// 弹出窗口第2列圆角处理
    CMP     R2,  R3
    ITTT    EQ
    ADDEQ   R5,  R5, #2
    SUBEQ   R6,  R6, #4
    MOVEQ   R7,  #0x20002

    SUB     R3,  R12, #3           ;// 弹出窗口第n-3列圆角处理
    CMP     R2,  R3
    ITTT    EQ
    ADDEQ   R5,  R5, #2
    SUBEQ   R6,  R6, #4
    MOVEQ   R7,  #0x20002

    SUB     R3,  R12, #2           ;// 弹出窗口第n-2列圆角处理
    CMP     R2,  R3
    ITTT    EQ
    ADDEQ   R5,  R5, #4
    SUBEQ   R6,  R6, #8
    MOVEQ   R7,  #0x40004

    SUB     R3,  R12, #1           ;// 弹出窗口第n-1列圆角处理
    CMP     R2,  R3
    ITTT    EQ
    ADDEQ   R5,  R5, #6
    SUBEQ   R6,  R6, #12
    MOVEQ   R7,  #0x80008

    ADD     R4,  SP, R5
    LDRH    R8,  [R1, #60+9*2]     ;// 取弹出窗口的背景颜色
    LDRH    R9,  [R1, #60+10*2]    ;// 取弹出窗口的前景颜色
    LDRH    R3,  [R10], #2         ;// 取双字节垂直高度数据(以双字节整数倍保存)

    CMP     R8,  #0
    BEQ     Pop_Loop_Penetrate

Pop_Loop
    TST     R3,  R7
    ITE     NE
    STRHNE  R9,  [R4], #2          ;// 画前景点
    STRHEQ  R8,  [R4], #2          ;// 画背景点
    SUBS    R6,  R6, #2
    IT      EQ
    BXEQ    LR                     ;// 该列显示完成
    LSLS    R7,  R7, #1
    ITT     CS
    LDRHCS  R3,  [R10], #2         ;// 重新装载双字节垂直高度数据
    MOVCS   R7,  #0x10001
    B       Pop_Loop

Pop_Loop_Penetrate
    TST     R3,  R7
    ITE      NE
    STRHNE  R9,  [R4], #2          ;// 画前景点
    ADDEQ   R4,  R4, #2
    SUBS    R6,  R6, #2
    IT      EQ
    BXEQ    LR                     ;// 该列显示完成
    LSLS    R7,  R7, #1
    ITT     CS
    LDRHCS  R3,  [R10], #2         ;// 重新装载双字节垂直高度数据
    MOVCS   R7,  #0x10001
    B       Pop_Loop_Penetrate

;//=============================================================================
; Draw_Digit(R0:pDat, R2:Col, R3:Color, R4:Posi, R8:Mask)// 画数字波形 Used: R5-R7
;//=============================================================================
Draw_Digit
    ADD     R7,  R0, #600          ;// 指针定在数字信号缓冲区
    ADD     R7,  R7, R2
    LDRB    R5,  [R7]              ;// 取当前列波形数据数据 n1
    LDRB    R6,  [R7, #-1]         ;// 取上一列波形数据数据 n0

    LSR     R4,  R4, #1
    ANDS    R5,  R5, R8            ;// 分离数据
    ITE     EQ
    MOVEQ   R5,  R4                ;// 确定振幅
    ADDNE   R5,  R4, #30

    ANDS    R6,  R6, R8            ;// 分离数据
    ITE     EQ
    MOVEQ   R4,  R4                ;// 确定振幅
    ADDNE   R4,  R4, #30

    B       Analog0
;//=============================================================================
; Draw_Analog(R2:Col, R3:Color, R4:pDat)// 画模拟波形曲线  Used: R3-R7
;//=============================================================================
Draw_Analog1
    ADD     R4,  R4, R2
    LDRB    R5,  [R4]              ;// 取当前列波形数据数据 n1
    LDRB    R4,  [R4, #-1]         ;// 取上一列波形数据数据 n0
    ADD     R5,  R5, #50
    ADD     R4,  R4, #50
    B       Analog0
Draw_Analog
    ADD     R4,  R4, R2
    LDRB    R5,  [R4]              ;// 取当前列波形数据数据 n1
    LDRB    R4,  [R4, #-1]         ;// 取上一列波形数据数据 n0
Analog0

    CMP     R5,  #255              ;// 上端截尾
    IT      EQ
    BXEQ    LR
    CMP     R5,  #0                ;// 下端截尾
    IT      EQ
    BXEQ    LR
    CMP     R5,  #255              ;// 上端截尾
    IT      EQ
    BXEQ    LR
    CMP     R5,  #0                ;// 下端截尾
    IT      EQ
    BXEQ    LR

    CMP     R5,  R4
    ITTEE   CS                     ;// R5 = | n1 - n0 |
    MOVCS   R6,  R4
    SUBCS   R5,  R5, R4
    MOVCC   R6,  R5                ;// n1, n0 中最小的送 R6
    SUBCC   R5,  R4, R5

    CMP     R6,  #199              ;// 上起点超界消隐
    IT      GT
    BXGT    LR
    ADD     R4,  R5, R6
    CMP     R4,  #199              ;// 上增幅超界限幅
    IT      GT
    RSBGT   R5,  R6, #199
    BGT     Analog2

    CMP     R4,  #3                ;// 下起点超界消隐
    IT      LT
    BXLT    LR
    CMP     R6,  #3                ;// 下增幅超界限幅
    ITT     LT
    MOVLT   R6,  #3
    SUBLT   R5,  R4, #3
    BLT     Analog2

    CMP     R5,  #0                ;// 水平线加粗
    ITT     EQ
    SUBEQ   R6,  R6, #1
    ADDEQ   R5,  R5, #2

Analog2
    CMP     R5,  #20                ;// 选择颜色
    IT      GE
    ADDGE   R3,  R3, #20
    LDRH    R3,  [R1, R3]

    LSL     R6,  R6, #1
    ADD     R6,  SP, R6            ;// 确定显示位置
Analog3
    STRH    R3,  [R6], #2          ;// 画波形点
    SUBS    R5,  R5, #1
    BCS     Analog3

Analog9
    BX      LR
;//=============================================================================
; Cursor_4(R1:pTab, R2:Col)// 外沿列画游标端点  Used: R3-R8
;//=============================================================================
Cursor_4
    MOV     R3,  #7                ;// 7~9项为垂直方向游标
Cursor40
    MOV     R4,  SP
    LDRH    R5,  [R1, R3, LSL #1]  ;// 取该游标变量的消隐标志
    TST     R5,  #1
    BEQ     Cursor41               ;// 非游标消隐转
    B       Cursor49               ;// 是游标消隐转
Cursor41
    ADD     R1,  R1, #20
    LDRH    R5,  [R1, R3, LSL #1]  ;// 取该游标变量的显示位置
    ADD     R1,  R1, #20
    LDRH    R6,  [R1, R3, LSL #1]  ;// 取该游标变量的显示颜色
    SUB     R1,  R1, #40           ;// 恢复变量表指针

    SUB     R8,  R5, #2
    CMP     R2,  R8                ;// 画左外沿行游标端点
    BNE     Cursor42
    STRH    R6,  [R4]              ;// 画左下外沿
    ADD     R4,  R4, #404
    STRH    R6,  [R4]              ;// 画左上外沿
    B       Cursor49
Cursor42
    ADD     R8,  R8, #1
    CMP     R2,  R8                ;// 画左边线行游标端点
    BNE     Cursor43
    STRH    R6,  [R4], #2          ;// 画左下外沿
    STRH    R6,  [R4]              ;// 画左下边线
    ADD     R4,  R4, #400
    STRH    R6,  [R4], #2          ;// 画左上边线
    STRH    R6,  [R4]              ;// 画左上外沿
    B       Cursor49
Cursor43
    ADD     R8,  R8, #1
    CMP     R2,  R8                ;// 画游标端点, 画游标线
    BNE     Cursor45
    STRH    R6,  [R4], #2          ;// 画下外沿线
    STRH    R6,  [R4], #2          ;// 画下边线更
    STRH    R6,  [R4]              ;// 画下内沿线
    ADD     R4,  SP, #400
    STRH    R6,  [R4], #2          ;// 画上内沿线
    STRH    R6,  [R4], #2          ;// 画上边线
    STRH    R6,  [R4]              ;// 画上外沿线

    LDRH    R5,  [R1, R3, LSL #1]  ;// 取该游标变量的消隐标志
    TST     R5,  #2
    BNE     Cursor45               ;// 是游标线消隐则转
    MOV     R4,  SP
    ADD     R7,  R4, #400
Cursor44
    STRH    R6,  [R4], #8          ;// 画游标线
    CMP     R7,  R4
    BCS     Cursor44
    B       Cursor49
Cursor45
    ADD     R8,  R8, #1
    CMP     R2,  R8                ;// 画右边线行游标端点
    BNE     Cursor46
    STRH    R6,  [R4], #2          ;// 画右下外沿
    STRH    R6,  [R4]              ;// 画右下边线
    ADD     R4,  R4, #400
    STRH    R6,  [R4], #2          ;// 画右上边线
    STRH    R6,  [R4]              ;// 画右上外沿
    B       Cursor49
Cursor46
    ADD     R8,  R8, #1
    CMP     R2,  R8                ;// 画右外沿行游标端点
    BNE     Cursor49
    STRH    R6,  [R4]              ;// 画右下外沿
    ADD     R4,  R4, #404
    STRH    R6,  [R4]              ;// 画右上外沿
Cursor49
    ADD     R3,  R3, #1
    CMP     R3,  #10
    BNE     Cursor40
    BX      LR
;//=============================================================================
; Cursor_3(R1:pTab, R2:Col)// 画其余列游标线  Used: R3-R6
;//=============================================================================
Cursor_3
    MOV     R3,  #6                ;// 0~6项为水平方向游标
    MOV     R4,  SP
Cursor31
    LDRH    R5,  [R1, R3, LSL #1]  ;// 取该游标变量的消隐标志
    TST     R5,  #2
    BNE     Cursor32               ;// 是游标线消隐则转
    SUB     R5,  R2, #1
    ANDS    R5,  R5, #3
    BNE     Cursor32               ;// 是游标线虚线位置则转
    ADD     R1,  R1, #20
    LDRH    R5,  [R1, R3, LSL #1]  ;// 取该游标变量的显示位置
    ADD     R4,  SP, R5
    ADD     R1,  R1, #20
    LDRH    R6,  [R1, R3, LSL #1]  ;// 取该游标变量的显示颜色
    STRH    R6,  [R4]              ;// 画游标线
    SUB     R1,  R1, #40           ;// 恢复变量表指针
Cursor32
    SUBS    R3,  #1
    BPL     Cursor31               ;// 处理下1个游标端点
    BX      LR
;//=============================================================================
; Cursor_0(R1:pTab, R2:Col)// 画外沿列游标端点  Used: R3-R6
;//=============================================================================
Cursor_0
    MOV     R3,  #6                ;// 0~6项为水平方向游标
    MOV     R4,  SP
Cursor01
    LDRH    R5,  [R1, R3, LSL #1]  ;// 取该游标变量的消隐标志
    TST     R5,  #1
    BNE     Cursor02               ;// 是游标端点消隐则转
    ADD     R1,  R1, #20
    LDRH    R5,  [R1, R3, LSL #1]  ;// 取该游标变量的显示位置
    ADD     R4,  SP, R5
    ADD     R1,  R1, #20
    LDRH    R6,  [R1, R3, LSL #1]  ;// 取该游标变量的显示颜色
    SUB     R4,  R4, #4
    STRH    R6,  [R4], #2
    STRH    R6,  [R4], #2
    STRH    R6,  [R4], #2          ;// 画外沿列游标端点
    STRH    R6,  [R4], #2
    STRH    R6,  [R4], #2
    SUB     R1,  R1, #40           ;// 恢复变量表指针
Cursor02
    SUBS    R3,  #1
    BPL     Cursor01               ;// 处理下1个游标端点
    BX      LR
;//=============================================================================
; Cursor_1(R1:pTab, R2:Col)// 画边线列游标端点  Used: R3-R6
;//=============================================================================
Cursor_1
    MOV     R3,  #6                ;// 0~6项为水平方向游标
    MOV     R4,  SP
Cursor11
    LDRH    R5,  [R1, R3, LSL #1]  ;// 取该游标变量的消隐标志
    TST     R5,  #1
    BNE     Cursor12               ;// 是游标端点消隐则转
    ADD     R1,  R1, #20
    LDRH    R5,  [R1, R3, LSL #1]  ;// 取该游标变量的显示位置
    ADD     R4,  SP, R5
    ADD     R1,  R1, #20
    LDRH    R6,  [R1, R3, LSL #1]  ;// 取该游标变量的显示颜色
    SUB     R4,  R4, #2
    STRH    R6,  [R4], #2
    STRH    R6,  [R4], #2          ;// 画边线列游标端点
    STRH    R6,  [R4], #2
    SUB     R1,  R1, #40           ;// 恢复变量表指针
Cursor12
    SUBS    R3,  #1
    BPL     Cursor11               ;// 处理下1个游标端点
    BX      LR
;//=============================================================================
; Cursor_2(R1:pTab, R2:Col)// 画内沿列游标端点  Used: R3-R6
;//=============================================================================
Cursor_2
    MOV     R3,  #6                ;// 0~6项为水平方向游标
    MOV     R4,  SP
Cursor21
    LDRH    R5,  [R1, R3, LSL #1]  ;// 取该游标变量的消隐标志
    TST     R5,  #1
    BNE     Cursor22               ;// 是游标端点消隐则转
    ADD     R1,  R1, #20
    LDRH    R5,  [R1, R3, LSL #1]  ;// 取该游标变量的显示位置
    ADD     R4,  SP, R5
    ADD     R1,  R1, #20
    LDRH    R6,  [R1, R3, LSL #1]  ;// 取该游标变量的显示颜色
    STRH    R6,  [R4]              ;// 画内沿列游标端点
    SUB     R1,  R1, #40           ;// 恢复变量表指针
Cursor22
    SUBS    R3,  #1
    BPL     Cursor21               ;// 处理下1个游标端点
    BX      LR
;//=============================================================================
;// R0:pDat, R1:pTab, R2:Col, R3:Var, R4:pBuf, R5:Cnt, R6:Tmp,
;//=============================================================================
; void Fill_Base(R3 = u32 Color)// 列缓冲区填底色 RET: R4+202 Used: R3-R5
;//=============================================================================
Fill_Base
    MOV     R4,  SP
    MOV     R5,  #102          ;// 204行/2 共408 Bytes
Fill_Loop_0
    STR     R3,  [R4], #4      ;// 传送完成后指针加4
    SUBS    R5,  #1
    BNE     Fill_Loop_0
    ADD     R4,  SP, #2        ;// 指针对齐
    MOV     R3,  #GRID         ;// 预装载网格颜色值
    BX      LR
;//=============================================================================
; void Buld_0(R4 = u16* pCol)   // 建立外沿列缓冲区的背景数据 Used: R3-R5
;//=============================================================================
Buld_0
    MOV     R3,  #BACKGROUND   ;// 背景颜色
    B       Fill_Base
;//=============================================================================
; void Buld_2(R4 = u16* pCol)   // 建立格内列缓冲区的背景数据 Used: R3-R6
;//=============================================================================
Buld_2
    MOV     R6,  LR
    MOV     R3,  #BACKGROUND   ;// 背景颜色
    BL      Fill_Base
    MOV     R5,  #400
    STRH    R3,  [R4, R5]      ;// 上边线
    STRH    R3,  [R4]          ;// 下边线
    BX      R6
;//=============================================================================
; void Buld_3(R4 = u16* pCol)   // 建立格点列缓冲区的背景数据 Used: R3-R6
;//=============================================================================
Buld_3
    MOV     R6,  LR
    MOV     R3,  #BACKGROUND   ;// 背景颜色
    BL      Fill_Base
    MOV     R5,  #450
Loop3
    SUBS    R5,  R5, #50
    STRH    R3,  [R4, R5]      ;// 每25行画1格点
    BNE     Loop3
    BX      R6
;//=============================================================================
; void Buld_4(R4 = u16* pCol)   // 建立格线列缓冲区的背景数据
;//=============================================================================
Buld_4
    MOV     R6,  LR
    MOV     R3,  #BACKGROUND   ;// 背景颜色
    BL      Fill_Base
    MOV     R5,  #410
Loop7
    SUBS    R5,  R5, #10
    STRH    R3,  [R4, R5]      ;// 每5行画1格点
    BNE     Loop7
    BX      R6
;//=============================================================================
; void Buld_1(R4 = u16* pCol)   // 建立边线列缓冲区的背景数据 Used: R3-R6
;//=============================================================================
Buld_1
    MOV     R6,  LR
    LDR     R3,  =GRID_COLOR   ;// 为提高传输效率，取32bits格线颜色
    BL      Fill_Base
    MOV     R3,  #0
    MOV     R5,  #402
    STRH    R3,  [R4, R5]      ;// 上边线外留空白
    STRH    R3,  [R4, #-2]     ;// 下边线外留空白
    BX      R6
;//=============================================================================
;// R0:pDat, R1:pTab, R2:Col, R3:Tmp, R4:pBuf, R5:WR , R6:LCD, R7:Ctrl, R8:RS
;//=============================================================================
; void Send_LCD(u16* pBuf, u16 Row) // 从缓冲区传送一列数据到LCD Used: R3-R8
;//=============================================================================
    IMPORT    __SendPixels
    IMPORT    __Set_Posi
//    IMPORT    __Set_Pixel
Send_LCD
    MOV     R5,  SP
    PUSH    {R0-R3, LR}
    MOVS    R0, R2
    MOVS    R1, #WINDOW_Y0
    BL      __Set_Posi
    MOV     R1,  #203
    MOV     R0,  R5
    BL      __SendPixels
    POP     {R0-R3, PC}
/*    
Send_LCD
    MOV     R5,  SP
    PUSH    {R0-R3, LR}
    MOVS    R0, R2
    MOVS    R1, #WINDOW_Y0
    BL      __Set_Posi
    MOV     R4,  #203
Send_LCD_Loop
    LDRH    R0,  [R5],#2
    BL      __Set_Pixel
    SUBS    R4,  R4, #1
    BNE     Send_LCD_Loop
    POP     {R0-R3, PC}
*/    
;//=============================================================================

   END

;******************************* END OF FILE ***********************************


