#include <Arduino.h>

#define SLEEP_DUR 10000000
#define RTCMEMORYSTART 66
#define RTCMEMORYLEN 126


extern "C" {
#include "user_interface.h" // this is for the RTC memory read/write functions
}


typedef struct {
  int counter;
  int other;
} rtcStore;

rtcStore rtcMem;

int i,buckets;
int toggleFlag;


const PROGMEM uint32_t freq[280]={369,329,369,391,369,329,293,293,329,369,391,391,391,391,391,369,329,329,220,293,
329,369,369,329,369,329,293,293,293,493,493,493,554,587,554,493,440,440,391,369,
391,391,391,440,493,440,391,369,369,293,293,493,493,493,554,587,554,493,440,440,
391,369,391,391,329,369,391,369,329,293,369,369,329,369,369,329,293,293,329,369,
391,391,391,391,391,369,329,329,220,293,329,369,369,329,369,329,293,293,293,493,
493,493,554,587,554,493,440,440,391,369,391,391,391,440,493,440,391,369,369,293,
293,493,493,493,554,587,554,493,440,440,391,369,391,391,329,369,391,369,329,293,
369,329,369,391,369,329,293,293,329,369,391,391,391,391,391,369,329,329,220,293,
329,369,369,329,369,329,293,293,293,493,493,493,554,587,554,493,440,440,391,369,
391,391,391,440,493,440,391,369,369,293,293,493,493,493,554,587,554,493,440,440,
391,369,391,391,329,369,391,369,329,293,369,329,369,391,369,329,293,293,329,369,
391,391,391,391,391,369,329,329,220,293,329,369,369,329,369,329,293,293,293,493,
493,493,554,587,554,493,440,440,391,369,391,391,391,440,493,440,391,369,369,293,
293,493,493,493,554,587,554,493,440,440,391,369,391,391,329,369,391,369,329,293};

const PROGMEM uint32_t duration[280]={1950,236,236,236,236,236,696,1671,473,236,696,1393,236,236,236,236,696,1393,236,473,
236,835,1393,236,473,236,835,1393,835,835,1114,236,236,236,236,236,835,1393,557,236,
835,1114,236,236,236,236,236,557,1114,473,236,557,1114,236,236,236,236,236,557,1114,
473,236,557,835,236,236,236,236,236,835,696,975,236,236,236,236,696,1671,473,236,
696,1393,236,236,236,236,696,1393,236,473,236,835,1393,236,473,236,835,1393,835,835,
1114,236,236,236,236,236,835,1393,557,236,835,1114,236,236,236,236,236,557,1114,473,
236,557,1114,236,236,236,236,236,557,1114,473,236,557,835,236,236,236,236,236,835,
1950,236,236,236,236,236,696,1671,473,236,696,1393,236,236,236,236,696,1393,236,473,
236,835,1393,236,473,236,835,1393,835,835,1114,236,236,236,236,236,835,1393,557,236,
835,1114,236,236,236,236,236,557,1114,473,236,557,1114,236,236,236,236,236,557,1114,
473,236,557,835,236,236,236,236,236,696,1950,236,236,236,236,236,696,1671,473,236,
696,1393,236,236,236,236,696,1393,236,473,236,835,1393,236,473,236,835,1393,835,835,
1114,236,236,236,236,236,835,1393,557,236,835,1114,236,236,236,236,236,557,1114,473,
236,557,1114,236,236,236,236,236,557,1114,473,236,557,835,236,236,236,236,236,835};

const PROGMEM uint32_t del_time[280]={2167,309,309,309,309,309,927,1857,618,309,927,1547,309,309,309,309,927,1547,309,618,
309,928,1547,309,618,309,928,1856,928,928,1238,309,309,309,309,309,928,1856,619,309,
928,1238,309,309,309,309,309,928,1857,618,309,928,1238,309,309,309,309,309,928,1857,
618,309,928,1237,309,309,309,309,309,3714,927,1547,309,309,309,309,927,1857,618,309,
927,1547,309,309,309,309,927,1547,309,618,309,928,1547,309,618,309,928,1856,928,928,
1238,309,309,309,309,309,928,1856,619,309,928,1238,309,309,309,309,309,928,1857,618,
309,928,1238,309,309,309,309,309,928,1857,618,309,928,1237,309,309,309,309,309,3714,
2167,309,309,309,309,309,927,1857,618,309,927,1547,309,309,309,309,927,1547,309,618,
309,928,1547,309,618,309,928,1856,928,928,1238,309,309,309,309,309,928,1856,619,309,
928,1238,309,309,309,309,309,928,1857,618,309,928,1238,309,309,309,309,309,928,1857,
618,309,928,1237,309,309,309,309,309,3714,2167,309,309,309,309,309,927,1857,618,309,
927,1547,309,309,309,309,927,1547,309,618,309,928,1547,309,618,309,928,1856,928,928,
1238,309,309,309,309,309,928,1856,619,309,928,1238,309,309,309,309,309,928,1857,618,
309,928,1238,309,309,309,309,309,928,1857,618,309,928,1237,309,309,309,309,309,928};


unsigned long last_time_play=0;
unsigned long last_time_pause=0;
int count=0;
bool emit_sound(unsigned long *ac_t,int *count,bool play);
bool play_flag=0;
bool line_flag=0;
void setup() {
  // put your setup code here, to run once:
  /*
   int rtcPos;
  Serial.begin(115200);
  Serial.println("Start");
  //ESP.deepSleep(SLEEP_DUR);
    buckets = (sizeof(rtcMem) / 4);
      Serial.print("Buckets ");
  Serial.println(buckets);
  system_rtc_mem_read(64, &toggleFlag, 4);
  Serial.print("toggle Flag ");
  Serial.println(toggleFlag);
  if (toggleFlag!=1) {
    Serial.println("Start Writing");

      rtcMem.counter=0;
      rtcMem.other=2;
      int rtcPos = RTCMEMORYSTART + 1 * buckets;
      system_rtc_mem_write(rtcPos, &rtcMem, buckets * 4);
      toggleFlag = 1;
      system_rtc_mem_write(64, &toggleFlag, 4);

      Serial.print(" Position: ");
      Serial.print(rtcPos);
      Serial.print(", count: ");
      Serial.print(rtcMem.counter);
      Serial.print(", other: ");
      Serial.println(rtcMem.other);
      yield();
    
    Serial.println("Writing done");
  }
  else {
    Serial.println("Start reading");
  
      rtcPos = RTCMEMORYSTART + 1 * buckets;
      system_rtc_mem_read(rtcPos, &rtcMem, sizeof(rtcMem));
     // toggleFlag = true;
     // system_rtc_mem_write(64, &toggleFlag, 4);

      Serial.print(" Position ");
      Serial.print(rtcPos);
      Serial.print(", count: ");
      Serial.print(rtcMem.counter);
      Serial.print(", other: ");
      Serial.println(rtcMem.other);
      yield();
    
    Serial.println("reading done");
    if(rtcMem.counter==279)
    {
      rtcMem.counter=0;
      rtcMem.other=0;
    }else{
            rtcMem.counter++;
      rtcMem.other+=2;
    }

      rtcPos = RTCMEMORYSTART + 1 * buckets;
      system_rtc_mem_write(rtcPos, &rtcMem, buckets * 4);
    
  }
 
  Serial.print("ARR:");
  Serial.print("\t");
  Serial.print(pgm_read_word_near(freq + rtcMem.counter));
  Serial.print("\t");
    Serial.print(pgm_read_word_near(duration + rtcMem.counter));
  Serial.print("\t");
    Serial.print(pgm_read_word_near(del_time + rtcMem.counter));
     Serial.print("\n");
   Serial.println("before sleep");
  ESP.deepSleep(SLEEP_DUR, WAKE_RFCAL);
  */
 pinMode(3,FUNCTION_3);
 count=0;
 play_flag=0;
 line_flag=0;
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long ac_time=millis();

   //analogWrite(3, 201);
   //delay(1000);
  play_flag=emit_sound(&ac_time,&count,play_flag);
  // count++;
}

bool emit_sound(unsigned long *ac_t,int *count,bool play)
{
  //if(*ac_t-last_time>)
  //delay jest oparty o ten sam timer co korzysta pwm nie mozna wiec go uzyc
  if(!play && *ac_t-last_time_pause>pgm_read_word_near(del_time + *count)/2)
  {
    if(line_flag) {//go to next line tone and plause
      //++(*count);
      *count=(*count==279) ? 0:(*count)+1;
      line_flag=0;
    }
    analogWriteFreq(pgm_read_word_near(freq + *count));
    analogWrite(3,512); //50% duty
    last_time_play=millis();
    //delay(pgm_read_word_near(duration + count));
    play=1;
    //(*count)++;
    return play;
  }
  if(play && *ac_t-last_time_play>pgm_read_word_near(duration + *count)/2)
  {
    last_time_pause=millis();
    analogWrite(3,0);
    //delay(pgm_read_word_near(del_time + count));
    play=0;
    line_flag=1;
    return play;
  }
  return play;//wtedy bez zmian jak sie zdane wnie wykona
}