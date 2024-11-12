#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/gpio.h>


/* Variables for device and device class */
#define DRIVER_OLED_NAME "oled_ssd1306"
#define DRIVER_CLASS "project_os"
#define DRIVER_DHT11_NAME "dht11"
#define GPIO_DHT11_PIN 516

#define I2C_BUS_AVAILABLE 1
#define SLAVE_DEVICE_NAME "OLED_SSD1306"
#define OLED_SLAVE_ADDRESS 0x3C
#define SSD1306_MAX_SEG    128
#define SSD1306_MAX_LINE   7
#define SSD1306_DEF_FONT_SIZE 5

static struct i2c_adapter * oled_i2c_adapter = NULL;
static struct i2c_client * oled_i2c_client = NULL;

static uint8_t SSD1306_LineNum   = 0;
static uint8_t SSD1306_CursorPos = 0;
static uint8_t SSD1306_FontSize  = SSD1306_DEF_FONT_SIZE;

/* Variables for Device and Deviceclass*/
static dev_t myDeviceNrOled;
static dev_t myDeviceNrDht11;
static struct class *myClass;
static struct cdev myDeviceOled;
static struct cdev myDeviceDht11;
/*
** Array Variable to store the letters.
*/
static const unsigned char SSD1306_font[][SSD1306_DEF_FONT_SIZE]=
{
    {0x00, 0x00, 0x00, 0x00, 0x00},   // space
    {0x00, 0x00, 0x2f, 0x00, 0x00},   // !
    {0x00, 0x07, 0x00, 0x07, 0x00},   // "
    {0x14, 0x7f, 0x14, 0x7f, 0x14},   // #
    {0x24, 0x2a, 0x7f, 0x2a, 0x12},   // $
    {0x23, 0x13, 0x08, 0x64, 0x62},   // %
    {0x36, 0x49, 0x55, 0x22, 0x50},   // &
    {0x00, 0x05, 0x03, 0x00, 0x00},   // '
    {0x00, 0x1c, 0x22, 0x41, 0x00},   // (
    {0x00, 0x41, 0x22, 0x1c, 0x00},   // )
    {0x14, 0x08, 0x3E, 0x08, 0x14},   // *
    {0x08, 0x08, 0x3E, 0x08, 0x08},   // +
    {0x00, 0x00, 0xA0, 0x60, 0x00},   // ,
    {0x08, 0x08, 0x08, 0x08, 0x08},   // -
    {0x00, 0x60, 0x60, 0x00, 0x00},   // .
    {0x20, 0x10, 0x08, 0x04, 0x02},   // /
    {0x3E, 0x51, 0x49, 0x45, 0x3E},   // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00},   // 1
    {0x42, 0x61, 0x51, 0x49, 0x46},   // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31},   // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10},   // 4
    {0x27, 0x45, 0x45, 0x45, 0x39},   // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30},   // 6
    {0x01, 0x71, 0x09, 0x05, 0x03},   // 7
    {0x36, 0x49, 0x49, 0x49, 0x36},   // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E},   // 9
    {0x00, 0x36, 0x36, 0x00, 0x00},   // :
    {0x00, 0x56, 0x36, 0x00, 0x00},   // ;
    {0x08, 0x14, 0x22, 0x41, 0x00},   // <
    {0x14, 0x14, 0x14, 0x14, 0x14},   // =
    {0x00, 0x41, 0x22, 0x14, 0x08},   // >
    {0x02, 0x01, 0x51, 0x09, 0x06},   // ?
    {0x32, 0x49, 0x59, 0x51, 0x3E},   // @
    {0x7C, 0x12, 0x11, 0x12, 0x7C},   // A
    {0x7F, 0x49, 0x49, 0x49, 0x36},   // B
    {0x3E, 0x41, 0x41, 0x41, 0x22},   // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C},   // D
    {0x7F, 0x49, 0x49, 0x49, 0x41},   // E
    {0x7F, 0x09, 0x09, 0x09, 0x01},   // F
    {0x3E, 0x41, 0x49, 0x49, 0x7A},   // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F},   // H
    {0x00, 0x41, 0x7F, 0x41, 0x00},   // I
    {0x20, 0x40, 0x41, 0x3F, 0x01},   // J
    {0x7F, 0x08, 0x14, 0x22, 0x41},   // K
    {0x7F, 0x40, 0x40, 0x40, 0x40},   // L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F},   // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F},   // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E},   // O
    {0x7F, 0x09, 0x09, 0x09, 0x06},   // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E},   // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46},   // R
    {0x46, 0x49, 0x49, 0x49, 0x31},   // S
    {0x01, 0x01, 0x7F, 0x01, 0x01},   // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F},   // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F},   // V
    {0x3F, 0x40, 0x38, 0x40, 0x3F},   // W
    {0x63, 0x14, 0x08, 0x14, 0x63},   // X
    {0x07, 0x08, 0x70, 0x08, 0x07},   // Y
    {0x61, 0x51, 0x49, 0x45, 0x43},   // Z
    {0x00, 0x7F, 0x41, 0x41, 0x00},   // [
    {0x55, 0xAA, 0x55, 0xAA, 0x55},   // Backslash (Checker pattern)
    {0x00, 0x41, 0x41, 0x7F, 0x00},   // ]
    {0x04, 0x02, 0x01, 0x02, 0x04},   // ^
    {0x40, 0x40, 0x40, 0x40, 0x40},   // _
    {0x00, 0x03, 0x05, 0x00, 0x00},   // `
    {0x20, 0x54, 0x54, 0x54, 0x78},   // a
    {0x7F, 0x48, 0x44, 0x44, 0x38},   // b
    {0x38, 0x44, 0x44, 0x44, 0x20},   // c
    {0x38, 0x44, 0x44, 0x48, 0x7F},   // d
    {0x38, 0x54, 0x54, 0x54, 0x18},   // e
    {0x08, 0x7E, 0x09, 0x01, 0x02},   // f
    {0x18, 0xA4, 0xA4, 0xA4, 0x7C},   // g
    {0x7F, 0x08, 0x04, 0x04, 0x78},   // h
    {0x00, 0x44, 0x7D, 0x40, 0x00},   // i
    {0x40, 0x80, 0x84, 0x7D, 0x00},   // j
    {0x7F, 0x10, 0x28, 0x44, 0x00},   // k
    {0x00, 0x41, 0x7F, 0x40, 0x00},   // l
    {0x7C, 0x04, 0x18, 0x04, 0x78},   // m
    {0x7C, 0x08, 0x04, 0x04, 0x78},   // n
    {0x38, 0x44, 0x44, 0x44, 0x38},   // o
    {0xFC, 0x24, 0x24, 0x24, 0x18},   // p
    {0x18, 0x24, 0x24, 0x18, 0xFC},   // q
    {0x7C, 0x08, 0x04, 0x04, 0x08},   // r
    {0x48, 0x54, 0x54, 0x54, 0x20},   // s
    {0x04, 0x3F, 0x44, 0x40, 0x20},   // t
    {0x3C, 0x40, 0x40, 0x20, 0x7C},   // u
    {0x1C, 0x20, 0x40, 0x20, 0x1C},   // v
    {0x3C, 0x40, 0x30, 0x40, 0x3C},   // w
    {0x44, 0x28, 0x10, 0x28, 0x44},   // x
    {0x1C, 0xA0, 0xA0, 0xA0, 0x7C},   // y
    {0x44, 0x64, 0x54, 0x4C, 0x44},   // z
    {0x00, 0x10, 0x7C, 0x82, 0x00},   // {
    {0x00, 0x00, 0xFF, 0x00, 0x00},   // |
    {0x00, 0x82, 0x7C, 0x10, 0x00},   // }
    {0x00, 0x06, 0x09, 0x09, 0x06}    // ~ (Degrees)
};


/*
** OLED Function prototypes.
*/
static int  I2C_Write( unsigned char *buf, unsigned int len );
static void SSD1306_PrintChar( unsigned char c );
static void SSD1306_String( unsigned char *str );
static int  SSD1306_DisplayInit( void );
static void SSD1306_Fill( unsigned char data );
static void SSD1306_GoToNextLine( void );
static void SSD1306_SetCursor( uint8_t lineNo, uint8_t cursorPos );
static void SSD1306_Write(bool is_cmd, unsigned char data);
static int oled_probe(struct i2c_client *client);
static void oled_remove(struct i2c_client *client);
static ssize_t driver_oled_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs);




/*
** DHT11 Function prototypes.
*/
static int dht11_read_data(unsigned char *data);
static ssize_t driver_dht11_read(struct file *File,char *user_buffer,size_t count,loff_t *offs);





/*
** This function writes the data into the I2C client
**
**  Arguments:
**      buff -> buffer to be sent
**      len  -> Length of the data
**
*/
static int I2C_Write(unsigned char *buf, unsigned int len)
{
    int ret = i2c_master_send(oled_i2c_client, buf, len);
  return ret;
}



/*
** This function sends the command/data to the OLED.
**
**  Arguments:
**      is_cmd -> true = command, flase = data
**      data   -> data to be written
**
*/
static void SSD1306_Write(bool is_cmd, unsigned char data)
{
  unsigned char buf[2] = {0};
  int ret;

  /*
  ** First byte is always control byte. Data is followed after that.
  **
  ** There are two types of data in SSD_1306 OLED.
  ** 1. Command
  ** 2. Data
  **
  */
  if( is_cmd == true )
  {
      buf[0] = 0x00;
  }
  else
  {
      buf[0] = 0x40;
  }

  buf[1] = data;

  ret = I2C_Write(buf, 2);
}


/*
**  Arguments:
**      lineNo    -> Line Number
**      cursorPos -> Cursor Position
**
*/
static void SSD1306_SetCursor( uint8_t lineNo, uint8_t cursorPos )
{
  /* Move the Cursor to specified position only if it is in range */
  if((lineNo <= SSD1306_MAX_LINE) && (cursorPos < SSD1306_MAX_SEG))
  {
    SSD1306_LineNum   = lineNo;             // Save the specified line number
    SSD1306_CursorPos = cursorPos;          // Save the specified cursor position
    SSD1306_Write(true, 0x21);              // cmd for the column start and end address
    SSD1306_Write(true, cursorPos);         // column start addr
    SSD1306_Write(true, SSD1306_MAX_SEG-1); // column end addr
    SSD1306_Write(true, 0x22);              // cmd for the page start and end address
    SSD1306_Write(true, lineNo);            // page start addr
    SSD1306_Write(true, SSD1306_MAX_LINE);  // page end addr
  }
}


/*
** This function move the cursor to the next line.
**
**  Arguments:
**      none
**
*/
static void  SSD1306_GoToNextLine( void )
{
  /*
  ** Increment the current line number.
  ** roll it back to first line, if it exceeds the limit.
  */
  SSD1306_LineNum++;
  SSD1306_LineNum = (SSD1306_LineNum & SSD1306_MAX_LINE);
  SSD1306_SetCursor(SSD1306_LineNum,0); /* Finally move it to next line */
}



/*
** This function sends the single char to the OLED.
**
**  Arguments:
**      c   -> character to be written
**
*/
static void SSD1306_PrintChar(unsigned char c)
{
  uint8_t data_byte;
  uint8_t temp = 0;
  /*
  ** If we character is greater than segment len or we got new line charcter
  ** then move the cursor to the new line
  */
  if( (( SSD1306_CursorPos + SSD1306_FontSize ) >= SSD1306_MAX_SEG ) ||
      ( c == '\n' )
  )
  {
    SSD1306_GoToNextLine();
  }
  // print charcters other than new line
  if( c != '\n' )
  {

    c -= 0x20;  //or c -= ' ';
    do
    {
      data_byte= SSD1306_font[c][temp]; // Get the data to be displayed from LookUptable
      SSD1306_Write(false, data_byte);  // write data to the OLED
      SSD1306_CursorPos++;

      temp++;

    } while ( temp < SSD1306_FontSize);
    SSD1306_Write(false, 0x00);         //Display the data
    SSD1306_CursorPos++;
  }
}


/*
** This function sends the string to the OLED.
**
**  Arguments:
**      str   -> string to be written
**
*/
static void SSD1306_String(unsigned char *str)
{
  while(*str)
  {
    SSD1306_PrintChar(*str++);
  }
}



/*
**
**  Arguments:
**      data  -> Data to be filled in the OLED
**
*/
static void SSD1306_Fill(unsigned char data)
{
  unsigned int total  = 128 * 8;  // 8 pages x 128 segments x 8 bits of data
  unsigned int i      = 0;

  //Fill the Display
  for(i = 0; i < total; i++)
  {
      SSD1306_Write(false, data);
  }
}


/*
** This function sends the commands that need to used to Initialize the OLED.
**
**  Arguments:
**      none
**
*/
static int SSD1306_DisplayInit(void)
{
  msleep(100);               // delay
  /*
  ** Commands to initialize the SSD_1306 OLED Display
  */
  SSD1306_Write(true, 0xAE); // Entire Display OFF
  SSD1306_Write(true, 0xD5); // Set Display Clock Divide Ratio and Oscillator Frequency
  SSD1306_Write(true, 0x80); // Default Setting for Display Clock Divide Ratio and Oscillator Frequency that is recommended
  SSD1306_Write(true, 0xA8); // Set Multiplex Ratio
  SSD1306_Write(true, 0x3F); // 64 COM lines
  SSD1306_Write(true, 0xD3); // Set display offset
  SSD1306_Write(true, 0x00); // 0 offset
  SSD1306_Write(true, 0x40); // Set first line as the start line of the display
  SSD1306_Write(true, 0x8D); // Charge pump
  SSD1306_Write(true, 0x14); // Enable charge dump during display on
  SSD1306_Write(true, 0x20); // Set memory addressing mode
  SSD1306_Write(true, 0x00); // Horizontal addressing mode
  SSD1306_Write(true, 0xA1); // Set segment remap with column address 127 mapped to segment 0
  SSD1306_Write(true, 0xC8); // Set com output scan direction, scan from com63 to com 0
  SSD1306_Write(true, 0xDA); // Set com pins hardware configuration
  SSD1306_Write(true, 0x12); // Alternative com pin configuration, disable com left/right remap
  SSD1306_Write(true, 0x81); // Set contrast control
  SSD1306_Write(true, 0x80); // Set Contrast to 128
  SSD1306_Write(true, 0xD9); // Set pre-charge period
  SSD1306_Write(true, 0xF1); // Phase 1 period of 15 DCLK, Phase 2 period of 1 DCLK
  SSD1306_Write(true, 0xDB); // Set Vcomh deselect level
  SSD1306_Write(true, 0x20); // Vcomh deselect level ~ 0.77 Vcc
  SSD1306_Write(true, 0xA4); // Entire display ON, resume to RAM content display
  SSD1306_Write(true, 0xA6); // Set Display in Normal Mode, 1 = ON, 0 = OFF
  SSD1306_Write(true, 0x2E); // Deactivate scroll
  SSD1306_Write(true, 0xAF); // Display ON in normal mode

  //Clear the display
  SSD1306_Fill(0x00);
  return 0;
}

/*
** This function getting called when the slave has been found
** Note : This will be called only once when we load the driver.
*/
static int oled_probe(struct i2c_client *client)
{
  SSD1306_DisplayInit();
  //Set cursor
  SSD1306_SetCursor(3,34);
  //Write String to OLED
  SSD1306_String("HELLO WORLD!");

  pr_info("OLED Probed!!!\n");

  return 0;
}

/*
** This function getting called when the slave has been removed
** Note : This will be called only once when we unload the driver.
*/
static void oled_remove(struct i2c_client *client)
{
  SSD1306_Fill(0x00);	

  SSD1306_SetCursor(3,34);
  SSD1306_String("GOODBYE!");

  msleep(2000);

  //Set cursor
  SSD1306_SetCursor(0,0);
  //clear the display
  SSD1306_Fill(0x00);

  SSD1306_Write(true, 0xAE); // Entire Display OFF

  pr_info("OLED Removed!!!\n");
}


static const struct i2c_device_id oled_id[] = {
		{ SLAVE_DEVICE_NAME, 0 },
		{ }
};

static struct i2c_board_info oled_i2c_board_info = {
	I2C_BOARD_INFO(SLAVE_DEVICE_NAME, OLED_SLAVE_ADDRESS)
};
/*
** I2C driver Structure that has to be added to linux
*/
static struct i2c_driver oled_driver = {
  .driver = {
    .name   = SLAVE_DEVICE_NAME,
    .owner  = THIS_MODULE,
  },
  .probe          = oled_probe,
  .remove         = oled_remove,
  .id_table       = oled_id
};
MODULE_DEVICE_TABLE(i2c,oled_id);


/**
 * @brief Write data to buffer
 */
static ssize_t driver_oled_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
     char kbuf[128];

     if (count > sizeof(kbuf) - 1)
        return -EINVAL;
     if (copy_from_user(kbuf, user_buffer, count))
        return -EFAULT;


     SSD1306_SetCursor(0,0);
     SSD1306_Fill(0x00);
     SSD1306_String(kbuf);
     return count;

}
/* Map the file operations */
static struct file_operations fops_oled = {
	.owner = THIS_MODULE,
	.write = driver_oled_write
};

/**
 * This function reads data from dht11
 */

static int dht11_read_data(unsigned char *data){
        int i,j;
	int timeout;
	if(gpio_direction_output(GPIO_DHT11_PIN,1)){
		printk("Cannot set GPIO 4 to output!\n");
		return -1;
	}
	gpio_set_value(GPIO_DHT11_PIN,0);
	mdelay(20);
	gpio_set_value(GPIO_DHT11_PIN,1);
	udelay(50);
        if(gpio_direction_input(GPIO_DHT11_PIN)){
		printk("Cannot set gpio4 to input!\n");
	        return -1;
	}
	udelay(40);

	timeout = 1000;
	while(gpio_get_value(GPIO_DHT11_PIN)==0&&timeout--){
		udelay(1);
	}
	if(timeout==0){
		return -1;
	}
	timeout=1000;
	while(gpio_get_value(GPIO_DHT11_PIN)==1&&timeout--){
		udelay(1);
	}
	if(timeout==0){
		return -1;
	}

	printk("dht11 already sent data\n");
         for(i=0;i<5;i++){
	          data[i] = 0;
		  for(j=0;j<8;j++){
			 timeout = 1000;
                 	 while(gpio_get_value(GPIO_DHT11_PIN)==0&&timeout--){
				       udelay(1);
			 }
	                  udelay(50);
		          if(gpio_get_value(GPIO_DHT11_PIN)==1){
				    data[i] |= (1<<(7-j));
			  }
			  timeout=1000;
			  while(gpio_get_value(GPIO_DHT11_PIN)==1&&timeout--){
				    udelay(1);
			   }
		  }
	  }

	return 0;
}

/**
 * Read data 
 */
static ssize_t driver_dht11_read(struct file *File,char *user_buffer,size_t count,loff_t *offs){
	unsigned char data[5];

	if(dht11_read_data(data)){
		return -EIO;
	}

	
	if(copy_to_user(user_buffer,data,count)){
		return EFAULT;
	}

	return count;
}

static struct file_operations fops_dht11 = {
	.owner = THIS_MODULE,
	.read = driver_dht11_read
};



/**
 * @brief function, which is called after loading module to kernel, do initialization there
 */
static int __init ModuleInit(void) {
	int ret = -1;
	printk("Hello Kernel\n");

	/* Allocate Device Nr */
	if (alloc_chrdev_region(&myDeviceNrOled, 0, 1, DRIVER_OLED_NAME) < 0) {
		printk("could not be allocated for oled!\n");
		return -1;
	}
	if (alloc_chrdev_region(&myDeviceNrDht11, 0, 1, DRIVER_DHT11_NAME) < 0) {
               printk("could not be allocated for dht11!\n");
               return -1;
	}
	
	/* Create Device Class */
	if ((myClass = class_create(DRIVER_CLASS)) == NULL) {
		printk("Device Class can not be created!\n");
		goto ClassError;
	}

	/* Create Device file */
	if (device_create(myClass, NULL, myDeviceNrOled, NULL, DRIVER_OLED_NAME) == NULL) {
		printk("Can not create device file oled_ssd1306!\n");
		goto FileError;
	}
	if(device_create(myClass,NULL,myDeviceNrDht11,NULL,DRIVER_DHT11_NAME) == NULL){
		printk("Can not create device file dht11!\n");
		goto FileError;
	}

	/* Initialize Device file */
	cdev_init(&myDeviceOled, &fops_oled);
	cdev_init(&myDeviceDht11, &fops_dht11);

	/* register device to kernel */
	if (cdev_add(&myDeviceOled, myDeviceNrOled, 1) == -1) {
		printk("Registering of device to kernel failed!\n");
		goto KernelError;
	}
	if(cdev_add(&myDeviceDht11,myDeviceNrDht11,1) == -1){
		printk("Registering of device to kernel failed!\n");
		goto KernelError;
	}
	if(gpio_request(GPIO_DHT11_PIN,"rpi-gpio-516")){
		printk("Can not allocate GPIO 4\n");
		goto AddError;
	}

	oled_i2c_adapter = i2c_get_adapter(I2C_BUS_AVAILABLE);

	if(oled_i2c_adapter != NULL) {
		oled_i2c_client = i2c_new_client_device(oled_i2c_adapter, &oled_i2c_board_info);
		if(oled_i2c_client != NULL) {
			if(i2c_add_driver(&oled_driver) != -1) {
				ret = 0;
			}
			else
				printk("Can't add driver...\n");
		}
		i2c_put_adapter(oled_i2c_adapter);
	}
	printk("OLED Driver added!\n");

		return ret;
AddError:
        gpio_free(GPIO_DHT11_PIN);
KernelError:
	device_destroy(myClass, myDeviceNrOled);
	device_destroy(myClass,myDeviceNrDht11);
FileError:
	class_destroy(myClass);
ClassError:
	unregister_chrdev(myDeviceNrOled, DRIVER_OLED_NAME);
	unregister_chrdev(myDeviceNrDht11,DRIVER_DHT11_NAME);
	return (-1);
}


static void __exit ModuleExit(void) {
	printk("Goodbye, Kernel!\n");
	gpio_free(GPIO_DHT11_PIN);
	i2c_unregister_device(oled_i2c_client);
	i2c_del_driver(&oled_driver);
	cdev_del(&myDeviceOled);
	cdev_del(&myDeviceDht11);
        device_destroy(myClass, myDeviceNrOled);
	device_destroy(myClass, myDeviceNrDht11);
        class_destroy(myClass);
        unregister_chrdev_region(myDeviceNrOled, 1);
	unregister_chrdev_region(myDeviceNrDht11, 1);
}
module_init(ModuleInit);
module_exit(ModuleExit);




MODULE_LICENSE("GPL");
MODULE_AUTHOR("TUAN");

