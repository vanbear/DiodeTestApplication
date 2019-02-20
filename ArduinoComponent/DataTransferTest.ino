void setup()
{
    Serial.begin(9600);

    print("{CalibrationStart:true}");
    print("{MeasureStart:HallA;25;32}");
    int data = 2;

    bool dir = true;
    for (int i=0; i<25; i++)
    {
        dir = !dir;
        for (int j=0; j<32; j++)
        {
            print("{Data:"+String(i)+";"+String(j)+";"+"0;"+String(dir)+";"+String(int(ceil(sqrt(data))))+"}");
        }
        data+=1000;
    }
    print("{MeasureEnd:true}");
}

void loop()
{
}

void print(String msg)
{
    Serial.println(msg);
    delay(250);
}