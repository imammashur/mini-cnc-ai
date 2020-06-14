#include <Nextion.h>
#include <AFMotor.h>
#include <Servo.h> 
#include <math.h>

int total_titik = 0;
int x = 0, y = 0, t = 0;
int dx = 0, dy = 0;
int mode;
int iterasi;
int titik_x[99];
int titik_y[  99];
long waktu;
long waktu_mulai;
long waktu_tiap_iterasi[99];

AF_DCMotor motor(4,2);
Servo servo1;

AF_Stepper stepperX(48, 1);
AF_Stepper stepperY(48, 2);

void setup() {
  Serial.begin(9600);
  servo1.attach(10);
  motor.setSpeed(200);
  motor.run(RELEASE);
}

void loop() {
  motor.run(BACKWARD);
  if (Serial.available()) {
    String indata = Serial.readStringUntil('#');
    /*
     * LIST COMMANDS :
     * 
     * xt = tambah x :
     * yt = tambah y
     * xk = kurang x
     * yk = kurang y
     * tb = titik baru
     * rs = reset data
     * rn = start ai
     * sp = stop data
     */
    if (indata.indexOf("t_xt") > -1) {
      dx = 1;
    }
    if (indata.indexOf("t_xk") > -1) {
      if (x > 0)  {
        dx = -1;
      }
    }
    if (indata.indexOf("t_yt") > -1) {
      dy = 1;
    }
    if (indata.indexOf("t_yk") > -1) {
      if (y > 0)  {
        dy = -1;
      }
    }
    
    if (indata.indexOf("l_xt") > -1) {
      dx = 0;
    }
    if (indata.indexOf("l_xk") > -1) {
      dx = 0;
    }
    if (indata.indexOf("l_yt") > -1) {
      dy = 0;
    }
    if (indata.indexOf("l_yk") > -1) {
      dy = 0;
    }

    if (indata.indexOf("tb") > -1) {
      titik_baru();
    }
    
    if (indata.indexOf("rn") > -1) {
      mulai_proses();
    }

    if (indata.indexOf("rs") > -1) {
      reset_data();
    }
  }

  if (dx == 1)  {
      x = x+1;
      Serial.print("n0.val=");
      Serial.print(x);
      Serial.write(0xFF);
      Serial.write(0xFF);
      Serial.write(0xFF);
      tambah_x();
    }
  if (dy == 1)  {
      y = y+1;
      Serial.print("n1.val=");
      Serial.print(y);
      Serial.write(0xFF);
      Serial.write(0xFF);
      Serial.write(0xFF);
      tambah_y();
    }
  if (dx == -1)  {
      x = x-1;
      Serial.print("n0.val=");
      Serial.print(x);
      Serial.write(0xFF);
      Serial.write(0xFF);
      Serial.write(0xFF);
      kurang_x();
    }
  if (dy == -1)  {
      y = y-1;
      Serial.print("n1.val=");
      Serial.print(y);
      Serial.write(0xFF);
      Serial.write(0xFF);
      Serial.write(0xFF);
      kurang_y();
    }

  if (x < 0)  {
      x = 0;
      Serial.print("n0.val=");
      Serial.print(x);
      Serial.write(0xFF);
      Serial.write(0xFF);
      Serial.write(0xFF);
   }
  if (y < 0)  {
      y = 0;
      Serial.print("n1.val=");
      Serial.print(y);
      Serial.write(0xFF);
      Serial.write(0xFF);
      Serial.write(0xFF);
   }
}

void tambah_y() {
  stepperX.step(1, FORWARD, DOUBLE);
  delay(5);
}

void tambah_x() {
  stepperY.step(1, FORWARD, DOUBLE);
  delay(5);
}

void kurang_y() {
  stepperX.step(1, BACKWARD, DOUBLE);
  delay(5);
}

void kurang_x() {
  stepperY.step(1, BACKWARD, DOUBLE);
  delay(5);
}

void titik_baru() {
  titik_x[total_titik] = x;
  titik_y[total_titik] = y;
  total_titik += 1;
  Serial.print("n2.val=");
  Serial.print(total_titik);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
}

void reset_data() {
  total_titik = 0;
  Serial.print("n2.val=");
  Serial.print(total_titik);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  iterasi = 0;
  Serial.print("n3.val=");
  Serial.print(iterasi);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  kembali_ke_0();
}

void stop_data() {
}

void(*reset_alat) (void) = 0;

void mulai_proses() {
  waktu_mulai = millis();
  iterasi += 1;
  kembali_ke_0();
  Serial.print("n4.val=");
  Serial.print(iterasi);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);

  proses_plot();

  waktu = millis();
  waktu = waktu - waktu_mulai;
  waktu_tiap_iterasi[iterasi] = waktu;
  Serial.print("n3.val=");
  Serial.print(waktu);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  kembali_ke_0();
  delay(500);
 
  proses_sorting_biasa();
  waktu_mulai = millis();
  iterasi += 1;
  set_tercepat();
  proses_plot();
  waktu = millis();
  waktu = waktu - waktu_mulai;
  waktu_tiap_iterasi[iterasi] = waktu;
  Serial.print("n3.val=");
  Serial.print(waktu);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  kembali_ke_0();
  delay(500);

  proses_sorting_naivebayes();
  waktu_mulai = millis();
  iterasi += 1;
  Serial.print("n3.val=");
  Serial.print(waktu);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  set_tercepat();
  proses_plot();
  waktu = millis();
  waktu = waktu - waktu_mulai;
  waktu_tiap_iterasi[iterasi] = waktu;
  Serial.print("n3.val=");
  Serial.print(waktu);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  set_tercepat();
  kembali_ke_0();
  delay(500);
  reset_alat();

}

void proses_sorting_naivebayes()  {
  float titik_baru[total_titik];
  for (int a = 0; a <= total_titik; a++)  {
    titik_baru[a] = (titik_x[a]*titik_x[a])+(titik_y[a]*titik_y[a]);
    titik_baru[a] = sqrt(titik_baru[a]);
  }

  for (int a = 0; a <= total_titik; a++)  {
    for (int b = 0; b <= total_titik; b++)  {
      if (titik_baru[b] > titik_baru[a])  {
        int temp = titik_baru[a];
        titik_baru[a] = titik_baru[b];
        titik_baru[b] = temp;

        temp = titik_x[a];
        titik_x[a] = titik_x[b];
        titik_x[b] = titik_x[a];
        temp = titik_y[a];
        titik_y[a] = titik_y[b];
        titik_y[b] = titik_y[a];
      }
    }
  }
}

void set_tercepat()  {
  int a = 0;
  float temp = 100;
  while (a <= iterasi)  {
    if (waktu_tiap_iterasi[a] < temp) {temp = waktu_tiap_iterasi[a];}
  }
  Serial.print("n4.val=");
  Serial.print(temp);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
}

void proses_plot() {
  int i = 0;
  while (i <= total_titik)  {
    while (x != titik_x[i])  {
      if (x > titik_x[i]) {
        kurang_x();
        x -= 1;
      }
      if (x < titik_x[i]) {
        tambah_x();
        x += 1;
      }
    }
    while (y != titik_y[i])  {
      if (y > titik_y[i]) {
        kurang_y();
        y -= 1;
      }
      if (y < titik_y[i]) {
        tambah_y();
        y += 1;
      }
    }
    write_servo();
    i++;
  }
}

void proses_sorting_biasa() {
  for (int a = 0; a <= total_titik; a++)  {
    for (int b = 0; b <= total_titik; b++)  {
      if (titik_x[b] > titik_x[a])  {
        int tmp = titik_x[a];
        titik_x[a] = titik_x[b];
        titik_x[b] = tmp;
      }
      if (titik_y[b] > titik_y[a])  {
        int tmp = titik_y[a];
        titik_y[a] = titik_y[b];
        titik_y[b] = tmp;
      }
    }
  }
}


void kembali_ke_0() {
  dx = -1;
  dy = -1;
  while (x > 0) {
    kurang_x();
    x -= 1;
  }
  x = 0;
  Serial.print("n0.val=");
  Serial.print(x);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);

  while (y > 0) {
    kurang_y();
    y -= 1;
  }
  y = 0;
  dx = 0;
  dy = 0;
  Serial.print("n1.val=");
  Serial.print(y);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
}

void write_servo()  {
 for (int i=75; i>0; i--) {
    servo1.write(i);
    delay(3);
 }
 delay(250);
 
 for (int i=0; i<75; i++) {
    servo1.write(i);
    delay(3);
 }
}
  
