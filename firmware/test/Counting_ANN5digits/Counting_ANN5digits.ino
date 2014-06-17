const int disp1 = A1;  // Displays are numbered from left to right
const int disp2 = A2;  // as disp1 to disp4.
const int disp3 = A3;  // Scan displays quickly and use POV to display
const int disp4 = A4;  // 4 digits with only 1 shift register

const byte SevenSeg[16] = 
{
  // Hex digits
  B11111100, B01100000, B11011010, B11110010,   // 0123
  B01100110, B10110110, B10111110, B11100000,   // 4567
  B11111110, B11110110, B11101110, B00111110,   // 89AB
  B00011010, B01111010, B10011110, B10001110,   // CDEF

};



////NN Stuff////////
const int N = 5;

int NN[N] = {
  1,    //Random Number Flag
  1,    //Has number been found before?
  1,    //Is it a Real Number? Flag
  1,    //Is it after the lowest known number?
  1,    //Is it before the highest known number?
};

int NeuralCheck[N] = {
1,    
  1,   
  1,    
  1,    
  1,    
};
int M[N][N] = {
  0,0,0,0,0,
  0,0,0,0,0,
  0,0,0,0,0,
  0,0,0,0,0,
  0,0,0,0,0};

int NM[N][N] = {
  0,0,0,0,0,
  0,0,0,0,0,
  0,0,0,0,0,
  0,0,0,0,0,
  0,0,0,0,0,};

int segment[7] = {
  9,7,5,3,2,8,6};
int segmentByte[7] = {
  0,0,0,0,0,0,0};
int currNum = 0;
int currentFoundNum = 0;
int lastNum = 0;
int learnedNum[10] = {
  0,0,0,0,0,0,0,0,0,0};
int realNum[10][7] = {
  {
    1,1,1,1,1,1,0          } //0
  ,
  {
    0,1,1,0,0,0,0          }//1
  ,
  {
    1,1,0,1,1,0,1          }//2
  ,
  {
    1,1,1,1,0,0,1          }//3
  ,
  {
    0,1,1,0,0,1,1          }//4
  ,
  {
    1,0,1,1,0,1,1          }//5
  ,
  {
    1,0,1,1,1,1,1          }//6
  ,
  {
    1,1,1,0,0,0,0          }//7
  ,
  {
    1,1,1,1,1,1,1          }//8
  ,
  {
    1,1,1,1,0,1,1          }//9
  ,
};



void setup(){
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  pinMode(disp1, OUTPUT);
  pinMode(disp2, OUTPUT);
  pinMode(disp3, OUTPUT);
  pinMode(disp4, OUTPUT);
  digitalWrite(disp1, LOW);
  digitalWrite(disp2, LOW);
  digitalWrite(disp3, LOW);
  digitalWrite(disp4, LOW);
  for (int x = 0; x<7; x++){
    pinMode(segment[x], OUTPUT);
    digitalWrite(segment[x], HIGH);
    delay(50);
  }
  delay(100);
  digitalWrite(disp1, HIGH);
  delay(50);
  digitalWrite(disp2, HIGH);
  delay(50);
  digitalWrite(disp3, HIGH);
  delay(50);
  digitalWrite(disp4, HIGH);
  for (int x = 0; x<7; x++){
    digitalWrite(segment[x], LOW);
  }
  generateRandNum();
}

void loop(){
  //while(1){
  //  printTest();
  //}
  //displayPrevNum();
  if (NN[1] == -1){
    generateRandNum();    //Generates a Random Number Displayed
  }
  checkKnownNum();      //Checks to see if previously discovered.
  checkRealNum();       //If New sequence check to see if its actually a number.
  checkNumberPlace();   //Checks to see if new number is above last number.

  VectorTimesVector();
  MatrixAddMatrix();
  MatrixTimesVector();

  for (int y = 0; y<N; y++){
    if (NN[y] != NeuralCheck[y]){
      break;
    }
    if (y == N-2){
      lastNum = currentFoundNum;
      Serial.println("Attempting Count.");
      attemptCount();
    }
    //shiftNum();
    for (int x = 0; x<7; x++){
      digitalWrite(segment[x], LOW);
    }
  }
}


void generateRandNum(){
  digitalWrite(disp4, LOW);
  for (int x = 0; x<7; x++){
    int randomByte = random(2);
    segmentByte[x] = randomByte;
    Serial.print(randomByte);
    if (x<6){
      //Serial.print(",");
    }
    digitalWrite(segment[x], randomByte);
  }
  Serial.println();
  //delay(50);

}

void checkRealNum(){
  Serial.println("Checking for Real Number...");
  NN[2] = -1;

  for (int x = 0; x<10; x++){
    int checkFlag = 1;
    for (int y = 0; y<7; y++){  
      if (segmentByte[y] == realNum[x][y]){
        //Serial.print(segmentByte[y]);
      }
      if (segmentByte[y] != realNum[x][y]){
        checkFlag = 0;
      } 
    }
    if (checkFlag == 1){
      //Serial.println();
      Serial.println("Found Number!");
      NN[2] = 1;
      delay(50);
      learnedNum[x] = 1;
      currentFoundNum = x;
      break;
    }
    //Serial.println();

  }
Serial.println();
}

void checkKnownNum(){
  Serial.println("Checking Memory...");

  NN[1] = -1;
  for (int x = 0; x<10; x++){
    int checkFlag = 1;
    if (learnedNum[x] == 1){
      for (int y = 0; y<7; y++){ 
        if (segmentByte[y] == realNum[x][y]){
          //Serial.print(segmentByte[y]);
        }
        if (segmentByte[y] != realNum[x][y]){
          checkFlag = 0;
        }

      }
      //Serial.println();
      if (checkFlag == 1){
        Serial.println("Memory Found");
        NN[1] = 1;
        currentFoundNum = x;
        delay(50);
        break;
      }
    }
  }
}

void checkNumberPlace(){
  int LowNum, HighNum = 0;
  for (int x = 0; x<10; x++){
    if (learnedNum[x] == 1){
      LowNum = x;
      break;
    }
  }
  for (int x = 10; x>0; x--){
    if (learnedNum[x] == 1){
      HighNum = x;
      break;
    }
  }
  NN[3] = -1;
  //NN[4] = -1;
  if (currentFoundNum > LowNum){
    NN[3] = 1;
    // currNum = currentFoundNum;
  }
  //if (currentFoundNum < HighNum){
   // NN[4] = 1;

  //}
}

void displayPrevNum(){

}

void VectorTimesVector(){
  //Serial.println("VtV:");
  int z = 0;
  for (int x = 0; x<N; x++){
    //Serial.print("{");
    for (int y = 0; y<N; y++){ 
      NM[x][y] = NN[x] * NN[y];

      //Serial.print(NM[x][y]);
      if (y<N) {
        //Serial.print(",");
      }
    }
    //Serial.print("}");
    //Serial.println();
  }
}

void MatrixAddMatrix(){
  for (int x = 0; x<N; x++){
    for (int y = 0; y<N; y++){
      M[x][y] = M[x][y] + NM[x][y];
    }
  }
} 
void MatrixTimesVector(){
  for (int x = 0; x<N; x++){
    
    for (int y = 0; y<N; y++){
      NN[x] += M[x][y] * NN[y];
      
      
    }
  }
  Serial.print("{");
  for (int x = 0; x<N; x++){
    
  if (NN[x] < 0){
        NN[x] = -1;
      }
      if (NN[x] >= 0){
        NN[x] = 1;
      }
      Serial.print(NN[x]);
      if (x<N){
        Serial.println("}");
      }
      
  }
}

void shiftNum(){
  digitalWrite(disp1, LOW);
  for (int x = 0; x<7; x++){
    digitalWrite(segment[x], realNum[lastNum][x]);
  }
  //digitalWrite(disp1, HIGH);
}

void attemptCount(){
  Serial.println("////////////");
  delay(100);
NN[4] = -1;
  for (int y = 0; y<10; y++){

    delay(200);
    if (learnedNum[y] == 1){
      Serial.print(y);
      Serial.print(",");
      if (y == 9){
        NN[5] = 1;
      }
      for (int x = 0; x<7; x++){
        digitalWrite(segment[x], realNum[y][x]);

      }
    }
    else{
      Serial.println();
      break;

    }


  }
  delay(500);
}
void printTest(){
  digitalWrite(disp1, LOW);
  for (int x = 0; x<10; x++){
    for (int y = 0; y<7; y++){ 
      digitalWrite(segment[y], realNum[x][y]);
    }
    delay(500);
    for (int x = 0; x<7; x++){
      digitalWrite(segment[x], LOW);
    }
  }
  Serial.println();
}





