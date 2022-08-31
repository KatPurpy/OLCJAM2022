/*  GIMP header image file format (RGB): C:\_PurpsStash\PROGRAMMING\OLCJAM2022\assets\fire.h  */

const unsigned int fire_gradient_width = 256;
static unsigned int height = 1;

/*  Call this macro repeatedly.  After each use, the pixel data can be extracted  */

#define HEADER_PIXEL(data, pixel)                                                                                      \
	{                                                                                                                  \
		pixel[0] = (((data[0] - 33) << 2) | ((data[1] - 33) >> 4));                                                    \
		pixel[1] = ((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2));                                            \
		pixel[2] = ((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33)));                                                 \
		data += 4;                                                                                                     \
	}
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
static char* fire_gradient_data = "`0T]`0TZ`0TT`0TM`0TH`0TB`0T<`0T6`0T1`0T.`0T'`0T\"`0S\\`0SU`0SP`0SJ"
								  "`0SG`0SA`0S;_`S6`0S0`0S)`0S#`0R^`0R[`0RV`0RO`0NI`0RC`0R=`0R7`0R4"
								  "`0R/`0R(`0R#`0Q]`0QW`0QQ`0QK`0QI`0QB`0Q<`0Q6`0Q0`0Q+`0Q$`0E!`0A!"
								  "`0A!`01!`01!`0-!`0!!`0!!_`!!`/Q!`/Q!`/M!`/A!`/A!`/5!`/1!`/1!`/-!"
								  "`/!!`/!!`.Q!`.Q!`.Q!`.A!`.A!`.A!`.5!`.1!`.1!`.!!`.!!`.!!`-Q!`-A!"
								  "`-1!`-)!`-!!`,Q!`,A!`,9!`,1!`,!!`+Q!`+I!`+A!`+1!`+!!`*Y!`*M!`*A!"
								  "`*-!`*!!`)Q!`)I!`)A!`)1!`)!!`(Y!`(M!`(A!`(1!`()!`']!`'Q!`'A!`'5!"
								  "`'-!`'!!`&M!`&A!`&1!`&%!`%]!`%Q!`%A!`%1!`%)!`%!!`$Q!`$A!`$=!`$1!"
								  "`$!!`#Q!`#I!`#A!`#1!^#%!\\R]![\"Q!YBA!X\"1!VR)!U\"!!SQQ!R!A!PA9!NQ1!"
								  "MA!!L!!!JA!!I!!!HA!!G!!!EA!!E!!!CA!!AQ!!A!!!>Q!%;Q!)8Q!-5Q!12A!5"
								  "/Q!8,A!=)A!@&Q!E#A!H!!!I!!!B!!!@!!!:!!!3!!!!!!!!!!!!!!!!!!!!!!!!"
								  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
								  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
								  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
								  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
								  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
								  "";
#pragma GCC diagnostic pop