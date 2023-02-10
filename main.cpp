#include "mbed.h"
#include "SDBlockDevice.h"
#include "FATFileSystem.h"
#include "AudioPlayer.h"
#include "WaveAudioStream.h"

SDBlockDevice sd(MBED_CONF_SD_SPI_MOSI, MBED_CONF_SD_SPI_MISO, MBED_CONF_SD_SPI_CLK, MBED_CONF_SD_SPI_CS);
FATFileSystem fs("sd", &sd);

PwmOut aout(PB_0);

int main()
{
    sd.frequency(5000000);
    File file;
    if (file.open(&fs, "hobbi8.wav") != 0) {
        error("Could not open 'hobbi8.wav'\r\n");
    }
    WaveAudioStream song(&file);  //"song" is the audio data object
    // Check to see if file is a valid WAV file
    if(song.get_valid() == 0){
        error("ERROR: not valid WAV file\r\n");
    }
    // WAV file must be 8-bit
    if(song.get_bytes_per_sample() != 1){
        error("ERROR: WAV file not 1 bytes per sample (8-bit)\r\n");
    }
    uint8_t buffer[1500];
    int num_bytes_read;
    aout.period(1.0/16000.0);
    aout.write(0.5);
    printf("Playing Audio 'hobbi8.wav'\r\n");
    while(1){
        num_bytes_read = song.read(buffer, 1500);
        if(num_bytes_read == -1){
            aout.suspend();
            printf("Song Over\r\n");
            break;
        }
        for (int i = 0; i < num_bytes_read; i++) {   
            // printf("%d\r\n",buffer[i]<<8);  
            aout.write(buffer[i]/256.0);
            wait_us(125);
        }
    }
    song.close();//Close the WAV file
}
