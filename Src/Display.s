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

BACKGROUND    = 0x00                 ;// ������ɫ
GRID          = 0x7BEF               ;// ���屳�������ߵ���ɫ
GRID_COLOR    = 0x7BEF7BEF           ;// 32bits �����ߵ���ɫ
WINDOW_Y0     = 19                   ;// ������ʾ������ʼ�߶�

;//=============================================================================
;//                 ������ʾ��ػ�������ӳ���
;//=============================================================================
;// void DrawWindow(u16* pBuf, u16* pTab)
;// R0:pDat, R1:pTab, R2:Col, R3:Var, R4:pCol, R5:Cnt, R6:Tmp,
;// R0:pBuf, R1:pTab, R2:Col, R3:LCD, R4:Ctrl, R5:WR,  R6:Tmp, R7:Cnt, R8:RS
;//=============================================================================
    EXPORT __DrawWindow
__DrawWindow
    PUSH    {R4-R12,LR}
    SUB     SP,  SP, #408          ;// Ԥ�� 204*2 Bytes �����ݻ�����
    MOV     R2,  #0                ;// �м�����ʼֵ
    ADD     R10, R0, #900          ;// �����ڻ�����ָ���ʼֵ
    LDRH    R11, [R1, #60+4*2]     ;// �������ڵ�ˮƽ��ʼλ��
    LDRH    R12, [R1, #60+5*2]     ;// �������ڵ�ˮƽ���
    ADD     R12, R11, R12          ;// �������ڵ�ˮƽ����λ��

;//----------- ������ -----------//
Draw_Loop
    CMP     R2,  #0
    ITT     EQ
    BLEQ    Buld_0                 ;// ���������л������ı�������
    BEQ     Draw_Wave
    CMP     R2,  #302
    ITT     EQ
    BLEQ    Buld_0                 ;// ���������л������ı�������
    BEQ     Draw_Wave

    CMP     R2,  #1
    ITT     EQ
    BLEQ    Buld_1                 ;// ���������л������ı�������
    BEQ     Draw_Wave
    MOV     R3,  #301
    CMP     R2,  R3
    ITT     EQ
    BLEQ    Buld_1                 ;// ���������л������ı�������
    BEQ     Draw_Wave

    SUB     R3,  R2, #1
    MOV     R6,  #25
    UDIV    R5,  R3, R6
    MUL     R5,  R5, R6
    SUBS    R5,  R3, R5
    ITT     EQ
    BLEQ    Buld_4                 ;// ���������л������ı�������
    BEQ     Draw_Wave

    MOV     R6,  #5
    UDIV    R5,  R3, R6
    MUL     R5,  R5, R6
    SUBS    R5,  R3, R5
    ITT     EQ
    BLEQ    Buld_3                 ;// ��������л������ı�������
    BEQ     Draw_Wave
    BL      Buld_2                 ;// ���������л������ı�������

;//--------- ���������� ---------//
Draw_Wave
    CMP     R2,  #3                ;// ��3~299
    BCC     Horizontal
    CMP     R2,  #300
    BCS     Horizontal

    LDRH    R3,  [R1, #3*2]        ;// ȡCH_3�������ߵ�������־
    TST     R3,  #4
    ITTTT   EQ
    MOVEQ   R3,  #40+3*2           ;// R3 = CH_3����������ɫ��ƫ��
    LDRHEQ  R4,  [R1, #20+3*2]     ;// R4 = CH_3�������ߵĻ���λ��
    MOVEQ   R8,  #8                ;// R8 = CH_3�������ݵ�����λ
    BLEQ    Draw_Digit

    LDRH    R3,  [R1, #2*2]        ;// ȡCH_2�������ߵ�������־
    TST     R3,  #4
    ITTTT   EQ
    MOVEQ   R3,  #40+2*2           ;// R3 = CH_2����������ɫ��ƫ��
    LDRHEQ  R4,  [R1, #20+2*2]     ;// R4 = CH_2�������ߵĻ���λ��
    MOVEQ   R8,  #4                ;// R8 = CH_2�������ݵ�����λ
    BLEQ    Draw_Digit

    LDRH    R3,  [R1, #1*2]        ;// ȡCH_1�������ߵ�������־
    TST     R3,  #4
    ITTT    EQ
    MOVEQ   R3,  #40+1*2           ;// R3 = CH_1����������ɫ��ƫ��
    ADDEQ   R4,  R0, #1*300
    BLEQ    Draw_Analog

    LDRH    R3,  [R1, #0*2]        ;// ȡCH_0�������ߵ�������־
    TST     R3,  #4
    ITTT    EQ
    MOVEQ   R3,  #40+0*2           ;// R3 = CH_0����������ɫ��ƫ��
    ADDEQ   R4,  R0, #0*300
    BLEQ    Draw_Analog

;//------- ��ˮƽ�����α� -------//
Horizontal
    CMP     R2,  #0
    ITT     EQ
    BLEQ    Cursor_0               ;// �����л��α�˵�
    BEQ     Vertical
    CMP     R2,  #302
    ITT     EQ
    BLEQ    Cursor_0               ;// �����л��α�˵�
    BEQ     Vertical

    CMP     R2,  #1
    ITT     EQ
    BLEQ    Cursor_1               ;// �����л��α�˵�
    BEQ     Vertical
    MOV     R5,  #301
    CMP     R2,  R5
    ITT     EQ
    BLEQ    Cursor_1               ;// �����л��α�˵�
    BEQ     Vertical

    CMP     R2,  #2
    ITT     EQ
    BLEQ    Cursor_2               ;// �����л��α�˵�
    BEQ     Vertical
    CMP     R2,  #300
    IT      EQ
    BLEQ    Cursor_2               ;// �����л��α�˵�
    BEQ     Vertical
    BL      Cursor_3               ;// �����л��α���

;//------- ����ֱ�����α� -------//
Vertical
    BL      Cursor_4

;//--------- ���������� ---------//

    LDRH    R3,  [R1, #60+8*2]     ;// ȡ�������ڵ�������־
    TST     R3,  #1
    BNE     Send
    CMP     R2,  R11               ;// �жϵ��������д���ʼ
    BLT     Send
    CMP     R2,  R12               ;// �жϵ��������д������
    IT      LT
    BLLT    Draw_Pop               ;// �м����ڵ���������

;//--------- ��ʾ������ ---------//
Send
    BL      Send_LCD               ;// �ӻ���������һ�����ݵ�LCD
    CMP     R2,  #302
    ITT     NE
    ADDNE   R2,  R2, #1
    BNE     Draw_Loop              ;// ������1����ʾ����

    ADD     SP,  SP, #408
    POP     {R4-R12,PC}
;//=============================================================================
; Draw_Pop(R2:Col, R10:pPop, R11:By0, R12:By1)// ���������� Used: R5-R7
;//=============================================================================
Draw_Pop
    MOV     R7,  #0x10001            ;// ��ʼλ��

    LDRH    R5,  [R1, #60+6*2]     ;// ȡ�������ڵĴ�ֱ��ʼλ��
    LDRH    R6,  [R1, #60+7*2]     ;// ȡ�������ڵĴ�ֱ�߶�

    ADD     R3,  R11, #0           ;// �������ڵ�0��Բ�Ǵ���
    CMP     R2,  R3
    ITTT    EQ
    ADDEQ   R5,  R5, #6
    SUBEQ   R6,  R6, #12
    MOVEQ   R7,  #0x80008

    ADD     R3,  R11, #1           ;// �������ڵ�1��Բ�Ǵ���
    CMP     R2,  R3
    ITTT    EQ
    ADDEQ   R5,  R5, #4
    SUBEQ   R6,  R6, #8
    MOVEQ   R7,  #0x40004

    ADD     R3,  R11, #2           ;// �������ڵ�2��Բ�Ǵ���
    CMP     R2,  R3
    ITTT    EQ
    ADDEQ   R5,  R5, #2
    SUBEQ   R6,  R6, #4
    MOVEQ   R7,  #0x20002

    SUB     R3,  R12, #3           ;// �������ڵ�n-3��Բ�Ǵ���
    CMP     R2,  R3
    ITTT    EQ
    ADDEQ   R5,  R5, #2
    SUBEQ   R6,  R6, #4
    MOVEQ   R7,  #0x20002

    SUB     R3,  R12, #2           ;// �������ڵ�n-2��Բ�Ǵ���
    CMP     R2,  R3
    ITTT    EQ
    ADDEQ   R5,  R5, #4
    SUBEQ   R6,  R6, #8
    MOVEQ   R7,  #0x40004

    SUB     R3,  R12, #1           ;// �������ڵ�n-1��Բ�Ǵ���
    CMP     R2,  R3
    ITTT    EQ
    ADDEQ   R5,  R5, #6
    SUBEQ   R6,  R6, #12
    MOVEQ   R7,  #0x80008

    ADD     R4,  SP, R5
    LDRH    R8,  [R1, #60+9*2]     ;// ȡ�������ڵı�����ɫ
    LDRH    R9,  [R1, #60+10*2]    ;// ȡ�������ڵ�ǰ����ɫ
    LDRH    R3,  [R10], #2         ;// ȡ˫�ֽڴ�ֱ�߶�����(��˫�ֽ�����������)

    CMP     R8,  #0
    BEQ     Pop_Loop_Penetrate

Pop_Loop
    TST     R3,  R7
    ITE     NE
    STRHNE  R9,  [R4], #2          ;// ��ǰ����
    STRHEQ  R8,  [R4], #2          ;// ��������
    SUBS    R6,  R6, #2
    IT      EQ
    BXEQ    LR                     ;// ������ʾ���
    LSLS    R7,  R7, #1
    ITT     CS
    LDRHCS  R3,  [R10], #2         ;// ����װ��˫�ֽڴ�ֱ�߶�����
    MOVCS   R7,  #0x10001
    B       Pop_Loop

Pop_Loop_Penetrate
    TST     R3,  R7
    ITE      NE
    STRHNE  R9,  [R4], #2          ;// ��ǰ����
    ADDEQ   R4,  R4, #2
    SUBS    R6,  R6, #2
    IT      EQ
    BXEQ    LR                     ;// ������ʾ���
    LSLS    R7,  R7, #1
    ITT     CS
    LDRHCS  R3,  [R10], #2         ;// ����װ��˫�ֽڴ�ֱ�߶�����
    MOVCS   R7,  #0x10001
    B       Pop_Loop_Penetrate

;//=============================================================================
; Draw_Digit(R0:pDat, R2:Col, R3:Color, R4:Posi, R8:Mask)// �����ֲ��� Used: R5-R7
;//=============================================================================
Draw_Digit
    ADD     R7,  R0, #600          ;// ָ�붨�������źŻ�����
    ADD     R7,  R7, R2
    LDRB    R5,  [R7]              ;// ȡ��ǰ�в����������� n1
    LDRB    R6,  [R7, #-1]         ;// ȡ��һ�в����������� n0

    LSR     R4,  R4, #1
    ANDS    R5,  R5, R8            ;// ��������
    ITE     EQ
    MOVEQ   R5,  R4                ;// ȷ�����
    ADDNE   R5,  R4, #30

    ANDS    R6,  R6, R8            ;// ��������
    ITE     EQ
    MOVEQ   R4,  R4                ;// ȷ�����
    ADDNE   R4,  R4, #30

    B       Analog0
;//=============================================================================
; Draw_Analog(R2:Col, R3:Color, R4:pDat)// ��ģ�Ⲩ������  Used: R3-R7
;//=============================================================================
Draw_Analog1
    ADD     R4,  R4, R2
    LDRB    R5,  [R4]              ;// ȡ��ǰ�в����������� n1
    LDRB    R4,  [R4, #-1]         ;// ȡ��һ�в����������� n0
    ADD     R5,  R5, #50
    ADD     R4,  R4, #50
    B       Analog0
Draw_Analog
    ADD     R4,  R4, R2
    LDRB    R5,  [R4]              ;// ȡ��ǰ�в����������� n1
    LDRB    R4,  [R4, #-1]         ;// ȡ��һ�в����������� n0
Analog0

    CMP     R5,  #255              ;// �϶˽�β
    IT      EQ
    BXEQ    LR
    CMP     R5,  #0                ;// �¶˽�β
    IT      EQ
    BXEQ    LR
    CMP     R5,  #255              ;// �϶˽�β
    IT      EQ
    BXEQ    LR
    CMP     R5,  #0                ;// �¶˽�β
    IT      EQ
    BXEQ    LR

    CMP     R5,  R4
    ITTEE   CS                     ;// R5 = | n1 - n0 |
    MOVCS   R6,  R4
    SUBCS   R5,  R5, R4
    MOVCC   R6,  R5                ;// n1, n0 ����С���� R6
    SUBCC   R5,  R4, R5

    CMP     R6,  #199              ;// ����㳬������
    IT      GT
    BXGT    LR
    ADD     R4,  R5, R6
    CMP     R4,  #199              ;// �����������޷�
    IT      GT
    RSBGT   R5,  R6, #199
    BGT     Analog2

    CMP     R4,  #3                ;// ����㳬������
    IT      LT
    BXLT    LR
    CMP     R6,  #3                ;// �����������޷�
    ITT     LT
    MOVLT   R6,  #3
    SUBLT   R5,  R4, #3
    BLT     Analog2

    CMP     R5,  #0                ;// ˮƽ�߼Ӵ�
    ITT     EQ
    SUBEQ   R6,  R6, #1
    ADDEQ   R5,  R5, #2

Analog2
    CMP     R5,  #20                ;// ѡ����ɫ
    IT      GE
    ADDGE   R3,  R3, #20
    LDRH    R3,  [R1, R3]

    LSL     R6,  R6, #1
    ADD     R6,  SP, R6            ;// ȷ����ʾλ��
Analog3
    STRH    R3,  [R6], #2          ;// �����ε�
    SUBS    R5,  R5, #1
    BCS     Analog3

Analog9
    BX      LR
;//=============================================================================
; Cursor_4(R1:pTab, R2:Col)// �����л��α�˵�  Used: R3-R8
;//=============================================================================
Cursor_4
    MOV     R3,  #7                ;// 7~9��Ϊ��ֱ�����α�
Cursor40
    MOV     R4,  SP
    LDRH    R5,  [R1, R3, LSL #1]  ;// ȡ���α������������־
    TST     R5,  #1
    BEQ     Cursor41               ;// ���α�����ת
    B       Cursor49               ;// ���α�����ת
Cursor41
    ADD     R1,  R1, #20
    LDRH    R5,  [R1, R3, LSL #1]  ;// ȡ���α��������ʾλ��
    ADD     R1,  R1, #20
    LDRH    R6,  [R1, R3, LSL #1]  ;// ȡ���α��������ʾ��ɫ
    SUB     R1,  R1, #40           ;// �ָ�������ָ��

    SUB     R8,  R5, #2
    CMP     R2,  R8                ;// �����������α�˵�
    BNE     Cursor42
    STRH    R6,  [R4]              ;// ����������
    ADD     R4,  R4, #404
    STRH    R6,  [R4]              ;// ����������
    B       Cursor49
Cursor42
    ADD     R8,  R8, #1
    CMP     R2,  R8                ;// ����������α�˵�
    BNE     Cursor43
    STRH    R6,  [R4], #2          ;// ����������
    STRH    R6,  [R4]              ;// �����±���
    ADD     R4,  R4, #400
    STRH    R6,  [R4], #2          ;// �����ϱ���
    STRH    R6,  [R4]              ;// ����������
    B       Cursor49
Cursor43
    ADD     R8,  R8, #1
    CMP     R2,  R8                ;// ���α�˵�, ���α���
    BNE     Cursor45
    STRH    R6,  [R4], #2          ;// ����������
    STRH    R6,  [R4], #2          ;// ���±��߸�
    STRH    R6,  [R4]              ;// ����������
    ADD     R4,  SP, #400
    STRH    R6,  [R4], #2          ;// ����������
    STRH    R6,  [R4], #2          ;// ���ϱ���
    STRH    R6,  [R4]              ;// ����������

    LDRH    R5,  [R1, R3, LSL #1]  ;// ȡ���α������������־
    TST     R5,  #2
    BNE     Cursor45               ;// ���α���������ת
    MOV     R4,  SP
    ADD     R7,  R4, #400
Cursor44
    STRH    R6,  [R4], #8          ;// ���α���
    CMP     R7,  R4
    BCS     Cursor44
    B       Cursor49
Cursor45
    ADD     R8,  R8, #1
    CMP     R2,  R8                ;// ���ұ������α�˵�
    BNE     Cursor46
    STRH    R6,  [R4], #2          ;// ����������
    STRH    R6,  [R4]              ;// �����±���
    ADD     R4,  R4, #400
    STRH    R6,  [R4], #2          ;// �����ϱ���
    STRH    R6,  [R4]              ;// ����������
    B       Cursor49
Cursor46
    ADD     R8,  R8, #1
    CMP     R2,  R8                ;// �����������α�˵�
    BNE     Cursor49
    STRH    R6,  [R4]              ;// ����������
    ADD     R4,  R4, #404
    STRH    R6,  [R4]              ;// ����������
Cursor49
    ADD     R3,  R3, #1
    CMP     R3,  #10
    BNE     Cursor40
    BX      LR
;//=============================================================================
; Cursor_3(R1:pTab, R2:Col)// ���������α���  Used: R3-R6
;//=============================================================================
Cursor_3
    MOV     R3,  #6                ;// 0~6��Ϊˮƽ�����α�
    MOV     R4,  SP
Cursor31
    LDRH    R5,  [R1, R3, LSL #1]  ;// ȡ���α������������־
    TST     R5,  #2
    BNE     Cursor32               ;// ���α���������ת
    SUB     R5,  R2, #1
    ANDS    R5,  R5, #3
    BNE     Cursor32               ;// ���α�������λ����ת
    ADD     R1,  R1, #20
    LDRH    R5,  [R1, R3, LSL #1]  ;// ȡ���α��������ʾλ��
    ADD     R4,  SP, R5
    ADD     R1,  R1, #20
    LDRH    R6,  [R1, R3, LSL #1]  ;// ȡ���α��������ʾ��ɫ
    STRH    R6,  [R4]              ;// ���α���
    SUB     R1,  R1, #40           ;// �ָ�������ָ��
Cursor32
    SUBS    R3,  #1
    BPL     Cursor31               ;// ������1���α�˵�
    BX      LR
;//=============================================================================
; Cursor_0(R1:pTab, R2:Col)// ���������α�˵�  Used: R3-R6
;//=============================================================================
Cursor_0
    MOV     R3,  #6                ;// 0~6��Ϊˮƽ�����α�
    MOV     R4,  SP
Cursor01
    LDRH    R5,  [R1, R3, LSL #1]  ;// ȡ���α������������־
    TST     R5,  #1
    BNE     Cursor02               ;// ���α�˵�������ת
    ADD     R1,  R1, #20
    LDRH    R5,  [R1, R3, LSL #1]  ;// ȡ���α��������ʾλ��
    ADD     R4,  SP, R5
    ADD     R1,  R1, #20
    LDRH    R6,  [R1, R3, LSL #1]  ;// ȡ���α��������ʾ��ɫ
    SUB     R4,  R4, #4
    STRH    R6,  [R4], #2
    STRH    R6,  [R4], #2
    STRH    R6,  [R4], #2          ;// ���������α�˵�
    STRH    R6,  [R4], #2
    STRH    R6,  [R4], #2
    SUB     R1,  R1, #40           ;// �ָ�������ָ��
Cursor02
    SUBS    R3,  #1
    BPL     Cursor01               ;// ������1���α�˵�
    BX      LR
;//=============================================================================
; Cursor_1(R1:pTab, R2:Col)// ���������α�˵�  Used: R3-R6
;//=============================================================================
Cursor_1
    MOV     R3,  #6                ;// 0~6��Ϊˮƽ�����α�
    MOV     R4,  SP
Cursor11
    LDRH    R5,  [R1, R3, LSL #1]  ;// ȡ���α������������־
    TST     R5,  #1
    BNE     Cursor12               ;// ���α�˵�������ת
    ADD     R1,  R1, #20
    LDRH    R5,  [R1, R3, LSL #1]  ;// ȡ���α��������ʾλ��
    ADD     R4,  SP, R5
    ADD     R1,  R1, #20
    LDRH    R6,  [R1, R3, LSL #1]  ;// ȡ���α��������ʾ��ɫ
    SUB     R4,  R4, #2
    STRH    R6,  [R4], #2
    STRH    R6,  [R4], #2          ;// ���������α�˵�
    STRH    R6,  [R4], #2
    SUB     R1,  R1, #40           ;// �ָ�������ָ��
Cursor12
    SUBS    R3,  #1
    BPL     Cursor11               ;// ������1���α�˵�
    BX      LR
;//=============================================================================
; Cursor_2(R1:pTab, R2:Col)// ���������α�˵�  Used: R3-R6
;//=============================================================================
Cursor_2
    MOV     R3,  #6                ;// 0~6��Ϊˮƽ�����α�
    MOV     R4,  SP
Cursor21
    LDRH    R5,  [R1, R3, LSL #1]  ;// ȡ���α������������־
    TST     R5,  #1
    BNE     Cursor22               ;// ���α�˵�������ת
    ADD     R1,  R1, #20
    LDRH    R5,  [R1, R3, LSL #1]  ;// ȡ���α��������ʾλ��
    ADD     R4,  SP, R5
    ADD     R1,  R1, #20
    LDRH    R6,  [R1, R3, LSL #1]  ;// ȡ���α��������ʾ��ɫ
    STRH    R6,  [R4]              ;// ���������α�˵�
    SUB     R1,  R1, #40           ;// �ָ�������ָ��
Cursor22
    SUBS    R3,  #1
    BPL     Cursor21               ;// ������1���α�˵�
    BX      LR
;//=============================================================================
;// R0:pDat, R1:pTab, R2:Col, R3:Var, R4:pBuf, R5:Cnt, R6:Tmp,
;//=============================================================================
; void Fill_Base(R3 = u32 Color)// �л��������ɫ RET: R4+202 Used: R3-R5
;//=============================================================================
Fill_Base
    MOV     R4,  SP
    MOV     R5,  #102          ;// 204��/2 ��408 Bytes
Fill_Loop_0
    STR     R3,  [R4], #4      ;// ������ɺ�ָ���4
    SUBS    R5,  #1
    BNE     Fill_Loop_0
    ADD     R4,  SP, #2        ;// ָ�����
    MOV     R3,  #GRID         ;// Ԥװ��������ɫֵ
    BX      LR
;//=============================================================================
; void Buld_0(R4 = u16* pCol)   // ���������л������ı������� Used: R3-R5
;//=============================================================================
Buld_0
    MOV     R3,  #BACKGROUND   ;// ������ɫ
    B       Fill_Base
;//=============================================================================
; void Buld_2(R4 = u16* pCol)   // ���������л������ı������� Used: R3-R6
;//=============================================================================
Buld_2
    MOV     R6,  LR
    MOV     R3,  #BACKGROUND   ;// ������ɫ
    BL      Fill_Base
    MOV     R5,  #400
    STRH    R3,  [R4, R5]      ;// �ϱ���
    STRH    R3,  [R4]          ;// �±���
    BX      R6
;//=============================================================================
; void Buld_3(R4 = u16* pCol)   // ��������л������ı������� Used: R3-R6
;//=============================================================================
Buld_3
    MOV     R6,  LR
    MOV     R3,  #BACKGROUND   ;// ������ɫ
    BL      Fill_Base
    MOV     R5,  #450
Loop3
    SUBS    R5,  R5, #50
    STRH    R3,  [R4, R5]      ;// ÿ25�л�1���
    BNE     Loop3
    BX      R6
;//=============================================================================
; void Buld_4(R4 = u16* pCol)   // ���������л������ı�������
;//=============================================================================
Buld_4
    MOV     R6,  LR
    MOV     R3,  #BACKGROUND   ;// ������ɫ
    BL      Fill_Base
    MOV     R5,  #410
Loop7
    SUBS    R5,  R5, #10
    STRH    R3,  [R4, R5]      ;// ÿ5�л�1���
    BNE     Loop7
    BX      R6
;//=============================================================================
; void Buld_1(R4 = u16* pCol)   // ���������л������ı������� Used: R3-R6
;//=============================================================================
Buld_1
    MOV     R6,  LR
    LDR     R3,  =GRID_COLOR   ;// Ϊ��ߴ���Ч�ʣ�ȡ32bits������ɫ
    BL      Fill_Base
    MOV     R3,  #0
    MOV     R5,  #402
    STRH    R3,  [R4, R5]      ;// �ϱ��������հ�
    STRH    R3,  [R4, #-2]     ;// �±��������հ�
    BX      R6
;//=============================================================================
;// R0:pDat, R1:pTab, R2:Col, R3:Tmp, R4:pBuf, R5:WR , R6:LCD, R7:Ctrl, R8:RS
;//=============================================================================
; void Send_LCD(u16* pBuf, u16 Row) // �ӻ���������һ�����ݵ�LCD Used: R3-R8
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


