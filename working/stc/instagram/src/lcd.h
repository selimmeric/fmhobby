void lcd_initial (void);
void dsp_single_colour(DH,DL);
void dsp_image(void);
void LCD_SetPos(unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1);
void LCD_CS(unsigned char value);
void dsp_single_colour_ex(unsigned char index,DH,DL);
void lcd_init(unsigned char index);
void dsp_image_ex(unsigned char index, unsigned char image_index);
void LCD_SetPos_ex(unsigned char index,unsigned int x0,unsigned int y0,unsigned int x1,unsigned int y1);


//void  delay(uint t);
