

#define LIN_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

float last_eight;
int count;
int now,sample_time;
int last = 0;
int sum;

#include <FFT.h> // include the library
int k;


void setup() {
  Serial.begin(115200); // use the serial port
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
  pinMode(A0, INPUT);
  pinMode(13,OUTPUT);
}

void loop() {
  while(1) { // reduces jitter
    for (int i = 0 ; i < FFT_N*2 ; i += 2) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = ((j << 8) | m)- 512; // form into an int (and subtract 512 to get it centered around 0
      //Serial.println(k); //DELETE
      //delay(1000); //DELETE
      //k -= 0x0200; // form into a signed int
      //k <<= 6; // form into a 16b signed int
      fft_input[i] = k; // put real data into even bins
      fft_input[i+1] = 0; // set odd bins to 0
    }
    
    fft_window(); // window the data for better frequency response
    fft_reorder(); // reorder the data before doing the fft
    fft_run(); // process the data in the fft
    fft_mag_lin(); // take the output of the fft
    //for(int ii = 0; ii < FFT_N/2 ; ii+=1){
      //Serial.println(fft_lin_out[ii]);
    //}
    
    
    //for(int ii = 0; ii < FFT_N; ii++){
      //Serial.println(fft_lin_out[0]+fft_lin_out[1]);
    //}


    //sum = fft_lin_out[0]+fft_lin_out[1];
    //Serial.println(sum);
    if(fft_lin_out[0] >= 8 && fft_lin_out[0] >= last_eight+2){
      //HIGH_HIT = fft_lin_out[0];
      digitalWrite(13,HIGH);
      count = 0;
    }
    else if(fft_lin_out[0] < 8){
      if(count <= 6){
        count++;
      }
      else{
        digitalWrite(13,LOW);
        last_eight = last_eight*15/16 + float(fft_lin_out[0])/16;
      }
    }
    //Serial.println(last_eight);

    //Serial.println(fft_lin_out[0]+fft_lin_out[1]);
    //Serial.println("\n\n\n\n\n\n");
    //Serial.println(millis());
  }
}
