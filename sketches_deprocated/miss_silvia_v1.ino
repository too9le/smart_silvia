 /***************************************************
* Liquid Level Sensor-XKC-Y25-T12V
* ****************************************************
* This example is to get liquid level

* @author jackli(Jack.li@dfrobot.com)
* @version V1.0
* @date 2016-1-30

* GNU Lesser General Public License.
* See <http://www.gnu.org/licenses/> for details.
* All above must be included in any redistribution
* ****************************************************/
int Liquid_level=0;
void setup() {
Serial.begin(9600);
pinMode(23,INPUT);
}

void loop() {
Liquid_level=digitalRead(23);
Serial.print("Liquid_level= ");
Serial.println(Liquid_level,DEC);
delay(500);
} 